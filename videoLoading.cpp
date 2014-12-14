#include "videoLoading.h"
#include <iostream>
using namespace cv;

VideoLoader::VideoLoader(std::string& filename)
{
	loadVideo(filename);
	videoObject = VideoObjectInfo(filename);
}

bool VideoLoader::loadVideo(std::string &filename)
{
	//首先可以明确的是一个视频必定对应一个videoObjectInfo，生成一个对应的视频信息文件
	//根据视频命名可以直接确定视频的开始时间信息，2014,
	if(cap.open(filename))
	{
		//这里稍微整理一下当前读取视频的信息，得到对应视频信息文件存放位置等
		return true;
	}
	else
	{
		std::cout<<"can not open the file"<<std::endl;
		return false;
	}
}

//弃用，一次进行三帧差法，不能够得到连续帧信息，虽然效果差不多
//未必弃用,相同的视频，这样得到的摘要视频更少，观望
void VideoLoader::threeFrameDiff()
{
	std::cout<<"three frame diff"<<std::endl;
	if(!cap.isOpened())
		return;

	Mat img_src1,img_src2,img_src3;//3帧法需要3帧图片
	Mat img_dst,gray1,gray2,gray3;
	Mat gray_diff1,gray_diff2;//存储2次相减的图片
	Mat gray;//用来显示前景的
	while(cap.read(img_src1))
	{
		cvtColor(img_src1,gray1,CV_BGR2GRAY);
		imshow("frame_src1",img_src1);
		//waitKey(23);
		if(!cap.read(img_src2))
			break;
		cvtColor(img_src2,gray2,CV_BGR2GRAY);
		//imshow("frame_src2",img_src2);//
		//waitKey(67);
		if(!cap.read(img_src3))
			break;
		cvtColor(img_src3,gray3,CV_BGR2GRAY);
		//imshow("frame_src3",img_src3);

		subtract(gray2,gray1,gray_diff1);//第二帧减第一帧
		subtract(gray3,gray2,gray_diff2);//第三帧减第二帧

		for(int i=0;i<gray_diff1.rows;i++)
		{
			for(int j=0;j<gray_diff1.cols;j++)
			{
				if(abs(gray_diff1.at<unsigned char>(i,j)) >= threshold_diff1)//这里模板参数一定要用unsigned char，否则就一直报错
					gray_diff1.at<unsigned char>(i,j)=255;            //第一次相减阈值处理
				else 
					gray_diff1.at<unsigned char>(i,j)=0;

				if(abs(gray_diff2.at<unsigned char>(i,j)) >= threshold_diff2)//第二次相减阈值处理
					gray_diff2.at<unsigned char>(i,j)=255;
				else 
					gray_diff2.at<unsigned char>(i,j)=0;
			}
		}
		bitwise_or(gray_diff1,gray_diff2,gray);
		imshow("foreground",gray);

		char c = (char)waitKey(5);
		if(c == ' ')
		{
			while((char)waitKey(5) != ' '){}
		}
		else if(c == 27)
		{
			break;
		}
	}
	return;
}

