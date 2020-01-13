#include <opencv2/core.hpp>	
#include <opencv2/imgproc.hpp>	
#include <opencv2/videoio.hpp>	
#include <opencv2/highgui.hpp>	
#include <iostream>

using namespace cv;
using namespace std;

void dummy(int, void*) // 트랙바 콜백함수
{

}


int threshold1 = 30;

Vec3b lower_blue1, upper_blue1, lower_blue2, upper_blue2, lower_blue3, upper_blue3;
Mat img_color;

void mouse_callback(int event, int x, int y, int flags, void* param)  //클릭한 픽셀의 정보를 받아서 가져옴
{
	if (event == EVENT_LBUTTONDOWN) // 마우스 왼쪽 버튼 누르는 이벤트
	{
		Vec3b color_pixel = img_color.at<Vec3b>(y, x);  // 클릭한 위치 픽셀 읽어오기

		Mat bgr_color = Mat(1, 1, CV_8UC3, color_pixel); // 한픽셀 이미지로 바꿈

		Mat hsv_color;
		cvtColor(bgr_color, hsv_color, COLOR_BGR2HSV); // 이미지를 비주얼 색공간에서 hsv 색공간으로

		int hue = hsv_color.at<Vec3b>(0, 0)[2];
		int saturation = hsv_color.at<Vec3b>(0, 0)[2];
		int value = hsv_color.at<Vec3b>(0, 0)[2];

		cout << "hue = " << hue << endl; // hue 값을 정수변수에 대입
		cout << "saturation = " << saturation << endl;
		cout << "value = " << value << endl;

		if (hue < 10) // 색의 범위지정
		{
			cout << "case 1" << endl;
			lower_blue1 = Vec3b(hue - 10 + 180, threshold1, threshold1);
			upper_blue1 = Vec3b(180, 255, 255);
			lower_blue2 = Vec3b(0, threshold1, threshold1);
			upper_blue2 = Vec3b(hue, 255, 255);
			lower_blue3 = Vec3b(hue, threshold1, threshold1);
			upper_blue3 = Vec3b(hue + 10, 255, 255);
		}
		else if (hue > 170)
		{
			cout << "case 2" << endl;
			lower_blue1 = Vec3b(hue, threshold1, threshold1);
			upper_blue1 = Vec3b(180, 255, 255);
			lower_blue2 = Vec3b(0, threshold1, threshold1);
			upper_blue2 = Vec3b(hue + 10 - 180, 255, 255);
			lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue3 = Vec3b(hue, 255, 255);
		}
		else
		{
			cout << "case 3" << endl;
			lower_blue1 = Vec3b(hue, threshold1, threshold1);
			upper_blue1 = Vec3b(hue + 10, 255, 255);
			lower_blue2 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue2 = Vec3b(hue, 255, 255);
			lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue3 = Vec3b(hue, 255, 255);
		}
		cout << "hue = " << hue << endl;
		cout << "#1 = " << lower_blue1 << "~" << upper_blue1 << endl;
		cout << "#2 = " << lower_blue2 << "~" << upper_blue2 << endl;
		cout << "#3 = " << lower_blue3 << "~" << upper_blue3 << endl;
	}
}

int main()
{
	namedWindow("img_color");
	setMouseCallback("img_color", mouse_callback);
	createTrackbar("threshold", "img_color", &threshold1, 255, dummy); //트랙바 생성
	setTrackbarPos("threshold", "img_color", 30); // 트랙바 초기값

	Mat img_hsv;

	VideoCapture cap(0); // 이미지 가져오기
	if (!cap.isOpened()) {
		cout << "카메라를 열 수 없습니다." << endl;
		return -1;
	}
	while (1)
	{
		cap.read(img_color);

		threshold1 = getTrackbarPos("threshold", "img_color"); //콜백함수에서 범위값 업데이트에 사용
		
		cvtColor(img_color, img_hsv, COLOR_BGR2HSV);

		Mat img_mask1, img_mask2, img_mask3, img_mask;
		inRange(img_hsv, lower_blue1, upper_blue1, img_mask1);
		inRange(img_hsv, lower_blue2, upper_blue2, img_mask2);
		inRange(img_hsv, lower_blue3, upper_blue3, img_mask3);
		img_mask = img_mask1 | img_mask2 | img_mask3;  // 이미지에서 마스크 이미지 생성 바이너리 이미지

		int morph_size = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1),
			Point(morph_size, morph_size));
		morphologyEx(img_mask, img_mask, MORPH_OPEN, element);
		morphologyEx(img_mask, img_mask, MORPH_CLOSE, element);
		// 오프닝(작은흰색영역 노이즈 제거)과 클로징(흰색영역내부의 검은색 영역 제거) 추가

		//마스크 이미지로 원본 이미지에서 범위값에 해당되는 영상부분을 획득
		Mat img_result;
		bitwise_and(img_color, img_color, img_result, img_mask);

		Mat img_labels, stats, centroids;
		int numOfLabels = connectedComponentsWithStats(img_mask, img_labels, stats, centroids, 8, CV_32S); // 바이너리 이미지 라벨링
		for (int j = 1; j < numOfLabels; j++)
		{
			int area = stats.at<int>(j, CC_STAT_AREA);
			int left = stats.at<int>(j, CC_STAT_LEFT);
			int top = stats.at<int>(j, CC_STAT_TOP);
			int width = stats.at<int>(j, CC_STAT_WIDTH);
			int height = stats.at<int>(j, CC_STAT_HEIGHT);

			int centerX = centroids.at<double>(j, 0);
			int centerY = centroids.at<double>(j, 1);

			if (area > 100) //영역 크기가 100 이상인 경우에만 중심좌표와 외곽 사각형그려주는 코드
			{
				circle(img_color, Point(centerX, centerY), 5, Scalar(255, 0, 0), 1);
				rectangle(img_color, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 1);
			}
		} //라벨링 구간 정의

		imshow("img_color", img_color);
		imshow("img_mask", img_mask);
		imshow("img_result", img_result);

		if (waitKey(1) > 0)
			break;

	}
	return 0;
}