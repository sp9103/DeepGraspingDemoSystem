#pragma once
#include <opencv2\opencv.hpp>
#include "BlobLabeling_pregrasp.h"

#define COLOR_THRESHOLD		10
#define DEPTH_THRESHOLD		10
#define PEDDING				4

class ColorBasedTracker_pregrasp
{
public:
	ColorBasedTracker_pregrasp(void);
	~ColorBasedTracker_pregrasp(void);

	void InsertBackGround(cv::Mat Color, cv::Mat Depth);

	cv::Mat calcImage(cv::Mat src, cv::Mat depth);
	cv::Mat calcImage(cv::Mat src, cv::Mat depth, cv::Mat *dstColor, cv::Mat *dstDepth);

private:
	int imgWidth;
	int imgHeight;

	BlobLabeling_pregrasp _blobLabeling;

	cv::Mat ColorBackGround;
	cv::Mat DepthBackGround;
	
	cv::Mat subBackground(cv::Mat srcColor, cv::Mat srcDepth);
	cv::Mat DetectColorMap(cv::Mat rgb, cv::Mat subMap);
	cv::Mat DeleteSub(cv::Mat map, cv::Mat src);
	void FindNMaxBlob(int N, std::vector<std::pair<std::vector<cv::Point2i>, cv::Rect>> *dst);
	cv::Rect FindHandBlob(std::vector<std::pair<std::vector<cv::Point2i>, cv::Rect>> src);
	cv::Rect FindArmBlob();
	
	cv::Mat drawBlobMap(cv::Mat src, std::vector<std::pair<std::vector<cv::Point2i>, cv::Rect>> blobInfo);
};

