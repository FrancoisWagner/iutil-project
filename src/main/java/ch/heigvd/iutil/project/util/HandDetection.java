package ch.heigvd.iutil.project.util;

import java.util.ArrayList;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

public class HandDetection {
	private Imgproc imgproc;
	
	public HandDetection() {
		imgproc = new Imgproc();
	}
	
	public Mat getContourBW(Mat image){
		Mat src_gray  = new Mat();
		Mat threshold_output  = new Mat();
		java.util.List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
		java.util.List<MatOfInt> hulls = new ArrayList<MatOfInt>();
		Mat hierarchy = new Mat();
		
		imgproc.cvtColor(image, src_gray,imgproc.COLOR_BGR2GRAY );
		imgproc.blur(src_gray, src_gray, new Size(3,3));
		imgproc.threshold(src_gray, threshold_output, 100, 255, imgproc.THRESH_BINARY);
		imgproc.findContours(threshold_output, contours, hierarchy,imgproc.RETR_TREE , imgproc.CHAIN_APPROX_SIMPLE, new Point(0,0));
		
		for (MatOfPoint contour : contours) {
			MatOfInt hull =new MatOfInt();
			imgproc.convexHull(contour, hull,false);
			hulls.add(hull);
		}
		
		int i = 0;
		Mat draw = new Mat(threshold_output.size(),CvType.CV_8UC3);
		for (MatOfPoint contour : contours) {
			Scalar color = new Scalar(255,255,255);
			imgproc.drawContours(draw, contours, i, color);
			i++;
		}
	    return draw;
	}

}
