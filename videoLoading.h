//负责视频的搜索加载，ROI选择及前景检测
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include "videoObjectInfo.h"
#define threshold_diff 10 //设置简单帧差法阈值
#define threshold_diff1 10 //设置简单帧差法阈值
#define threshold_diff2 10 //设置简单帧差法阈值

class VideoLoader
{
	cv::Rect roi;//预先设定的视频检测区域，默认为加载视频大小
	cv::VideoCapture cap;
	VideoObjectInfo videoObject;

public:
	//搜索视频函数要不要放在这里呢? 
	VideoLoader(std::string& filename);
	bool loadVideo(std::string &filename);
	void setSelectROI(cv::Rect rect);//设定选择矩形区域
	void threeFrameDiff();//三帧差法,弃用
	void threeFrameDiffImprove(std::string &filename);//三帧差法改进算法

	void GMM();//高斯混合模型进行背景剪除，弃用
	void GMM2(std::string &filename);//高斯混合模型2

	void MixGMM2andFrameDiff(std::string &filename);

};