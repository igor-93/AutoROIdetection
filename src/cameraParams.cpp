#include <cameraParams.hpp>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;


CameraParams::CameraParams() {
	outputFileName = "output.xml";       // The name of the file where to write
	goodInput = false;
	error1 = 1000;
	error2 = 1000;
	squareSize = 0;
}

bool CameraParams::isGoodInput() {
	return goodInput ? true : false;
}

//Write serialization for this class
void CameraParams::write(FileStorage& fs) const {
	fs << "{"
	<< "camera_matrix1"  << camMat1
	<< "camera_matrix2" << camMat2
	<< "Square_Size"   << squareSize
	<< "distortion_coefficients1" << distCoeffs1
	<< "distortion_coefficients2" << distCoeffs2
	<< "rotation_matrix" << rotation
	<< "translation_matrix" << translation
	<< "avg_reprojection_error1"  << error1
	<< "avg_reprojection_error2" << error2
	<< "}";
}

//Read serialization for this class
void CameraParams::read(const FileNode& node){
	node["camera_matrix1"] >> camMat1;
	node["camera_matrix2"] >> camMat2;
	node["distortion_coefficients1"] >> distCoeffs1;
	node["distortion_coefficients2"] >> distCoeffs2;
	node["rotation_matrix"] >> rotation;
	node["translation_matrix"] >> translation;
	node["Square_Size"] >> squareSize;
	node["avg_reprojection_error1"] >> error1;
	node["avg_reprojection_error2"] >> error2;
	validate();
}

/*
 * TODO
 */
// checks if input data in xml settings file is ok
void CameraParams::validate() {
	goodInput = true;


}
