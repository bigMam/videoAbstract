#include "videoComposition.h"
#include <opencv.hpp>
#include "videoLoading.h"
VideoCompositor::VideoCompositor()
{
	videoObject = VideoObjectInfo();
}
VideoCompositor::VideoCompositor(std::string& videoname)
{
	originalVideo = videoname;
	videoObject = VideoObjectInfo(videoname);//�õ���Ƶ��ʼʱ��ڵ���Ϣ
}
void VideoCompositor::analysisVideo(std::string &objectInfoFile)
{
	videoObject.readObjectInfo(objectInfoFile);//�ɹ�����ļ��Ķ�ȡ���̣�֮������ν����ļ��ĺϳɹ�����
	int type = videoObject.analyzeVideoType();
	//std::string videoComposition = "D:/ImageDataSets/videoAbstract/videoComposition.avi";
	if(type == 1)
	{

	}
	else if(type == 2)
	{

	}
	else if(type == 3)
	{

	}
}
//��ʼʱ���ʽ2014 0902 094142
std::string VideoCompositor::getShowTime(std::string &startTime,int shiftTime)
{
	//������ʼʱ��͵�ǰƫ��ʱ��õ���ǰ֡ʱ��������ڶ��˶�Ŀ����б�ʾ
	//��Ҫ��string����ת��Ϊint���ͣ�֮���ٽ��м�����ǣ�
	int hour = std::stoi(startTime.substr(8,2));
	int min = std::stoi(startTime.substr(10,2));
	int sec = std::stoi(startTime.substr(12,2));
	sec =sec + shiftTime;
	if(sec > 59)
	{
		int min_add = sec / 60;
		sec = sec % 60;
		min = min + min_add;
		if(min > 59)
		{
			int hour_add = min / 60;
			min = min % 60;
			hour = hour + hour_add;//�������ڵ������Ժ���֮����˵��
		}
	}

	std::ostringstream oss;
	if(hour < 10)
		oss<<'0';
	oss<<hour<<':';
	if(min < 10)
		oss<<'0';
	oss<<min<<':';
	if(sec < 10)
		oss<<'0';
	oss<<sec;

	std::string showTime = "";
	showTime = showTime + oss.str();
	return showTime;

}



