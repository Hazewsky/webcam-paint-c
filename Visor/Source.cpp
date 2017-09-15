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

	while (mainLoop)
	{
		draw.programLoop();
		imshow("filter", draw.drawing);
		
		k = waitKey(10);
		draw.keyHandler(k);
		//Check if X is pressed
		//Esc
		if (draw.filterSettingsActive && getWindowProperty(draw.filterSettingsFrameName, 0) < 0) draw.filterSettingsActive 
			= !draw.filterSettingsActive;
		if (draw.brushSettingsActive && getWindowProperty(draw.brushSettingsFrameName, 0) < 0) 
			draw.brushSettingsActive = !draw.brushSettingsActive;
		if (k == 27 || getWindowProperty("filter", 0) < 0)mainLoop = !mainLoop;
		
		//TAB
		if (k == 9) draw.filterSettingsActive = !draw.filterSettingsActive;
		//1
		if (k == 49) draw.brushSettingsActive = !draw.brushSettingsActive;
	}
	draw.endWork();
	return 0;
}