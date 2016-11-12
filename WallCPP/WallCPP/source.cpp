#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
using namespace cv;

void GetEdgeFromImage(Mat &source, Mat &output, int bulr_value)
{
	Mat gray, image;
	cvtColor(source, gray, CV_BGR2GRAY);
	Mat result;

	blur(gray, result, Size(bulr_value, bulr_value));
	Mat edgedimage, edge;
	Canny(result, edge, 100, 200, 3);
	edge.convertTo(output, CV_8U); //여백이 부족하여 설명을 달지 않겠다.
}
void Blob(Mat & input, Mat &output)
{
	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 100;
	// Filter by Area.

	std::vector<KeyPoint> keypoints;


#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

	// Set up detector with params
	SimpleBlobDetector detector(params); 

	// Detect blobs
	detector.detect(input, keypoints);
#else 

	// Set up detector with params
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	// Detect blobs
	detector->detect(input, keypoints);
#endif 


	drawKeypoints(input, keypoints, output, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

}
int main()
{
	cv::Mat frame,edgedFrame, claredge , copyed;
	cv::VideoCapture capture(0);

	std::vector<Mat> smallImages;
	Mat im_with_keypoints;
	if (!capture.isOpened()) {
		std::cerr << "Could not open camera" << std::endl;
		return 0;
	}
	while (true) {
		bool frame_valid = true;

		smallImages.clear();

		try {
			capture >> frame; // get a new frame from webcam
			copyed = frame;
		}
		catch (cv::Exception& e) {
			std::cerr << "Exception occurred. Ignoring frame... " << e.err
				<< std::endl;
			frame_valid = false;
		}

		Mat  stats, centroids, eeeee;
		if (frame_valid) {
			try {
				cv::imshow("cam", frame);
				GetEdgeFromImage(frame, edgedFrame, 4);
				erode(edgedFrame, edgedFrame,0);
				blur(edgedFrame, edgedFrame, Size(3, 3));
				Blob(edgedFrame, claredge);
				threshold(edgedFrame, claredge, 0, 255, CV_THRESH_BINARY);
				cv::imshow("claredge", claredge);
				
				int numOfLables = connectedComponentsWithStats(claredge, eeeee,
					stats, centroids, 8, CV_32S);
				std::cout << numOfLables << std::endl;
				for (int j = 1; j < numOfLables; j++) {
					int area = stats.at<int>(j, CC_STAT_AREA);
					int left = stats.at<int>(j, CC_STAT_LEFT); 
					int top = stats.at<int>(j, CC_STAT_TOP);
					int width = stats.at<int>(j, CC_STAT_WIDTH);
					int height = stats.at<int>(j, CC_STAT_HEIGHT);
					
					int x = centroids.at<double>(j, 0); //중심좌표
					int y = centroids.at<double>(j, 1);
					cv::Rect box(x, y, width, height);
					if (0 <= x
						&& 0 <= width
						&& x + width <= copyed.cols
						&& 0 <= y
						&& 0 <= height
						&& y + height <= copyed.rows) {
						// box within the image plane
						Mat region(copyed, box);
						smallImages.push_back(region);
					}
					rectangle(frame, Point(left, top), Point(left + width, top + height),
						Scalar(0, 255,0), 0);

			

				
				}
				std::cout << "T "<<smallImages.size() << std::endl;
				cv::imshow("edgeccam", frame);
			}	

			catch (cv::Exception& e) {
				std::cerr << "Exception occurred. Ignoring frame... " << e.err
					<< std::endl;
			}
		}

		for (int i = 0; i < smallImages.size(); i++)
		{
			cv::imshow( ""+ i, smallImages.at(i));
		}

		if (cv::waitKey(30) >= 0) break;
	}

	waitKey(0);
	return 0;
}