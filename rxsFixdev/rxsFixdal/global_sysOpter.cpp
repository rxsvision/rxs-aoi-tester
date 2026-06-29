#pragma once
#include<stdlib.h>
#include<time.h>

#include <fstream>
#include <iomanip>

#include "rxsFixdal.h"
#include "protected.h"


ZMotionDal  *zMotionDal;
WorkProcessDal *wrkp;
rxsMainFrame    *mainFrame;



rxsMainFrame* rxsSystemReady() {
	wrkp = new WorkProcessDal;
	mainFrame = new rxsMainFrame;

#if MOTIONDAL_OFF
	zMotionDal = 0;
#else
	zMotionDal = new ZMotionDal;	
#endif // MOTIONDAL_OFF

	if (mainFrame->Ready(zMotionDal, wrkp)) {

	}
	return mainFrame;
}

void rxsSystemRelease() {
	mainFrame->FontMessageProcessCB(0, 0);
	mainFrame->Close();
	delete wrkp;

#if MOTIONDAL_OFF
	zMotionDal = 0;
#else
	delete zMotionDal;
	zMotionDal = 0;
#endif // MOTIONDAL_OFF

	delete mainFrame;

	mainFrame = 0;
	wrkp = 0;
}

WorkProcessDal* WorkProcessDalGet() {
    return wrkp;
}

rxsMainFrame* rxsMainFrameGet() {
    return mainFrame;
}


short rxsCreateRandomPointCould(rxsPointCouldp* pcGrps, unsigned pcNum) {
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < pcNum; ++i)
    {
        pcGrps[i].x = 1024.0f * rand() / (RAND_MAX + 1.0f);
        pcGrps[i].y = 1024.0f * rand() / (RAND_MAX + 1.0f);
        pcGrps[i].z = 1024.0f * rand() / (RAND_MAX + 1.0f);
    }
    return 0;
}




bool TotalLeastSquares(double* Xgather, double* Ygather, unsigned gatherNum, double& k, double& b) {

	double sum_xy_j = 0;
	double sum_xpow = 0;

	double sum_x = 0;
	double sum_y = 0;

	for (unsigned x = 0; x < gatherNum; x++) {
		sum_xy_j += Xgather[x] * Ygather[x];
		sum_xpow += Xgather[x] * Xgather[x];
		sum_x += Xgather[x];
		sum_y += Ygather[x];
	}

	double kfz = sum_xpow * sum_y - sum_x * sum_xy_j;
	double kfm = gatherNum * sum_xpow - sum_x * sum_x;

	double bfz = gatherNum * sum_xy_j - sum_x * sum_y;
	double bfm = gatherNum * sum_xpow - sum_x * sum_x;

	b = kfz / kfm;
	k = bfz / bfm;

	return true;
}


double straightness(double* Xgather, double* Ygather, unsigned gatherNum, double k, double b, double& pDiff, double& nDiff) {

	double* Yvals = new double[gatherNum];

	double dy = 0;
	double maxPd = 0;
	double maxNd = 0;

	for (unsigned i = 0; i < gatherNum; i++) {
		//Yvals[i] = k * Xgather[i] + b;
		//dy = Yvals[i] - Ygather[i];
		dy = (Xgather[i] * k - Ygather[i] + b) / sqrt(1 + k * k);

		if (dy > 0) {
			if (dy > maxPd) {
				maxPd = dy;
			}
		}
		else {
			if (dy < maxNd) {
				maxNd = dy;
			}
		}

	}

	delete[] Yvals;
	pDiff = maxPd;
	nDiff = maxNd;
	//std::cout << "最大+公差:" << maxPd << ",最大-公差:" << maxNd << "\r\n";
	return maxPd + (0 - maxNd);
}


void valueCenter(double* srcVgather, unsigned gatherNum) {
	double avg = 0;
	for (unsigned i = 0; i < gatherNum; i++) {
		avg += srcVgather[i];
	}
	avg = avg / gatherNum;

	for (unsigned i = 0; i < gatherNum; i++) {
		srcVgather[i] -= avg;
	}
}

bool WorkProcessDal::StraightnessGet(unsigned pointNums, double* X, double* Y, _STDP* outResult) {
	double k = 0, b = 0;
	valueCenter(X, pointNums);
	valueCenter(Y, pointNums);
	TotalLeastSquares(X, Y, pointNums, k, b);
	outResult->k = k;
	outResult->b = b;
	outResult->fDiffVal = straightness(X, Y, pointNums, k, b, outResult->pDiff, outResult->nDiff);
	return true;
}

std::vector<std::string> stringSplit(const std::string& str, char delim) {
	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}
