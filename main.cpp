#include "highgui.h"
#include "videoLoading.h"
#include "videoComposition.h"
#include <iostream>
#include <opencv2\highgui\highgui.hpp>
void main()
{ 
	//int count = 5;
	////���ݷ�������������ɸ�videoCapture�����ڴ�ָ��λ�ÿ�ʼ��Ƶ��ȡ
	//cv::VideoCapture* capList = new cv::VideoCapture[count];
	//
	//int *shift = new int[count];//��¼��ǰ����֡��ƫ���������λ��
	//int startPos[5] = {1,361,721,1001,1445};//ʵ����ο��ٵ���ָ����ʼ֡λ�ã����Խ�set��get��������ʹ��

	//for(int i = 0; i < count; ++i)
	//{
	//	std::cout<<startPos[i]<<" ";
	//	shift[i] = 0;
	//}
	//std::cout<<std::endl;
	////��ÿ��Ƭ�ν���ѭ����ȡֱ������ָ����ʼ֡λ��
	////set�ؼ�֡����250Ϊ�����[1,250]->250;[251,500]->500;...
	//cv::Mat frame;
	//for(int i = 0; i < count; ++i)
	//{
	//	if (!capList[i].open(originalVideo)) return;

	//	if((startPos[i] - 1) % 250 == 0)//��Ϊ250����ʱ����ֱ���趨
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
	//}//ѭ��������ÿ��cap��ָ��ָ����ʼ֡λ��
	//std::cout<<std::endl;
	//for(int i = 0; i < count; ++i)
	//{
	//	std::cout<<capList[i].get(CV_CAP_PROP_POS_FRAMES) + 1<<" ";
	//}
	//std::cout<<std::endl;


	//���ز�������Ƶ


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

	//���ݷ����ļ���Ϣ��ԭʼ��Ƶ���ϳ��µ�ѹ����Ƶ
	//VideoCompositor compositor(originalVideo);
	//compositor.analysisVideo(objectInfoFile);	
	//compositor.videoCompoistionSpare(videoComposition);
	//compositor.videoCompositionDense(videoComposition,0.2);

	system("pause");
    return;
}