//��ʵ����򵥵��޳���̬֡��������̬֡���ϳ���Ƶ
void VideoCompositor::videoCompoistionSpare(std::string &targetVideo)
{
	//���ݵ�ǰ��Ƶ�е�֡�˶���Ϣ���ݶ���Ƶ���кϳɣ�ͬʱ�鿴ԭʼ����Ƶ�ļ��Ƿ���Ҫ����µ���Ϣ��
	cv::VideoCapture cap;
	if(!cap.open(originalVideo))
		return;
	int ex=static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //������ʲô 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	double fps = cap.get(CV_CAP_PROP_FPS);//֡Ƶ������ÿ�����֡����
	cv::VideoWriter cap_write;
	cap_write.open(targetVideo,ex, cap.get(CV_CAP_PROP_FPS),S, true); //��д���ļ�����ָ����ʽ

	//���ڱ���ĸ�ʽ���⣬��ʱ��������Ƚ��ܹ�ʵ�ֵ�����ʵ�ֳ���
	std::cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height  
       << " of nr#: " << cap.get(CV_CAP_PROP_FRAME_COUNT) 
	   <<"fps:"<<cap.get(CV_CAP_PROP_FPS)<< std::endl;  
    std::cout << "Input codec type: " << EXT << std::endl; 

	std::vector<VideoObjectInfo::FrameInfoPtr> frameList = videoObject.getFrameList();
	std::vector<VideoObjectInfo::FrameInfoPtr>::iterator iter = frameList.begin();
	cv::Mat frame;
	bool mustBeStopped = false;
	int curFrameNum = 0;
	while(!mustBeStopped)
	{
		if(!cap.read(frame))
		{
			std::cout<<"��ȡ��Ƶʧ��"<<std::endl;
			return;	
		}
		curFrameNum++;
		int shiftTime =std::floor(curFrameNum / fps) + 1;
		int frameNum = (*iter)->frameNum;
		if(frameNum == curFrameNum)//��ǰ��ȡ֡Ϊ��̬֡������ǰ֡��ŵ��ϳ���Ƶ��
		{
			//�Ե�ǰ֡�е��˶���Ϣ����
			std::string showTime = getShowTime(videoObject.getStartTime(),shiftTime);
			int count = (*iter)->objectList.size();//��ǰ֡������Ŀ�����
			for(int i = 0; i < count; ++i)//Ϊÿ���˶�Ŀ���趨��ʾ��Ϣ
			{

				cv::Point point((*iter)->objectList[i].x * 6 ,(*iter)->objectList[i].y * 6);//��������λ������
				cv::Rect rect = cv::Rect((*iter)->objectList[i].x * 6 ,(*iter)->objectList[i].y * 6,
					(*iter)->objectList[i].width * 6,(*iter)->objectList[i].height * 6);//�˶�Ŀ����Ӿ�������
				//������Ҫ���ǵ�һ�㣬��������ֳ����߽罫��ν��д�����Ҫ�������뵽��Ƶ�����ڲ����ǣ�Ҳ�����޸�����λ������
				//int baseLine = 0;
				//cv::getTextSize(showTime,CV_FONT_HERSHEY_COMPLEX,1,2,&baseLine);

				putText( frame, showTime ,point,CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0,255,0),2);
				cv::rectangle(frame,rect,cv::Scalar(0,255,0),1);
			}

			cap_write<<frame;
			iter++;
		}
		if(iter == frameList.end())
		{
			mustBeStopped = true;
		}
	}
	cap.release();

}
//��Ϊ��ֱ�ӽ�����֡ѹ����ͬһ��ʱ��Ƭ�����棬��Ҫ���ǵ�������ܹ�������ײ���
//���opencv���ܶ�ȡָ���ؼ�֡���⣬���ǲ��ñ����ķ�����������Ƶ���֣�
//�������ɸ�videoCapture����ָ��λ�ÿ�ʼ��ȡ������ȡ��������������֡���кϲ���ֱ����ȡ����
void VideoCompositor::videoCompositionDense(std::string &targetVideo,float rate)
{
	cv::VideoCapture cap;//ԭʼ��Ƶ
	if(!cap.open(originalVideo)) return;

	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //������ʲô 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	cv::VideoWriter cap_write;//д����Ƶ
	cap_write.open(targetVideo , ex, cap.get(CV_CAP_PROP_FPS),S, true); //��д���ļ�����ָ����ʽ

	std::vector<VideoObjectInfo::FrameInfoPtr> frameList = videoObject.getFrameList();//�˶�֡��Ϣ�б�

	//�����Ƶ������Ϣ[[****][****][****][****][**]]//interval = 4��Count= 5��
	int dynamicFrameCount = videoObject.getDynamicFrameCount();
	int interval = std::ceil(dynamicFrameCount * rate);//�ֶμ����С��ÿ������������̬֡����
	int count = std::ceil((float)dynamicFrameCount/interval);//�ܹ��������

	//���ݷ�������������ɸ�videoCapture�����ڴ�ָ��λ�ÿ�ʼ��Ƶ��ȡ
	cv::VideoCapture* capList = new cv::VideoCapture[count];
	int *startPos = new int[count];//��¼ÿ����Ƶ����ʼ�����Ϣ������֮�����ƫ�����õ�����֡��Ϣ,
	int *shift = new int[count];//��¼��ǰ����֡��ƫ���������λ��

	for(int i = 0; i < count; ++i)
		shift[i] = 0;
	
	/////����,������ȡ��ֱ������ָ����ʼ֡λ��֡
	for(int i = 0; i < count; ++i)
	{
		startPos[i] = frameList[i * interval]->frameNum;//���ÿ��Ƭ����ʼ֡λ��
		std::cout<<startPos[i]<<" ";
	}
	std::cout<<std::endl;
	//��ÿ��Ƭ�ν���ѭ����ȡֱ������ָ����ʼ֡λ��
	//set�ؼ�֡����250Ϊ�����[1,250]->250;[251,500]->500;...
	cv::Mat frame;
	for(int i = 0; i < count; ++i)
	{
		if (!capList[i].open(originalVideo)) return;

		if((startPos[i] - 1) % 250 == 0)//��Ϊ250����ʱ����ֱ���趨
			capList[i].set(CV_CAP_PROP_POS_FRAMES,startPos[i] - 1);
		else if((startPos[i] - 1) - 250 > 0)//��ָ����ʼ֡����250ʱ��
			capList[i].set(CV_CAP_PROP_POS_FRAMES,(startPos[i] - 1) - 250);
		else//С��250ʱ��ֻ���趨Ϊ0��֮���ٽ��б�������
			capList[i].set(CV_CAP_PROP_POS_FRAMES,0);

		if(!capList[i].read(frame)) return;
		int nextFrame = capList[i].get(CV_CAP_PROP_POS_FRAMES);//���nextFrame���
		std::cout<<nextFrame<<" ";
		//���������жϲ���֮��ÿ��Ƭ������ȡ249֡����Ƚ�֮ǰ�Ĳ����������˱���ʱ��
		while(nextFrame < (startPos[i] - 1) && capList[i].read(frame))
		{
			nextFrame++;
		}
		std::cout<<nextFrame<<" "<<std::endl;
	}//ѭ��������ÿ��cap��ָ��ָ����ʼ֡λ��
	std::cout<<std::endl;
	for(int i = 0; i < count; ++i)
	{
		std::cout<<capList[i].get(CV_CAP_PROP_POS_FRAMES) + 1<<" ";
	}
	std::cout<<std::endl;

	VideoObjectInfo::FrameInfoPtr* frameArray = new VideoObjectInfo::FrameInfoPtr[count];//�����ã����ҷ�������
	//�������Ƶ���кϳ�
	cv::Mat compositionFrame;//����д��ϳ���Ƶ�еĺϳ�֡
	cv::Mat tmpFrame[5];//ͨ����ͬ����õ�����ʱ֡
	bool flag = false;
	for(int i = 0; i < interval; ++i)
	{
		std::vector<cv::Rect> rectContainer;//���ڴ�ŵ�ǰ�����������
		//�õ�һ����ϳɵ��˶�֡��Ϣ
		for(int k = 0; k < count; ++k)
		{
			if(k * interval + i < frameList.size())//���һ����Ŀ��������interval��
			{
				frameArray[k] = frameList[k*interval + i];
				if(!capList[k].read(tmpFrame[k]))
				{
					std::cout<<"��ȡ��Ƶʧ��"<<" "<<i<<std::endl;
					flag = true;
					continue;
				}
				shift[k]++;
				//��Ϊ�ؼ��ĵط����غ����ܲ�����ÿ�������еõ���Ӧ��֡
				while((startPos[k] + shift[k] - 1) != frameArray[k]->frameNum)//startPos + shift�õ���ǰ��Ƶ���
				{
					if(!capList[k].read(tmpFrame[k]))
					{
						std::cout<<"��ȡ��Ƶʧ��"<<" "<<i<<std::endl;
						flag = true;
						break;
					}
					else
					{
						shift[k]++;//ƫ������һ
					}
				}
				if(!flag)
				{
					if(k == 0)//��ʾ�ǵ�һ�飬ֱ�ӽ����ݸ��Ƹ�compositionFrame����
					{
						std::swap(compositionFrame,tmpFrame[k]);//���ﻹ��Ҫ���ǵ��Ǽ�¼�����˶�Ŀ����Ϣ
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							rectContainer.push_back(*iter);
							iter++;
						}//����ǰ֡��rect��ȫ���Ƶ�container��

					}
					else//��ǰ֡�˶�Ŀ�����������ۼӵ�compositionFrame�У�
					{
						//�õ������˶�Ŀ�꣬���ۼӵ���ʼ֡��
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							cv::Rect rect = *iter;//�õ�һ���˶�Ŀ�������Ϣ
							//������Ҫ�Ե�ǰ���rect������rect�����ж��Ƿ�����ص���������ص�����ν��д���
						    ///////////////////////////////////////
							
							//cv::Rect originalRect = cv::Rect(rect.x * 6, rect.y * 6, rect.width * 6,rect.height * 6);
							cv::Rect originalRect = cv::Rect(rect);
							cv::Mat rectImage = tmpFrame[k](originalRect);
							cv::Mat srcImage = compositionFrame(originalRect);

							if(haveOverlap(rectContainer,rect))
							{
								//������Ҫ������ײ��⣬����Ե�ǰ�Ѵ������ݺͼ���������ݽ��м�⣬����͸���ȴ���
								//ʹ�ö��Ŀ���ܹ�ͬʱ��ʾ
								cv::addWeighted(srcImage,0.5,rectImage,0.5,0.0,srcImage);
							}
							else
							{
								cv::addWeighted(srcImage,0.0,rectImage,1.0,0.0,srcImage);
							}
							rectContainer.push_back(*iter);
							iter++;
						}
					}
				}
				flag = false;
			}
		}
		cap_write<<compositionFrame;
	}
	//֮����Ҫ���ǵ����ݰ�������֡�е�ָ�����򿽱����ϳ�֡�У�
	//�����Ѻϳ�֡�е��˶�Ŀ����Ϣ�����¼���֡��֮ǰ���н�����ײ��⣬���������ײ��ʹ��Ȩ�ص��ӣ�����ֱ���ۼӼ���
	for(int i = 0; i < count; ++i)
	{
		capList[i].release();
	}
}

