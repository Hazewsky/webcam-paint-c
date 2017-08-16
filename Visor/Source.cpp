#include "Visor.h"

using namespace std;
using namespace cv;
using namespace Visoring;

int main(){
	std::cout << "Controls:" << std::endl
		<< "Space - Enter/Exit drawing mode" << std::endl
		<< "1 (Dont' ask) - Enter/Exit brush settings" << std::endl
		<< "TAB - Enter/Exit filter calibration mode" << std::endl
		<< "` - reset HSV filter" << std::endl 
		<< "~ - Clear drawing screen" << std::endl;
	Visor draw;
	int k;
	bool mainLoop = true;
	bool settingsActive = false;
	bool brushSettingsActive = false;
	while (mainLoop)
	{
		draw.onDrawSettings(draw.createDrawSettings(),brushSettingsActive);
		draw.cap.read(draw.frame);
		flip(draw.frame, draw.frame, 1);
		cvtColor(draw.frame, draw.hsv, CV_BGR2HSV);
		inRange(draw.hsv, draw.lowerColor, draw.upperColor, draw.colorRange);

		draw.filteredFrame = Mat::zeros(draw.frame.size(), CV_8UC1);
		bitwise_and(draw.frame, draw.frame, draw.filteredFrame, draw.colorRange);
		
		draw.onFilterSettings(draw.filterSettingsFrameName, draw.filteredFrame, settingsActive);
		GaussianBlur(draw.filteredFrame, draw.blurredFrame, Size(3, 3), 2);
		cvtColor(draw.blurredFrame, draw.grayScale, CV_BGR2GRAY);
		threshold(draw.grayScale, draw.thresh, 200, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
		erode(draw.thresh, draw.eroded, draw.kernel);
		morphologyEx(draw.eroded, draw.openingFrame, MORPH_OPEN, draw.kernel, Point(-1, -1), 5);

		distanceTransform(draw.openingFrame, draw.distTransform, CV_L2, CV_DIST_MASK_PRECISE);
		normalize(draw.distTransform, draw.normalized, 0.0, 1.0, NORM_MINMAX);
		Mat cont;
		Mat dr = Mat::zeros(draw.frame.size(), CV_8UC3);
		draw.normalized.convertTo(cont, CV_8UC1);
		//draw.openingFrame.convertTo(cont, CV_8UC1);
		findContours(cont, draw.contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		
		vector<Moments> mu(draw.contours.size());
		for (size_t i = 0; i < draw.contours.size(); i++){
			mu[i] = moments(draw.contours[i], false);
		}
		vector<Point2f> centrs(draw.contours.size());
		for (size_t i = 0; i < draw.contours.size(); i++){
			centrs[i] = Point2f(static_cast<float>(mu[i].m10 / mu[i].m00), static_cast<float>(mu[i].m01 / mu[i].m00));
			if (contourArea(draw.contours[i]) > 1500 && arcLength(draw.contours[i], false) > 100){
				draw.drawCenter = centrs[i];
			}
		}
		for (size_t i = 0; i < draw.contours.size(); i++){
			//radius
			drawContours(dr, draw.contours, (int)i, CV_RGB(255, 0, 0), 3);
			//cout << contourArea(draw.contours[i]) << endl;
			circle(dr, draw.drawCenter, 3, CV_RGB(0, 0, 255), -1);
		}
		draw.onDraw(draw.drawing, draw.buffer, draw.writing, draw.drawMode);
		
		imshow("filter", draw.drawing);
		
		k = waitKey(10);
		draw.keyHandler(k);
		//Check if X is pressed
		//Esc
		if (settingsActive && getWindowProperty(draw.filterSettingsFrameName, 0) < 0) settingsActive = !settingsActive;
		if (brushSettingsActive && getWindowProperty(draw.brushSettingsFrameName, 0) < 0) 
			brushSettingsActive = !brushSettingsActive;
		if (k == 27 || getWindowProperty("filter", 0) < 0)mainLoop = !mainLoop;
		
		//TAB
		if (k == 9) settingsActive = !settingsActive;
		//1
		if (k == 49) brushSettingsActive = !brushSettingsActive;
	}
	draw.endWork();
	return 0;
}