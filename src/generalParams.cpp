#include <iostream>
#include <sstream>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "generalParams.hpp"

using namespace cv;
using namespace std;

#ifdef WINDOWS_KEY_WORD
	#define dirsymbol "\\"
#else
	#define dirsymbol "/"
#endif

GeneralParams::GeneralParams() {
	outputFileName = "output.xml";       // The name of the file where to write
	atImageList_1_de = 0;
	atImageList_2_de = 0;
	atImageList_main = 0;
	goodInput = false;
	framesPerSec = 0;
	canalWidth= 0;
	canalHeight= 0;
	criticalDepthLevel= 0;
	criticalVelovityLevel= 0;
	camToBottomDist= 0;
	camToCamDist= 0;
}

//Write serialization for this class
void GeneralParams::write(FileStorage& fs) const {
	fs << "{"
	<< "image_resolution" << imgSize
	<< "fps"  << framesPerSec
	<< "canal_width" << canalWidth
	<< "canal_height"   << canalHeight
	<< "canal_slope"   << canalSlope
	<< "canal_roughness"   << canalRoughness
	<< "canny" << canny
	<< "clahe" << clahe
	<< "radius" << radius
	<< "sigma" << sigma
	<< "threshold" << threshold
	<< "critical_depth_level" << criticalDepthLevel
	<< "critical_velocity_level" << criticalVelovityLevel
	<< "cam_distance_to_bottom" << camToBottomDist
	<< "distance_between_cameras" << camToCamDist
	<< "images_folder" << images_folder
	<< "cam1_id" << cam1_id
	<< "cam2_id" << cam2_id
	<< "flow_tracking_timestamp" << flow_tracking_timestamp
	<< "location" << location
	<< "main_camera" << main_camera_int
	<< "roi_manual" << roi_manual
	<< "}";
}

//Read serialization for this class
void GeneralParams::read(const FileNode& node){
	node["images_folder"] >> images_folder;
	node["cam1_id"] >> cam1_id;
	node["cam2_id"] >> cam2_id;
	node["image_size"] >> imgSize;
	node["fps"] >> framesPerSec;
	node["canal_width"] >> canalWidth;
	node["canal_height"] >> canalHeight;
	node["canal_slope"] >> canalSlope;
	node["canal_roughness"] >> canalRoughness;
	node["canny"] >> canny;
	node["clahe"] >> clahe;
	node["radius"] >> radius;
	node["sigma"] >> sigma;
	node["threshold"] >> threshold;
	node["critical_depth_level"] >> criticalDepthLevel;
	node["critical_velocity_level"] >> criticalVelovityLevel;
	node["cam_distance_to_bottom"] >> camToBottomDist;
	node["distance_between_cameras"] >> camToCamDist;
	node["flow_tracking_timestamp"] >> flow_tracking_timestamp;
	node["location"] >> location;
	node["main_camera"] >> main_camera_int;
	//validate();
}

bool GeneralParams::isGoodInput(){
	return goodInput ? true : false;
}

/*
 * TODO: checks if input data in xml settings file is ok
 *
void GeneralParams::validate() {
	// Check if images_folder is a legit directory
	struct stat s;
	if( stat(images_folder.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)){
		goodInput = true;
	}
	else{
		cerr << "Images folder does not exist!" << endl;
		goodInput = false;
	}

	if(main_camera_int != 1 && main_camera_int != 2){
		cerr << "Main camera ID is wrong. It must be either 1 or 2 but it is " << main_camera_int << endl;
		goodInput = false;
	}
	else if(main_camera_int == 1){
		main_camera = CAM1;
	}
	else{
		main_camera = CAM2;
	}

	if(!getImgListsForDepthEst(imageList_1_de, imageList_2_de) || !getImgList(imageList_main)){
		goodInput = false;
	}

	if(canalSlope >= 1000 || canalRoughness >= 500){
		cerr << "Canal slope must be given in promiles and canal roughness must be given in millimeters." << endl;
		goodInput = false;
	}
	atImageList_main = 0;
	atImageList_1_de = 0;
	atImageList_2_de = 0;
}*/

