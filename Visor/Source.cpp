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
		imshow("filter", draw.getDrawing());
		k = waitKey(10);
		draw.keyHandler(k);
		
		//Esc
		if (k == 27 || cv::getWindowProperty("filter", 0) < 0)mainLoop = !mainLoop;
	}
	draw.endWork();
	return 0;
}