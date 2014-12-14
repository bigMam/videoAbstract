#include "videoObjectInfo.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <opencv2\highgui\highgui.hpp>

//VideoObjectInfo
//��ʱ�ȷ���������������Ƕ�뵽�����Ĵ����еİ�
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
//����filename�õ���Ƶʱ����Ϣ��������Ϣ���/0481A3_10.0.0.6_1_20140902094142.avi

//���������Ե�ǰ��Ƶ������Ч����������Ƶ������ʽ�����Ǹ��޷��õ�ʱ����Ϣ
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

//��ȡ��ǰ������ǰ��ͼ�е�ǰ����Ϣ
//�����ｫ���С��Ƭ�����ںϣ�
//��GMM������Ƭ��������
void VideoObjectInfo::extractObjectInfo(cv::Mat &foreground,int frameID)
{
	imshow("foregroundByGMM",foreground);
	std::cout<<"extract object info()"<<std::endl;
	//����������ȡ����ǰ��Ŀ����ȡ
	FrameInfoPtr frameInfo = new FrameInfo(frameID);

	std::vector<std::vector<cv::Point> > contours;
	findContours(foreground, 
		contours, // a vector of contours 
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_SIMPLE); // retrieve all pixels of each contours
	// Print contours' length�����ĸ���
	std::cout << "Contours: " << contours.size() << std::endl;


	cv::Mat result(foreground.size(),CV_8U,cv::Scalar(0));
	drawContours(result,contours,      //��������
		-1, // draw all contours
		cv::Scalar(255), // in black
		1); // with a thickness of 2

	std::vector<std::vector<cv::Point> >::const_iterator itContours = contours.begin();
	for ( ; itContours!=contours.end(); ++itContours) {

		std::cout << "Size: " << itContours->size() << std::endl;//ÿ�����������ĵ���
		double area = cv::contourArea(*itContours);
		std::cout<<"contours area:"<<area<<std::endl;
		if(area > 50)//�����С��ֱ�����ε�������Ҫ���ǽ���
		{
			cv::Rect rect = cv::boundingRect(*itContours);
			cv::rectangle(result,rect,cv::Scalar(255));
			//����ǰ֡������Ŀ����Ϣ�������������մ浽�ļ�ϵͳ��
			frameInfo->addObjectInfo(rect);
		}
	}
	if(frameInfo->moveObjectCount == 0)
	{
		staticFrameCount++;
		//���ʣ���̬֡�費��Ҫ���д洢�������ǲ���ֻ��Ҫ֪��֡��žͿ����ˣ����ҷ�������
	}
	else
	{
		dynamicFrameCount++;
		frameList.push_back(frameInfo);
	}
	frameCount++;
	cv::imshow("Contours",result);
}

//����Ϣ���浽ϵͳ�ļ���
void VideoObjectInfo::saveObjectInfo(std::string &filename)
{	
	cv::FileStorage fs(filename,cv::FileStorage::WRITE);
	fs<<"videoName"<<"gate_5_01";
	fs<<"frameCount"<<frameCount;
	fs<<"staticFrameCount"<<staticFrameCount;
	fs<<"dynamicFrameCount"<<dynamicFrameCount;
	fs<<"frameList"<<"[";
	std::vector<FrameInfoPtr>::iterator iter = frameList.begin();
	while(iter != frameList.end())//����frameList�б�������֡����Ϣ
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
	//����֮���ǲ�����Ҫ��ԭ���ڴ�����ͷ��أ����ǵȵ������н��У�
}

//��ϵͳ�ļ��ж�ȡ��Ϣ��������Ƶ�ķ��࣬
void VideoObjectInfo::readObjectInfo(std::string &filename)
{
	//��֪�ļ��̶���ʽ����εõ�ָ����Ƶ����Ӧ��Ϣ
	cv::FileStorage fs(filename,cv::FileStorage::READ);
	if(!fs.isOpened())//�����ǰָ���ļ�δ�򿪣�˵�������ڣ���Ҫ���±�����ȡ,����ط����Ų����������ǲ������������鷢����
		//�Ƚ������ε�
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
	cv::FileNode frameListNode = fs["frameList"];//��ȡ�˶�Ŀ����Ϣ
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

//���ݾ�̬֡�붯̬֡�ı������ж���ǰ��Ƶ�ĳ���������ϡ�裬���ɣ�����
int VideoObjectInfo::analyzeVideoType()
{
	//���������Ǹ��ݾ�̬֡�붯̬֡�ı��ʽ����жϣ�
	//֮�󻹿��Խ�һ�����ݶ�̬֡�ķֲ�������и�Ϊ��ϸ�Ļ���
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
