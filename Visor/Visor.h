#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>

namespace Visoring{
	class Visor
	{
	public:
		Visor();
		~Visor();
		void programLoop();
		static void writeImage(cv::InputOutputArray &source, std::string name);
		void onDraw(cv::Mat &src, cv::Mat &buffer, cv::Mat &writeFile, bool mode);
		void keyHandler(int &key);
		void endWork();
		void drawHUD(cv::Mat &src);
		void onFilterSettings(char* frameName, cv::Mat& src, bool isActive);
		
	
		void onTrackbar(int, void*);
		void saveFilter(char* fileName);
		std::vector<int> getFilterData(char* fileName);

		cv::Mat createDrawSettings();
		void onDrawSettings(cv::Mat&, bool isActive);
	
	public:
		cv::Point clickPoint = cv::Point(0,0);


		bool drawMode = false;
		bool clearScr = false;
		//HSV filter
		char * filterSettingsFrameName = "HSV Filter Calibration";
		char * filterSettingsFileName = "settings/hsvValues.txt";	
		//brush
		char* brushPalettePath = "settings/RGBPalette.png";
		char * brushSettingsFrameName = "Brush settings";
		char * brushSettingsFileName = "settings/brushInfo.txt";
		//Change VideoCapture value if you aren't using default system camera
		cv::VideoCapture cap = cv::VideoCapture(0);

		//frames
		cv::Mat brushSettingsFrame;
		cv::Mat settings;
		cv::Mat frame;
		cv::Mat hsv;
		cv::Mat colorRange;
		cv::Mat filteredFrame;
		cv::Mat grayScale;
		cv::Mat blurredFrame;
		cv::Mat thresh;
		cv::Mat eroded;
		cv::Mat openingFrame;
		cv::Mat distTransform;
		cv::Mat normalized;
		cv::Mat contourred;
		cv::Mat kernel;
		//drawing
		cv::Mat drawing;
		cv::Mat buffer;
		cv::Mat writing;

		std::vector<int> lowerColor; 
		std::vector<int> upperColor;
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Moments> moments;

		cv::Scalar color;
		cv::Scalar colorBuf;
		cv::Point2f drawCenter;
		cv::Point anchor;
		int width;
		int height;

		int brushSize;
	};

}