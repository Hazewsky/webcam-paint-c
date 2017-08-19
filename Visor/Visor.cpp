/*** Created by Dmitry Trushkin ***/
#include "Visor.h"
#include <math.h>
#include <fstream>
#include <string>
namespace Visoring{

	Visor::Visor() :
		lowerColor({ 0, 0,0 }),
		upperColor({255,255,255}),
		kernel(cv::Mat::ones(3, 3, CV_8UC1)),
		anchor(0,0),
		color(255,255,255),
		brushSize(2)
	{
		this->cap.read(this->frame);
		height = this->frame.rows;
		width = this->frame.cols;
		this->lowerColor = getFilterData(filterSettingsFileName);
		getBrushData(brushSettingsFileName, this->color, this->brushSize);
#define preSet  cv::Mat::zeros(frame.size(), CV_8UC1);
		this->hsv = preSet;
		this->colorRange = preSet;
		this->filteredFrame = preSet;
		this->openingFrame = preSet;
		this->grayScale = preSet;
		this->blurredFrame = preSet;
		this->thresh = preSet;
		this->distTransform = preSet;
		this->eroded = preSet;
		this->settings = preSet;
		//draw
#define drawSet cv::Mat::zeros(frame.size(), CV_8UC3);
		this->drawing = drawSet;
		this->buffer = drawSet;
		this->writing = drawSet;
	}

	void Visor::programLoop() {

	}
	void Visor::writeImage(cv::InputOutputArray &source, std::string name){
		if (source.size() != cv::Size(0,0))
		{
	//		cv::cvtColor(source, source, CV_BGR2RGB);
			cv::imwrite("images/" + name + ".jpg", source);
		}
		
	}
	void Visor::drawHUD(cv::Mat & src){
		
	
		
	}
	void Visor::onDraw(cv::Mat &src, cv::Mat &buffer, cv::Mat &writeFile, bool mode){
		cv::Mat buf;
		
		if (buf.size() == cv::Size(0, 0)){
			buf = cv::Mat::zeros(src.size(), CV_8UC3);
		}
		
		if (this->drawMode == false){
			src = cv::Mat::zeros(src.size(), CV_8UC3);
			
			src = buffer.clone();
			drawHUD(src);
			cv::circle(src, this->drawCenter, brushSize / .5, color, 1);
			this->anchor = this->drawCenter;
		}
		if (this->drawMode == true && this->anchor.x != 0 && this->anchor.y != 0){
			if (this->clearScr == true){
				src = cv::Mat::zeros(src.size(), CV_8UC3);
				clearScr = !clearScr;
			}
			src = buffer.clone();
			
			cv::line(src, this->anchor, this->drawCenter,color, brushSize);
			cv::line(writeFile, this->anchor, this->drawCenter, color, brushSize);
			
			buffer = src.clone();
			//drawHUD(src);
		}
		
		//drawHUD(src);

		this->anchor = this->drawCenter;

//		cv::imshow("b", buffer);

		
	}
	void Visor::keyHandler(int &key){

		switch (key){
		case 32:
			this->drawMode = !this->drawMode;
			this->clearScr = true;
			break;
		
			//`
			//clear the HSV filter
		case 96:
			this->lowerColor = std::vector<int>{ 0,0,0 };
			break;
			//~
			//clear screen
			
		case 126:
			this->drawing = cv::Mat::zeros(this->drawing.size(), CV_8UC3);
			this->writing = cv::Mat::zeros(this->drawing.size(), CV_8UC3);
			this->buffer = cv::Mat::zeros(this->drawing.size(), CV_8UC3);
			break;

		}

	}
	void Visor::onFilterSettings(char* frameName, cv::Mat& frame, bool isActive){
		if (isActive) {
			cvNamedWindow(frameName, CV_WINDOW_AUTOSIZE);

			int maxSliderValue = 180;
			std::string lowerH = "Low. H";
			std::string lowerS = "Low. S";
			std::string lowerV = "Low. V";
			std::string upperH = "Up. H";
			std::string upperS = "Up. S";
			std::string upperV = "Up. V";
			cv::createTrackbar(lowerH, frameName, &lowerColor[0], maxSliderValue);
			cv::createTrackbar(lowerS, frameName, &lowerColor[1], maxSliderValue);
			cv::createTrackbar(lowerV, frameName, &lowerColor[2], maxSliderValue);
			//cv::createTrackbar(upperH, frameName, &upperColor[0], maxSliderValue);
			//cv::createTrackbar(upperS, frameName, &upperColor[1], maxSliderValue);
			//cv::createTrackbar(upperV, frameName, &upperColor[2], maxSliderValue);
			cv::imshow(frameName, frame);
			
		}
		else {
			saveFilter(filterSettingsFileName);
			cv::destroyWindow(filterSettingsFrameName);
			
		}
		
	}
	void Visor::onTrackbar(int, void*){

	}

