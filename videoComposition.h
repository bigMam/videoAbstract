//负责摘要视频的合成及保存
#include "videoObjectInfo.h"
#include <string>
class VideoCompositor
{
private:
	VideoObjectInfo videoObject;
	std::string originalVideo;
public:
	VideoCompositor();
	VideoCompositor(std::string &filenmae);
	void analysisVideo(std::string &filename);//对指定路径图像进行场景分析，如果当前分析文件不存在则生成
	void videoCompoistionSpare(std::string &filename);//完成稀疏视频合成，
	void videoCompoistionSpareTest(std::string &filename);
	void videoCompositionDense(std::string &filename,float rate);//完成稠密视频合成，rate表示压缩比例
	void videoCompositionDenseTest(std::string &filename,float rate);
	void videoCompositionCycle(std::string &filename);//完成规律视频合成，
	void collisionDetection();//运动目标的碰撞检测，这个应该是在合成的过程中进行的，
	std::string getShowTime(std::string &startTime,int shifTime);

	bool haveOverlap(std::vector<cv::Rect>& container,cv::Rect rect);//判断当前rect与容器内rect是否存在重叠区域
};