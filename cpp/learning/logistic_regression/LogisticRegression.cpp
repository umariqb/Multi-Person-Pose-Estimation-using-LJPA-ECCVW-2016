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

Modified By: UMAR IQBAL @ 22.07.2015

*/

#include <iostream>
#include <assert.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <glog/logging.h>
#include "LogisticRegression.hpp"

using namespace cv;
using namespace std;

LogisticRegression::CvLR_TrainParams::CvLR_TrainParams()
{
}

LogisticRegression::CvLR_TrainParams::CvLR_TrainParams(double _alpha, int _num_iters, int _normalization, bool _debug, bool _regularized, int _train_method): alpha(_alpha), num_iters(_num_iters), normalization(_normalization), debug(_debug), regularized(_regularized), train_method(_train_method)
{
}
LogisticRegression::CvLR_TrainParams::~CvLR_TrainParams()
{
}

bool LogisticRegression::CvLR::set_param(const CvLR_TrainParams& params){
  this->params = params;
	return true;
}

bool LogisticRegression::CvLR::train(const Mat& DataI, const Mat& LabelsI)
{
	bool ok = false;

	//CV_FUNCNAME("CvLR::train");

	// __BEGIN__;

	LogisticRegression::CvLR::set_label_map(LabelsI);

	int num_classes = this->forward_mapper.size();

	Mat DataT = Mat::zeros(DataI.rows, DataI.cols+1, CV_64F);

	//vconcat(Mat(DataI.rows, 1, DataI.type(), Scalar::all(1.0)), DataI.col(0));

	for (int i=0;i<DataT.cols;i++)
	{
		if(i==0)
		{
			vconcat(Mat(DataI.rows, 1, DataI.type(), Scalar::all(1.0)), DataT.col(i));
			continue;
		}
		vconcat(DataI.col(i-1), DataT.col(i));
	}


	CV_Assert(LabelsI.rows == DataT.rows);

	//cout<<"num_classes = "<<num_classes<<endl;
	//cout<<LabelsI<<endl;
	CV_Assert(num_classes>=2);

	Mat Data;
	Mat Labels;



	Mat Thetas = Mat::zeros(num_classes, DataT.cols, CV_64F);
	Mat Init_Theta = Mat::zeros(DataT.cols, 1, CV_64F);
	Mat LLabels = LogisticRegression::CvLR::remap_labels(LabelsI, this->forward_mapper);
	Mat NewLocalLabels;

	int ii=0;

	if(num_classes == 2)
	{
		DataT.convertTo(Data, CV_64F);

		LLabels.convertTo(Labels, CV_64F);

		Mat NewTheta;
		if(params.train_method == CvLR::BATCH)
		{
			NewTheta = LogisticRegression::CvLR::compute_batch_gradient(Data, Labels, Init_Theta);
			//cout<<"final cost: "<<LogisticRegression::CvLR::compute_cost(Data, Labels, NewTheta)<<endl;
		}
		else
		{
			NewTheta = LogisticRegression::CvLR::compute_mini_batch_gradient(Data, Labels, Init_Theta);
			//cout<<"final cost: "<<LogisticRegression::CvLR::compute_cost(Data, Labels, NewTheta)<<endl;
		}

		Thetas = NewTheta.t();
	}

	else
	{
		/* take each class and rename classes you will get a theta per class
		as in multi class class scenario, we will have n thetas for n classes */
		ii = 0;

  		for(map<int,int>::iterator it = this->forward_mapper.begin(); it != this->forward_mapper.end(); ++it)
		{
			NewLocalLabels = (LLabels == it->second)/255;
			cout<<"processing class "<<it->second<<"\n-------------------------"<<endl;
			DataT.convertTo(Data, CV_64F);
			NewLocalLabels.convertTo(Labels, CV_64F);
			// cout<<"initial theta: "<<Init_Theta<<endl;
			//Mat NewTheta = LogisticRegression::CvLR::compute_batch_gradient(Data, Labels, Init_Theta);
			Mat NewTheta;
			if(params.train_method == CvLR::BATCH)
			{
				NewTheta = LogisticRegression::CvLR::compute_batch_gradient(Data, Labels, Init_Theta);
				cout<<"final cost: "<<LogisticRegression::CvLR::compute_cost(Data, Labels, NewTheta)<<endl;
			}
			else
			{
				NewTheta = LogisticRegression::CvLR::compute_mini_batch_gradient(Data, Labels, Init_Theta);
				cout<<"final cost: "<<LogisticRegression::CvLR::compute_cost(Data, Labels, NewTheta)<<endl;
			}


			// cout<<"learnt theta: "<<NewTheta<<endl;
			hconcat(NewTheta.t(), Thetas.row(ii));
			ii += 1;
		}

	}

	this->learntThetas = Thetas.clone();

	ok = true;
	// __END__
	return ok;
}

