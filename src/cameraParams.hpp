#ifndef CAMERAPARAMS_HPP
#define CAMERAPARAMS_HPP

/*
 *  CameraParams object holds all camera calibration data from the input xml/yaml file
 */
#include "opencv/cv.hpp"
using namespace cv;
using namespace std;

class CameraParams {
public:
	CameraParams();

    //Write serialization for this class
    void write(FileStorage& fs) const;

    //Read serialization for this class
    void read(const FileNode& node);

    // checks if input data in xml settings file is ok
    void validate();

    bool isGoodInput();

    double squareSize;
	double error1;
	double error2;
	// Intrinsic parameters obtained from camera calibration
	Mat camMat1, camMat2, distCoeffs1, distCoeffs2;
	// Extrinsic parameters obtained from (stereo) camera calibration
	Mat rotation, translation;
	// needed for undistrotion
	Mat R1, P1, R2, P2, Q;

private:
    string outputFileName;       // The name of the file where to write

    bool goodInput;
};
#endif
