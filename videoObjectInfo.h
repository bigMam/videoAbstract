//完成单帧中运动目标信息的提取
//整段视频中运动目标信息的保存、读取功能
#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class VideoObjectInfo
{
public:
	//继续定义帧结构体，用于保存单帧中所有运动目标信息
	typedef struct _frameInfo
	{
		int frameNum;//帧编号
		int moveObjectCount;//当前帧中包含运动目标总个数
		std::vector<cv::Rect> objectList;//用于存储当前帧中所有运动目标信息
		_frameInfo()
		{
			frameNum = -1; 
			moveObjectCount = 0;
		}
		_frameInfo(int num)
		{
			frameNum = num;
			moveObjectCount = 0;
		}
		void addObjectInfo(cv::Rect rect)//向帧中添加一个新的运动目标信息
		{
			objectList.push_back(rect);
			moveObjectCount++;
		}
	}FrameInfo,*FrameInfoPtr;
	//疑惑的一点，动态申请的内存空间在什么时间进行释放，定义这样的结构体对内存的要求是怎样的？这个问题不大，还是考虑怎么释放内存比较重要
	//不要一次写一堆代码，要能够进行阶段测试才好
private:
	int frameCount;//当前视频中帧总个数
	int dynamicFrameCount;//当前视频中静态帧个数
	int staticFrameCount;//当前视频中动态帧个数，通过三者，判定当前视频的类型
	std::vector<FrameInfoPtr> frameList;//存放所有帧信息
	std::string videoName;
	std::string startTime;

public:
	VideoObjectInfo();
	VideoObjectInfo(std::string &videoName);
	std::string getVideoTime(std::string & filename);//根据文件绝对路径信息，分析提取视频起始时间信息

	void extractObjectInfo(cv::Mat &foreground,int frameID);//提取当前给出的前景图中的前景信息
	void saveObjectInfo(std::string &filename);//将信息保存到系统文件中
	void readObjectInfo(std::string &filename);//从系统文件中读取信息，用于视频的分类，设计时先将考虑到的内容添加进来
	int analyzeVideoType();//根据静态帧与动态帧的比例，判定当前视频的场景所属，稀疏0，规律1，稠密

	std::vector<FrameInfoPtr> getFrameList();
	int getDynamicFrameCount();
	std::string getStartTime();
};