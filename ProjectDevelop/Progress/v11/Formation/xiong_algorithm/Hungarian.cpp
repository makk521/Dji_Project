#include "Hungarian.h"


void Hungarian::init(vector<vector<double>> CostMat)
{
	nRows = CostMat.size();
	nCols = CostMat[0].size();
	n = max(nRows, nCols);
	// vector<vector<int>> assignment(nRows, vector<double>(nCols, 0));
	cost = 0;
	dMat = Eigen::MatrixXd::Zero(nRows, nCols);
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			dMat(i, j) = CostMat[i][j];
		}
	}
}

Eigen::MatrixXd Hungarian::bsxfun(Eigen::MatrixXd dm, Eigen::MatrixXd mr, string mode)
{	
	int h, w;
	Eigen::MatrixXd result;
	if (mode == "minus")
	{
		h = dm.rows();
		w = dm.cols();
		result = Eigen::MatrixXd::Zero(h, w);
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				result(i, j) = dm(i, j) - mr(i, 0);
			}
		}
	}
	else
	{
		h = mr.rows();
		w = dm.cols();
		result = Eigen::MatrixXd::Zero(h, w);
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				result(i, j) = dm(0, j) + mr(i, 0);
				//cout << result(i, j) << " ";
			}
			//cout << endl;
		}
	}
	return result;
}

Eigen::MatrixXd Hungarian::isequal(Eigen::MatrixXd a, Eigen::MatrixXd b)
{
	int h = a.rows();
	int w = a.cols();
	Eigen::MatrixXd result = Eigen::MatrixXd::Zero(h,w);
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			result(i, j) = a(i, j) == b(i, j);
			//cout << result(i, j) << " ";
		}
		//cout << endl;
	}
	return result;
}
Eigen::MatrixXd Hungarian::findx(Eigen::MatrixXd coverMat)
{
	int nx = coverMat.rows() == 1 ? coverMat.cols() : coverMat.rows();
	Eigen::MatrixXd result;
	int k = 0;
	for (int i = 0; i < nx; i++)
	{
		if (nx == coverMat.rows())
		{
			if (coverMat(i, 0) == 0)
			{
				k++;
			}
		}
		else
		{
			if (coverMat(0, i) == 0)
			{
				k++;
			}
		}
	}
	if (nx == coverMat.rows())
	{
		result = Eigen::MatrixXd::Zero(k, 1);
	}
	else
	{
		result = Eigen::MatrixXd::Zero(1, k);
	}
	k = 0;
	for (int i = 0; i < nx; i++)
	{
		if (nx == coverMat.rows())
		{
			if (coverMat(i, 0) == 0)
			{
				result(k, 0) = i;
				k++;
			}
		}
		else
		{
			if (coverMat(0, i) == 0)
			{
				result(0, k) = i;
				k++;
			}
		}
	}
	return result;
}
void Hungarian::findx2(Eigen::MatrixXd& tminr, Eigen::MatrixXd& tminc, Eigen::MatrixXd& dtMat)
{
	int r = 0, c = 0;
	for (int i = 0; i < n; i++)
	{
		if (coverRow(i, 0) == 0)
		{
			r++;
		}
		if (coverColumn(0, i) == 0)
		{
			c++;
		}
	}
	tminr = Eigen::MatrixXd::Zero(r, 1);
	tminc = Eigen::MatrixXd::Zero(1, c);
	r = 0, c = 0;
	for (int i = 0; i < n; i++)
	{
		if (coverRow(i, 0) == 0)
		{
			tminr(r, 0) = minR(i, 0);
			//cout << tminr(r, 0) << " ";
			r++;
		}
		if (coverColumn(0, i) == 0)
		{
			tminc(0, c) = minC(0, i);
			//cout << tminc(0, c) << " ";
			c++;
		}
	}
	//cout << endl;
	dtMat = Eigen::MatrixXd::Zero(r, c);
	r = 0;
	for (int i = 0; i < nRows; i++)
	{
		c = 0;
		if (coverRow(i, 0) == 0)
		{
			for (int j = 0; j < nCols; j++)
			{
				if (coverColumn(0, j) == 0)
				{
					dtMat(r, c) = dMat(i, j);
					//cout << dtMat(r, c) << " ";
					c++;
				}
				//cout << dtMat(r, c) << " ";
			}
			//cout << endl;
			r++;
		}
	}
}

void Hungarian::outerplus(vector<int>& rdx, vector<int>& cdx, double& minval)
{
	Eigen::MatrixXd tminr, tminc, dtMat;
	findx2(tminr, tminc, dtMat);
	int ny = dtMat.cols();
	int nx = tminr.rows();
	//minval = INT_MAX;
	for (int i = 0; i < ny; i++)
	{
		Eigen::MatrixXd tm = tminr + Eigen::MatrixXd::Constant(tminr.rows(), tminr.cols(), tminc(0, i));
		dtMat.block(0, i, dtMat.rows(), 1) = dtMat.block(0, i, nx, 1) - tm.block(0, 0, nx, 1);
		minval = min(minval, (dtMat.col(i).minCoeff()));
	}
	for (int j = 0; j < dtMat.cols(); j++)
	{
		for (int i = 0; i < dtMat.rows(); i++)
		{
			if (dtMat(i, j) == minval)
			{
				rdx.push_back(i);
				cdx.push_back(j);
			}
		}
	}
	//for (int i = 0; i < dtMat.rows(); i++)
	//{
	//	for (int j = 0; j < dtMat.cols(); j++)
	//	{
	//		if (dtMat(i, j) == minval)
	//		{
	//			rdx.push_back(i);
	//			cdx.push_back(j);
	//		}
	//	}
	//}
}

