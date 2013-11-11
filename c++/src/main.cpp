/*
 * main.cpp

 *
 *  Created on: 6 nov. 2013
 *      Author: JUL
 */

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include <iostream>
#include <windows.h>

using namespace cv;
using namespace std;

const int nmixtures = 9;
const bool shadow = false;
const int history = 1000;

BackgroundSubtractorMOG2 bgs(history, nmixtures, shadow);

/** Global variables */
String hand_cascade_name =
		"C:/Users/JUL/dev/iutil-project/c++/src/lbpcascade_frontalface.xml";
CascadeClassifier hand_cascade;

// took from https://github.com/bytefish/opencv/blob/master/misc/skin_color.cpp
bool R1(int R, int G, int B) {
	bool e1 = (R > 95) && (G > 40) && (B > 20)
			&& ((max(R, max(G, B)) - min(R, min(G, B))) > 15)
			&& (abs(R - G) > 15) && (R > G) && (R > B);
	bool e2 = (R > 220) && (G > 210) && (B > 170) && (abs(R - G) <= 15)
			&& (R > B) && (G > B);
	return (e1 || e2);
}

// took from https://github.com/bytefish/opencv/blob/master/misc/skin_color.cpp
bool R2(float Y, float Cr, float Cb) {
	bool e3 = Cr <= 1.5862 * Cb + 20;
	bool e4 = Cr >= 0.3448 * Cb + 76.2069;
	bool e5 = Cr >= -4.5652 * Cb + 234.5652;
	bool e6 = Cr <= -1.15 * Cb + 301.75;
	bool e7 = Cr <= -2.2857 * Cb + 432.85;
	return e3 && e4 && e5 && e6 && e7;
}

// took from https://github.com/bytefish/opencv/blob/master/misc/skin_color.cpp
bool R3(float H, float S, float V) {
	return (H < 25) || (H > 230);
}

// took from https://github.com/bytefish/opencv/blob/master/misc/skin_color.cpp
Mat ThresholdSkin(const Mat &src) {
	// Allocate the result matrix
	Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC1);
	// We operate in YCrCb and HSV:
	Mat src_ycrcb, src_hsv;
	// OpenCV scales the YCrCb components, so that they
	// cover the whole value range of [0,255], so there's
	// no need to scale the values:
	cvtColor(src, src_ycrcb, CV_BGR2YCrCb);
	// OpenCV scales the Hue Channel to [0,180] for
	// 8bit images, make sure we are operating on
	// the full spectrum from [0,360] by using floating
	// point precision:
	src.convertTo(src_hsv, CV_32FC3);
	cvtColor(src_hsv, src_hsv, CV_BGR2HSV);
	// And then scale between [0,255] for the rules in the paper
	// to apply. This uses normalize with CV_32FC3, which may fail
	// on older OpenCV versions. If so, you probably want to split
	// the channels first and call normalize independently on each
	// channel:
	normalize(src_hsv, src_hsv, 0.0, 255.0, NORM_MINMAX, CV_32FC3);
	// Iterate over the data:
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			// Get the pixel in BGR space:
			Vec3b pix_bgr = src.ptr<Vec3b>(i)[j];
			int B = pix_bgr.val[0];
			int G = pix_bgr.val[1];
			int R = pix_bgr.val[2];
			// And apply RGB rule:
			bool a = R1(R, G, B);
			// Get the pixel in YCrCB space:
			Vec3b pix_ycrcb = src_ycrcb.ptr<Vec3b>(i)[j];
			int Y = pix_ycrcb.val[0];
			int Cr = pix_ycrcb.val[1];
			int Cb = pix_ycrcb.val[2];
			// And apply the YCrCB rule:
			bool b = R2(Y, Cr, Cb);
			// Get the pixel in HSV space:
			Vec3f pix_hsv = src_hsv.ptr<Vec3f>(i)[j];
			float H = pix_hsv.val[0];
			float S = pix_hsv.val[1];
			float V = pix_hsv.val[2];
			// And apply the HSV rule:
			bool c = R3(H, S, V);
			// If not skin, then black
			if (a && b && c) {
				dst.at<unsigned char>(i, j) = 255;
			}
		}
	}
	return dst;
}

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	hand_cascade.detectMultiScale(frame_gray, faces, 1.1, 2,
			0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (unsigned int i = 0; i < faces.size(); i++) {
		rectangle(frame, Point(faces[i].x, faces[i].y),
				Point(faces[i].x + faces[i].width,
						faces[i].y + faces[i].height), Scalar(0, 0, 0),
				CV_FILLED);
	}
	//-- Show what you got
	imshow("Hand Gesture Detection 2", frame);
}

int main(int argc, char **argv) {

	Mat background, foreground;

	// Open the video camera no.0
	VideoCapture cam(0);

	// Wait for cam initialisation
	Sleep(1000);

	// Controle if video stream has been opened correctly
	if (!cam.isOpened()) {
		cout << "Cannot open video stream" << endl;
	}

	namedWindow("Hand Gesture Detection", CV_WINDOW_AUTOSIZE);
	namedWindow("FOREGROUND");

	//-- 1. Load the cascades
	if (!hand_cascade.load(hand_cascade_name)) {
		cout << "--(!)Error loading\n";
		return -1;
	};

	while (1) {

		Mat frame;

		bool isReadSuccess = cam.read(frame);

		if (!isReadSuccess) {
			cout << "Cannot read a frame from video file" << endl;
			break;
		}

		flip(frame, frame, 1);

		//Substract BackGround
		bgs.operator()(frame, foreground);
		bgs.getBackgroundImage(background);

		detectAndDisplay(frame);

		blur(frame, frame, Size(15, 15));

		Mat skin = ThresholdSkin(frame);

		int dilation_size = 1;
		int erosion_size = 1;

		dilate(skin, skin,
				getStructuringElement(MORPH_RECT,
						Size(2 * dilation_size + 1, 2 * dilation_size + 1),
						Point(dilation_size, dilation_size)));

		erode(skin, skin,
				getStructuringElement(MORPH_RECT,
						Size(2 * erosion_size + 1, 2 * erosion_size + 1),
						Point(erosion_size, erosion_size)));

		for (int i = 0; i < skin.rows; i++) {
			for (int j = 0; j < skin.cols; j++) {
				if (foreground.at<unsigned char>(i, j) == 0) {
					skin.at<unsigned char>(i, j) = 0;
				}
			}
		}

		imshow("Hand Gesture Detection", skin);
		imshow("FOREGROUND", foreground);

		if (waitKey(30) == 27) {
			cout << " esc key is pressed by user" << endl;
			break;
		}

	}
	return 0;
}

