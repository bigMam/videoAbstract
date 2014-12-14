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
	//���ȿ�����ȷ����һ����Ƶ�ض���Ӧһ��videoObjectInfo������һ����Ӧ����Ƶ��Ϣ�ļ�
	//������Ƶ��������ֱ��ȷ����Ƶ�Ŀ�ʼʱ����Ϣ��2014,
	if(cap.open(filename))
	{
		//������΢����һ�µ�ǰ��ȡ��Ƶ����Ϣ���õ���Ӧ��Ƶ��Ϣ�ļ����λ�õ�
		return true;
	}
	else
	{
		std::cout<<"can not open the file"<<std::endl;
		return false;
	}
}

//���ã�һ�ν�����֡������ܹ��õ�����֡��Ϣ����ȻЧ�����
//δ������,��ͬ����Ƶ�������õ���ժҪ��Ƶ���٣�����
void VideoLoader::threeFrameDiff()
{
	std::cout<<"three frame diff"<<std::endl;
	if(!cap.isOpened())
		return;

	Mat img_src1,img_src2,img_src3;//3֡����Ҫ3֡ͼƬ
	Mat img_dst,gray1,gray2,gray3;
	Mat gray_diff1,gray_diff2;//�洢2�������ͼƬ
	Mat gray;//������ʾǰ����
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

		subtract(gray2,gray1,gray_diff1);//�ڶ�֡����һ֡
		subtract(gray3,gray2,gray_diff2);//����֡���ڶ�֡

		for(int i=0;i<gray_diff1.rows;i++)
		{
			for(int j=0;j<gray_diff1.cols;j++)
			{
				if(abs(gray_diff1.at<unsigned char>(i,j)) >= threshold_diff1)//����ģ�����һ��Ҫ��unsigned char�������һֱ����
					gray_diff1.at<unsigned char>(i,j)=255;            //��һ�������ֵ����
				else 
					gray_diff1.at<unsigned char>(i,j)=0;

				if(abs(gray_diff2.at<unsigned char>(i,j)) >= threshold_diff2)//�ڶ��������ֵ����
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
	Mat img_src1,img_src2,img_src3;//3֡����Ҫ3֡ͼƬ
	Mat img_dst,gray1,gray2,gray3;
	Mat gray_diff1,gray_diff2;//�洢2�������ͼƬ
	Mat gray;//������ʾǰ����
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
	cv ::Size change = cv::Size(orig.width / changeRatio, orig.height / changeRatio);//����Ϊԭʼ�ߴ��

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

		subtract(gray2,gray1,gray_diff1);//�ڶ�֡����һ֡
		subtract(gray3,gray2,gray_diff2);//����֡���ڶ�֡

		for(int i=0;i<gray_diff1.rows;i++)
		{
			uchar* ptr1 = gray_diff1.ptr<uchar>(i);
			uchar* ptr2 = gray_diff2.ptr<uchar>(i);

			for(int j=0;j<gray_diff1.cols;j++)
			{
				if(abs(ptr1[j]) >= threshold_diff)//��һ�������ֵ����
					ptr1[j] = 255;
				else
					ptr1[j] = 0;

				if(abs(ptr2[j]) >= threshold_diff)//�ڶ��������ֵ����
					ptr2[j] = 255;
				else
					ptr2[j] = 0;

			}
		}
		bitwise_xor(gray_diff1,gray_diff2,gray);
		//���������ǰ��ͼ��ȥ�빤�������ݵ�extract�е�ǰ��ͼ�Ǹ���Ըɾ��ģ�
		//�������ǰ����ȡ������ͬ��ȥ�뷽��Ҳ������ͬ����˲�����extract�����ȥ�빤����
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
//�ٶȹ���������
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
	cv ::Size change = cv::Size(orig.width / 6, orig.height / 6);//����Ϊԭʼ�ߴ��1/6

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
	cv ::Size change = cv::Size(orig.width / changeRatio, orig.height / changeRatio);//����Ϊԭʼ�ߴ��

	BackgroundSubtractorMOG2 bgSub = cv::BackgroundSubtractorMOG2(100,16,false);
	bgSub.nmixtures = 5;
	
	Mat original;
	Mat foreground;
	Mat frame;
	int frameNum = 0;
	while(cap.read(frame))
	{
		//imshow("original",original);
		cv::resize(frame,original,change);//�ߴ�Ϊԭʼͼ��ߴ�� 1/6,�ǵý���ͬ�������㣬������������̫���ˣ���û�и���ͨ�õķ�����
		imshow("frame",original);
		frameNum++;//֡��Ŵ�1��ʼ����ʵ��֡��ȡ�Ǵ�0��ʼ������Ժ���ܻ�����⣬Ҫ����һ��
		bgSub(original,foreground);
		imshow("foregroundBefore",foreground);
		cv::medianBlur(foreground,foreground,5);//��ǰ��ͼ����м򵥹��ˣ�
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

//��ϸ�˹���ģ�ͼ�֡���
//ϣ���ܹ������˹���ģ�ͶԹ������е�ȱ�㣬���õ��㹻�˶�Ŀ����Ϣ
void VideoLoader::MixGMM2andFrameDiff(std::string &filename)
{
	std::cout<<"mix GMM and three frame diff improve"<<std::endl;
	if(!cap.isOpened())
		return;
	Mat frame;//������ʱ�洢��Ƶ��ȡÿһ֡

	Mat img_src1,img_src2,img_src3;//��֡����Ҫ3֡ͼƬ
	Mat gray1,gray2,gray3;
	Mat gray_diff1,gray_diff2;//�洢2�������ͼƬ
	Mat gray;//������ʾǰ����
	Mat foreground;//��˹���ģ�͵õ�ǰ��ͼ
	Mat img_dst;

	cv::Size orig = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	const int changeRatio = 6;
	cv ::Size change = cv::Size(orig.width / changeRatio, orig.height / changeRatio);//����Ϊԭʼ�ߴ��

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

		subtract(gray2,gray1,gray_diff1);//�ڶ�֡����һ֡
		subtract(gray3,gray2,gray_diff2);//����֡���ڶ�֡

		for(int i=0;i<gray_diff1.rows;i++)
		{
			uchar* ptr1 = gray_diff1.ptr<uchar>(i);
			uchar* ptr2 = gray_diff2.ptr<uchar>(i);

			for(int j=0;j<gray_diff1.cols;j++)
			{
				if(abs(ptr1[j]) >= threshold_diff)//��һ�������ֵ����
					ptr1[j] = 255;
				else
					ptr1[j] = 0;

				if(abs(ptr2[j]) >= threshold_diff)//�ڶ��������ֵ����
					ptr2[j] = 255;
				else
					ptr2[j] = 0;

			}
		}
		bitwise_xor(gray_diff1,gray_diff2,gray);
		//���������ǰ��ͼ��ȥ�빤�������ݵ�extract�е�ǰ��ͼ�Ǹ���Ըɾ��ģ�
		//�������ǰ����ȡ������ͬ��ȥ�뷽��Ҳ������ͬ����˲�����extract�����ȥ�빤����
		//imshow("foregroundByFrameDiff",gray);
		
		bgSub(img_src3,foreground);
		//imshow("foregroundByGMM",foreground);

		bitwise_and(gray,foreground,img_dst);
		//imshow("foregroundByMix",img_dst);


		cv::medianBlur(img_dst,img_dst,3);
		cv::dilate(img_dst,img_dst,element);
		//cv::GaussianBlur(gray,gray,cv::Size(3,3),0);
		//��ǰ�ܹ����˶�Ŀ���⵽��ͬʱ�����������Ӱ�죬���������⣬���ܹ�������Ŀ��������⣬ֻ�ܹ���ⲿ�֣������Ǻ���һ���֣�
		//�ڵ�ǰ�����£�GMM�ܹ����˶�Ŀ���Ϊ�����ļ�⣬������������Ƭ������ν����߽��������
		//��򵥵�˼·�������ܹ���Ӱ���ֱ֡���޳���ʣ�ಿ�֣����GMM���ݽ��м�⣬���ܹ������ڵ�ǰ��Ƶ��Ӧ�����������ԣ�

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