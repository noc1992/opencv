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
		cerr << "ī�޶� �� �� �����ϴ�." << endl;
		return -1;
	}

	int fps = cap.get(CAP_PROP_FPS); //������ ĸ�� ���
	int width = cap.get(CAP_PROP_FRAME_WIDTH);
	int height = cap.get(CAP_PROP_FRAME_HEIGHT);
	int fourcc = VideoWriter::fourcc('X', 'V', 'I', 'D');

	VideoWriter outputVideo;
	outputVideo.open("output.avi", fourcc, fps, Size(width, height), true);
	if (!outputVideo.isOpened())
	{
		cerr << "������ ������ ���� �ʱ�ȭ�� ����" << endl;
		return -1;
	}
	

	while (1)
	{
		cap.read(frame);
		if (frame.empty())
		{
			cerr << "ĸ�� ����" << endl;
			break;
		}

		cvtColor(frame, frame, COLOR_BGR2GRAY); // �̹��� �׷��� �����Ϸ�
		imshow("Live", frame);

		outputVideo.write(frame);

		int wait = int(1.0 / fps * 1000);
		if (waitKey(wait) >= 0)
			break;
	}

	return 0;
	 
}