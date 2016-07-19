#ifndef GENERALPARAMS_HPP
#define GENERALPARAMS_HPP

/*
 * GeneralParams object holds all general data from the input xml/yaml file
 */
#include "opencv/cv.hpp"
#include "undistort.hpp"

using namespace cv;
using namespace std;

class GeneralParams{
public:

	enum CamID{CAM1 = 1, CAM2 = 2};

	GeneralParams();

    /*
     * Write serialization for this class.
     */
    void write(FileStorage& fs) const;

    /*
     * Read serialization for this class.
     */
    void read(const FileNode& node);

    /*
     * Returns the i-th image for the specified camera. Used ONLY for depth estimation.
     */
    Mat getImageForDepthEst(int camID, uchar index);

    /*
     * Returns the i-th image for the specified camera. Used ONLY for depth estimation.
     */
    Mat getImageForDepthEst(int camID, uchar index, string& timestamp);

    /*
     * Returns next image for the main camera
     */
    Mat nextImage();

    /*
     * Returns next image for the main camera together with its time stamp.
     */
    Mat nextImage(string& timeStamp);

    /*
     * Returns the i-th image for the main camera.
     */
    Mat getImage(uchar index);

    /*
     * Returns the i-th image for the main camera together with its time stamp.
     */
    Mat getImage(uchar index, string& timeStamp);

    /*
	 * Returns the time between given time stamps.
	 */
    double timeframe(string timeStamp_1, string timeStamp_2);

    /*
     * Returns the time between images at given indices, where the first index must be smaller.
     */
    double timeframe(uint index_1, uint index_2);

    /*
     * Returns the number of ALL images from the main camera.
     */
    size_t getNumberOfImages();

    /*
     * Returns the number of pairs of images from both cameras (ONLY for depth estimation)
     */
	size_t getNumberOfImagesForDepthEst();

	/*
	 * Returns true if input is valid or false when input is incorrect.
	 */
    bool isGoodInput();

    /*
     * With this function we can change what is our main camera.
     * This changes from which camera we get images with getImage() and it also changes the attribute main_camera.
     * Thus it will also affect the depth estimation.
     * If current main camera was CAM1, it will change to CAM2 and vice-versa.
     */
    void changeMainCamera();

    /*
     * Returns the ID for the main camera. Either 1 or 2.
     */
    CamID getMainCameraID();

    // ONLY for testing
    string outputFileName;

    // path to the folder from which to read images
    string images_folder;

    // Size of the images, but this is redundant as we can read it from the images.
    Size imgSize;

    // Region of interest which is given manually by the user.
    Rect roi_manual;

    // Number of frames per second
	int framesPerSec;

	// Information about the canal
	double canalWidth;
	double canalHeight;
	double canalSlope;
	double canalRoughness;

	// Thresholds for the overflow
	double criticalDepthLevel;
	double criticalVelovityLevel;

	// Information about the camera position.
	double camToBottomDist;
	// This is actually redundant as we can obtain this information from camera clibration.
	double camToCamDist;

	// Parameters used for image processing
	double canny;
	double clahe;
	double radius;
	int sigma;
	uchar threshold;

	// Information about the tracking
	string flow_tracking_timestamp;
	string location;

private:
	// checks if input data in xml settings file is ok
	void validate();

	// Creates the images lists for CAM1 and CAM2.
	bool getImgListsForDepthEst(vector<string>& cam1Imgs, vector<string>& cam2Imgs);

	// Creates the images list of ALL images from the main camera.
	bool getImgList(vector<string>& camImgs);

	void getImageStamp(uchar index, string& timeStamp);

	// IDs for camera 1 and 2
	string cam1_id, cam2_id;

	// Lists of image names used for depth estimation.
	vector<string> imageList_1_de, imageList_2_de;

	// Iterators for lists of image names used for depth estimation.
	size_t atImageList_1_de, atImageList_2_de;

	// List of all image names of camera 1.
	vector<string> imageList_main;

	// Iterator list of all image names of camera 1.
	size_t atImageList_main;

    bool goodInput;

    int main_camera_int;

    // Defines the main camera. Can be adjusted with changeMainCamera()
	CamID main_camera;

};

#endif
