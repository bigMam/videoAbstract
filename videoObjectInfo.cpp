#include "videoObjectInfo.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <opencv2\highgui\highgui.hpp>

//VideoObjectInfo
//暂时先放在这里，看看是如何嵌入到其他的代码中的吧
VideoObjectInfo::VideoObjectInfo()
{
	frameCount = 0;
	staticFrameCount = 0;
	dynamicFrameCount = 0;
}

VideoObjectInfo::VideoObjectInfo(std::string &filename)
{
	frameCount = 0;
	staticFrameCount = 0;
	dynamicFrameCount = 0;
	videoName = filename;
	startTime = getVideoTime(filename);
}
//根据filename得到视频时间信息，用于信息标记/0481A3_10.0.0.6_1_20140902094142.avi

//这里仅仅针对当前视频命名有效，其他的视频命名方式敬爱那个无法得到时间信息
std::string VideoObjectInfo::getVideoTime(std::string & filename)
{
	int pos = filename.find_last_of('/');
	std::string videoname = filename.substr(pos + 1);
	pos = videoname.find_last_of('_');
	int pos2 = videoname.find_last_of('.');
	std::string videoTime = videoname.substr(pos + 1,pos2 - pos - 1);
	std::cout<<videoTime<<std::endl;
	return videoTime;
}

//提取当前给出的前景图中的前景信息
//在这里将会对小碎片进行融合，
//对GMM产生碎片进行整理
void VideoObjectInfo::extractObjectInfo(cv::Mat &foreground,int frameID)
{
	imshow("foregroundByGMM",foreground);
	std::cout<<"extract object info()"<<std::endl;
	//利用轮廓提取进行前景目标提取
	FrameInfoPtr frameInfo = new FrameInfo(frameID);

	std::vector<std::vector<cv::Point> > contours;
	findContours(foreground, 
		contours, // a vector of contours 
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_SIMPLE); // retrieve all pixels of each contours
	// Print contours' length轮廓的个数
	std::cout << "Contours: " << contours.size() << std::endl;


	cv::Mat result(foreground.size(),CV_8U,cv::Scalar(0));
	drawContours(result,contours,      //画出轮廓
		-1, // draw all contours
		cv::Scalar(255), // in black
		1); // with a thickness of 2

	std::vector<std::vector<cv::Point> >::const_iterator itContours = contours.begin();
	for ( ; itContours!=contours.end(); ++itContours) {

		std::cout << "Size: " << itContours->size() << std::endl;//每个轮廓包含的点数
		double area = cv::contourArea(*itContours);
		std::cout<<"contours area:"<<area<<std::endl;
		if(area > 50)//面积过小的直接屏蔽到，不需要考虑进来
		{
			cv::Rect rect = cv::boundingRect(*itContours);
			cv::rectangle(result,rect,cv::Scalar(255));
			//讲当前帧的运行目标信息保存起来，最终存到文件系统中
			frameInfo->addObjectInfo(rect);
		}
	}
	if(frameInfo->moveObjectCount == 0)
	{
		staticFrameCount++;
		//疑问：静态帧需不需要进行存储，或者是不是只需要知道帧编号就可以了，暂且放在这里
	}
	else
	{
		dynamicFrameCount++;
		frameList.push_back(frameInfo);
	}
	frameCount++;
	cv::imshow("Contours",result);
}

//将信息保存到系统文件中
void VideoObjectInfo::saveObjectInfo(std::string &filename)
{	
	cv::FileStorage fs(filename,cv::FileStorage::WRITE);
	fs<<"videoName"<<"gate_5_01";
	fs<<"frameCount"<<frameCount;
	fs<<"staticFrameCount"<<staticFrameCount;
	fs<<"dynamicFrameCount"<<dynamicFrameCount;
	fs<<"frameList"<<"[";
	std::vector<FrameInfoPtr>::iterator iter = frameList.begin();
	while(iter != frameList.end())//遍历frameList列表，并保存帧内信息
	{
		fs<<"{:"<<"frameNum"<<(*iter)->frameNum<<"moveObjectCount"<<(*iter)->moveObjectCount
			<<"objectList"<<"[:";
		std::vector<cv::Rect>::iterator objectIter = (*iter)->objectList.begin();
		while(objectIter != (*iter)->objectList.end())
		{
			fs<<(*objectIter);
			objectIter++;
		}
		fs<<"]"<<"}";
		iter++;
	}
	fs<<"]";
	fs.release();
	//保存之后是不是需要对原有内存进行释放呢，还是等到析构中进行？
}

//从系统文件中读取信息，用于视频的分类，
void VideoObjectInfo::readObjectInfo(std::string &filename)
{
	//已知文件固定格式，如何得到指定视频的响应信息
	cv::FileStorage fs(filename,cv::FileStorage::READ);
	if(!fs.isOpened())//如果当前指定文件未打开，说明不存在，需要重新遍历获取,这个地方安排不合理，本身是不允许这种事情发生的
		//先将其屏蔽掉
	{
		//VideoLoader loader;
		//loader.loadVideo(filename);
		//loader.GMM2();
		//fs.open(filename,cv::FileStorage::READ);
		return;
	}
	videoName = fs["videoName"];
	frameCount = fs["frameCount"];
	staticFrameCount = fs["staticFrameCount"];
	dynamicFrameCount = fs["dynamicFrameCount"];
	cv::FileNode frameListNode = fs["frameList"];//读取运动目标信息
	cv::FileNodeIterator iter = frameListNode.begin(), it_end = frameListNode.end();
	cv::vector<cv::Rect> objectList;
	for(; iter != it_end; ++iter)
	{
		FrameInfoPtr frameInfo = new FrameInfo((int)(*iter)["frameNum"]);
		frameInfo->moveObjectCount = (int)(*iter)["moveObjectCount"];
		cv::FileNode objectList = (*iter)["objectList"];
		cv::FileNodeIterator objectIter = objectList.begin(),objectIter_end = objectList.end();
		for(; objectIter != objectIter_end; ++objectIter)
		{
			cv::Rect rect((int)(*objectIter)[0],(int)(*objectIter)[1],(int)(*objectIter)[2],(int)(*objectIter)[3]);
			frameInfo->objectList.push_back(rect);
		}
		frameList.push_back(frameInfo);
	}
	fs.release();
}

//根据静态帧与动态帧的比例，判定当前视频的场景所属，稀疏，规律，稠密
int VideoObjectInfo::analyzeVideoType()
{
	//这里首先是根据静态帧与动态帧的比率进行判断，
	//之后还可以进一步根据动态帧的分布情况进行更为精细的划分
	float ratio = (float)dynamicFrameCount / frameCount;
	if(ratio < 0.3)
		return 0;
	else 
		return 3;
}

std::vector<VideoObjectInfo::FrameInfoPtr> VideoObjectInfo::getFrameList()
{
	return frameList;
}

int VideoObjectInfo::getDynamicFrameCount()
{
	return dynamicFrameCount;
}

std::string VideoObjectInfo::getStartTime()
{
	return startTime;
}