// Creates the images lists for CAM1 and CAM2.
bool GeneralParams::getImgListsForDepthEst(vector<string>& cam1Imgs, vector<string>& cam2Imgs){
	cam1Imgs.clear();
	cam2Imgs.clear();
	vector<string> allImgs;
	DIR *dir;
	class dirent *ent;
	class stat st;
	string ending = ".bmp";

	dir = opendir(images_folder.c_str());
	while ((ent = readdir(dir)) != NULL) {
		const string file_name = ent->d_name;
		const string full_file_name = images_folder + file_name;

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		if(0 == file_name.compare(file_name.length() - ending.length(), ending.length(), ending)){
			allImgs.push_back(file_name);
		}
	}
	closedir(dir);

	if(allImgs.size() == 0) return false;

	vector<string> cam1_tmp, cam2_tmp;
	for(size_t i = 0; i < allImgs.size(); i++){
		if (allImgs[i].find(cam1_id) != std::string::npos) {
			cam1_tmp.push_back(allImgs[i]);
		}
		if (allImgs[i].find(cam2_id) != std::string::npos) {
			cam2_tmp.push_back(allImgs[i]);
		}
	}
	vector<string>().swap(allImgs);

	string prefix;
	for(size_t i = 0; i < cam1_tmp.size(); i++){
		prefix = cam1_tmp[i].substr(0,9) + cam2_id;
		vector<string>::iterator itr = cam2_tmp.begin();
		while(itr != cam2_tmp.end() ){
			//if(i == 1) cout << "*itr: " << *itr << endl;
			if(itr->compare(0, prefix.size(), prefix) == 0){
				cam1Imgs.push_back(cam1_tmp[i]);
				break;
			}
			itr++;
		}
	}
	for(size_t i = 0; i < cam2_tmp.size(); i++){
		prefix = cam2_tmp[i].substr(0,9) + cam1_id;
		vector<string>::iterator itr = cam1_tmp.begin();
		while(itr != cam1_tmp.end() ){
			if(itr->compare(0, prefix.size(), prefix) == 0){
				cam2Imgs.push_back(cam2_tmp[i]);
				break;
			}
			itr++;
		}
	}

	sort(cam1Imgs.begin(), cam1Imgs.end());
	sort(cam2Imgs.begin(), cam2Imgs.end());

	//cout << "We have " << cam1Imgs.size() << "/" << cam1_tmp.size() << " images." << endl;

	if(cam1Imgs.size() != cam2Imgs.size() || cam1Imgs.size() == 0){
		cerr << "FATAL ERROR!! Cam lists have different sizes or might be both empty!!" << endl;
		cerr << cam1Imgs.size() << " vs. " << cam2Imgs.size() << endl;
		cout << "This can happen because we have the more than one image witht the same time stamp!" << endl;
		cout << "This should be resolved manually because it can destroy the results. " << endl;
		cout << "Check the list below. " << endl;
		for(size_t i = 0; i < cam1Imgs.size(); i++){
			cout << cam1Imgs[i] << " " << cam2Imgs[i] << endl;
		}
		return false;
	}
	else{
		for(size_t i = 0; i < cam1Imgs.size(); i++){
			// error, timestamps are diff
			if(cam1Imgs[i].substr(0,9) != cam2Imgs[i].substr(0,9)){
				cerr << "Time stamps should be the same!!" << endl;
				cerr << cam1Imgs[i].substr (0,9) << "!= " << cam2Imgs[i].substr (0,9) << endl;
			}
			cam1Imgs[i] = images_folder + cam1Imgs[i];
			cam2Imgs[i] = images_folder + cam2Imgs[i];
		}
	}
	return true;
}



// Creates the images list of ALL images from CAM1.
bool GeneralParams::getImgList(vector<string>& camImgs){
	camImgs.clear();
	DIR *dir;
	class dirent *ent;
	class stat st;
	string cam_id;
	string ending = ".bmp";
	switch(main_camera)
	{
	    case CAM1: cam_id = cam1_id; break;
	    case CAM2: cam_id = cam2_id; break;
	}

	dir = opendir(images_folder.c_str());
	while ((ent = readdir(dir)) != NULL) {
		const string file_name = ent->d_name;
		const string full_file_name = images_folder + file_name;

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		if(0 == file_name.compare(file_name.length() - ending.length(), ending.length(), ending)
				&& file_name.find(cam_id) != std::string::npos){
			camImgs.push_back(file_name);
		}
	}
	closedir(dir);

	sort(camImgs.begin(), camImgs.end());

	if(camImgs.size() == 0){
		cerr << "FATAL ERROR!! Was not able to load images!!!" << endl;
		return false;
	}

	//cout << "We have " << cam1Imgs.size() << " images in total from camera 1." << endl;
	for(size_t i = 0; i < camImgs.size(); i++){
		//cout << camImgs[i] << endl;
		camImgs[i] = images_folder + camImgs[i];
	}
	return true;
}

Mat GeneralParams::getImageForDepthEst(int camID, uchar index){
	string foo;
	return getImageForDepthEst(camID, index, foo);
}

