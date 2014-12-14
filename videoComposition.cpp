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
	videoObject = VideoObjectInfo(videoname);//得到视频起始时间节点信息
}
void VideoCompositor::analysisVideo(std::string &objectInfoFile)
{
	videoObject.readObjectInfo(objectInfoFile);//成功完成文件的读取过程，之后是如何进行文件的合成工作了
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
//开始时间格式2014 0902 094142
std::string VideoCompositor::getShowTime(std::string &startTime,int shiftTime)
{
	//根据起始时间和当前偏移时间得到当前帧时间戳，用于对运动目标进行标示
	//需要将string类型转化为int类型，之后再进行计算才是，
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
			hour = hour + hour_add;//关于日期的整合稍后处理，之后再说，
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



//先实现最简单的剔除静态帧，保留动态帧，合成视频
void VideoCompositor::videoCompoistionSpare(std::string &targetVideo)
{
	//根据当前视频中的帧运动信息内容对视频进行合成，同时查看原始的视频文件是否还需要添加新的信息，
	cv::VideoCapture cap;
	if(!cap.open(originalVideo))
		return;
	int ex=static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //作用是什么 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	double fps = cap.get(CV_CAP_PROP_FPS);//帧频数，即每秒包含帧个数
	cv::VideoWriter cap_write;
	cap_write.open(targetVideo,ex, cap.get(CV_CAP_PROP_FPS),S, true); //打开写入文件，并指定格式

	//关于编码的格式问题，暂时不予深究，先将能够实现的内容实现出来
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
			std::cout<<"读取视频失败"<<std::endl;
			return;	
		}
		curFrameNum++;
		int shiftTime =std::floor(curFrameNum / fps) + 1;
		int frameNum = (*iter)->frameNum;
		if(frameNum == curFrameNum)//当前读取帧为动态帧，将当前帧存放到合成视频中
		{
			//对当前帧中的运动信息进行
			std::string showTime = getShowTime(videoObject.getStartTime(),shiftTime);
			int count = (*iter)->objectList.size();//当前帧中运行目标个数
			for(int i = 0; i < count; ++i)//为每个运动目标设定标示信息
			{

				cv::Point point((*iter)->objectList[i].x * 6 ,(*iter)->objectList[i].y * 6);//字体左下位置坐标
				cv::Rect rect = cv::Rect((*iter)->objectList[i].x * 6 ,(*iter)->objectList[i].y * 6,
					(*iter)->objectList[i].width * 6,(*iter)->objectList[i].height * 6);//运动目标外接矩形轮廓
				//这里需要考虑到一点，如果绘制字超出边界将如何进行处理，需要将其纳入到视频区域内部才是，也就是修改左下位置坐标
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
//较为简单直接将若干帧压缩到同一个时间片段里面，需要考虑的是如何能够进行碰撞检测
//针对opencv不能读取指定关键帧问题，还是采用遍历的方法，根据视频划分，
//创建若干个videoCapture，从指定位置开始读取，将读取到且满足条件的帧进行合并，直至读取结束
void VideoCompositor::videoCompositionDense(std::string &targetVideo,float rate)
{
	cv::VideoCapture cap;//原始视频
	if(!cap.open(originalVideo)) return;

	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //作用是什么 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	cv::VideoWriter cap_write;//写入视频
	cap_write.open(targetVideo , ex, cap.get(CV_CAP_PROP_FPS),S, true); //打开写入文件，并指定格式

	std::vector<VideoObjectInfo::FrameInfoPtr> frameList = videoObject.getFrameList();//运动帧信息列表

	//完成视频分组信息[[****][****][****][****][**]]//interval = 4，Count= 5；
	int dynamicFrameCount = videoObject.getDynamicFrameCount();
	int interval = std::ceil(dynamicFrameCount * rate);//分段间隔大小，每段中最多包含动态帧个数
	int count = std::ceil((float)dynamicFrameCount/interval);//总共分组个数

	//根据分组情况创建若干个videoCapture，用于从指定位置开始视频读取
	cv::VideoCapture* capList = new cv::VideoCapture[count];
	int *startPos = new int[count];//记录每段视频的起始编号信息，方便之后根据偏移量得到绝对帧信息,
	int *shift = new int[count];//记录当前分组帧的偏移量，相对位置

	for(int i = 0; i < count; ++i)
		shift[i] = 0;
	
	/////方法,连续读取，直至到达指定起始帧位置帧
	for(int i = 0; i < count; ++i)
	{
		startPos[i] = frameList[i * interval]->frameNum;//获得每个片段起始帧位置
		std::cout<<startPos[i]<<" ";
	}
	std::cout<<std::endl;
	//对每个片段进行循环读取直至到达指定起始帧位置
	//set关键帧是以250为间隔，[1,250]->250;[251,500]->500;...
	cv::Mat frame;
	for(int i = 0; i < count; ++i)
	{
		if (!capList[i].open(originalVideo)) return;

		if((startPos[i] - 1) % 250 == 0)//当为250倍数时可以直接设定
			capList[i].set(CV_CAP_PROP_POS_FRAMES,startPos[i] - 1);
		else if((startPos[i] - 1) - 250 > 0)//当指定起始帧超过250时，
			capList[i].set(CV_CAP_PROP_POS_FRAMES,(startPos[i] - 1) - 250);
		else//小于250时，只能设定为0，之后再进行遍历操作
			capList[i].set(CV_CAP_PROP_POS_FRAMES,0);

		if(!capList[i].read(frame)) return;
		int nextFrame = capList[i].get(CV_CAP_PROP_POS_FRAMES);//获得nextFrame编号
		std::cout<<nextFrame<<" ";
		//进过上述判断操作之后，每个片段最多读取249帧，相比较之前的操作大大减少了遍历时间
		while(nextFrame < (startPos[i] - 1) && capList[i].read(frame))
		{
			nextFrame++;
		}
		std::cout<<nextFrame<<" "<<std::endl;
	}//循环结束，每个cap均指向指定开始帧位置
	std::cout<<std::endl;
	for(int i = 0; i < count; ++i)
	{
		std::cout<<capList[i].get(CV_CAP_PROP_POS_FRAMES) + 1<<" ";
	}
	std::cout<<std::endl;

	VideoObjectInfo::FrameInfoPtr* frameArray = new VideoObjectInfo::FrameInfoPtr[count];//可有用？暂且放在这里
	//分组对视频进行合成
	cv::Mat compositionFrame;//最终写入合成视频中的合成帧
	cv::Mat tmpFrame[5];//通过不同分组得到的临时帧
	bool flag = false;
	for(int i = 0; i < interval; ++i)
	{
		std::vector<cv::Rect> rectContainer;//用于存放当前处理矩形区域
		//得到一组待合成的运动帧信息
		for(int k = 0; k < count; ++k)
		{
			if(k * interval + i < frameList.size())//最后一组数目可能少于interval，
			{
				frameArray[k] = frameList[k*interval + i];
				if(!capList[k].read(tmpFrame[k]))
				{
					std::cout<<"读取视频失败"<<" "<<i<<std::endl;
					flag = true;
					continue;
				}
				shift[k]++;
				//最为关键的地方，关乎到能不能在每个分组中得到对应的帧
				while((startPos[k] + shift[k] - 1) != frameArray[k]->frameNum)//startPos + shift得到当前视频序号
				{
					if(!capList[k].read(tmpFrame[k]))
					{
						std::cout<<"读取视频失败"<<" "<<i<<std::endl;
						flag = true;
						break;
					}
					else
					{
						shift[k]++;//偏移量加一
					}
				}
				if(!flag)
				{
					if(k == 0)//表示是第一组，直接将内容复制给compositionFrame即可
					{
						std::swap(compositionFrame,tmpFrame[k]);//这里还需要考虑的是记录已有运动目标信息
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							rectContainer.push_back(*iter);
							iter++;
						}//将当前帧中rect完全复制到container中

					}
					else//当前帧运动目标区域内容累加到compositionFrame中，
					{
						//得到所有运动目标，并累加到起始帧上
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							cv::Rect rect = *iter;//得到一个运动目标矩形信息
							//这里需要对当前添加rect与已有rect进行判断是否存在重叠区域，如果重叠，如何进行处理
						    ///////////////////////////////////////
							
							//cv::Rect originalRect = cv::Rect(rect.x * 6, rect.y * 6, rect.width * 6,rect.height * 6);
							cv::Rect originalRect = cv::Rect(rect);
							cv::Mat rectImage = tmpFrame[k](originalRect);
							cv::Mat srcImage = compositionFrame(originalRect);

							if(haveOverlap(rectContainer,rect))
							{
								//这里需要进行碰撞检测，即针对当前已存在内容和即将添加内容进行检测，采用透明度处理
								//使得多个目标能够同时显示
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
	//之后需要考虑的内容包括：将帧中的指定区域拷贝到合成帧中，
	//保存已合成帧中的运动目标信息，，新加入帧与之前所有进行碰撞检测，如果发生碰撞则使用权重叠加，否则直接累加即可
	for(int i = 0; i < count; ++i)
	{
		capList[i].release();
	}
}

//判断当前rect与容器中已存在rect是否存在重叠，如果存在则在后续的叠加中使用权重叠加
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
			//不相交
		}
		else
		{
			//相交
			return true;
		}
		iter++;
	}
	return false;
}

void VideoCompositor::videoCompoistionSpareTest(std::string &targetVideo)
{
	//根据当前视频中的帧运动信息内容对视频进行合成，同时查看原始的视频文件是否还需要添加新的信息，
	cv::VideoCapture cap;//原始视频
	if(!cap.open(originalVideo))
		return;
	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //作用是什么 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	double fps = cap.get(CV_CAP_PROP_FPS);//帧频数，即每秒包含帧个数
	cv::VideoWriter cap_write;//写入视频
	cap_write.open(targetVideo , ex, cap.get(CV_CAP_PROP_FPS),S, true); //打开写入文件，并指定格式

	//关于编码的格式问题，暂时不予深究，先将能够实现的内容实现出来
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
			std::cout<<"读取视频失败"<<std::endl;
			return;	
		}
		curFrameNum++;
		int shiftTime =std::floor(curFrameNum / fps) + 1;
		int frameNum = (*iter)->frameNum;
		if(frameNum == curFrameNum)//当前读取帧为动态帧，将当前帧存放到合成视频中
		{
			int count = (*iter)->objectList.size();//当前帧中运行目标个数
			for(int i = 0; i < count; ++i)//为每个运动目标设定标示信息
			{
				//对当前帧中的运动信息进行
				//cv::Point point((*iter)->objectList[i].x ,(*iter)->objectList[i].y);//字体左下位置坐标
				cv::Rect rect = cv::Rect((*iter)->objectList[i].x ,(*iter)->objectList[i].y,
					(*iter)->objectList[i].width,(*iter)->objectList[i].height);//运动目标外接矩形轮廓
				//这里需要考虑到一点，如果绘制字超出边界将如何进行处理，需要将其纳入到视频区域内部才是，也就是修改左下位置坐标
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
	cv::VideoCapture cap;//原始视频
	if(!cap.open(originalVideo)) return;

	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC)); 
	char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0}; //作用是什么 
	cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),  
        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT) ); 

	cv::VideoWriter cap_write;//写入视频
	cap_write.open(targetVideo , ex, cap.get(CV_CAP_PROP_FPS),S, true); //打开写入文件，并指定格式

	std::vector<VideoObjectInfo::FrameInfoPtr> frameList = videoObject.getFrameList();//运动帧信息列表

	//完成视频分组信息[[****][****][****][****][**]]//interval = 4，Count= 5；
	int dynamicFrameCount = videoObject.getDynamicFrameCount();
	int interval = std::ceil(dynamicFrameCount * rate);//分段间隔大小，每段中最多包含动态帧个数
	int count = std::ceil((float)dynamicFrameCount/interval);//总共分组个数

	//根据分组情况创建若干个videoCapture，用于从指定位置开始视频读取
	cv::VideoCapture* capList = new cv::VideoCapture[count];
	int *startPos = new int[count];//记录每段视频的起始编号信息，方便之后根据偏移量得到绝对帧信息,
	int *shift = new int[count];//记录当前分组帧的偏移量，相对位置

	for(int i = 0; i < count; ++i)
		shift[i] = 0;
	
	/////方法,连续读取，直至到达指定起始帧位置帧
	for(int i = 0; i < count; ++i)
	{
		startPos[i] = frameList[i * interval]->frameNum;//获得每个片段起始帧位置
		std::cout<<startPos[i]<<" ";
	}
	std::cout<<std::endl;
	//对每个片段进行循环读取直至到达指定起始帧位置
	//set关键帧是以250为间隔，[1,250]->250;[251,500]->500;...
	cv::Mat frame;
	for(int i = 0; i < count; ++i)
	{
		if (!capList[i].open(originalVideo)) return;

		if((startPos[i] - 1) % 250 == 0)//当为250倍数时可以直接设定
			capList[i].set(CV_CAP_PROP_POS_FRAMES,startPos[i] - 1);
		else if((startPos[i] - 1) - 250 > 0)//当指定起始帧超过250时，
			capList[i].set(CV_CAP_PROP_POS_FRAMES,(startPos[i] - 1) - 250);
		else//小于250时，只能设定为0，之后再进行遍历操作
			capList[i].set(CV_CAP_PROP_POS_FRAMES,0);

		if(!capList[i].read(frame)) return;
		int nextFrame = capList[i].get(CV_CAP_PROP_POS_FRAMES) - 1;//获得nextFrame编号
		std::cout<<nextFrame<<" ";
		//进过上述判断操作之后，每个片段最多读取249帧，相比较之前的操作大大减少了遍历时间
		while(nextFrame != (startPos[i] - 1) && capList[i].read(frame))
		{
			nextFrame++;
		}
		std::cout<<nextFrame<<" "<<std::endl;
	}//循环结束，每个cap均指向指定开始帧位置
	std::cout<<std::endl;
	for(int i = 0; i < count; ++i)
	{
		std::cout<<capList[i].get(CV_CAP_PROP_POS_FRAMES) + 1<<" ";
	}
	std::cout<<std::endl;

	VideoObjectInfo::FrameInfoPtr* frameArray = new VideoObjectInfo::FrameInfoPtr[count];//可有用？暂且放在这里
	//分组对视频进行合成
	cv::Mat compositionFrame;//最终写入合成视频中的合成帧
	cv::Mat tmpFrame[5];//通过不同分组得到的临时帧
	std::string name = "tmpString";
	std::string windows = name;
	bool flag = false;
	for(int i = 0; i < interval; ++i)
	{
		std::vector<cv::Rect> rectContainer;//用于存放当前处理矩形区域

		//得到一组待合成的运动帧信息
		for(int k = 0; k < count; ++k)
		{
			if(k * interval + i < frameList.size())//最后一组数目可能少于interval，
			{
				frameArray[k] = frameList[k*interval + i];
				if(!capList[k].read(tmpFrame[k]))
				{
					std::cout<<"读取视频失败"<<" "<<i<<std::endl;
					flag = true;
					continue;
				}
				shift[k]++;
				//最为关键的地方，关乎到能不能在每个分组中得到对应的帧
				while((startPos[k] + shift[k] - 1) != frameArray[k]->frameNum)//startPos + shift得到当前视频序号
				{
					if(!capList[k].read(tmpFrame[k]))
					{
						std::cout<<"读取视频失败"<<" "<<i<<std::endl;
						flag = true;
						break;
					}
					else
					{
						shift[k]++;//偏移量加一
					}
				}
				if(!flag)
				{
					if(k == 0)//表示是第一组，直接将内容复制给compositionFrame即可
					{
						std::swap(compositionFrame,tmpFrame[k]);//这里还需要考虑的是记录已有运动目标信息
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							rectContainer.push_back(*iter);
							iter++;
						}//将当前帧中rect完全复制到container中

					}
					else//当前帧运动目标区域内容累加到compositionFrame中，
					{
						//得到所有运动目标，并累加到起始帧上
						std::vector<cv::Rect> objectlist = frameArray[k]->objectList;
						std::vector<cv::Rect>::iterator iter = objectlist.begin();
						while(iter != objectlist.end())
						{
							cv::Rect rect = *iter;//得到一个运动目标矩形信息
							//这里需要对当前添加rect与已有rect进行判断是否存在重叠区域，如果重叠，如何进行处理
						    ///////////////////////////////////////
							//cv::Rect originalRect = cv::Rect(rect.x * 6, rect.y * 6, rect.width * 6,rect.height * 6);
							cv::Rect originalRect = cv::Rect(rect);
							cv::Mat rectImage = tmpFrame[k](originalRect);
							cv::Mat srcImage = compositionFrame(originalRect);

							if(haveOverlap(rectContainer,rect))
							{
								//这里需要进行碰撞检测，即针对当前已存在内容和即将添加内容进行检测，采用透明度处理
								//使得多个目标能够同时显示
								
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
	//之后需要考虑的内容包括：将帧中的指定区域拷贝到合成帧中，
	//保存已合成帧中的运动目标信息，，新加入帧与之前所有进行碰撞检测，如果发生碰撞则使用权重叠加，否则直接累加即可
	for(int i = 0; i < count; ++i)
	{
		capList[i].release();
	}

}
