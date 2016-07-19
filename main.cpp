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

bool myComparisonD(const pair<double,int> &a,const pair<double,int> &b)
{
	return a.first > b.first;
}

// sobel
int main( int argc, char** argv )
{
	Mat src, src_gray, dst;
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

	// get gradient of col_sum
	vector<int> grad(col_sum.cols-1, 0);
	for(int i = 0; i < grad.size(); i++)
	{
		grad[i] = abs(col_sum.at<int>(i) - col_sum.at<int>(i+1));
	}


	// pixels to cut off left and right
	// TODO: we'll need this later to adjust the results
	int cutoff = 7;
	//grad.erase(grad.begin(), grad.begin()+cutoff);
	//grad.erase(grad.end()-cutoff , grad.end());

	// save 20 cols with biggest values
	int size_to_save = 30;
	vector<pair<int,int> > max_grad(size_to_save);
	for(int i = 0; i < grad.size(); i++){
		if(grad[i] > max_grad[size_to_save-1].first){
			max_grad[size_to_save-1].first = grad[i];
			max_grad[size_to_save-1].second = i;
			sort(max_grad.begin(), max_grad.end(), myComparison);
		}
	}

	// for all of the found, find ave (with the other +-20 pixels) and choose those with the biggest diff from ave
	int local_area = 10;
	vector<pair<double,int> > diff_to_ave;
	for(int i = 0; i < size_to_save; i++)
	{
		int local_sum = 0;
		for(int j = max_grad[i].second-local_area; j < max_grad[i].second+local_area; j++)
		{
			if(j < 0 || j > grad.size())
			{
				local_sum = 0;
				break;
			}
			if(j == max_grad[i].second)
				continue;
			local_sum += grad[j];
		}
		double ave = (double)(local_sum) / (double)(local_area*2.0);
		double res;
		ave == 0 ? res = 0 : res = max_grad[i].first - ave;		// res = 0 for edges of the image
		diff_to_ave.push_back(pair<double,int>(res, max_grad[i].second));
	}
	sort(diff_to_ave.begin(), diff_to_ave.end(), myComparisonD);

	int line_col_1, line_col_2;	// columns of the two ROI lines
	bool line_col_2_found = false;
	bool left_found = false;

	line_col_1 = diff_to_ave[0].second;
	if(line_col_1 < round(abs_grad_x.cols / 2.0))
		left_found = true;
	for(int i = 0; i < size_to_save; i++)
	{
		cout << "max: " << diff_to_ave[i].first << " at place " <<  diff_to_ave[i].second <<  endl;
		if(abs(diff_to_ave[i].second - line_col_1) > 10 && !line_col_2_found
				&& ((left_found && diff_to_ave[i].second > round(abs_grad_x.cols / 2.0))
					 || (!left_found && diff_to_ave[i].second < round(abs_grad_x.cols / 2.0)) ) )
		{
			line_col_2 = diff_to_ave[i].second;
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


