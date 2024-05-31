#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/core/core.hpp>        // basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // OpenCV image processing
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

/*---------------------------------------------------------------------------
TASK 2
	!! Only add code to the reserved places.
	!! The resulting program must not print anything extra on any output (nothing more than the prepared program framework).
*/


/*	TASK 2.1 - Geometry transformation

	The function performs a geometric transformation of the image using nearest neighbor interpolation.

	Input Transform describes the transformation of pixels from the input image to the output image.
	We use the inverse procedure (see lecture), so we calculate the inverse transformation and
	for each point of the output image we will look for its position in the input image and use
	nearest neighbor interpolation to find the value of the output pixel.

	Input (src) image is in grayscale (single channel 8bit).
*/

int geometricalTransform( const cv::Mat& src, cv::Mat& dst, const cv::Mat& transformation )
{
	if( src.empty() || transformation.empty())
		return 1;

	// inverse transformation
	cv::Mat T = transformation.inv();
   
	// output image
	dst = cv::Mat::zeros(src.size(), CV_8UC1 );
	
	/*
		For each pixel of the output image
		1. find its position in the source image (using the prepared inverse transformation in the matrix T)
			see http://docs.opencv.org/modules/imgproc/doc/geometric_transformations.html?highlight=warpaffine#warpaffine
		2. check if the coordinates are not outside the source image
		3. use the nearest neighbor interpolation to calculate the resulting brightness of the target pixel (use the cvRound () function)

		Allowed Mat attributes, methods and OpenCV functions for task solution are:
			Mat:: rows, cols, step(), size(), at<>(), zeros(), ones(), eye(), cvRound()
	*/

	/* ***** Working area - begin ***** */
	for(int y = 0; y < dst.rows; y++){
		for(int x = 0; x < dst.cols; x++){
			//get position from src image based on equation from opencv docs
			float src_x = T.at<float>(0,0) * x + T.at<float>(0,1) * y + T.at<float>(0,2);
			float src_y = T.at<float>(1,0) * x + T.at<float>(1,1) * y + T.at<float>(1,2);

			//check if inside the image
			if((src_x <= src.cols && src_x >= 0.f) && (src_y <= src.rows && src_y >= 0.f))
			{
				//if inside copy brightness to dst image (used cvRound for finding nearest integer neighbour)
				dst.at<unsigned char>(y,x) = src.at<unsigned char>(cvRound(src_y), cvRound(src_x));
			}
		
		}
	}
	
	/* ***** Working area - end ***** */

	return 0;
}




//---------------------------------------------------------------------------
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);
//---------------------------------------------------------------------------

/* Examples of input parameters
	mt02 image_path [rotation in degrees] [scale]
*/
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Not enough parameters." << endl;
		cout << "Usage: mt02 image_path [rotation in degrees] [scale]" << endl;
		return -1;
	}

    std::string img_path = "";
    float s = 1.f;
    float r = 0.f;

	// check and read input parameters
	if( argc > 1 ) img_path = std::string( argv[1] );
	if( argc > 2 ) r = atof( argv[2] );
	if( argc > 3 ) s = atof( argv[3] );

	// load image, load as grayscale image
	Mat src_gray = imread(img_path, IMREAD_GRAYSCALE);
	if(src_gray.empty() )
	{
		cout <<  "Could not open the original image: " << img_path << endl ;
		return 1;
	}

	/* *** Geometry transform *** */
	
	// center of the image
	cv::Point2f c = cv::Point2f(0.5f*src_gray.cols,0.5f*src_gray.rows);
	
	// translation matrices, rotations, scaling and the resulting transformation
	cv::Mat T = cv::Mat::eye(3,3,CV_32FC1);
	cv::Mat R = cv::Mat::eye(3,3,CV_32FC1);
	cv::Mat S = cv::Mat::eye(3,3,CV_32FC1);
	cv::Mat M = cv::Mat::eye(3,3,CV_32FC1);	
	
	/* 
		Set the coefficients of the translation, rotation and scaling matrix
		and multiply the transformation matrices in the correct order 
		so that the resulting image is rotated and scaled around its center.
		The values are in the variables 'c', 'r' and 's'.

		Attention:
		- angle is in degrees (not radians),
		- we want to rotate counterclockwise (the rotation matrix must be inverted before use),
		- multiplication of matrices A*B*C is in c++ from the left, it is necessary to use 
		  parentheses (A*(B*C)), if we want to multiply from the right
	 	 
		Allowed Mat attributes, methods and OpenCV/math functions for task solution are:
			Mat:: rows, cols, at<>(), zeros(), ones(), eye(), inv(), cos, sin, CV_PI 	
	*/

	/* ***** Working area - begin ***** */
	
	//Transform
	T.at<float>(0,2) = c.x;
	T.at<float>(1,2) = c.y;

	//Rotate
	float radians = r/180.f * CV_PI;
	R.at<float>(0,0) = cos(radians);
	R.at<float>(0,1) = -sin(radians);
	R.at<float>(1,0) = sin(radians);
	R.at<float>(1,1) = cos(radians);

	R = R.inv();

	//Scale
	S.at<float>(0,0) = s;
	S.at<float>(1,1) = s;

	//Final translation matrix
	M = T * S * R * T.inv();


		
	/* ***** Working area - end ***** */


	// output images
	cv::Mat tran_res, tran_ref;
	
	// compute solution using your own function
	geometricalTransform( src_gray, tran_res, M );

	// compute reference solution - the warpAffine function internally inverts the transformation matrix and interpolates the output pixel values from the input image
	cv::Mat Mref = cv::getRotationMatrix2D( c, r, s );
	cv::warpAffine( src_gray, tran_ref, Mref, src_gray.size(), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar::all(0) );


	//---------------------------------------------------------------------------
	// compute and report differences
	checkDifferences( tran_res, tran_ref, "geometry", true );
	std::cout << std::endl;
	//---------------------------------------------------------------------------

    return 0;
}
//---------------------------------------------------------------------------




void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save )
{
	double mav = 255., err = 255., nonzeros = 1000.;

	if( !test.empty() ) {
		cv::Mat diff;
		cv::absdiff( test, ref, diff );
		cv::minMaxLoc( diff, NULL, &mav );
		nonzeros = 1. * cv::countNonZero( diff ); // / (diff.rows*diff.cols);
		err = (nonzeros > 0 ? ( cv::sum(diff).val[0] / nonzeros ) : 0);

		if( save ) {
			diff *= 255;
			cv::imwrite( (tag+".0.ref.png").c_str(), ref );
			cv::imwrite( (tag+".1.test.png").c_str(), test );
			cv::imwrite( (tag+".2.diff.png").c_str(), diff );
		}
	}

	printf( "%s_avg_cnt_max, %.1f, %.0f, %.0f, ", tag.c_str(), err, nonzeros, mav );

	return;
}