void Hungarian::step1()
{	
	minR = Eigen::MatrixXd::Zero(dMat.rows(), 1);
	minC = Eigen::MatrixXd::Zero(1, dMat.cols());
	for (int i = 0; i < dMat.rows(); i++)
	{
		minR(i, 0) = dMat.row(i).minCoeff();
		//cout << minR(i, 0) << " ";
	}
	//cout << endl;
	string mode1 = "minus";
	Eigen::MatrixXd rmin = bsxfun(dMat, minR, mode1);
	for (int i = 0; i < dMat.cols(); i++)
	{
		minC(0, i) = rmin.col(i).minCoeff();
		//cout << minC(0, i) << " ";
	}
	//cout << endl;
}
void Hungarian::step2()
{
	string mode2 = "plus";
	Eigen::MatrixXd tMat = bsxfun(minC, minR, mode2);
	Eigen::MatrixXd zp = isequal(dMat, tMat);
	startZ = Eigen::MatrixXd::Zero(n, 1);
	while (!zp.isZero())
	{
		for (int i = 0; i < zp.rows(); i++)
		{
			for (int j = 0; j < zp.cols(); j++)
			{
				if (zp(i, j) == 1)
				{
					startZ(i, 0) = j+1;//索引加一
					zp.row(i).setZero();
					zp.col(j).setZero();
				}
			}
			//cout << startZ(i, 0) << " ";
		}
		//cout << endl;
	}
}

void Hungarian::step3()
{
	coverColumn = Eigen::MatrixXd::Zero(1, n);
	for (int i = 0; i < n; i++) {
    if (startZ(i, 0) > 0) {
        int index = static_cast<int>(startZ(i, 0)) - 1; // 将 double 转换为 int 并减 1
        if (index >= 0 && index < n) { // 确保索引在有效范围内
            coverColumn(0, index) = 1;
        }
    }
}
	//for (int i = 0; i < n; i++)
	//{
	//	//cout << coverColumn(0, i) << " ";
	//	cout << startZ(i, 0) << " ";
	//}
	//cout << endl;
	coverRow = Eigen::MatrixXd::Zero(n, 1);
	primeZ = Eigen::MatrixXd::Zero(n, 1);
	Eigen::MatrixXd tminr, tminc, dtMat;
	findx2(tminr, tminc, dtMat);
	string mode = "plus";
	Eigen::MatrixXd tMat = bsxfun(tminc, tminr, mode);
	int rc = 0;
	ridx.clear();
	cidx.clear();
	for (int j = 0; j < dtMat.cols(); j++)
	{
		for (int i = 0; i < dtMat.rows(); i++)
		{
			if (dtMat(i, j) == tMat(i, j))
			{
				ridx.push_back(i);
				cidx.push_back(j);
			}
		}
	}
	//for (int i = 0; i < dtMat.rows(); i++)
	//{
	//	for (int j = 0; j < dtMat.cols(); j++)
	//	{
	//		if (dtMat(i, j) == tMat(i, j))
	//		{
	//			ridx.push_back(i);
	//			cidx.push_back(j);
	//		}
	//	}
	//}
}

void Hungarian::step4()
{
	cR = findx(coverRow);
	cC = findx(coverColumn);
	// for (int i = 0; i < ridx.size(); i++)
	// {
	// 	ridx[i] = cR(ridx[i], 0);
	// 	cidx[i] = cC(0, cidx[i]);
	// }
	for (int i = 0; i < ridx.size(); i++) {
    int rowIndex = static_cast<int>(ridx[i]);
    int colIndex = static_cast<int>(cidx[i]);
    if (rowIndex >= 0 && rowIndex < cR.rows() && colIndex >= 0 && colIndex < cC.cols()) {
        ridx[i] = cR(rowIndex, 0);
        cidx[i] = cC(0, colIndex);
    }
}

	step = 6;
	//for (int i = 0; i < startZ.rows(); i++)
	//{
	//	cout << cR(i, 0) << " ";
	//}
	//cout << endl;
	while (cidx.size() != 0)
	{
		uzr = ridx[0];
		uzc = cidx[0];
		//cout << uzr <<" "<<uzc<< endl;
		primeZ(uzr, 0) = uzc;
		int stz = startZ(uzr, 0);
		if (stz==0)
		{
			step = 5;
			break;
		}
		stz = stz - 1;
		coverRow(uzr, 0) = 1;
		coverColumn(0, stz) = 0;
		vector<double> z(ridx.size(), 0);
		vector<int> rtdx = ridx;
		vector<int> ctdx = cidx;
		int k = 0;
		for (int i = 0; i < ridx.size(); i++)
		{
			if (ridx[i] == uzr)
			{
				z[i] = 1;
				rtdx.erase(rtdx.begin()+(i-k));
				ctdx.erase(ctdx.begin() + (i-k));
				k++;
			}
		}
		ridx = rtdx;
		cidx = ctdx;
		cR = findx(coverRow);
		//for (int i = 0; i < cR.rows(); i++)
		//{
		//	cout << cR(i, 0) << " ";
		//}
		//cout << endl;
		//Eigen::MatrixXd zn;
		vector<int> zn;
		//int zk = 0;
		for (int i = 0; i < coverRow.rows(); i++)
		{
			if (coverRow(i, 0) == 0)
			{
				int a1 = dMat(i, stz);
				int a2 = minR(i, 0) + minC(0, stz);
				if (a1 == a2)
				{
					zn.push_back(1);
				}
				else
				{
					zn.push_back(0);
				}
				//zk++;
			}
		}
		for (int i = 0; i < zn.size(); i++)
		{
			if (zn[i] == 1)
			{
				ridx.push_back(cR(i, 0));
				cidx.push_back(stz);
			}
		}
	}
}

