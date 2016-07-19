#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"

#include "cameraParams.hpp"
#include "undistort.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;


Undistortion::Undistortion(CameraParams& params, Size& imgSize){
	parameters = &params;
	imageSize = imgSize;
	init();
}

void Undistortion::init(){
	// -1, 0 or 1
	int alpha = 0;
	stereoRectify(parameters->camMat1, parameters->distCoeffs1, parameters->camMat2,
			parameters->distCoeffs2, imageSize, parameters->rotation, parameters->translation,
			parameters->R1, parameters->R2, parameters->P1, parameters->P2, parameters->Q,
			CALIB_ZERO_DISPARITY, alpha, imageSize, &roi1, &roi2 );


	initUndistortRectifyMap(parameters->camMat1, parameters->distCoeffs1,
			parameters->R1, parameters->P1, imageSize, CV_16SC2, map11, map12);
	initUndistortRectifyMap(parameters->camMat2, parameters->distCoeffs2,
			parameters->R2, parameters->P2, imageSize, CV_16SC2, map21, map22);
}

bool Undistortion::run(Mat& img1, Mat& img2){
	Mat temp1, temp2;
	if (img1.empty() && img2.empty()){
		cerr << "ERROR: Both images in Undistortion::run() are empty." << endl;
		return false;
	}
	if (!img1.empty()){
		//cout << "Undistorting left image..." << endl;
		remap(img1, temp1, map11, map12, INTER_LINEAR);
		img1 = temp1;
	}
	if (!img2.empty()){
		//cout << "Undistorting right image..." << endl;
		remap(img2, temp2, map21, map22, INTER_LINEAR);
		img2 = temp2;
	}
	return true;
}
bool Undistortion::run(Mat& img1){
	Mat img2;
	return run(img1, img2);
}

void Undistortion::getROIs(Rect& roi1, Rect& roi2){
	roi1 = this->roi1;
	roi2 = this->roi2;
}

