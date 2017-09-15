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
		void endWork(); 
		void keyHandler(int &key);
		//GETTERS
		cv::Mat getBrushSettingsFrame() { return brushSettingsFrame;};
		cv::Mat getSettings() { return settings; };
		cv::Mat getFrame() { return frame; };
		cv::Mat getHsv() { return hsv; };
		cv::Mat getColorRange() { return colorRange; };
		cv::Mat getFilteredFrame() { return filteredFrame; };
		cv::Mat getGrayScale() { return grayScale; };
		cv::Mat getBlurredFrame() { return blurredFrame; };
		cv::Mat getThresh() { return thresh; };
		cv::Mat getEroded() { return eroded; };
		cv::Mat getOpeningFrame() { return openingFrame; };
		cv::Mat getDistTransform() { return distTransform; };
		cv::Mat getNormalized() { return normalized; };
		cv::Mat getContourred() { return contourred; };
		cv::Mat getDrawing() { return drawing; };
	private:
		void prepare();

		static void writeImage(cv::InputOutputArray &source, std::string name);
		void onDraw(cv::Mat &src, cv::Mat &buffer, cv::Mat &writeFile, bool mode);
		
		void drawHUD(cv::Mat &src);
		void onFilterSettings(char* frameName, cv::Mat& src, bool isActive);
		
		void onTrackbar(int, void*);

		void saveFilter(char* fileName);
		std::vector<int> getFilterData(char* fileName);

		void saveBrushInfo(char* fileName);
		void getBrushData(char* fileName, cv::Scalar&color, int&brushSize);

		cv::Mat createDrawSettings();
		void onDrawSettings(cv::Mat&, bool isActive);

		bool progressBar(cv::Mat& src);
		
		
	private:
		int timerCounter = 0;
		float shiftX, shiftY;
		int borderSize = 1;
		cv::Point clickPoint = cv::Point(0,0);

		bool filterSettingsActive = false;
		bool brushSettingsActive = false;

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