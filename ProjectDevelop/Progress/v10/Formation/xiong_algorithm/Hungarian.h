//#pragma once

#include<iostream>
#include<vector>
#include <chrono>
#include<Eigen/Dense>

using namespace std;
class Hungarian 
{
public:
	void solve(vector<vector<double>> CostMat, vector<double>& assignment);
private:
	double cost;
	double nRows, nCols, n;
	int uzr, uzc, step;
	Eigen::MatrixXd minR, minC;
	Eigen::MatrixXd dMat;
	Eigen::MatrixXd startZ;
	Eigen::MatrixXd coverColumn, coverRow, primeZ, cR, cC;
	vector<int> ridx, cidx;
	void init(vector<vector<double>> CostMat);
	void step1();
	void step2();
	void step3();
	void step4();
	void step5();
	void step6();
	Eigen::MatrixXd bsxfun(Eigen::MatrixXd dm, Eigen::MatrixXd mr, string mode);
	Eigen::MatrixXd isequal(Eigen::MatrixXd a, Eigen::MatrixXd b);
	Eigen::MatrixXd findx(Eigen::MatrixXd coverMat);
	void outerplus(vector<int>& rdx, vector<int>& cdx, double& minval);
	void findx2(Eigen::MatrixXd& tminr, Eigen::MatrixXd& tminc, Eigen::MatrixXd& dtMat);
};

