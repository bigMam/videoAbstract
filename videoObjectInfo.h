//��ɵ�֡���˶�Ŀ����Ϣ����ȡ
//������Ƶ���˶�Ŀ����Ϣ�ı��桢��ȡ����
#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class VideoObjectInfo
{
public:
	//��������֡�ṹ�壬���ڱ��浥֡�������˶�Ŀ����Ϣ
	typedef struct _frameInfo
	{
		int frameNum;//֡���
		int moveObjectCount;//��ǰ֡�а����˶�Ŀ���ܸ���
		std::vector<cv::Rect> objectList;//���ڴ洢��ǰ֡�������˶�Ŀ����Ϣ
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
		void addObjectInfo(cv::Rect rect)//��֡�����һ���µ��˶�Ŀ����Ϣ
		{
			objectList.push_back(rect);
			moveObjectCount++;
		}
	}FrameInfo,*FrameInfoPtr;
	//�ɻ��һ�㣬��̬������ڴ�ռ���ʲôʱ������ͷţ����������Ľṹ����ڴ��Ҫ���������ģ�������ⲻ�󣬻��ǿ�����ô�ͷ��ڴ�Ƚ���Ҫ
	//��Ҫһ��дһ�Ѵ��룬Ҫ�ܹ����н׶β��Բź�
private:
	int frameCount;//��ǰ��Ƶ��֡�ܸ���
	int dynamicFrameCount;//��ǰ��Ƶ�о�̬֡����
	int staticFrameCount;//��ǰ��Ƶ�ж�̬֡������ͨ�����ߣ��ж���ǰ��Ƶ������
	std::vector<FrameInfoPtr> frameList;//�������֡��Ϣ
	std::string videoName;
	std::string startTime;

public:
	VideoObjectInfo();
	VideoObjectInfo(std::string &videoName);
	std::string getVideoTime(std::string & filename);//�����ļ�����·����Ϣ��������ȡ��Ƶ��ʼʱ����Ϣ

	void extractObjectInfo(cv::Mat &foreground,int frameID);//��ȡ��ǰ������ǰ��ͼ�е�ǰ����Ϣ
	void saveObjectInfo(std::string &filename);//����Ϣ���浽ϵͳ�ļ���
	void readObjectInfo(std::string &filename);//��ϵͳ�ļ��ж�ȡ��Ϣ��������Ƶ�ķ��࣬���ʱ�Ƚ����ǵ���������ӽ���
	int analyzeVideoType();//���ݾ�̬֡�붯̬֡�ı������ж���ǰ��Ƶ�ĳ���������ϡ��0������1������

	std::vector<FrameInfoPtr> getFrameList();
	int getDynamicFrameCount();
	std::string getStartTime();
};