void Hungarian::step5()
{
	int nn = startZ.rows();
	vector<int> rowZ1;
	for (int i = 0; i < nn; i++)
	{
		if (startZ(i, 0)-1 == uzc)
		{
			rowZ1.push_back(i);
		}
		//cout << startZ(i, 0) << " ";
	}
	//cout << endl;
	//if(rowZ1.size()>0)
	//	cout << "rowZ1: " << rowZ1[0] <<endl;
	startZ(uzr, 0) = uzc+1;
	while (rowZ1.size()>0)
	{
		startZ(rowZ1[0], 0) = 0;
		uzc = primeZ(rowZ1[0], 0);
		uzr = rowZ1[0];
		rowZ1.clear();
		for (int i = 0; i < nn; i++)
		{
			if (startZ(i, 0) == uzc+1)
			{
				rowZ1.push_back(i);
			}
		}
		startZ(uzr, 0) = uzc+1;
	}
}

void Hungarian::step6()
{
	vector<int> rdx, cdx;
	double minval = std::numeric_limits<double>::max();
	outerplus(rdx, cdx, minval);
	ridx = rdx;
	cidx = cdx;
	for (int i = 0; i < coverColumn.cols(); i++)
	{
		if (coverColumn(0, i) == 0)
		{
			//minC(0, coverColumn(0, i)) = minC(0, coverColumn(0, i)) + minval;
			minC(0, i) = minC(0, i) + minval;
			//cout << minC(0, i) <<" ";
		}
	}
	//cout << endl;
	for (int j = 0; j < coverRow.rows(); j++)
	{
		if (coverRow(j, 0) == 1)
		{
			//minR(coverRow(j, 0), 0) = minR(coverRow(j, 0), 0) - minval;
			minR(j, 0) = minR(j, 0) - minval;
		}
	}
}

void Hungarian::solve(vector<vector<double>> CostMat, vector<double>& assignment)
{
	init(CostMat);
	step1();
	step2();
	while (true)
	{
		if (startZ.minCoeff()>0)
		{
			break;
		}
		step3();
		while (true)
		{
			step4();
			if (step == 6)
			{
				step6();
			}
			else
			{
				break;
			}
		}
		step5();
	}
	for (int i = 0; i < startZ.size(); i++)
	{
		assignment.push_back(startZ(i,0)-1);
	}
}

// int main(void)
// {
//     //please use "-std=c++11" for this initialization of vector.
// 	vector<vector<double>> costMatrix = { { 15, 28, 41, 44, 0 }, 
// 									   { 23, 20, 33, 36, 0 }, 
// 									   { 31, 28, 25, 28, 0 }, 
// 									   { 39, 36,28, 20, 0 },
// 									   {47, 44, 41, 28, 0} };
// 	// vector<vector<double>> costMatrix = { { 15, 28, 41, 44}, 
// 	// 								   { 23, 20, 33, 36 }, 
// 	// 								   { 31, 28, 25, 28 }, 
// 	// 								   { 39, 36,28, 20 },
// 	// 								   {47, 44, 41, 28} };

// 	Hungarian HungAlgo;
// 	vector<double> assignment;
// 	auto start_time = std::chrono::high_resolution_clock::now();
// 	HungAlgo.solve(costMatrix, assignment);

// 	auto end_time = std::chrono::high_resolution_clock::now();
// 	std::chrono::duration<double> elapsed = end_time - start_time;

// 	std::cout << "Consume Time: " << elapsed.count() << " seconds" << std::endl;

// 	for (unsigned int x = 0; x < costMatrix.size(); x++)
// 		std::cout << x << "," << assignment[x] << "\t";
//     cout<<endl;
// 	//std::cout << "\ncost: " << cost << std::endl;

// 	return 0;
// }
