#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>


struct Img
{
	cv::Mat image;
	int x;
	int y;
	int width;
	int height;
	int area;
};

void GetEdgeFromMat(cv::Mat &source, cv::Mat &output, int bulr_value)
{
	cv::Mat edge;
	cvtColor(source, edge, CV_BGR2GRAY);
	blur(edge, edge, cv::Size(bulr_value, bulr_value));
	Canny(edge, edge, 100, 200, 3);
	edge.convertTo(output, CV_8U);
}

int main()
{
	cv::VideoCapture capture(0);
	cv::Mat frame,edgedFrame, binarizationEdgeFrame , clonFrame;
	std::vector<Img> cutedFrmaeList;
	cv::Mat im_with_keypoints;
	if (!capture.isOpened()) {
		std::cerr << "Could not open camera" << std::endl;
		return 0;
	}
	while (true) {
		cutedFrmaeList.clear();
		bool frame_valid = true;
		try {
			capture >> frame;
			clonFrame = frame.clone();
		}
		catch (cv::Exception& e) {
			std::cerr << "Exception occurred. Ignoring frame... " << e.err<< std::endl;
			frame_valid = false;
		}
		cv::Mat  stats, centroids, forConnectArg;
		if (frame_valid) {
			try {
				cv::imshow("cam", clonFrame);
				GetEdgeFromMat(clonFrame, edgedFrame, 3);
				erode(edgedFrame, edgedFrame,0);
				blur(edgedFrame, edgedFrame, cv::Size(3, 3));
				threshold(edgedFrame, binarizationEdgeFrame, 0, 255, CV_THRESH_BINARY);
				cv::imshow("claredge", binarizationEdgeFrame);
				int numOfLables = connectedComponentsWithStats(binarizationEdgeFrame, 
					forConnectArg, stats, centroids, 8, CV_32S);

				std::cout << numOfLables << std::endl;
				for (int j = 1; j < numOfLables; j++) {
					int area = stats.at<int>(j, cv::CC_STAT_AREA);
					int left = stats.at<int>(j, cv::CC_STAT_LEFT); 
					int top = stats.at<int>(j, cv::CC_STAT_TOP);
					int width = stats.at<int>(j, cv::CC_STAT_WIDTH);
					int height = stats.at<int>(j, cv::CC_STAT_HEIGHT);					
					int x = centroids.at<double>(j, 0); 
					int y = centroids.at<double>(j, 1);

					if (area > 400)
					{
						if (0  <= x
						 && 0  <= width
						 && x  +  width <= clonFrame.cols
						 && 0  <= y
						 && 0  <= height
						 && y  +  height <= clonFrame.rows) 
						{
							cv::Rect box(x, y, width, height);	
							cv::Mat region(frame, box);
							Img im;
							im.image = region;
							im.x = left;
							im.y = top;
							im.width = width;
							im.height = height;
							im.area = width * height;
							cutedFrmaeList.push_back(im);
						}
						rectangle(clonFrame, cv::Point(left, top), cv::Point(left + width, top + height),
							cv::Scalar(0, 255,0), 0);
					}
				}
				std::cout << "In List "<<cutedFrmaeList.size() << std::endl;
				cv::imshow("edgeccam", clonFrame);
			}	
			catch (cv::Exception& e) {
				std::cerr << "Exception occurred. Ignoring frame... " << e.err<< std::endl;
			}
		}

		for (int i = 0; i < cutedFrmaeList.size(); i++)
		{
			cv::imshow( ""+ i , cutedFrmaeList.at(i).image);
		}

		if (cv::waitKey(30) >= 0) break;
	}
	cv::waitKey(0);
	return 0;
}