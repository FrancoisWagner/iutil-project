package ch.heigvd.iutil.project;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.objdetect.CascadeClassifier;

public class Utils {
	
	public static Mat getMainImageDetect(Mat image){
		image = getImageFaceDetect(image);
		image = getFistDetect(image);
		image = getPalmDetect(image);
		return image;
	}
	
	public static  Mat getImageFaceDetect(Mat image){
		System.out.println("\nRunning DetectFaceDemo");

	    // Create a face detector from the cascade file in the resources
	    // directory.
	    CascadeClassifier faceDetector = new CascadeClassifier("src/main/resources/lbpcascade_frontalface.xml");

	    // Detect faces in the image.
		// MatOfRect is a special container class for Rect.
		MatOfRect faceDetections = new MatOfRect();
		faceDetector.detectMultiScale(image, faceDetections);

		System.out.println(String.format("Detected %s faces", faceDetections.toArray().length));

		if(faceDetections.toArray().length > 0){
			// Draw a bounding box around each face.
			for (Rect rect : faceDetections.toArray()) {
			    Core.rectangle(image, new Point(rect.x, rect.y), new Point(rect.x + rect.width, rect.y + rect.height), new Scalar(0, 255, 0));
			}
		}
	    
	    return image;
	}
	
	public static  Mat getFistDetect(Mat image){
		System.out.println("\nRunning FistDetect");

	    // Create a face detector from the cascade file in the resources
	    // directory.
	    CascadeClassifier faceDetector = new CascadeClassifier("src/main/resources/fist.xml");

	    // Detect faces in the image.
		// MatOfRect is a special container class for Rect.
		MatOfRect faceDetections = new MatOfRect();
		faceDetector.detectMultiScale(image, faceDetections);

		System.out.println(String.format("Detected %s fist", faceDetections.toArray().length));

		if(faceDetections.toArray().length > 0){
			// Draw a bounding box around each face.
			for (Rect rect : faceDetections.toArray()) {
			    Core.rectangle(image, new Point(rect.x, rect.y), new Point(rect.x + rect.width, rect.y + rect.height), new Scalar(255, 0, 0));
			}
		}
	    
	    return image;
	}
	
	public static  Mat getPalmDetect(Mat image){
		System.out.println("\nRunning PalmDetect");

	    // Create a face detector from the cascade file in the resources
	    // directory.
	    CascadeClassifier faceDetector = new CascadeClassifier("src/main/resources/palm.xml");

	    // Detect faces in the image.
		// MatOfRect is a special container class for Rect.
		MatOfRect faceDetections = new MatOfRect();
		faceDetector.detectMultiScale(image, faceDetections);

		System.out.println(String.format("Detected %s palm", faceDetections.toArray().length));

		if(faceDetections.toArray().length > 0){
			// Draw a bounding box around each face.
			for (Rect rect : faceDetections.toArray()) {
			    Core.rectangle(image, new Point(rect.x, rect.y), new Point(rect.x + rect.width, rect.y + rect.height), new Scalar(0, 0, 255));
			}
		}
	    
	    return image;
	}
}
