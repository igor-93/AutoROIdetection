#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

bool myComparison(const pair<int,int> &a,const pair<int,int> &b)
{
	 return a.first > b.first;
}


/* laplacian
int main( int argc, char** argv )
{
	Mat src, src_gray, dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	char* window_name = "Laplace Demo";

	int c;

	/// Load an image
	src = imread( filename );

	if( !src.data )
	{ return -1; }

	/// Remove noise by blurring with a Gaussian filter
	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

	/// Convert the image to grayscale
	cvtColor( src, src_gray, CV_BGR2GRAY );

	/// Create window
	namedWindow( window_name, CV_WINDOW_NORMAL );

	/// Apply Laplace function
	Mat abs_dst;

	Laplacian( src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( dst, abs_dst );

	/// Show what you got
	imshow( window_name, abs_dst );

	waitKey(0);

	return 0;
}*/

// sobel
int main( int argc, char** argv )
{
	Mat src, src_gray, dst;
	Mat grad;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	int c;

    string filename = "/home/igorpesic/ClionProjects/AutoROIdetection/test6.bmp";
	/// Load an image
	src = imread(filename);

	if( !src.data )
	{ return -1; }

	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

	/// Convert it to gray
	cvtColor( src, src_gray, CV_BGR2GRAY );

	double claheSize = 1.8;
	Ptr<CLAHE> clahe = createCLAHE(claheSize, Size(claheSize, claheSize));
	Mat outputImg;
	clahe->apply(src_gray, src_gray);

	/// Create window
	namedWindow( "after clahe", CV_WINDOW_NORMAL );
	imshow( "after clahe", src_gray );
	waitKey(0);

	/// Create window
	namedWindow( window_name, CV_WINDOW_NORMAL );

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel( src_gray, grad_x, ddepth, 4, 0, 5, scale, delta, BORDER_ISOLATED );
	convertScaleAbs( grad_x, abs_grad_x );

	// filter dark pixels
	inRange(abs_grad_x, 150, 255, abs_grad_x);

	Mat col_sum;
	cv::reduce(abs_grad_x, col_sum, 0, CV_REDUCE_SUM, CV_32S);
	cout << "columns sumed up" << endl;
	cout << col_sum << endl;
	ofstream myfile;
	myfile.open ("/home/igorpesic/ClionProjects/AutoROIdetection/example.txt");
	myfile << col_sum;
	myfile.close();

	// save 30 cols with biggest values
	int size_to_save = 50;
	vector<pair<int,int> > max(size_to_save);
	for(int i = 0; i < col_sum.cols; i++){
		if(col_sum.at<int>(i) > max[size_to_save-1].first){
			max[size_to_save-1].first = col_sum.at<int>(i);
			max[size_to_save-1].second = i;
			sort(max.begin(), max.end(), myComparison);
		}
	}

	int line_col_1, line_col_2;	// columns of the two ROI lines
	bool line_col_2_found = false;
	bool left_found = false;
	for(int i = 0; i < size_to_save; i++){
		if(abs(max[i].second - abs_grad_x.cols) > 5 && max[i].second > 5)
		{
			line_col_1 = max[i].second;
			cout << "line_col_1 = " << line_col_1 << endl;
			break;
		}
	}
	if(line_col_1 < round(abs_grad_x.cols / 2.0))
		left_found = true;
	for(int i = 0; i < size_to_save; i++)
	{
		cout << "max: " << max[i].first << " at place " <<  max[i].second <<  endl;
		if(abs(max[i].second - max[0].second) > 10 && !line_col_2_found
				&& abs(max[i].second - abs_grad_x.cols) > 5 && max[i].second > 5	// must be at least 5 px away from the edge
				&& ((left_found && max[i].second > round(abs_grad_x.cols / 2.0))
					 || (!left_found && max[i].second < round(abs_grad_x.cols / 2.0)) ) )
		{
			line_col_2 = max[i].second;
			line_col_2_found = true;
			cout << "line_col_2: " << line_col_2 << endl;
		}

	}


	imshow( window_name, abs_grad_x );
	Point ptUL(line_col_1, 0);
	Point ptUR(line_col_2, 0);
	Point ptDL(line_col_1, src.rows);
	Point ptDR(line_col_2, src.rows);
	line(src, ptUL, ptDL, Scalar(255,0,0));
	line(src, ptUR, ptDR, Scalar(0,255,0));
	waitKey(0);
	namedWindow( "ROI", CV_WINDOW_NORMAL );
	imshow( "ROI", src );
	waitKey(0);
	return 0;
}