void VideoLoader::threeFrameDiffImprove(std::string &filename)
{
	std::cout<<"three frame diff improve"<<std::endl;
	if(!cap.isOpened())
		return;
	Mat img_src1,img_src2,img_src3;//3帧法需要3帧图片
	Mat img_dst,gray1,gray2,gray3;
	Mat gray_diff1,gray_diff2;//存储2次相减的图片
	Mat gray;//用来显示前景的
	Mat frame;
	
	
	int dilation_type = MORPH_ELLIPSE; 
	Mat element = getStructuringElement(dilation_type,
			Size(5,5));

	int erosion_type = MORPH_CROSS;
	Mat element_e = getStructuringElement(erosion_type,
			Size(3,3));

	cv::Size orig = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	const int changeRatio = 6;
	cv ::Size change = cv::Size(orig.width / changeRatio, orig.height / changeRatio);//更改为原始尺寸的

	cap.read(frame);
	cv::resize(frame,img_src1,change);
	cvtColor(img_src1,gray1,CV_BGR2GRAY);
	//imshow("frame_src1",img_src1);
	//waitKey(60);

	cap.read(frame);
	cv::resize(frame,img_src2,change);
	cvtColor(img_src2,gray2,CV_BGR2GRAY);
	//imshow("frame_src2",img_src2);//
	//waitKey(60);

	while(cap.read(frame))
	{
		cv::resize(frame,img_src3,change);
		cvtColor(img_src3,gray3,CV_BGR2GRAY);
		imshow("frame_src3",img_src3);

		subtract(gray2,gray1,gray_diff1);//第二帧减第一帧
		subtract(gray3,gray2,gray_diff2);//第三帧减第二帧

		for(int i=0;i<gray_diff1.rows;i++)
		{
			uchar* ptr1 = gray_diff1.ptr<uchar>(i);
			uchar* ptr2 = gray_diff2.ptr<uchar>(i);

			for(int j=0;j<gray_diff1.cols;j++)
			{
				if(abs(ptr1[j]) >= threshold_diff)//第一次相减阈值处理
					ptr1[j] = 255;
				else
					ptr1[j] = 0;

				if(abs(ptr2[j]) >= threshold_diff)//第二次相减阈值处理
					ptr2[j] = 255;
				else
					ptr2[j] = 0;

			}
		}
		bitwise_xor(gray_diff1,gray_diff2,gray);
		//在这里完成前景图的去噪工作，传递到extract中的前景图是个相对干净的，
		//这里根据前景提取方法不同，去噪方法也不尽相同，因此不能再extract中完成去噪工作，
		imshow("foregroundBefore",gray);
		//cv::erode(gray,gray,element_e);
		
		cv::medianBlur(gray,gray,3);
		cv::dilate(gray,gray,element);
		//cv::GaussianBlur(gray,gray,cv::Size(3,3),0);
		imshow("foreground",gray);
		//videoObject.extractObjectInfo(gray,1);
		char c = (char)waitKey(5);
		if(c == ' ')
		{
			while((char)waitKey(5) != ' '){}
		}
		else if(c == 27)
		{
			break;
		}
		gray2.copyTo(gray1);
		//imshow("frame_src1",img_src1);
		//cvtColor(img_src1,gray1,CV_BGR2GRAY);

		gray3.copyTo(gray2);
		//imshow("frame_src2",img_src2);
		//cvtColor(img_src2,gray2,CV_BGR2GRAY);
	}

	//videoObject.saveObjectInfo(filename);
}
//速度过慢，弃用
void VideoLoader::GMM()
{
	std::cout<<"BackgroundSubtractorMOG"<<std::endl;
	if(!cap.isOpened())
		return;

	//BackgroundSubtractorMOG bgSub;
	Mat frame,original,gray;
	Mat foreground;
	cv::Size orig = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	cv ::Size change = cv::Size(orig.width / 6, orig.height / 6);//更改为原始尺寸的1/6

	while(cap.read(original))
	{
		cv::resize(original,frame,change);
		imshow("frame",frame);

		cv::cvtColor(frame,gray,CV_BGR2GRAY);
		//bgSub(frame,foreground,0.1);
		imshow("gray",gray);

		char c = (char)waitKey(10);
		if(c == ' ')
		{
			while((char)waitKey(1) != ' '){}
		}
		else if(c == 27)
		{
			break;
		}
	}
}

void VideoLoader::GMM2(std::string &filename)
{
	std::cout<<"BackgroundSubtractorMOG2"<<std::endl;
	if(!cap.isOpened())
		return;
	cv::Size orig = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	const int changeRatio = 6;
	cv ::Size change = cv::Size(orig.width / changeRatio, orig.height / changeRatio);//更改为原始尺寸的

	BackgroundSubtractorMOG2 bgSub = cv::BackgroundSubtractorMOG2(100,16,false);
	bgSub.nmixtures = 5;
	
	Mat original;
	Mat foreground;
	Mat frame;
	int frameNum = 0;
	while(cap.read(frame))
	{
		//imshow("original",original);
		cv::resize(frame,original,change);//尺寸为原始图像尺寸的 1/6,记得进行同比例换算，这样做的限制太多了，有没有更加通用的方法？
		imshow("frame",original);
		frameNum++;//帧编号从1开始，而实际帧读取是从0开始，这个以后可能会出问题，要留意一下
		bgSub(original,foreground);
		imshow("foregroundBefore",foreground);
		cv::medianBlur(foreground,foreground,5);//对前景图像进行简单过滤，
		imshow("foreground",foreground);

		videoObject.extractObjectInfo(foreground,frameNum);
		char c = (char)waitKey(2);
		if(c == ' ')
		{
			while((char)waitKey(5) != ' '){}
		}
		else if(c == 27)
		{
			break;
		}
	}
	videoObject.saveObjectInfo(filename);
}

