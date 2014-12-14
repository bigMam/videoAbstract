//������Ƶ���������أ�ROIѡ��ǰ�����
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include "videoObjectInfo.h"
#define threshold_diff 10 //���ü�֡���ֵ
#define threshold_diff1 10 //���ü�֡���ֵ
#define threshold_diff2 10 //���ü�֡���ֵ

class VideoLoader
{
	cv::Rect roi;//Ԥ���趨����Ƶ�������Ĭ��Ϊ������Ƶ��С
	cv::VideoCapture cap;
	VideoObjectInfo videoObject;

public:
	//������Ƶ����Ҫ��Ҫ����������? 
	VideoLoader(std::string& filename);
	bool loadVideo(std::string &filename);
	void setSelectROI(cv::Rect rect);//�趨ѡ���������
	void threeFrameDiff();//��֡�,����
	void threeFrameDiffImprove(std::string &filename);//��֡��Ľ��㷨

	void GMM();//��˹���ģ�ͽ��б�������������
	void GMM2(std::string &filename);//��˹���ģ��2

	void MixGMM2andFrameDiff(std::string &filename);

};