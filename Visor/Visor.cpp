/*** Created by Dmitry Trushkin ***/
#include "Visor.h"
#include <math.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>

namespace Visoring {


	Visor::Visor() :
		lowerColor({ 0, 0,0 }),
		upperColor({ 255,255,255 }),
		kernel(cv::Mat::ones(3, 3, CV_8UC1)),
		anchor(0, 0)
	{
		this->cap.read(this->frame);
		height = this->frame.rows;
		width = this->frame.cols;
		this->lowerColor = getFilterData(filterSettingsFileName);
		

		prepare();
	}
#define preSet  cv::Mat::zeros(frame.size(), CV_8UC1);
#define drawSet cv::Mat::zeros(frame.size(), CV_8UC3);

	void Visor::prepare() {
		getBrushData(brushSettingsFileName, this->color, this->brushSize);
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
		this->drawing = drawSet;
		this->buffer = drawSet;
		this->writing = drawSet;
		shiftX = (float)(this->width / 10);
		shiftY = (float)(this->height / 10);
	}
	void Visor::programLoop() {
		onDrawSettings(createDrawSettings(), brushSettingsActive);
		cap.read(frame);
		flip(frame, frame, 1);
		cvtColor(frame, hsv, CV_BGR2HSV);
		inRange(hsv, lowerColor, upperColor, colorRange);

		filteredFrame = cv::Mat::zeros(frame.size(), CV_8UC1);
		bitwise_and(frame, frame, filteredFrame, colorRange);

		onFilterSettings(filterSettingsFrameName, filteredFrame, filterSettingsActive);
		GaussianBlur(filteredFrame, blurredFrame, cv::Size(3, 3), 2);
		cvtColor(blurredFrame, grayScale, CV_BGR2GRAY);
		threshold(grayScale, thresh, 200, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
		erode(thresh, eroded, kernel);
		morphologyEx(eroded, openingFrame, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 5);

		distanceTransform(openingFrame, distTransform, CV_L2, CV_DIST_MASK_PRECISE);
		normalize(distTransform, normalized, 0.0, 1.0, cv::NORM_MINMAX);
		cv::Mat cont;
		cv::Mat dr = cv::Mat::zeros(frame.size(), CV_8UC3);
		normalized.convertTo(cont, CV_8UC1);
		//openingFrame.convertTo(cont, CV_8UC1);
		findContours(cont, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		
		std::vector<cv::Moments> mu(contours.size());
		for (size_t i = 0; i < contours.size(); i++) {
			mu[i] = cv::moments(contours[i], false);
		}
		std::vector<cv::Point2f> centrs(contours.size());
		for (size_t i = 0; i < contours.size(); i++) {
			centrs[i] = cv::Point2f(static_cast<float>(mu[i].m10 / mu[i].m00), static_cast<float>(mu[i].m01 / mu[i].m00));
			if (contourArea(contours[i]) > 1500 && arcLength(contours[i], false) > 100) {
				drawCenter = centrs[i];
			}
		}
		for (size_t i = 0; i < contours.size(); i++) {
			//radius
			drawContours(dr, contours, (int)i, CV_RGB(255, 0, 0), 3);
			//cout << contourArea(contours[i]) << endl;
			circle(dr, drawCenter, 3, CV_RGB(0, 0, 255), -1);
		}
		onDraw(drawing, buffer, writing, drawMode);
	}
	void Visor::writeImage(cv::InputOutputArray &source, std::string name) {
		if (source.size() != cv::Size(0, 0))
		{
			//		cv::cvtColor(source, source, CV_BGR2RGB);
			cv::imwrite("images/" + name + ".jpg", source);
		}

	}

	bool Visor::progressBar(cv::Mat& src) {
		//cv::rectangle(src, cv::Point(0, 0), cv::Point(this->width, 0.5 * shiftY), cv::Scalar(0, 0, 255), -1);
		std::time_t timer = std::clock();
		if (timerCounter == 0) timerCounter = timer;
		
		while (timer - timerCounter < 3000) {
			std::time_t timer = std::clock();
			std::cout << timerCounter << "---" << timer << "..." << timer - timerCounter << std::endl;
			return false;
		}
		timerCounter = 0;
		return true;
	}
	void Visor::drawHUD(cv::Mat & src){
		
		//FILTER
		cv::rectangle(src, cv::Point2f(0.5f * shiftX, shiftY),
			cv::Point2f(2.5f* shiftX, 2 * shiftY),
			cv::Scalar(127, 127, 127), -1);
		cv::rectangle(src, cv::Point2f(0.5f * shiftX- borderSize, shiftY-borderSize),
			cv::Point2f(2.5f* shiftX+ borderSize, 2 * shiftY+ borderSize),
			cv::Scalar(255, 255, 255));
		cv::putText(src, "Fllter", cv::Point2d(shiftX, 1.5 * shiftY),
			cv::FONT_HERSHEY_COMPLEX,.5,cv::Scalar(255,255,255));
		
		//BRUSH
		cv::rectangle(src, cv::Point2f(3.0f* shiftX, shiftY),
			cv::Point2f(4.5f* shiftX, 2 * shiftY),
			cv::Scalar(127, 127, 127),-1);
		cv::rectangle(src, cv::Point2f(3.0f* shiftX-borderSize, shiftY-borderSize),
			cv::Point2f(4.5f* shiftX + borderSize, 2 * shiftY+ borderSize),
			cv::Scalar(255, 255, 255));
		cv::putText(src, "Brush", cv::Point2d(3.5f * shiftX, 1.5f * shiftY), 
			cv::FONT_HERSHEY_COMPLEX, .5, cv::Scalar(255, 255, 255));
		
		//SAVE
		cv::rectangle(src, cv::Point2f(5.0f * shiftX, shiftY),
			cv::Point2f(6.5f * shiftX, 2.0f * shiftY),
			cv::Scalar(127, 127, 127), -1);
		cv::rectangle(src, cv::Point2f(5.0f * shiftX-borderSize, shiftY-borderSize),
			cv::Point2f(6.5f * shiftX+ borderSize, 2.0f * shiftY+ borderSize),
			cv::Scalar(255, 255, 255));
		cv::putText(src, "Save", cv::Point2f(5.5f * shiftX, 1.5f * shiftY),
			cv::FONT_HERSHEY_COMPLEX, .5, cv::Scalar(255, 255, 255));

		//EXIT
		cv::rectangle(src, cv::Point2f(7.0f * shiftX, shiftY),
			cv::Point2f(9.0f * shiftX, 2.0f * shiftY),
			cv::Scalar(127, 127, 127), -1);
		cv::rectangle(src, cv::Point2f(7.0f * shiftX-borderSize, shiftY-borderSize),
			cv::Point2f(9.0f * shiftX+borderSize, 2.0f * shiftY+borderSize),
			cv::Scalar(255, 255, 255));
		cv::putText(src, "Exit", cv::Point2f(7.5f * shiftX, 1.5f * shiftY),
			cv::FONT_HERSHEY_COMPLEX, .5, cv::Scalar(255, 255, 255));
		if (drawCenter.x >= 0.5f * shiftX&&
			drawCenter.y >= shiftY &&
			drawCenter.x <= 2.5f * shiftX&&
			drawCenter.y <= 2.0f * shiftY && progressBar(src)) {
			filterSettingsActive = true;
			//std::cout << progressBar(src);
		}
		
		//	cv::rectangle(src, cv::Point(0, 0), cv::Point(counter, 0.5 * shiftY), cv::Scalar(255, 0, 0), -1);
		//	counter++;
		//	if (counter == this->width - 1) counter = 0;
			//progressBar(src);
		//}
	//	if (this->drawCenter.x <= (int)this->width / 6 &&
	//		this->drawCenter.y <= (int)this->height / 20)
	//		this->color = cv::Scalar(0, 0, 255);
		
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
		//Check if X is pressed
		if (filterSettingsActive && cv::getWindowProperty(filterSettingsFrameName, 0) < 0) filterSettingsActive
			= !filterSettingsActive;
		if (brushSettingsActive && cv::getWindowProperty(brushSettingsFrameName, 0) < 0)
			brushSettingsActive = !brushSettingsActive;
		switch (key){
		case 9:
			//TAB
			filterSettingsActive = !filterSettingsActive;
			break;
		case 49:
			//1
			brushSettingsActive = !brushSettingsActive;
			break;
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
			cv::rectangle(frame, cv::Point2d(frame.rows / 45, frame.cols / 15 + padding),
				cv::Point2d(frame.rows / 7, frame.cols / 8 + 40), cv::Scalar(127, 127, 127), -1);
			cv::rectangle(frame, cv::Point2d(frame.rows / 45 - borderSize, frame.cols / 15 + padding - borderSize),
				cv::Point2d(frame.rows / 7 + borderSize, frame.cols / 8 + padding + borderSize), cv::Scalar(255, 255, 255));
			cv::putText(frame, "-1", cv::Point2d(frame.rows / 45 + frame.rows / 15 - frame.rows / 45 - 2 * borderSize,
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
			fin.close();
		}
		catch (cv::Exception ex) {
			std::cout << ex.msg;
		}
	}

	void Visor::endWork(){
		this->cap.release();
		cv::destroyAllWindows();
		std::string filename;
		std::string choice;
		bool checkedInput = false;
		bool correctFilename = true;
		std::cout << "Save? Y/N \t";
		std::cin >> choice;
		if (choice != "N" && choice != "n" && choice != "Y" && choice != "y") {
			do {
				std::cout << "Incorrect input! \t";
				std::cin >> choice;
			} while (choice != "N" && choice != "n" && choice != "Y" && choice != "y");
		}
		if (choice == "N" || choice == "n")
			checkedInput = false;
		if (choice == "Y" || choice == "y")
			checkedInput = true;
		while (checkedInput) {
				std::cout << "Enter filename.\t" << std::endl;
				std::cin >> filename;
				correctFilename = true;
				for (int i = 0; i < filename.length(); i++) {
					//std::cout << (int)filename[i] << "\t";
					if ((int)filename[i] < 0) {
						std::cout << "Spell in english please!\n";
						correctFilename = false;
						break;
					}
				}
				if (correctFilename) {
					Visor::writeImage(this->writing, filename);
					checkedInput = false;
				}
		}
		
	}
	Visor::~Visor()
	{
	
	}
}