//�жϵ�ǰrect���������Ѵ���rect�Ƿ�����ص�������������ں����ĵ�����ʹ��Ȩ�ص���
bool VideoCompositor::haveOverlap(std::vector<cv::Rect> &container,cv::Rect rect)
{
	int x1 = rect.x,y1 = rect.y;
	int x2 = x1 + rect.width, y2 = y1 + rect.height;
	int area = rect.area();
	int x3,y3,x4,y4;
	//int minX,minY,maxX,maxY;
	int m,n;
	std::vector<cv::Rect>::iterator iter = container.begin();
	while(iter != container.end())
	{
		cv::Rect tmp = (*iter);
		x3 = tmp.x, y3 = tmp.y;
		x4 = x3 + tmp.width, y4 = y3 + tmp.height;
		m =  (x1>x4) | (x2<x3);
		n= (y2<y3) | (y1>y4);
		if(m | n)
		{
			//���ཻ
		}
		else
		{
			//�ཻ
			return true;
		}
		iter++;
	}
	return false;
}

void VideoCompositor::videoCompoistionSpareTest(std::string &targetVideo)
{
	//���ݵ�ǰ��Ƶ�е�֡�˶���Ϣ���ݶ���Ƶ���кϳɣ�ͬʱ�鿴ԭʼ����Ƶ�ļ��Ƿ���Ҫ����µ���Ϣ��
	cv::VideoCapture cap;//ԭʼ��Ƶ
	if(!cap.open(originalVideo))
		return;
	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //������ʲô 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	double fps = cap.get(CV_CAP_PROP_FPS);//֡Ƶ������ÿ�����֡����
	cv::VideoWriter cap_write;//д����Ƶ
	cap_write.open(targetVideo , ex, cap.get(CV_CAP_PROP_FPS),S, true); //��д���ļ�����ָ����ʽ

	//���ڱ���ĸ�ʽ���⣬��ʱ��������Ƚ��ܹ�ʵ�ֵ�����ʵ�ֳ���
	std::cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height  
       << " of nr#: " << cap.get(CV_CAP_PROP_FRAME_COUNT) 
	   <<"fps:"<<cap.get(CV_CAP_PROP_FPS)<< std::endl;  
    std::cout << "Input codec type: " << EXT << std::endl; 

	std::vector<VideoObjectInfo::FrameInfoPtr> frameList = videoObject.getFrameList();
	std::vector<VideoObjectInfo::FrameInfoPtr>::iterator iter = frameList.begin();
	cv::Mat frame;
	bool mustBeStopped = false;
	int curFrameNum = 0;
	while(!mustBeStopped)
	{
		if(!cap.read(frame))
		{
			std::cout<<"��ȡ��Ƶʧ��"<<std::endl;
			return;	
		}
		curFrameNum++;
		int shiftTime =std::floor(curFrameNum / fps) + 1;
		int frameNum = (*iter)->frameNum;
		if(frameNum == curFrameNum)//��ǰ��ȡ֡Ϊ��̬֡������ǰ֡��ŵ��ϳ���Ƶ��
		{
			int count = (*iter)->objectList.size();//��ǰ֡������Ŀ�����
			for(int i = 0; i < count; ++i)//Ϊÿ���˶�Ŀ���趨��ʾ��Ϣ
			{
				//�Ե�ǰ֡�е��˶���Ϣ����
				//cv::Point point((*iter)->objectList[i].x ,(*iter)->objectList[i].y);//��������λ������
				cv::Rect rect = cv::Rect((*iter)->objectList[i].x ,(*iter)->objectList[i].y,
					(*iter)->objectList[i].width,(*iter)->objectList[i].height);//�˶�Ŀ����Ӿ�������
				//������Ҫ���ǵ�һ�㣬��������ֳ����߽罫��ν��д�����Ҫ�������뵽��Ƶ�����ڲ����ǣ�Ҳ�����޸�����λ������
				//int baseLine = 0;
				//cv::getTextSize(showTime,CV_FONT_HERSHEY_COMPLEX,1,2,&baseLine);

				//putText( frame, showTime ,point,CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0,255,0),2);
				cv::rectangle(frame,rect,cv::Scalar(0,255,0),1);
			}
			cap_write<<frame;
			iter++;
		}
		if(iter == frameList.end())
		{
			mustBeStopped = true;
		}
	}
	cap.release();

}
void VideoCompositor::videoCompositionDenseTest(std::string &targetVideo,float rate)
{
	cv::VideoCapture cap;//ԭʼ��Ƶ
	if(!cap.open(originalVideo)) return;

	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //������ʲô 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	cv::VideoWriter cap_write;//д����Ƶ
	cap_write.open(targetVideo , ex, cap.get(CV_CAP_PROP_FPS),S, true); //��д���ļ�����ָ����ʽ

	std::vector<VideoObjectInfo::FrameInfoPtr> frameList = videoObject.getFrameList();//�˶�֡��Ϣ�б�

	//�����Ƶ������Ϣ[[****][****][****][****][**]]//interval = 4��Count= 5��
	int dynamicFrameCount = videoObject.getDynamicFrameCount();
	int interval = std::ceil(dynamicFrameCount * rate);//�ֶμ����С��ÿ������������̬֡����
	int count = std::ceil((float)dynamicFrameCount/interval);//�ܹ��������

	//���ݷ�������������ɸ�videoCapture�����ڴ�ָ��λ�ÿ�ʼ��Ƶ��ȡ
	cv::VideoCapture* capList = new cv::VideoCapture[count];
	int *startPos = new int[count];//��¼ÿ����Ƶ����ʼ�����Ϣ������֮�����ƫ�����õ�����֡��Ϣ,
	int *shift = new int[count];//��¼��ǰ����֡��ƫ���������λ��

	for(int i = 0; i < count; ++i)
		shift[i] = 0;
	
	/////����,������ȡ��ֱ������ָ����ʼ֡λ��֡
	for(int i = 0; i < count; ++i)
	{
		startPos[i] = frameList[i * interval]->frameNum;//���ÿ��Ƭ����ʼ֡λ��
		std::cout<<startPos[i]<<" ";
	}
	std::cout<<std::endl;
	//��ÿ��Ƭ�ν���ѭ����ȡֱ������ָ����ʼ֡λ��
	//set�ؼ�֡����250Ϊ�����[1,250]->250;[251,500]->500;...
	cv::Mat frame;
	for(int i = 0; i < count; ++i)
	{
		if (!capList[i].open(originalVideo)) return;

		if((startPos[i] - 1) % 250 == 0)//��Ϊ250����ʱ����ֱ���趨
			capList[i].set(CV_CAP_PROP_POS_FRAMES,startPos[i] - 1);
		else if((startPos[i] - 1) - 250 > 0)//��ָ����ʼ֡����250ʱ��
			capList[i].set(CV_CAP_PROP_POS_FRAMES,(startPos[i] - 1) - 250);
		else//С��250ʱ��ֻ���趨Ϊ0��֮���ٽ��б�������
			capList[i].set(CV_CAP_PROP_POS_FRAMES,0);

		if(!capList[i].read(frame)) return;
		int nextFrame = capList[i].get(CV_CAP_PROP_POS_FRAMES) - 1;//���nextFrame���
		std::cout<<nextFrame<<" ";
		//���������жϲ���֮��ÿ��Ƭ������ȡ249֡����Ƚ�֮ǰ�Ĳ����������˱���ʱ��
		while(nextFrame != (startPos[i] - 1) && capList[i].read(frame))
		{
			nextFrame++;
		}
		std::cout<<nextFrame<<" "<<std::endl;
	}//ѭ��������ÿ��cap��ָ��ָ����ʼ֡λ��
	std::cout<<std::endl;
	for(int i = 0; i < count; ++i)
	{
		std::cout<<capList[i].get(CV_CAP_PROP_POS_FRAMES) + 1<<" ";
	}
	std::cout<<std::endl;

	VideoObjectInfo::FrameInfoPtr* frameArray = new VideoObjectInfo::FrameInfoPtr[count];//�����ã����ҷ�������
	//�������Ƶ���кϳ�
	cv::Mat compositionFrame;//����д��ϳ���Ƶ�еĺϳ�֡
	cv::Mat tmpFrame[5];//ͨ����ͬ����õ�����ʱ֡
	std::string name = "tmpString";
	std::string windows = name;
	bool flag = false;
	for(int i = 0; i < interval; ++i)
	{
		std::vector<cv::Rect> rectContainer;//���ڴ�ŵ�ǰ�����������

		//�õ�һ����ϳɵ��˶�֡��Ϣ
		for(int k = 0; k < count; ++k)
		{
			if(k * interval + i < frameList.size())//���һ����Ŀ��������interval��
			{
				frameArray[k] = frameList[k*interval + i];
				if(!capList[k].read(tmpFrame[k]))
				{
					std::cout<<"��ȡ��Ƶʧ��"<<" "<<i<<std::endl;
					flag = true;
					continue;
				}
				shift[k]++;
				//��Ϊ�ؼ��ĵط����غ����ܲ�����ÿ�������еõ���Ӧ��֡
				while((startPos[k] + shift[k] - 1) != frameArray[k]->frameNum)//startPos + shift�õ���ǰ��Ƶ���
				{
					if(!capList[k].read(tmpFrame[k]))
					{
						std::cout<<"��ȡ��Ƶʧ��"<<" "<<i<<std::endl;
						flag = true;
						break;
					}
					else
					{
						shift[k]++;//ƫ������һ
					}
				}
				if(!flag)
				{
					if(k == 0)//��ʾ�ǵ�һ�飬ֱ�ӽ����ݸ��Ƹ�compositionFrame����
					{
						std::swap(compositionFrame,tmpFrame[k]);//���ﻹ��Ҫ���ǵ��Ǽ�¼�����˶�Ŀ����Ϣ
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							rectContainer.push_back(*iter);
							iter++;
						}//����ǰ֡��rect��ȫ���Ƶ�container��

					}
					else//��ǰ֡�˶�Ŀ�����������ۼӵ�compositionFrame�У�
					{
						//�õ������˶�Ŀ�꣬���ۼӵ���ʼ֡��
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							cv::Rect rect = *iter;//�õ�һ���˶�Ŀ�������Ϣ
							//������Ҫ�Ե�ǰ���rect������rect�����ж��Ƿ�����ص���������ص�����ν��д���
						    ///////////////////////////////////////
							//cv::Rect originalRect = cv::Rect(rect.x * 6, rect.y * 6, rect.width * 6,rect.height * 6);
							cv::Rect originalRect = cv::Rect(rect);
							cv::Mat rectImage = tmpFrame[k](originalRect);
							cv::Mat srcImage = compositionFrame(originalRect);

							if(haveOverlap(rectContainer,rect))
							{
								//������Ҫ������ײ��⣬����Ե�ǰ�Ѵ������ݺͼ���������ݽ��м�⣬����͸���ȴ���
								//ʹ�ö��Ŀ���ܹ�ͬʱ��ʾ
								
								cv::addWeighted(srcImage,0.5,rectImage,0.5,0.0,srcImage);
							}
							else
							{
								cv::addWeighted(srcImage,0.0,rectImage,1.0,0.0,srcImage);
							}
							rectContainer.push_back(*iter);
							iter++;
						}
					}
				}
				flag = false;
			}
		}
		cap_write<<compositionFrame;

	}
	//֮����Ҫ���ǵ����ݰ�������֡�е�ָ�����򿽱����ϳ�֡�У�
	//�����Ѻϳ�֡�е��˶�Ŀ����Ϣ�����¼���֡��֮ǰ���н�����ײ��⣬���������ײ��ʹ��Ȩ�ص��ӣ�����ֱ���ۼӼ���
	for(int i = 0; i < count; ++i)
	{
		capList[i].release();
	}

}
