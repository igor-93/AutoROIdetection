#ifndef UNDISTORT_HPP
#define UNDISTORT_HPP

#include "opencv/cv.hpp"

using namespace cv;
using namespace std;

class CameraParams;

/*
 * This class implements the function for undistortion and rectification of images
 */
class Undistortion {
public:
	Undistortion(CameraParams& params, Size& imgSize);

	/*
	 * Main function for undistortion and rectification of images img1 and img2
	 * where img1 is from the left camera and img2 from the right camera
	 * Any of these two inputs can be empty matrix, but not both at the same time.
	 */

	bool run(Mat& img1, Mat& img2);

	/*
	 * Main function for undistortion and rectification of the LEFT image
	 */
	bool run(Mat& img1);
	/*
	 * Needed in DepthEst::runOpenCVandElas(), but when we delete that function, we can also delte this one.
	 */
	void getROIs(Rect& roi1, Rect& roi2);

private:
	void init(void);
	CameraParams* parameters;
	Size imageSize;
	Rect roi1, roi2;
	Mat map11, map12, map21, map22;
};
#endif
