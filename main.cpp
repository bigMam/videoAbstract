#include "highgui.h"
#include "videoLoading.h"
#include "videoComposition.h"
#include <iostream>
#include <opencv2\highgui\highgui.hpp>
void main()
{ 
	//int count = 5;
	////根据分组情况创建若干个videoCapture，用于从指定位置开始视频读取
	//cv::VideoCapture* capList = new cv::VideoCapture[count];
	//
	//int *shift = new int[count];//记录当前分组帧的偏移量，相对位置
	//int startPos[5] = {1,361,721,1001,1445};//实验如何快速到达指定起始帧位置，尝试将set、get与遍历结合使用

	//for(int i = 0; i < count; ++i)
	//{
	//	std::cout<<startPos[i]<<" ";
	//	shift[i] = 0;
	//}
	//std::cout<<std::endl;
	////对每个片段进行循环读取直至到达指定起始帧位置
	////set关键帧是以250为间隔，[1,250]->250;[251,500]->500;...
	//cv::Mat frame;
	//for(int i = 0; i < count; ++i)
	//{
	//	if (!capList[i].open(originalVideo)) return;

	//	if((startPos[i] - 1) % 250 == 0)//当为250倍数时可以直接设定
	//		capList[i].set(CV_CAP_PROP_POS_FRAMES,startPos[i] - 1);
	//	else if((startPos[i] - 1) - 250 > 0)
	//		capList[i].set(CV_CAP_PROP_POS_FRAMES,(startPos[i] - 1) - 250);
	//	else
	//		capList[i].set(CV_CAP_PROP_POS_FRAMES,0);

	//	if(!capList[i].read(frame)) return;
	//	int nextFrame = capList[i].get(CV_CAP_PROP_POS_FRAMES);
	//	std::cout<<nextFrame<<" ";

	//	while(nextFrame != (startPos[i]-1) && capList[i].read(frame))
	//	{
	//		nextFrame++;
	//	}
	//	std::cout<<nextFrame<<" "<<std::endl;;
	//}//循环结束，每个cap均指向指定开始帧位置
	//std::cout<<std::endl;
	//for(int i = 0; i < count; ++i)
	//{
	//	std::cout<<capList[i].get(CV_CAP_PROP_POS_FRAMES) + 1<<" ";
	//}
	//std::cout<<std::endl;


	//加载并分析视频


	//std::string videoname = "0481A3_10.0.0.6_1_20140902094142";
	std::string videoname = "0481A3_10.0.0.6_1_20140902094401";
	//std::string videoname = "0481A3_10.0.0.6_1_20140902094612";
	//std::string videoname = "gate_5_01_30-03_30";
	std::string originalVideo = "D:/ImageDataSets/videoAbstract/"+ videoname + ".avi";
	std::string objectInfoFile = "D:/ImageDataSets/videoAbstract/"+ videoname + ".yml";
	std::string videoComposition = "D:/ImageDataSets/videoAbstract/composition/"+ videoname + ".avi";

	VideoLoader loader(originalVideo);
	//loader.threeFrameDiffImprove(objectInfoFile);
	//loader.GMM2(objectInfoFile);
	loader.MixGMM2andFrameDiff(objectInfoFile);

	//根据分析文件信息及原始视频，合成新的压缩视频
	//VideoCompositor compositor(originalVideo);
	//compositor.analysisVideo(objectInfoFile);	
	//compositor.videoCompoistionSpare(videoComposition);
	//compositor.videoCompositionDense(videoComposition,0.2);

	system("pause");
    return;
}