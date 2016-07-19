#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "generalParams.hpp"
#include "cameraParams.hpp"
#include "undistort.hpp"

using namespace cv;
using namespace std;

#define TEST

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
	// Read input data and load it in camP and genP objects
	CameraParams camP;
	GeneralParams genP;
	const string inputSettingsFile = "/home/igorpesic/ClionProjects/AutoROIdetection/in_proposal.xml";
	FileStorage fs(inputSettingsFile, FileStorage::READ);
	if (!fs.isOpened())
	{
		cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
		return -1;
	}
	fs["camera_calibration"] >> camP;
	fs["general_parameters"] >> genP;
	fs.release();
	Undistortion u(camP, genP.imgSize);

	Mat src, src_gray, dst;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	int c;

	string filename;
	int id = 0;
	if(argc == 3)
	{
		filename = argv[1];
		id = atoi(argv[2]);
	}
	else
	{
		cout << "No input given!" << endl;
		return -1;
	}
    //string filename = "/home/igorpesic/ClionProjects/AutoROIdetection/test6.bmp";
	/// Load an image
	src = imread(filename);
	Mat foo;
	if(filename.find("_21628798_") != std::string::npos)
	{
		u.run(src, foo);
	}
	else if(filename.find("_21714620_") != std::string::npos)
	{
		u.run(src, foo);
	}
	else
	{
		cout << "Was not able to detect which camera is it!!!!" << endl;
		cout << filename << endl;
		return -3;
	}

	if(!src.data)
	{
		cout << "Cannot read the image " << filename << endl;
		return -2;
	}

	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

	/// Convert it to gray
	cvtColor( src, src_gray, CV_BGR2GRAY );

	double claheSize = 1.8;
	Ptr<CLAHE> clahe = createCLAHE(claheSize, Size(claheSize, claheSize));
	Mat outputImg;
	clahe->apply(src_gray, src_gray);

	// Create window
#ifdef TEST
	namedWindow( "after clahe", CV_WINDOW_NORMAL );
	imshow( "after clahe", src_gray );
	waitKey(0);
#endif

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
	//cout << "columns sumed up" << endl;
	//cout << col_sum << endl;
#ifdef TEST
	ofstream myfile;
	myfile.open ("/home/igorpesic/ClionProjects/AutoROIdetection/example.txt");
	myfile << col_sum;
	myfile.close();
#endif

	// get gradient of col_sum
	vector<int> grad(col_sum.cols-1, 0);
	for(int i = 0; i < grad.size(); i++)
	{
		grad[i] = abs(col_sum.at<int>(i) - col_sum.at<int>(i+1));
	}


	// pixels to cut off left and right
	// TODO: we'll need this later to adjust the results
	//int cutoff = 7;
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
	int local_area = 25;
	int avoid_area = 5;
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
			if(j == max_grad[i].second
			   || (j > max_grad[i].second-avoid_area && j < max_grad[i].second+avoid_area))
				continue;
			local_sum += grad[j];
		}
		double ave = (double)(local_sum) / (double)(local_area*2.0);
		double res;
		ave == 0 ? res = 0 : res = (double)(max_grad[i].first) / ave;		// res = 0 for edges of the image
		if(ave < 1000)
		{
			diff_to_ave.push_back(pair<double,int>(res, max_grad[i].second));
		}
	}
	sort(diff_to_ave.begin(), diff_to_ave.end(), myComparisonD);

	int line_col_1;
	int line_col_2 = 0;	// columns of the two ROI lines
	bool line_col_2_found = false;
	bool left_found = false;

	line_col_1 = diff_to_ave[0].second;
	cout << "line_col_1: " << line_col_1 << endl;
	if(line_col_1 < round(abs_grad_x.cols / 2.0))
		left_found = true;
	for(int i = 0; i < (int)diff_to_ave.size(); i++)
	{
		cout << "max: " << diff_to_ave[i].first << " at place " <<  diff_to_ave[i].second <<  endl;
		if(abs(diff_to_ave[i].second - line_col_1) > 100 && !line_col_2_found
			&& diff_to_ave[i].first > 0
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
	if(line_col_2_found)
		line(src, ptUR, ptDR, Scalar(0,255,0));
#ifdef TEST
	waitKey(0);
	namedWindow( "ROI", CV_WINDOW_NORMAL );
	imshow( "ROI", src );
	waitKey(0);
#endif
	// find the last occurrence of '.'
	size_t pos = filename.find_last_of("/");
	string pure_name = "";
	// make sure the poisition is valid
	if (pos != string::npos)
		pure_name = filename.substr(pos+1);
	else
		cout << "Coud not find / in the string\n";
	string res_directory = "/home/igorpesic/ClionProjects/AutoROIdetection/res/";
	std::ostringstream oss;
	oss << res_directory << id << "_" << pure_name;
	res_directory = oss.str();
	imwrite(res_directory, src);
	return 0;
}

// This function is used by openCV, and therefore needed in order to read and write the input xml files
static inline void read(const FileNode& node, CameraParams& x, const CameraParams& default_value = CameraParams())
{
	if(node.empty())
		x = default_value;
	else
		x.read(node);
}
// This function is used by openCV, and therefore needed in order to read and write the input xml files
static inline void write(FileStorage& fs, const CameraParams&, const CameraParams& s )
{
	s.write(fs);
}
// This function is used by openCV, and therefore needed in order to read and write the input xml files
static inline void read(const FileNode& node, GeneralParams& x, const GeneralParams& default_value = GeneralParams())
{
	if(node.empty())
		x = default_value;
	else
		x.read(node);
}
// This function is used by openCV, and therefore needed in order to read and write the input xml files
static inline void write(FileStorage& fs, const String&, const GeneralParams& s )
{
	s.write(fs);
}

