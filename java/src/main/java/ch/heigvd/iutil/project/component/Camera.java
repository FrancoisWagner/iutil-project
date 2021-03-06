package ch.heigvd.iutil.project.component;

// Import the basic graphics classes.  
// The problem here is that we read the image with OpenCV into a Mat object.  
// But OpenCV for java doesn't have the method "imshow", so, we got to use  
// java for that (drawImage) that uses Image or BufferedImage.  
// So, how to go from one the other... Here is the way...  
import java.awt.*;
import java.awt.image.BufferedImage;

import javax.swing.*;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.highgui.VideoCapture;

import ch.heigvd.iutil.project.util.HandDetection;

public class Camera extends JPanel {
	private static final long serialVersionUID = 1L;
	private BufferedImage image;

	// Create a constructor method
	public Camera() {
		super();
	}

	private BufferedImage getImage() {
		return image;
	}

	private void setImage(BufferedImage newimage) {
		image = newimage;
		return;
	}

	/**
	 * Converts/writes a Mat into a BufferedImage.
	 * 
	 * @param matrix
	 *            Mat of type CV_8UC3 or CV_8UC1
	 * @return BufferedImage of type TYPE_3BYTE_BGR or TYPE_BYTE_GRAY
	 */
	public static BufferedImage matToBufferedImage(Mat matrix) {
		int cols = matrix.cols();
		int rows = matrix.rows();
		int elemSize = (int) matrix.elemSize();
		byte[] data = new byte[cols * rows * elemSize];
		int type;
		matrix.get(0, 0, data);
		switch (matrix.channels()) {
		case 1:
			type = BufferedImage.TYPE_BYTE_GRAY;
			break;
		case 3:
			type = BufferedImage.TYPE_3BYTE_BGR;
			// bgr to rgb
			byte b;
			for (int i = 0; i < data.length; i = i + 3) {
				b = data[i];
				data[i] = data[i + 2];
				data[i + 2] = b;
			}
			break;
		default:
			return null;
		}
		BufferedImage image2 = new BufferedImage(cols, rows, type);
		image2.getRaster().setDataElements(0, 0, cols, rows, data);
		return image2;
	}

	public void paintComponent(Graphics g) {
		BufferedImage temp = getImage();

		if(temp != null){
			g.drawImage(temp, temp.getWidth(), 0, 0, temp.getHeight(), 0, 0, temp.getWidth(), temp.getHeight(), this);
		}
	}
	
	public void play() {
		
	}

	public static void main(String arg[]) throws InterruptedException {
		// Load the native library.
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		HandDetection handDetection = new HandDetection();
		JFrame frame = new JFrame("BasicPanel");
		frame.setResizable(false);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(1920, 1080);
		Camera panel = new Camera();
		panel.setSize(1920,1080);
		frame.setContentPane(panel);
		frame.setVisible(true);
		Mat webcam_image = new Mat();
		BufferedImage temp;
		VideoCapture capture = new VideoCapture(0);
		capture.set(Highgui.CV_CAP_PROP_FRAME_WIDTH, 1600);
		capture.set(Highgui.CV_CAP_PROP_FRAME_HEIGHT, 900);
		if (capture.isOpened()) {
			Thread.sleep(1000);
			while (true) {
				capture.read(webcam_image);
				if (!webcam_image.empty()) {
					//frame.setSize(webcam_image.width() + 40,webcam_image.height() + 60);
					//temp = matToBufferedImage(handDetection.getPalmDetect(webcam_image));
					temp = matToBufferedImage(handDetection.getContourBW(webcam_image));
					panel.setImage(temp);
					panel.repaint();
				} else {
					System.out.println(" --(!) No captured frame -- Break!");
					break;
				}
			}
		}
		return;
	}
}