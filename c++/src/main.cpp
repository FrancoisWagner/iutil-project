/*	Title     : HandGestureDetectionUI
 * 	Autors    : François Wagner & Julien Piccaluga
 * 	Date      : 18.11.2013
 *  Comments  : This code has been released for our project IUtil.
 */
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/video/tracking.hpp"
#include <iostream>
#include <windows.h>

using namespace cv;
using namespace std;

const int DEBUG = false;

Mat frame,imgSkin;;
int old = 0;
RNG rng(12345);
String hand_cascade_name = "C:/dev/iutil-project/c++/src/lbpcascade_frontalface.xml";
CascadeClassifier face_cascade;

Mat detectSkin(Mat frame) {
	Mat hsv, imgSkin;
	Mat hue, sat, val;
	Mat hue_plane, sat_plane, val_plane, blue_plane;

	cvtColor(frame, hsv, CV_BGR2HSV);

	hue.create(hsv.size(), hsv.depth());
	sat.create(hsv.size(), hsv.depth());
	val.create(hsv.size(), hsv.depth());
	int ch_hue[] = { 0, 0 };
	int ch_sat[] = { 1, 0 };
	int ch_val[] = { 2, 0 };
	mixChannels(&hsv, 1, &hue, 1, ch_hue, 1);
	mixChannels(&hsv, 1, &sat, 1, ch_sat, 1);
	mixChannels(&hsv, 1, &val, 1, ch_val, 1);

	threshold(hue, hue_plane, 18, UCHAR_MAX, CV_THRESH_BINARY_INV);
	threshold(sat, sat_plane, 50, UCHAR_MAX, CV_THRESH_BINARY);
	threshold(val, val_plane, 80, UCHAR_MAX, CV_THRESH_BINARY);

	imgSkin.create(frame.size(), frame.depth());
	bitwise_and(hue_plane, sat_plane, imgSkin);
	bitwise_and(imgSkin, val_plane, imgSkin);
	if(DEBUG == true) {
		imshow("Hue", hue);
		imshow("Sat", sat);
		imshow("Val", val);
		imshow("Hue_plane", hue_plane);
		imshow("Sat_plane", sat_plane);
		imshow("Val_plane", val_plane);
	}

	return imgSkin;
}

void detectAndHideFace(Mat frame) {
	std::vector<Rect> rect;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame, rect, 1.1, 2, 0, Size(50, 50),
			Size(400, 400));

	for (unsigned int i = 0; i < rect.size(); i++) {
		rectangle(frame, Point(rect[i].x, rect[i].y - 80),
				Point(rect[i].x + rect[i].width,
						rect[i].y + rect[i].height + 80), Scalar(0, 0, 0),
				CV_FILLED);
	}
}

void detectContour(Mat img) {
	Mat drawing = Mat::zeros(img.size(), CV_8UC3);
	vector<vector<Point> > contours;
	vector<vector<Point> > bigContours;
	vector<Vec4i> hierarchy;

	findContours(img, contours, hierarchy, cv::RETR_LIST,
			cv::CHAIN_APPROX_SIMPLE, Point());
	int cpt_finger = 0;
	if (contours.size() > 0) {
		vector<vector<int> > hull(contours.size());
		vector<vector<Vec4i> > convDef(contours.size());
		vector<vector<Point> > hull_points(contours.size());
		vector<vector<Point> > defect_points(contours.size());

		for (int i = 0; i < contours.size(); i++) {
			if (contourArea(contours[i]) > 5000) {
				convexHull(contours[i], hull[i], false);
				convexityDefects(contours[i], hull[i], convDef[i]);

				// start_index, end_index, farthest_pt_index, fixpt_depth

				for (int k = 0; k < hull[i].size(); k++) {
					int ind = hull[i][k];
					hull_points[i].push_back(contours[i][ind]);
				}

				for (int k = 0; k < convDef[i].size(); k++) {
					if (convDef[i][k][3] > 20 * 256) {
						cpt_finger++;
						int ind_0 = convDef[i][k][0];
						int ind_1 = convDef[i][k][1];
						int ind_2 = convDef[i][k][2];
						defect_points[i].push_back(contours[i][ind_2]);
						cv::circle(drawing, contours[i][ind_0], 5,
								Scalar(0, 255, 0), -1);
						cv::circle(drawing, contours[i][ind_1], 5,
								Scalar(0, 255, 0), -1);
						cv::circle(drawing, contours[i][ind_2], 5,
								Scalar(0, 0, 255), -1);
						cv::line(drawing, contours[i][ind_2],
								contours[i][ind_0], Scalar(0, 0, 255), 1);
						cv::line(drawing, contours[i][ind_2],
								contours[i][ind_1], Scalar(0, 0, 255), 1);
					}
				}

				drawContours(drawing, contours, i, Scalar(0, 255, 0), 1, 8,
						vector<Vec4i>(), 0, Point());
				drawContours(drawing, hull_points, i, Scalar(255, 0, 0), 1,
						8, vector<Vec4i>(), 0, Point());
			}

		}
		if (cpt_finger == 0 && old != cpt_finger) {
			cout << "Nombre de doight = " << cpt_finger << endl;
		}
		if (old != cpt_finger) {
			cout << "Nombre de doight = " << cpt_finger + 1 << endl;
		}

		old = cpt_finger;
	}
	namedWindow("Hand Detection", cv::WINDOW_AUTOSIZE);
	imshow("Hand Detection", drawing);

}

int main(int argc, char **argv) {

	// Open the video camera no.0
	VideoCapture cam(0);

	// Wait for cam initialisation
	Sleep(1000);

	// Controle if video stream has been opened correctly
	if (!cam.isOpened()) {
		cout << "Cannot open video stream" << endl;
	}

	//Load the cascades
	if (!face_cascade.load(hand_cascade_name)) {
		cout << "--(!)Error loading\n";
		return -1;
	};

	while (1) {

		bool isReadSuccess = cam.read(frame);

		if (!isReadSuccess) {
			cout << "Cannot read a frame from video file" << endl;
			break;
		}
		flip(frame, frame, 1);
		detectAndHideFace(frame);
		blur(frame, frame, Size(5, 5));

		imgSkin = detectSkin(frame);

		detectContour(imgSkin);

		if (waitKey(30) == 27) {
			cout << " esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}