	cv::Mat Visor::createDrawSettings() {
		try {
			cv::Mat frame = cv::imread(brushPalettePath, 1);	
			//brushSize
			std::string s = "Brush size = " + std::to_string(brushSize);
			int borderSize = 1;
			int padding = 40;
			cv::putText(frame, s, cv::Point(0, 20), CV_FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
			//+ symbol
			cv::rectangle(frame, cv::Point(frame.rows/45, frame.cols/15), 
				cv::Point(frame.rows/7, frame.cols/8), cv::Scalar(127, 127, 127), -1);
			cv::rectangle(frame, cv::Point(frame.rows/45 - borderSize, frame.cols / 15-borderSize),
				cv::Point(frame.rows / 7+ borderSize, frame.cols / 8+ borderSize), cv::Scalar(255, 255, 255));
			cv::putText(frame, "+1", cv::Point(frame.rows / 45 + frame.rows/15 - frame.rows/45 - 2*borderSize,
				frame.cols/15 + 0.75*(frame.cols/8 - frame.cols/15)), CV_FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
			//- symbol
			cv::rectangle(frame, cv::Point(frame.rows / 45, frame.cols / 15 + padding),
				cv::Point(frame.rows / 7, frame.cols / 8 + 40), cv::Scalar(127, 127, 127), -1);
			cv::rectangle(frame, cv::Point(frame.rows / 45 - borderSize, frame.cols / 15 + padding - borderSize),
				cv::Point(frame.rows / 7 + borderSize, frame.cols / 8 + padding + borderSize), cv::Scalar(255, 255, 255));
			cv::putText(frame, "-1", cv::Point(frame.rows / 45 + frame.rows / 15 - frame.rows / 45 - 2 * borderSize,
				frame.cols / 15 + 0.55*(frame.cols / 8 - frame.cols / 15 + 2* padding)), CV_FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));

			brushSettingsFrame = frame;
			return frame;
		}
		catch (cv::Exception ex) {
			std::cout << std::endl << ex.msg << std::endl;
		}

	}
	
	void mouseClick(int event, int x, int y, int flags, void*userdata) {
		if (event == cv::EVENT_LBUTTONDOWN) {
			cv::Point* ptPtr = (cv::Point*)userdata;
			ptPtr->x = x;
			ptPtr->y = y;
		}
	}
	void Visor::onDrawSettings(cv::Mat& frame, bool isActive) {
		if (isActive) {
			cv::namedWindow(brushSettingsFrameName, 1);
			cv::setMouseCallback(brushSettingsFrameName, mouseClick, (void*)&clickPoint);
			//cv::Vec3b col = frame.at<cv::Vec3b>(clickPoint);
			//on + button
		
			if ((clickPoint.x >= frame.rows / 45)
				&& (clickPoint.y >= frame.cols / 15)
				&& clickPoint.x <= frame.rows / 7
				&& clickPoint.y <= frame.cols / 8
				&& brushSize < 50) {
				brushSize += 1;
				clickPoint.x = 0;
				clickPoint.y = 0;
			}
			if (clickPoint.x >= frame.rows / 45 && clickPoint.y >= frame.cols / 15 + 40 &&
				clickPoint.x <= frame.rows / 7 && clickPoint.y <= frame.cols / 8 + 40 && brushSize > 1) {
				brushSize -= 1;
				clickPoint.x = 0;
				clickPoint.y = 0;
			}
			if(clickPoint.x > 0 && clickPoint.y > 0) color = frame.at<cv::Vec3b>(clickPoint);
			cv::imshow(brushSettingsFrameName, frame);
		}
		else {
			saveBrushInfo(brushSettingsFileName);
			cv::destroyWindow(brushSettingsFrameName);
		}
	}
	void Visor::saveFilter(char* fileName) {
		std::ofstream fout(fileName, std::fstream::trunc);
		for (auto& i :lowerColor) {
			fout << i << std::endl;
		}
		fout.close();

	}
	std::vector<int> Visor::getFilterData(char* fileName) {
		try {
			std::ifstream fin(fileName);
			//check if the file is empty
			std::vector<int> buff = std::vector<int>{};
			if (fin.peek() == std::ifstream::traits_type::eof()) {
				return  std::vector<int>{0, 0, 0};
			}
			int value = 0;
			while (fin >> value) {
				buff.push_back(value);
			}
			fin.close();
			return buff;
		}
		catch (cv::Exception ex) {

		}
	}

	void Visor::saveBrushInfo(char* fileName) {
		std::ofstream fout(fileName, std::fstream::trunc);
		fout << color[0] << " " << color[1] << " " << color[2] << " " << brushSize;
		fout.close();
	}

	void Visor::getBrushData(char* filename, cv::Scalar&color, int&brushSize) {
		try {
			std::ifstream fin(filename);
			//check if the file is empty
			
			if (fin.peek() == std::ifstream::traits_type::eof()) {
				color = cv::Scalar(255, 255, 255);
				brushSize = 2;
			}
			fin >> color[0] >> color[1] >> color[2] >> brushSize;
			
			//fin >> colBuf[0] >> colBuf[1] >> colBuf[2] >> brushBuf;
			//std::cout << colBuf << " " << brushBuf;
			fin.close();
		}
		catch (cv::Exception ex) {

		}
	}

	void Visor::endWork(){
		this->cap.release();
		cv::destroyAllWindows();
		std::string filename;
		std::cout << "Enter filename.\t Type '-' to cancel saving \t" << std::endl;
		std::cin >> filename;
		//scanf("%[a-zA-Z0-9 ]", filename);
		if (filename != "-")
		{
			Visor::writeImage(this->writing, filename);
		}
	}
	Visor::~Visor()
	{
	
	}
}