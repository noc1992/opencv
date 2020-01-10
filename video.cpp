#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>	

using namespace cv;
using namespace std;

int main()
{
	Mat frame;

	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cerr << "카메라를 열 수 없습니다." << endl;
		return -1;
	}

	int fps = cap.get(CAP_PROP_FPS); //동영상 캡쳐 방법
	int width = cap.get(CAP_PROP_FRAME_WIDTH);
	int height = cap.get(CAP_PROP_FRAME_HEIGHT);
	int fourcc = VideoWriter::fourcc('X', 'V', 'I', 'D');

	VideoWriter outputVideo;
	outputVideo.open("output.avi", fourcc, fps, Size(width, height), true);
	if (!outputVideo.isOpened())
	{
		cerr << "동영상 저장을 위한 초기화중 에러" << endl;
		return -1;
	}
	

	while (1)
	{
		cap.read(frame);
		if (frame.empty())
		{
			cerr << "캡쳐 실패" << endl;
			break;
		}

		cvtColor(frame, frame, COLOR_BGR2GRAY); // 이미지 그레이 스케일로
		imshow("Live", frame);

		outputVideo.write(frame);

		int wait = int(1.0 / fps * 1000);
		if (waitKey(wait) >= 0)
			break;
	}

	return 0;
	 
}