Mat GeneralParams::getImageForDepthEst(int camID, uchar index, string& timeStamp){
	Mat result;
	size_t time_stamp_length = 9;
	if(camID != 1 && camID != 2){
		cerr << "Invalid 'camID' argument for getImage() " << camID << endl;
		return result;
	}
	if(camID == 1){
		if(index < imageList_1_de.size()){
			timeStamp = imageList_1_de[index];
			size_t lastSlash = timeStamp.find_last_of(dirsymbol);
			timeStamp = timeStamp.substr(lastSlash+1, time_stamp_length);
			result = imread(imageList_1_de[index], CV_LOAD_IMAGE_GRAYSCALE);
		}
	}
	if(camID == 2){
		if(index < imageList_2_de.size()){
			timeStamp = imageList_2_de[index];
			size_t lastSlash = timeStamp.find_last_of(dirsymbol);
			timeStamp = timeStamp.substr(lastSlash+1, time_stamp_length);
			result = imread(imageList_2_de[index], CV_LOAD_IMAGE_GRAYSCALE);
		}
	}
	return result;
}

Mat GeneralParams::nextImage(){
	string foo;
	return nextImage(foo);
}

Mat GeneralParams::nextImage(string& timeStamp) {
	Mat result;
	size_t time_stamp_length = 9;
	if( atImageList_main < imageList_main.size()){
		timeStamp = imageList_main[atImageList_main];
		size_t lastSlash = timeStamp.find_last_of(dirsymbol);
		timeStamp = timeStamp.substr(lastSlash+1, time_stamp_length);
		result = imread(imageList_main[atImageList_main++], CV_LOAD_IMAGE_GRAYSCALE);
	}
	return result;
}

Mat GeneralParams::getImage(uchar index){
	string foo;
	return getImage(index, foo);
}

Mat GeneralParams::getImage(uchar index, string& timeStamp){
	Mat result;
	size_t time_stamp_length = 9;
	if(index < imageList_main.size()){
		timeStamp = imageList_main[index];
		size_t lastSlash = timeStamp.find_last_of(dirsymbol);
		timeStamp = timeStamp.substr(lastSlash+1, time_stamp_length);
		result = imread(imageList_main[index], CV_LOAD_IMAGE_GRAYSCALE);
	}
	return result;
}

size_t GeneralParams::getNumberOfImages(){
	return imageList_main.size();
}

size_t GeneralParams::getNumberOfImagesForDepthEst(){
	return imageList_1_de.size();
}

double GeneralParams::timeframe(string timeStamp_1, string timeStamp_2){
	int time1 = 0, time2 = 0;

	time1 += ( (int)(timeStamp_1[0] - '0') * 10 + (int)(timeStamp_1[1] - '0') ) * 60;
	time1 += ( (int)(timeStamp_1[3] - '0') * 10 + (int)(timeStamp_1[4] - '0') ) * 1000;
	time1 += (int)(timeStamp_1[6] - '0') * 100 + (int)(timeStamp_1[7] - '0')  * 10 + (int)(timeStamp_1[8] - '0');

	time2 += ( (int)(timeStamp_2[0] - '0') * 10 + (int)(timeStamp_2[1] - '0') ) * 60;
	time2 += ( (int)(timeStamp_2[3] - '0') * 10 + (int)(timeStamp_2[4] - '0') ) * 1000;
	time2 += (int)(timeStamp_2[6] - '0') * 100 + (int)(timeStamp_2[7] - '0')  * 10 + (int)(timeStamp_2[8] - '0');

	return time2 - time1;
}

double GeneralParams::timeframe(uint index_1, uint index_2){
	if(index_1 > getNumberOfImages() || index_2 > getNumberOfImages()){
		cerr << "ERROR: GeneralParams::timeframe(): indices are bigger than the number of images. " << endl;
		return -1;
	}
	if(index_2 <= index_1){
		cerr << "ERROR: GeneralParams::timeframe(): first index must be smaller than the second index!" << endl;
		return -1;
	}
	string timeStamp_1, timeStamp_2;
	getImageStamp(index_1, timeStamp_1);
	getImageStamp(index_1, timeStamp_2);
	return timeframe(timeStamp_1, timeStamp_2);
}

void GeneralParams::getImageStamp(uchar index, string& timeStamp){
	size_t time_stamp_length = 9;
	if(index < imageList_main.size()){
		timeStamp = imageList_main[index];
		size_t lastSlash = timeStamp.find_last_of(dirsymbol);
		timeStamp = timeStamp.substr(lastSlash+1, time_stamp_length);
	}
}

void GeneralParams::changeMainCamera(){
	if(main_camera == CAM1){
		main_camera = CAM2;
		main_camera_int = 2;
	}
	else{
		main_camera = CAM1;
		main_camera_int = 1;
	}
	if(!getImgList(imageList_main)){
		cerr << "ERROR: getImgList failed in changeMainCamera()" << endl;
	}
	cout << "Main camera changed to CAMERA " << main_camera_int << endl;
}

GeneralParams::CamID GeneralParams::getMainCameraID(){
	CamID result;
	if(main_camera == CAM1){
		result = CAM1;
	}
	else{
		result = CAM2;
	}
	return result;
}