//混合高斯混合模型及帧差法，
//希望能够解决高斯混合模型对光照敏感的缺点，并得到足够运动目标信息
void VideoLoader::MixGMM2andFrameDiff(std::string &filename)
{
	std::cout<<"mix GMM and three frame diff improve"<<std::endl;
	if(!cap.isOpened())
		return;
	Mat frame;//用于暂时存储视频读取每一帧

	Mat img_src1,img_src2,img_src3;//三帧法需要3帧图片
	Mat gray1,gray2,gray3;
	Mat gray_diff1,gray_diff2;//存储2次相减的图片
	Mat gray;//用来显示前景的
	Mat foreground;//高斯混合模型得到前景图
	Mat img_dst;

	cv::Size orig = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	const int changeRatio = 6;
	cv ::Size change = cv::Size(orig.width / changeRatio, orig.height / changeRatio);//更改为原始尺寸的

	BackgroundSubtractorMOG2 bgSub = cv::BackgroundSubtractorMOG2(100,16,false);
	bgSub.nmixtures = 5;

	int dilation_type = MORPH_ELLIPSE; 
	Mat element = getStructuringElement(dilation_type,
			Size(11,11));

	int frameNum = 0;

	cap.read(frame);
	frameNum++;
	cv::resize(frame,img_src1,change);
	cvtColor(img_src1,gray1,CV_BGR2GRAY);
	bgSub(img_src1,foreground);

	//imshow("frame_src1",img_src1);
	//waitKey(60);

	cap.read(frame);
	frameNum++;
	cv::resize(frame,img_src2,change);
	cvtColor(img_src2,gray2,CV_BGR2GRAY);
	bgSub(img_src2,foreground);
	//imshow("frame_src2",img_src2);//
	//waitKey(60);

	while(cap.read(frame))
	{
		frameNum++;
		cv::resize(frame,img_src3,change);
		cvtColor(img_src3,gray3,CV_BGR2GRAY);
		imshow("frame_src3",img_src3);

		subtract(gray2,gray1,gray_diff1);//第二帧减第一帧
		subtract(gray3,gray2,gray_diff2);//第三帧减第二帧

		for(int i=0;i<gray_diff1.rows;i++)
		{
			uchar* ptr1 = gray_diff1.ptr<uchar>(i);
			uchar* ptr2 = gray_diff2.ptr<uchar>(i);

			for(int j=0;j<gray_diff1.cols;j++)
			{
				if(abs(ptr1[j]) >= threshold_diff)//第一次相减阈值处理
					ptr1[j] = 255;
				else
					ptr1[j] = 0;

				if(abs(ptr2[j]) >= threshold_diff)//第二次相减阈值处理
					ptr2[j] = 255;
				else
					ptr2[j] = 0;

			}
		}
		bitwise_xor(gray_diff1,gray_diff2,gray);
		//在这里完成前景图的去噪工作，传递到extract中的前景图是个相对干净的，
		//这里根据前景提取方法不同，去噪方法也不尽相同，因此不能再extract中完成去噪工作，
		//imshow("foregroundByFrameDiff",gray);
		
		bgSub(img_src3,foreground);
		//imshow("foregroundByGMM",foreground);

		bitwise_and(gray,foreground,img_dst);
		//imshow("foregroundByMix",img_dst);


		cv::medianBlur(img_dst,img_dst,3);
		cv::dilate(img_dst,img_dst,element);
		//cv::GaussianBlur(gray,gray,cv::Size(3,3),0);
		//当前能够将运动目标检测到，同时提出光照因素影响，但存在问题，不能够将运行目标完整检测，只能够检测部分，甚至是很少一部分，
		//在当前情形下，GMM能够将运动目标较为完整的检测，存在问题是碎片化，如何将两者结合起来，
		//最简单的思路，将仅受关照影响的帧直接剔除，剩余部分，则对GMM内容进行检测，不能够局限于当前视频，应当具有普适性，

		cv::resize(img_dst,img_dst,cv::Size(img_dst.cols / 6, img_dst.rows / 6));
		bool isEmpty = true;
		for(int i = 0; isEmpty && i < img_dst.rows; i = i + 3)
		{
			uchar* ptr =  img_dst.ptr<uchar>(i);
			for(int j = 0; isEmpty && j < img_dst.cols; j = j + 2)
			{
				if(ptr[j] != 0)
				{
					isEmpty = false;
				}
			}
		}
		if(!isEmpty)
		{
			//imshow("foreground",img_dst);
			cv::medianBlur(foreground,foreground,5);
			videoObject.extractObjectInfo(foreground,frameNum);
		}
		char c = (char)waitKey(5);
		if(c == ' ')
		{
			while((char)waitKey(5) != ' '){}
		}
		else if(c == 27)
		{
			break;
		}
		gray2.copyTo(gray1);
		gray3.copyTo(gray2);
	}
	//videoObject.saveObjectInfo(filename);
}