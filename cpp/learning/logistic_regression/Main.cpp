/*

This is a implementation of the Logistic Regression algorithm in C++ using OpenCV.

AUTHOR: RAHUL KAVI

# You are free to use, change, or redistribute the code in any way you wish for
# non-commercial purposes, but please maintain the name of the original author.
# This code comes with no warranty of any kind.

#
# You are free to use, change, or redistribute the code in any way you wish for
# non-commercial purposes, but please maintain the name of the original author.
# This code comes with no warranty of any kind.

# Logistic Regression ALGORITHM

www.github.com/aceveggie

*/

#include <iostream>
#include <assert.h>
#include <opencv2/opencv.hpp>

// #include <opencv2/core/core.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include "LogisticRegression.hpp"

using namespace std;
using namespace cv;
using namespace LogisticRegression;

int main(int argc, char** argv)
{
	
	
	Mat Data = (Mat_<double>(150, 4)<< 5.1,3.5,1.4,0.2, 4.9,3.0,1.4,0.2, 4.7,3.2,1.3,0.2, 4.6,3.1,1.5,0.2, 5.0,3.6,1.4,0.2, 5.4,3.9,1.7,0.4, 4.6,3.4,1.4,0.3, 5.0,3.4,1.5,0.2, 4.4,2.9,1.4,0.2, 4.9,3.1,1.5,0.1, 5.4,3.7,1.5,0.2, 4.8,3.4,1.6,0.2, 4.8,3.0,1.4,0.1, 4.3,3.0,1.1,0.1, 5.8,4.0,1.2,0.2, 5.7,4.4,1.5,0.4, 5.4,3.9,1.3,0.4, 5.1,3.5,1.4,0.3, 5.7,3.8,1.7,0.3, 5.1,3.8,1.5,0.3, 5.4,3.4,1.7,0.2, 5.1,3.7,1.5,0.4, 4.6,3.6,1.0,0.2, 5.1,3.3,1.7,0.5, 4.8,3.4,1.9,0.2, 5.0,3.0,1.6,0.2, 5.0,3.4,1.6,0.4, 5.2,3.5,1.5,0.2, 5.2,3.4,1.4,0.2, 4.7,3.2,1.6,0.2, 4.8,3.1,1.6,0.2, 5.4,3.4,1.5,0.4, 5.2,4.1,1.5,0.1, 5.5,4.2,1.4,0.2, 4.9,3.1,1.5,0.1, 5.0,3.2,1.2,0.2, 5.5,3.5,1.3,0.2, 4.9,3.1,1.5,0.1, 4.4,3.0,1.3,0.2, 5.1,3.4,1.5,0.2, 5.0,3.5,1.3,0.3, 4.5,2.3,1.3,0.3, 4.4,3.2,1.3,0.2, 5.0,3.5,1.6,0.6, 5.1,3.8,1.9,0.4, 4.8,3.0,1.4,0.3, 5.1,3.8,1.6,0.2, 4.6,3.2,1.4,0.2, 5.3,3.7,1.5,0.2, 5.0,3.3,1.4,0.2, 7.0,3.2,4.7,1.4, 6.4,3.2,4.5,1.5, 6.9,3.1,4.9,1.5, 5.5,2.3,4.0,1.3, 6.5,2.8,4.6,1.5, 5.7,2.8,4.5,1.3, 6.3,3.3,4.7,1.6, 4.9,2.4,3.3,1.0, 6.6,2.9,4.6,1.3, 5.2,2.7,3.9,1.4, 5.0,2.0,3.5,1.0, 5.9,3.0,4.2,1.5, 6.0,2.2,4.0,1.0, 6.1,2.9,4.7,1.4, 5.6,2.9,3.6,1.3, 6.7,3.1,4.4,1.4, 5.6,3.0,4.5,1.5, 5.8,2.7,4.1,1.0, 6.2,2.2,4.5,1.5, 5.6,2.5,3.9,1.1, 5.9,3.2,4.8,1.8, 6.1,2.8,4.0,1.3, 6.3,2.5,4.9,1.5, 6.1,2.8,4.7,1.2, 6.4,2.9,4.3,1.3, 6.6,3.0,4.4,1.4, 6.8,2.8,4.8,1.4, 6.7,3.0,5.0,1.7, 6.0,2.9,4.5,1.5, 5.7,2.6,3.5,1.0, 5.5,2.4,3.8,1.1, 5.5,2.4,3.7,1.0, 5.8,2.7,3.9,1.2, 6.0,2.7,5.1,1.6, 5.4,3.0,4.5,1.5, 6.0,3.4,4.5,1.6, 6.7,3.1,4.7,1.5, 6.3,2.3,4.4,1.3, 5.6,3.0,4.1,1.3, 5.5,2.5,4.0,1.3, 5.5,2.6,4.4,1.2, 6.1,3.0,4.6,1.4, 5.8,2.6,4.0,1.2, 5.0,2.3,3.3,1.0, 5.6,2.7,4.2,1.3, 5.7,3.0,4.2,1.2, 5.7,2.9,4.2,1.3, 6.2,2.9,4.3,1.3, 5.1,2.5,3.0,1.1, 5.7,2.8,4.1,1.3, 6.3,3.3,6.0,2.5, 5.8,2.7,5.1,1.9, 7.1,3.0,5.9,2.1, 6.3,2.9,5.6,1.8, 6.5,3.0,5.8,2.2, 7.6,3.0,6.6,2.1, 4.9,2.5,4.5,1.7, 7.3,2.9,6.3,1.8, 6.7,2.5,5.8,1.8, 7.2,3.6,6.1,2.5, 6.5,3.2,5.1,2.0, 6.4,2.7,5.3,1.9, 6.8,3.0,5.5,2.1, 5.7,2.5,5.0,2.0, 5.8,2.8,5.1,2.4, 6.4,3.2,5.3,2.3, 6.5,3.0,5.5,1.8, 7.7,3.8,6.7,2.2, 7.7,2.6,6.9,2.3, 6.0,2.2,5.0,1.5, 6.9,3.2,5.7,2.3, 5.6,2.8,4.9,2.0, 7.7,2.8,6.7,2.0, 6.3,2.7,4.9,1.8, 6.7,3.3,5.7,2.1, 7.2,3.2,6.0,1.8, 6.2,2.8,4.8,1.8, 6.1,3.0,4.9,1.8, 6.4,2.8,5.6,2.1, 7.2,3.0,5.8,1.6, 7.4,2.8,6.1,1.9, 7.9,3.8,6.4,2.0, 6.4,2.8,5.6,2.2, 6.3,2.8,5.1,1.5, 6.1,2.6,5.6,1.4, 7.7,3.0,6.1,2.3, 6.3,3.4,5.6,2.4, 6.4,3.1,5.5,1.8, 6.0,3.0,4.8,1.8, 6.9,3.1,5.4,2.1, 6.7,3.1,5.6,2.4, 6.9,3.1,5.1,2.3, 5.8,2.7,5.1,1.9, 6.8,3.2,5.9,2.3, 6.7,3.3,5.7,2.5, 6.7,3.0,5.2,2.3, 6.3,2.5,5.0,1.9, 6.5,3.0,5.2,2.0, 6.2,3.4,5.4,2.3, 5.9,3.0,5.1,1.8);
	Mat Labels = (Mat_<int>(150, 1)<< 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3);
	cout<<Labels.type()<<endl;
	cout<<Data.type()<<endl;
	Mat Responses;

	CvLR_TrainParams params = CvLR_TrainParams();
	

	params.alpha = 1.00;
	params.num_iters = 10000;
	params.normalization = CvLR::REG_L1;
	params.debug = true;
	params.regularized = true;
	params.train_method = CvLR::BATCH;
	
	CvLR lr_(Data, Labels, params);
	

	lr_.predict(Data, Responses);
	
	Mat Result = (Labels == Responses)/255;
	
	cout<<"[Original Label]\t[Predicted Label]\t[Result]"<<endl;

	for(int i =0;i<Labels.rows;i++)
	{
		cout<<Labels.row(i)<<"\t"<<Responses.row(i)<<"\t"<<Result.row(i)<<endl;
	}

	cout<<"accuracy: "<<((double)cv::sum(Result)[0]/Result.rows)*100<<"%\n";


	Mat NData = (Mat_<double>(1,4)<<5.9,3.0,5.1,1.8);

	cout<<"predicted label of "<<NData<<"is :"<<lr_.predict(NData)<<endl;

	cout<<"done"<<endl;

	lr_.print_learnt_mats();

	CvMLData cvml;
    

    cvml.read_csv("digitdata2.txt");
    
    cvml.set_response_idx(64);

    const CvMat* vs = cvml.get_values();
    cout << "Rows: " << vs->rows << " Cols: " << vs->cols << endl;
    
    Mat DataMat = vs;
    Mat LLabels, DData;
    cout<<DataMat.rows<<", "<<DataMat.cols<<endl;

    Data = DataMat(Range::all(), Range(0,DataMat.cols-1));
	Data.convertTo(Data, CV_64F);	

    Labels = DataMat(Range::all(), Range(64,65));
    Labels.convertTo(LLabels, CV_32S);
    Labels = LLabels.clone();

	Mat Responses1;
	
	CvLR_TrainParams params1;

	params1.alpha = 0.37;
	params1.num_iters = 10000;
	params1.normalization = CvLR::REG_L2;
	params1.debug = true;
	params1.regularized = true;
	params1.train_method = CvLR::MINI_BATCH;
	
	CvLR lr1_(Data, Labels, params1);

	lr1_.predict(Data, Responses1);
    
	Result = (Labels == Responses1)/255;
	
	cout<<"[Original Label]\t[Predicted Label]\t[Result]"<<endl;

	// for(int i =0;i<Labels.rows;i++)
	// {
	// 	cout<<Labels.row(i)<<"\t"<<Responses1.row(i)<<"\t"<<Result.row(i)<<endl;
	// }

	cout<<"accuracy: "<<((double)cv::sum(Result)[0]/Result.rows)*100<<"%\n";

	return 0;
}