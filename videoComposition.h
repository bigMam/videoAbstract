//����ժҪ��Ƶ�ĺϳɼ�����
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
	void analysisVideo(std::string &filename);//��ָ��·��ͼ����г��������������ǰ�����ļ�������������
	void videoCompoistionSpare(std::string &filename);//���ϡ����Ƶ�ϳɣ�
	void videoCompoistionSpareTest(std::string &filename);
	void videoCompositionDense(std::string &filename,float rate);//��ɳ�����Ƶ�ϳɣ�rate��ʾѹ������
	void videoCompositionDenseTest(std::string &filename,float rate);
	void videoCompositionCycle(std::string &filename);//��ɹ�����Ƶ�ϳɣ�
	void collisionDetection();//�˶�Ŀ�����ײ��⣬���Ӧ�����ںϳɵĹ����н��еģ�
	std::string getShowTime(std::string &startTime,int shifTime);

	bool haveOverlap(std::vector<cv::Rect>& container,cv::Rect rect);//�жϵ�ǰrect��������rect�Ƿ�����ص�����
};