float LogisticRegression::CvLR::predict(const Mat& Data)
{
	Mat PredLabs = Mat::zeros(1,1, Data.type());
	CV_Assert(Data.rows ==1);
	LogisticRegression::CvLR::predict(Data, PredLabs);
	// if(Data.rows == 1 && Data.cols == 1)
	// 	LogisticRegression::CvLR::predict(Data, PredLabs);
	// else
	// 	CV_ERROR( CV_StsBadArg, "Data should contain only 1 sample per row");


	return static_cast<float>(PredLabs.at<int>(0,0));
}
float LogisticRegression::CvLR::predict(const Mat& Data, cv::Mat& PredLabs)
{
	/* returns a class of the predicted class
	 class names can be 1,2,3,4, .... etc */

	// add a column of ones
	Mat Thetas = this->learntThetas;

	//vconcat(Mat(Data.rows, 1, Data.type(), Scalar::all(1.0)), Data.col(0));

	Mat DataT = Mat::zeros(Data.rows, Data.cols+1, CV_64F);

	for (int i=0;i<DataT.cols;i++)
	{
		if(i==0)
		{
			vconcat(Mat(Data.rows, 1, Data.type(), Scalar::all(1.0)), DataT.col(i));
			continue;
		}
		vconcat(Data.col(i-1), DataT.col(i));
	}

	CV_Assert(Thetas.rows > 0);

	int classified_class = 0;
	double minVal;
	double maxVal;

	Point minLoc;
	Point maxLoc;
    Point matchLoc;


	cv::Mat Labels;
	cv::Mat CLabels;
	cv::Mat TempPred;

	cv::Mat MPred = Mat::zeros(DataT.rows, Thetas.rows, DataT.type());

	if(Thetas.rows == 1)
	{
		TempPred = LogisticRegression::CvLR::calc_sigmoid(DataT*Thetas.t());
		CV_Assert(TempPred.cols==1);
		// if greater than 0.5, predict class 0 or predict class 1
		TempPred = (TempPred>0.5)/255;
		TempPred.convertTo(CLabels, CV_32S);
	}

	else
	{
		for(int i = 0;i<Thetas.rows;i++)
		{
			TempPred = LogisticRegression::CvLR::calc_sigmoid(DataT * Thetas.row(i).t());
			cv::vconcat(TempPred, MPred.col(i));
		}


		for(int i = 0;i<MPred.rows;i++)
		{
			TempPred = MPred.row(i);

			minMaxLoc( TempPred, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
			Labels.push_back(maxLoc.x);
		}

		Labels.convertTo(CLabels, CV_32S);
	}

	PredLabs = LogisticRegression::CvLR::remap_labels(CLabels, this->reverse_mapper);
	return 0.0;
}

cv::Mat LogisticRegression::CvLR::get_sigmoid_responses(const Mat& Data){
	// add a column of ones
	Mat Thetas = this->learntThetas;

	//vconcat(Mat(Data.rows, 1, Data.type(), Scalar::all(1.0)), Data.col(0));

	Mat DataT = Mat::zeros(Data.rows, Data.cols+1, CV_64F);

	for (int i=0;i<DataT.cols;i++)
	{
		if(i==0)
		{
			vconcat(Mat(Data.rows, 1, Data.type(), Scalar::all(1.0)), DataT.col(i));
			continue;
		}
		vconcat(Data.col(i-1), DataT.col(i));
	}

	CV_Assert(Thetas.rows > 0);

	cv::Mat SigmoidResponse;

	if(Thetas.rows == 1)
	{
		SigmoidResponse = LogisticRegression::CvLR::calc_sigmoid(DataT*Thetas.t());
		CV_Assert(SigmoidResponse.cols==1);
		return SigmoidResponse;
	}
	else{
    LOG(ERROR)<<"Multi-class case is not supported yet."<<endl;
    return Mat();
	}
}


cv::Mat LogisticRegression::CvLR::calc_sigmoid(const Mat& Data)
{
	cv::Mat Dest;
	cv::exp(-Data, Dest);
	return 1.0/(1.0+Dest);
}

double LogisticRegression::CvLR::compute_cost(const Mat& Data, const Mat& Labels, const Mat& Init_Theta)
{

	int llambda = 0;
	int m;
	int n;

	double cost = 0;
	double rparameter = 0;

	cv::Mat Gradient;
	cv::Mat Theta2;
	cv::Mat Theta2Theta2;

	m = Data.rows;
	n = Data.cols;


	Gradient = Mat::zeros( Init_Theta.rows, Init_Theta.cols, Init_Theta.type());

	Theta2 = Init_Theta(Range(1, n), Range::all());

	cv::multiply(Theta2, Theta2, Theta2Theta2, 1);

	if(this->params.regularized == true)
	{
		llambda = 1.0;
	}

	if(this->params.normalization == CvLR::REG_L1)
	{
		rparameter = (llambda/(2*m)) * cv::sum(Theta2)[0];
	}
	else
	{
		// assuming it to be L2 by default
		rparameter = (llambda/(2*m)) * cv::sum(Theta2Theta2)[0];
	}


	Mat D1 = LogisticRegression::CvLR::calc_sigmoid(Data* Init_Theta);


	cv::log(D1, D1);
	cv::multiply(D1, Labels, D1);

	Mat D2 = 1 - LogisticRegression::CvLR::calc_sigmoid(Data * Init_Theta);
	cv::log(D2, D2);
	cv::multiply(D2, 1-Labels, D2);

	cost = (-1.0/m) * (cv::sum(D1)[0] + cv::sum(D2)[0]);
	cost = cost + rparameter;

	return cost;
}

cv::Mat LogisticRegression::CvLR::compute_batch_gradient(const Mat& Data, const Mat& Labels, const Mat& Init_Theta)
{
	// implements batch gradient descent

	int llambda = 0;
	long double ccost;
	int m, n;

	cv::Mat A;
	cv::Mat B;
	cv::Mat AB;
	cv::Mat Gradient;
	cv::Mat PTheta = Init_Theta.clone();

	// cout<<"Data size "<<Data.rows<<", "<<Data.cols<<endl;
	// cout<<"Init_Theta size "<<Init_Theta.rows<<", "<<Init_Theta.cols<<endl;

	// exit(0);
	m = Data.rows;
	n = Data.cols;

	if(this->params.regularized == true)
	{
		llambda = 1;
	}

	for(int i = 0;i<this->params.num_iters;i++)
	{
		ccost = LogisticRegression::CvLR::compute_cost(Data, Labels, PTheta);

		//cout<<"calculated cost: "<<ccost<<endl;

		if(this->params.debug == true && i%(this->params.num_iters/2)==0)
		{
			cout<<"iter: "<<i<<endl;
			cout<<"cost: "<<ccost<<endl;
		}

		B = LogisticRegression::CvLR::calc_sigmoid((Data*PTheta) - Labels);
		// A = ((double)1/m) * Data.t();
		A = (static_cast<double>(1/m)) * Data.t();

		Gradient = A * B;


		A = LogisticRegression::CvLR::calc_sigmoid(Data*PTheta) - Labels;
		B = Data(Range::all(), Range(0,1)).reshape((Data.rows,1));

		cv::multiply(A, B, AB, 1);

		Gradient.row(0) = ((float)1/m) * sum(AB)[0];


		B = Data(Range::all(), Range(1,n));

		//cout<<"for each training data entry"<<endl;
		for(int i = 1;i<Gradient.rows;i++)
		{
			B = Data(Range::all(), Range(i,i+1));
			//cout<<A.rows<<", "<<A.cols<<endl;
			//cout<<B.rows<<", "<<B.cols<<endl;
			cv::multiply(A, B, AB, 1);
			Gradient.row(i) = (1.0/m)*cv::sum(AB)[0] + (llambda/m) * PTheta.row(i);

		}


		PTheta = PTheta - ( static_cast<double>(this->params.alpha)/m)*Gradient;
		//cout<<"updated PTheta"<<endl;
	}
	cout<<"Final cost: "<<ccost<<endl;

	return PTheta;

}

cv::Mat LogisticRegression::CvLR::compute_mini_batch_gradient(const Mat& Data, const Mat& Labels, const Mat& Init_Theta)
{
	// implements batch gradient descent

	int llambda = 0;
	long double ccost;
	int m, n;

	cv::Mat A;
	cv::Mat B;
	cv::Mat AB;
	cv::Mat Gradient;
	cv::Mat PTheta = Init_Theta.clone();
	cv::Mat DData;
	cv::Mat LLabels;

	// cout<<"Data size "<<Data.rows<<", "<<Data.cols<<endl;
	// cout<<"Init_Theta size "<<Init_Theta.rows<<", "<<Init_Theta.cols<<endl;

	// exit(0);


	if(this->params.regularized == true)
	{
		llambda = 1;
	}

	int j = 0;
	int bsize = 10;

	for(int i = 0;i<this->params.num_iters;i++)
	{
		// cout<<j + bsize<<endl;
		// cout<<Data.rows<<endl;
		if(j+bsize<=Data.rows)
		{
			DData = Data(Range(j,j+bsize), Range::all());
			LLabels = Labels(Range(j,j+bsize),Range::all());
		}
		else
		{
			DData = Data(Range(j, Data.rows), Range::all());
			LLabels = Labels(Range(j, Labels.rows),Range::all());
		}

		if(DData.rows ==0 || LLabels.rows ==0)
			break;

		// cout<<"DData"<<endl;
		// cout<<DData.rows<<", "<<DData.cols<<endl;
		// cout<<"LLabels"<<endl;
		// cout<<LLabels.rows<<", "<<LLabels.cols<<endl;
		// cout<<"processing "<<DData.rows<<" at a time"<<endl;

		m = DData.rows;
		n = DData.cols;


		ccost = LogisticRegression::CvLR::compute_cost(DData, LLabels, PTheta);
		// cout<<"calculated cost: "<<ccost<<endl;

		if(this->params.debug == true && i%(this->params.num_iters/2)==0)
		{
			cout<<"iter: "<<i<<endl;
			cout<<"cost: "<<ccost<<endl;
		}

		B = LogisticRegression::CvLR::calc_sigmoid((DData*PTheta) - LLabels);

		A = (static_cast<double>(1/m)) * DData.t();

		//cout<<A.rows<<", "<<A.cols<<endl;
		//exit(0);

		Gradient = A * B;


		A = LogisticRegression::CvLR::calc_sigmoid(DData*PTheta) - LLabels;
		B = DData(Range::all(), Range(0,1)).reshape((DData.rows,1));

		cv::multiply(A, B, AB, 1);

		Gradient.row(0) = ((float)1/m) * sum(AB)[0];


		B = DData(Range::all(), Range(1,n));

		//cout<<"for each training data entry"<<endl;

		for(int i = 1;i<Gradient.rows;i++)
		{
			B = DData(Range::all(), Range(i,i+1));

			//cout<<A.rows<<", "<<A.cols<<endl;

			cv::multiply(A, B, AB, 1);

			Gradient.row(i) = (1.0/m)*cv::sum(AB)[0] + (llambda/m) * PTheta.row(i);


		}


		PTheta = PTheta - ( static_cast<double>(this->params.alpha)/m)*Gradient;
		// cout<<"updated PTheta"<<endl;
		j+=10;
		if(j+bsize>Data.rows)
			break;
	}
	//cout<<"returning PTheta"<<endl;
	return PTheta;

}

std::map<int, int> LogisticRegression::CvLR::get_label_map(const Mat& Labels)
{
	// this function creates two maps to map user defined labels to program friendsly labels
	// two ways.
	CV_Assert(Labels.type() == CV_32S);

	std::map<int, int> forward_mapper;
	std::map<int, int> reverse_mapper;


	for(int i = 0;i<Labels.rows;i++)
	{
		forward_mapper[Labels.at<int>(i)] += 1;
	}

	int ii = 0;

	for(map<int,int>::iterator it = forward_mapper.begin(); it != forward_mapper.end(); ++it)
	{
	 	forward_mapper[it->first] = ii;
	 	ii += 1;
  	}

  	for(map<int,int>::iterator it = forward_mapper.begin(); it != forward_mapper.end(); ++it)
	{
	 	reverse_mapper[it->second] = it->first;
	}

	this->forward_mapper = forward_mapper;
	this->reverse_mapper = reverse_mapper;

	return forward_mapper;
}

bool LogisticRegression::CvLR::set_label_map(const Mat& Labels)
{
	// this function creates two maps to map user defined labels to program friendsly labels
	// two ways.
	bool ok = false;

	CV_Assert(Labels.type() == CV_32S);



	for(int i = 0;i<Labels.rows;i++)
	{
		this->forward_mapper[Labels.at<int>(i)] += 1;
	}

	int ii = 0;

	for(map<int,int>::iterator it = this->forward_mapper.begin(); it != this->forward_mapper.end(); ++it)
	{
	 	this->forward_mapper[it->first] = ii;
	 	ii += 1;
  	}

  	for(map<int,int>::iterator it = this->forward_mapper.begin(); it != this->forward_mapper.end(); ++it)
	{
	 	this->reverse_mapper[it->second] = it->first;
	}

	ok = true;

	return ok;
}

cv::Mat LogisticRegression::CvLR::remap_labels(const Mat& Labels, std::map<int, int> lmap)
{
	cv::Mat NewLabels = Mat::zeros(Labels.rows, Labels.cols, Labels.type());
	for(int i =0;i<Labels.rows;i++)
	{
		NewLabels.at<int>(i,0) = lmap[Labels.at<int>(i,0)];
	}
	return NewLabels;
}


bool LogisticRegression::CvLR::set_default_params()
{
	bool ok = false;
	// set default parameters for the Logisitic Regression classifier
	this->params.alpha = 1.0;
	this->params.num_iters = 10000;
	this->params.normalization = CvLR::REG_L2;
	this->params.debug = true;
	this->params.regularized = true;
	this->params.train_method = CvLR::BATCH;

	ok = true;

	return ok;
}

void LogisticRegression::CvLR::print_learnt_mats()
{

	cout<<this->learntThetas<<endl;
}
void clear()
{

}
void read()
{

}
void write()
{

}
