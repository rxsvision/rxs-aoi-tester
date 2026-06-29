#include "private.h"
#include <thread>
#include <malloc.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>



rxsPointCouldp getBp(void* rp, int i) {
	rxsPointCouldp p = { -1.0f,-1.0f ,-1.0f };
	rxsResultReport* rxspp = (rxsResultReport*)rp;
	const char* v = rxspp->GetValue(i, "X");
	if (v != 0) {
		p.x = atof(v);
	}
	v = rxspp->GetValue(i, "Y");
	if (v != 0) {
		p.y = atof(v);
	}
	v = rxspp->GetValue(i, "Z");
	if (v != 0) {
		p.z = atof(v);
	}
	return p;
}

rxsPointCouldp difPoint(rxsPointCouldp& p1, rxsPointCouldp& p2) {
	rxsPointCouldp dif = { p2.x - p1.x,p2.y - p1.y,p2.z - p1.z };
	return dif;
}

rxsPointCouldp endPoint(rxsPointCouldp& p1, rxsPointCouldp& offset) {
	rxsPointCouldp end = { offset.x + p1.x,offset.y + p1.y,offset.z + p1.z };
	return end;
}

rxsPointCouldp centerPointSE(rxsPointCouldp& p1, rxsPointCouldp& p2) {
	rxsPointCouldp dif = difPoint(p1, p2);
	return { p1.x + (dif.x / 2),p1.y + (dif.y / 2),p1.z + (dif.z / 2) };
}

rxsPointCouldp centerPointSO(rxsPointCouldp& p1, rxsPointCouldp& offset) {
	rxsPointCouldp dif = { offset.x / 2, offset.y / 2, offset.z / 2 };
	return { p1.x + (dif.x / 2),p1.y + (dif.y / 2),p1.z + (dif.z / 2) };
}

short RoiConvertAbsToRel(roiRect& ref, roiRect& src, roiRect& outTage) {
	if ((ref.Flag & 2) == 2) {
		// 参考点不是绝对
		return -1;
	}
	if ((src.Flag & 2) == 2) {
		// 原(待转换)点位已经是相对参考
		return -2;
	}
	u8 refmode = (ref.Flag >> 2);
	rxsPointCouldp refCenter;
	if (refmode == 0) {
		refCenter = centerPointSE(ref.SORC, ref.End);
	}
	else if (refmode == 1) {
		refCenter = centerPointSO(ref.SORC, ref.End);
	}

	u8 srcmode = (src.Flag >> 2);
	rxsPointCouldp srcCenter;
	if (refmode == 0) {
		srcCenter = centerPointSE(src.SORC, src.End);
	}
	else if (refmode == 1) {
		srcCenter = centerPointSO(src.SORC, src.End);
	}
	rxsPointCouldp D = difPoint(src.SORC, src.End);
	outTage.Flag = 3;//相对
	outTage.Flag |= (2 << 2);//中心 半径模式
	outTage.SORC = difPoint(refCenter, srcCenter);
	outTage.LWH = { D.x / 2,D.y / 2 ,D.z / 2 };
	outTage.rName = src.rName;
	outTage.forRi = ref.rName;
	return 0;
}


short RoiConvertRelToAbs(roiRect& ref, roiRect& src, roiRect& outTage) {
	if ((ref.Flag & 2) == 2) {
		// 参考点不是绝对
		return -1;
	}

}


imgprocToolbase::imgprocToolbase(rxsImg* data)
{
	SinImg = 0;
	rxsImg inData = *data;
	inData.Data = malloc(inData.Len);
	if (inData.Data != 0)
		memcpy(inData.Data, data->Data, inData.Len);

	result = new std::vector<rxsImg>;
	((std::vector<rxsImg> *)result)->push_back(inData);
	outSelected();
}

imgprocToolbase::imgprocToolbase(const char* path, int flag) {
	SinImg = 0;
	result = new std::vector<rxsImg>;
	Mat image = imread(path, flag);
	rxsImg inData = { image.channels(),0,image.rows * image.cols,image.rows,image.cols };
	inData.Data = malloc(inData.Len);
	if (inData.Data != 0)
		memcpy(inData.Data, image.data, inData.Len);

	((std::vector<rxsImg> *)result)->push_back(inData);
	outSelected();
}

imgprocToolbase::~imgprocToolbase()
{
	if (result != 0) {
		std::vector<rxsImg>* r = (std::vector<rxsImg> *)result;
		int  rs = r->size();
		for (unsigned m = 0; m < rs; m++) {
			if ((*r)[m].Data != 0) {
				free((*r)[m].Data);
				(*r)[m].Data = 0;
			}
		}
		r->clear();
		delete ((std::vector<rxsImg> *)result);
		result = 0;
	}
}

rxsImg imgprocToolbase::OutDataSet(void* d, u8 channels, unsigned rows, unsigned cols) {
	rxsImg outData;
	outData.cols = cols;
	outData.rows = rows;
	outData.Channels = channels;
	outData.Len = cols * rows * channels;
	outData.Data = malloc(outData.Len);
	if (outData.Data != 0) {
		memcpy(outData.Data, d, outData.Len);
		((std::vector<rxsImg> *)result)->push_back(outData);
	}
	return outData;
}

rxsImg* imgprocToolbase::outSelected() {
	if (result == 0) { return 0; }
	std::vector<rxsImg>* r = (std::vector<rxsImg> *)result;
	int c = r->size();
	if (c < 1) { return 0; }
	SinImg = &(*r)[c - 1];
	return SinImg;
}


unsigned imgprocToolbase::OutCount() {
	if (result == 0) { return 0; }
	std::vector<rxsImg>* r = (std::vector<rxsImg> *)result;
	int c = r->size();
	if (c < 2) { return 0; }
	return ((std::vector<rxsImg> *)result)->size() - 1;
}

rxsImg* imgprocToolbase::OutHistoryGet(unsigned no) {
	if (result == 0) { return 0; }
	std::vector<rxsImg>* r = (std::vector<rxsImg> *)result;
	int c = r->size();
	if (c < 1) { return 0; }
	if (no < c) {
		SinImg = &(*r)[no];
		return SinImg;
	}
	return 0;
}

rxsImg imgprocToolbase::ChannelGet(imgprocToolbase::ChannelTYPE ctpe) {

	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgprocToolbase::Gray() {
	if (SinImg->Channels > 1) {
		if (SinImg->Data != 0) {
			Mat image1(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
			Mat Gray;
			cvtColor(image1, Gray, COLOR_BGR2GRAY);
			OutDataSet(Gray.data, Gray.channels(), Gray.rows, Gray.cols);
			return *outSelected();
		}
	}
	return (*SinImg);
}

rxsImg imgprocToolbase::GrayToRGB() {
	Mat image1(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	Mat rgb;
	cvtColor(image1, rgb, COLOR_GRAY2RGB);
	OutDataSet(rgb.data, rgb.channels(), rgb.rows, rgb.cols);
	return *outSelected();
}

rxsImg imgprocToolbase::Thresholding(short ThresType, double thrv, double maxv) {
	Mat image1(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	Mat thrvImg;
	if (ThresType < 0) {
		adaptiveThreshold(image1, thrvImg, maxv, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 8);
	}
	else {
		threshold(image1, thrvImg, thrv, maxv, ThresType);
	}
	OutDataSet(thrvImg.data, thrvImg.channels(), thrvImg.rows, thrvImg.cols);
	return *outSelected();
}


rxsImg imgprocToolbase::Scaling(float xRate, float yRate) {
	Mat image1(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgprocToolbase::RoiClipping(rxsPointI s, rxsPointI wh) {
	Mat image1(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	Mat imageROI(image1, Rect(s.x, s.y, wh.x, wh.y));
	Mat roi = imageROI.clone();
	OutDataSet(roi.data, roi.channels(), roi.rows, roi.cols);
	return *outSelected();
}

rxsImg imgprocToolbase::RoiClipping(rxsRectI rin, rxsRectI& rect) {
	int Cx = rin.x + rin.w / 2;
	int Cy = rin.y + rin.h / 2;
	rxsPointI cp = { Cx,Cy };
	rxsPointI wh = { rin.w,rin.h };
	return RoiClipping(cp, wh, rin.Angle, rect);
}

rxsImg imgprocToolbase::RoiClipping(rxsPointI center, rxsPointI wh, double deg, rxsRectI& box) {
	cv::Point2i p(center.x, center.y);
	cv::Size s(wh.x, wh.y);
	cv::RotatedRect rr(p, s, deg);
	Point2f ptCorner[4];
	rr.points(ptCorner);

	Mat image1(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	Point ppt[] = { ptCorner[0], ptCorner[1], ptCorner[2], ptCorner[3] };
	const Point* pts[] = { ppt };
	int npt[] = { 4 };
	Mat mask = Mat::zeros(image1.size(), CV_MAKETYPE(CV_8U, image1.channels()));
	fillPoly(mask, pts, npt, 1, Scalar::all(255), 8); //画填充多边形
	Mat result;
	bitwise_and(image1, image1, result, mask);

	std::vector<Point>contour;
	contour.push_back(ptCorner[0]);
	contour.push_back(ptCorner[1]);
	contour.push_back(ptCorner[2]);
	contour.push_back(ptCorner[3]);
	Rect rect = boundingRect(contour);
	box.x = rect.x;
	box.y = rect.y;
	box.w = rect.width;
	box.h = rect.height;
	Mat srcROI = image1(rect);
	Mat maskROI = mask(rect);
	Mat resultROI;
	bitwise_and(srcROI, srcROI, resultROI, maskROI);
	OutDataSet(resultROI.data, resultROI.channels(), resultROI.rows, resultROI.cols);
	return *outSelected();
}

void imgprocToolbase::WndShowPicture(rxsImg* ri, const char* wndName, bool watk) {
	if (ri == 0) {
		ri = SinImg;
	}
	Mat si(ri->rows, ri->cols, CV_MAKETYPE(CV_8U, ri->Channels), ri->Data, ri->cols * ri->Channels);
	namedWindow(wndName, WINDOW_AUTOSIZE);
	imshow(wndName, si);
	if (watk)
		waitKey(0);
}

void imgprocToolbase::Save(const char* path, rxsImg* ri) {
	if (ri == 0) {
		ri = SinImg;
	}
	Mat si(ri->rows, ri->cols, CV_MAKETYPE(CV_8U, ri->Channels), ri->Data, ri->cols * ri->Channels);
	imwrite(path, si);
}

rxsImg imgprocToolbase::xMirror() {
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgprocToolbase::yMirror() {
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgprocToolbase::Rotation() {
	rxsImg ri = { 0,0,0 };

	return ri;
}


void imgTool_Draw::SetPen(idrawPen ipen) {
	pen = ipen;
}

imgTool_Draw::idrawPen* imgTool_Draw::GetPen() {
	return &pen;
}


void imgTool_Draw::Circle(rxsPointI center, int raduis) {
	Mat img(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	cv::Point p(center.x, center.y);
	circle(img, p, raduis, Scalar(pen.RGB[0], pen.RGB[1], pen.RGB[2]), pen.thickness, pen.lineType);
}

void imgTool_Draw::Rectangle(rxsPointI s, rxsPointI e) {
	Mat img(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	cv::Point p1(s.x, s.y);
	cv::Point p2(e.x, e.y);
	rectangle(img, p1, p2, Scalar(pen.RGB[0], pen.RGB[1], pen.RGB[2]), pen.thickness, pen.lineType);
}

void imgTool_Draw::Rectangle(rxsRectI rect) {
	int Cx = rect.x + rect.w / 2;
	int Cy = rect.y + rect.h / 2;
	rxsPointI c = { Cx,Cy };
	rxsPointI wh = { rect.w ,rect.h };
	Rectangle(c, wh, rect.Angle);
}

void imgTool_Draw::Rectangle(rxsPointI center, rxsPointI wh, float deg) {
	cv::Point2i p(center.x, center.y);
	cv::Size s(wh.x, wh.y);
	cv::RotatedRect rr(p, s, deg);
	Point2f ptCorner[4];
	rr.points(ptCorner);
	Mat img(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	for (int j = 0; j < 4; j++)
	{
		line(img, ptCorner[j], ptCorner[(j + 1) % 4], Scalar(0, 0, 255), 2, 8);  //绘制最小外接矩形每条边
	}
}

void imgTool_Draw::Polygon(rxsPointI* pGrps, unsigned pNums, rxsPointI offset) {
	Mat img(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	pNums -= 1;
	int ei = 0;
	for (int i = 0; i <= pNums; i++)
	{
		cv::Point pStsrt(pGrps[i].x + offset.x, pGrps[i].y + offset.y);
		ei = (i + 1) % 4;
		cv::Point pEnd(pGrps[ei].x + offset.x, pGrps[ei].y + offset.y);
		line(img, pStsrt, pEnd, Scalar(pen.RGB[0], pen.RGB[1], pen.RGB[2]), pen.thickness, pen.lineType);
	}
}

void imgTool_Draw::Shape(rxsPointI* pGrps, unsigned pNums, rxsPointI offset) {
	Mat img(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	std::vector<std::vector<Point>> contours;
	std::vector<Point> sp;
	for (int i = 0; i < pNums; i++) {
		cv::Point p(pGrps[i].x + offset.x, pGrps[i].y + offset.y);
		sp.push_back(p);
	}
	contours.push_back(sp);
	drawContours(img, contours, 0, Scalar(pen.RGB[0], pen.RGB[1], pen.RGB[2]), pen.thickness, pen.lineType);
}

void imgTool_Draw::Elliptical() {

}

void imgTool_Draw::Text(const char* text, rxsPointI* org)
{
	Mat img(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	cv::Point s(org->x, org->y);
	putText(img, text, s, FONT_HERSHEY_COMPLEX, pen.fontSize, Scalar(pen.RGB[0], pen.RGB[1], pen.RGB[2]), pen.thickness, pen.lineType);
}



rxsImg imgTool_Filter::Filter()
{
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgTool_Filter::Gassus()
{
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgTool_Filter::Middle() {
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgTool_Filter::Average() {
	rxsImg ri = { 0,0,0 };

	return ri;
}



rxsImg imgTool_EdgeCheck::Canndy() {
	rxsImg ri = { 0,0,0 };

	return ri;
}

rxsImg imgTool_EdgeCheck::Sobel() {
	rxsImg ri = { 0,0,0 };

	return ri;
}


imgTool_Template::imgTool_Template(const char* tempPath, int flag) {
	mr = 0;
	Mat image = imread(tempPath, flag);
	temp = new Mat();
	image.copyTo(*(Mat*)temp);
	memset(P, 0, sizeof(void*) * 2);
	sin = new Mat();
	fb = 0;
	rcps = new std::vector<std::vector<rxsPointI>>;
}

imgTool_Template::imgTool_Template(rxsImg* tempData) {
	mr = 0;
	int c = tempData->Channels;
	temp = new Mat(tempData->rows, tempData->cols, CV_MAKETYPE(CV_8U, c), tempData->Data, c * tempData->cols);
	memset(P, 0, sizeof(void*) * 2);
	sin = new Mat();
	fb = 0;
	rcps = new std::vector<std::vector<rxsPointI>>;
}

imgTool_Template::~imgTool_Template() {
	delete (Mat*)temp;
	temp = 0;
	delete ((std::vector<std::vector<rxsPointI>> *)rcps);
	rcps = 0;
	if (mr != 0) {
		delete (Mat*)mr;
		mr = 0;
	}
	for (int x = 0; x < 2; x++) {
		if (P[x] != 0) {
			delete (cv::Point*)P[x];
			P[x] = 0;
		}
	}
	if (sin != 0) {
		delete (Mat*)sin;
		sin = 0;
	}
}


void imgTool_Template::Match(const char* path, int flag) {
	Mat imimg = imread(path, flag);
	if (imimg.empty() || imimg.data == NULL) { return; }
	imimg.copyTo(*(Mat*)sin);
	rxsImg intemp = { imimg.channels(),imimg.data,imimg.rows * imimg.cols,imimg.rows , imimg.cols };
	fb = 1;
	Match(&intemp);
}

void imgTool_Template::Match(rxsImg* in) {
	Mat img(in->rows, in->cols, CV_MAKETYPE(CV_8U, in->Channels), in->Data, in->Channels * in->cols);
	Mat result;
	if (mr == 0) {
		mr = new Mat();
	}
	else {
		((Mat*)mr)->release();
	}
	if (fb != 1) {
		img.copyTo(*(Mat*)sin);
	}
	fb = 0;
	//1.构建结果图像resultImg(注意大小和类型)
	//如果原图(待搜索图像)尺寸为W x H, 而模版尺寸为 w x h, 则结果图像尺寸一定是(W-w+1)x(H-h+1)
	//结果图像必须为单通道32位浮点型图像
	int result_cols = img.cols - ((Mat*)temp)->cols + 1;//result的尺寸大小
	int result_rows = img.rows - ((Mat*)temp)->rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);


	//CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。
	//CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。
	//CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。
	//CV_TM_SQDIFF_NORMED 归一化平方差匹配法
	//CV_TM_CCORR_NORMED 归一化相关匹配法
	//CV_TM_CCOEFF_NORMED 归一化相关系数匹配法

	matchTemplate(img, *(Mat*)temp, result, CV_TM_SQDIFF_NORMED);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	result.copyTo(*(Mat*)mr);

	double minVal = -1;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	std::cout << "\n" << std::endl;
	std::cout << "min:" << minVal << ",		max:" << maxVal << std::endl;

	float average = 0;
	uchar* dd = result.data;
	
	int index = 0;
	for (unsigned r = 0; r < result.rows; r++) {
		for (unsigned c = 0; c < result.cols; c++) {
			index = r * result.cols + c;
			average += dd[index];
		}
	}
	average = average / (result.cols * result.rows);
	std::cout << "	,AVG:" << average << "\n" << std::endl;



	matchLoc = minLoc;
	if (P[0] == 0) {
		P[0] = new cv::Point(minLoc);
	}
	else {
		*((cv::Point*)P[0]) = minLoc;
	}

	if (P[1] == 0) {
		P[1] = new cv::Point(matchLoc);
	}
	else {
		*((cv::Point*)P[1]) = matchLoc;
	}
}

unsigned imgTool_Template::Count() {
	


	return 0;
}

rxsPointI imgTool_Template::Read(unsigned index) {
	Point center = Point(((cv::Point*)P[0])->x + ((Mat*)temp)->cols / 2, ((cv::Point*)P[0])->y + ((Mat*)temp)->rows / 2);
	//(*(cv::Point*)P[1]).

	rxsPointI rpi = { 0 };
	return rpi;
}



void imgTool_Template::Save(const char* path) {
	Point center = Point(((cv::Point*)P[0])->x + ((Mat*)temp)->cols / 2, ((cv::Point*)P[0])->y + ((Mat*)temp)->rows / 2);
	rectangle((*(Mat*)sin), *(cv::Point*)P[1], Point(((cv::Point*)P[1])->x + ((Mat*)temp)->cols, ((cv::Point*)P[1])->y + ((Mat*)temp)->rows), Scalar(0, 255, 0), 2, 8, 0);
	circle((*(Mat*)sin), center, 2, Scalar(255, 0, 0), 2);
	imwrite(path, (*(Mat*)sin));
}




imgTool_Features::imgTool_Features() {
	p = 0;
}
imgTool_Features::~imgTool_Features() {

}

void imgTool_Features::Extract(rxsImg*) {

	//int64 t1, t2;
	double tkpt, tdes, tmatch_bf, tmatch_knn;

	// 1. 读取图片
	const cv::Mat Src1 = cv::imread("C:\\Users\\Administrator\\Desktop\\QtUi\\TestFile\\3_19yz1\\2600us-2gm-003.bmp", 0); //Load as grayscale
	const cv::Mat Src2 = cv::imread("C:/Users/Administrator/Desktop/QtUi/TestFile/Features/2.jpg", 0); //Load as grayscale
	Mat image1;
	Mat image2;
	resize(Src1, image1, Size(Src1.cols * 0.15, Src1.rows * 0.15));
	resize(Src2, image2, Size(Src2.cols * 0.15, Src2.rows * 0.15));
	std::vector<cv::KeyPoint> keypoints1;
	std::vector<cv::KeyPoint> keypoints2;

	cv::Ptr<cv::SiftFeatureDetector> sift = cv::SiftFeatureDetector::create();
	// 2. 计算特征点
	//t1 = cv::getTickCount();
	sift->detect(image1, keypoints1);
	//t2 = cv::getTickCount();
	//tkpt = 1000.0 * (t2 - t1) / cv::getTickFrequency();
	sift->detect(image2, keypoints2);


	// 3. 计算特征描述符
	cv::Mat descriptors1, descriptors2;
	//t1 = cv::getTickCount();
	sift->compute(image1, keypoints1, descriptors1);
	//t2 = cv::getTickCount();
	//tdes = 1000.0 * (t2 - t1) / cv::getTickFrequency();
	sift->compute(image2, keypoints2, descriptors2);


	// 4. 特征匹配
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
	// cv::BFMatcher matcher(cv::NORM_L2);

	// (1) 直接暴力匹配
	std::vector<cv::DMatch> matches;
	//t1 = cv::getTickCount();
	matcher->match(descriptors1, descriptors2, matches);
	//t2 = cv::getTickCount();
	//tmatch_bf = 1000.0 * (t2 - t1) / cv::getTickFrequency();
	// 画匹配图
	cv::Mat img_matches_bf;
	drawMatches(image1, keypoints1, image2, keypoints2, matches, img_matches_bf);
	imshow("bf_matches", img_matches_bf);

	// (2) KNN-NNDR匹配法
	std::vector<std::vector<cv::DMatch> > knn_matches;
	const float ratio_thresh = 0.7f;
	std::vector<cv::DMatch> good_matches;
	//t1 = cv::getTickCount();
	matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);
	for (auto& knn_matche : knn_matches) {
		if (knn_matche[0].distance < ratio_thresh * knn_matche[1].distance) {
			good_matches.push_back(knn_matche[0]);
		}
	}
	//t2 = cv::getTickCount();
	//tmatch_knn = 1000.0 * (t2 - t1) / cv::getTickFrequency();

	// 画匹配图
	cv::Mat img_matches_knn;
	drawMatches(image1, keypoints1, image2, keypoints2, good_matches, img_matches_knn, cv::Scalar::all(-1),
		cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	cv::imshow("knn_matches", img_matches_knn);
	cv::waitKey(0);



	cv::Mat output;
	cv::drawKeypoints(image1, keypoints1, output);
	cv::imwrite("sift_image1_keypoints.jpg", output);
	cv::drawKeypoints(image2, keypoints2, output);
	cv::imwrite("sift_image2_keypoints.jpg", output);

}

void imgTool_Features::Extract(const char*, int flag) {

}

float imgTool_Features::Match(const char*, int flag) {

	return 0.0;
}

float imgTool_Features::Match(rxsImg*) {
	return 0.0;
}



void imgTool_CodeDataReader::Reader(CODETYPE t, unsigned& Count)
{
	Count = 0;
	rxsImg* ir = outSelected();
	if (codes != 0) {
		((std::vector<SCResult>*)codes)->clear();
	}
	else
	{
		codes = new std::vector<SCResult>;
	}
	switch (t)
	{
	case imgTool_CodeDataReader::QR:
		qrCodeReader(ir, Count);
		break;
	case imgTool_CodeDataReader::DM:
		dmCodeReader(ir, Count);
		break;
	case imgTool_CodeDataReader::BARCODE:
		barCodeReader(ir, Count);
		break;
	default:
		break;
	}
}

unsigned imgTool_CodeDataReader::Count() {
	if (codes == 0)return 0;
	std::vector<SCResult>* c = ((std::vector<SCResult>*)codes);
	return c->size();
}

const char* imgTool_CodeDataReader::Code(unsigned index, rxsPointI** pos) {
	*pos = 0;
	if (codes == 0)return 0;
	std::vector<SCResult>* c = ((std::vector<SCResult>*)codes);
	int cs = c->size();
	if (cs < 1) { return 0; }
	if (index < cs) {
		*pos = ((*c)[index].pos);
		return (*c)[index].code.c_str();
	}
	return 0;
}

void imgTool_CodeDataReader::qrCodeReader(rxsImg* img, unsigned& nums) {
	//std::vector<SCResult> ret = cv_wechat_qrdecode(img);
	//*(std::vector<SCResult>*)codes = ret;
	std::vector<SCResult> ret;
	nums = ret.size();
}

void imgTool_CodeDataReader::barCodeReader(rxsImg* img, unsigned& nums) {
	Ptr<barcode::BarcodeDetector> bardet;
	Mat frame(img->rows, img->cols, CV_MAKETYPE(CV_8U, img->Channels), img->Data, img->cols * img->Channels);
	std::vector<std::string> result;
	std::vector<std::string> decode_type;
	Mat corners;
	bardet->detectAndDecodeWithType(frame, result, decode_type, corners);
	*(std::vector<std::string>*)codes = result;
	nums = result.size();
}

void imgTool_CodeDataReader::dmCodeReader(rxsImg* img, unsigned& nums) {
	//std::vector<SCResult> ret = cv_wechat_qrdecode(img);
	//*(std::vector<SCResult>*)codes = ret;
	std::vector<SCResult> ret;
	nums = ret.size();
}


imgTool_CodeDataReader::~imgTool_CodeDataReader()
{
	if (codes != 0) {
		delete (std::vector<SCResult>*)codes;
		codes = 0;
	}
}



imgTool_Calibration::imgTool_Calibration(const char* path) {
	curpath = 0;
	memset(&out, 0, sizeof(rxsImg));
	plen = 0;
	if (path == 0) { return; }
	plen = strlen(path);
	if (plen > 0) {
		curpath = (char*)malloc(plen);
	}
}

imgTool_Calibration::~imgTool_Calibration() {
	free(curpath);
	curpath = 0;
	for (int xi = 0; xi < 4; xi++) {
		if (Params[xi] != 0) {
			delete (Mat*)Params[xi];
			Params[xi] = 0;
		}
	}
}

void imgTool_Calibration::TransformTest() {
	//Point2f imgPts[4], objPts[4];  //透视前和透视后
	// //原坐标
	//imgPts[0].x = 20 * 1020 / 230; imgPts[0].y = 95 * 647 / 145;
	//imgPts[1].x = 210 * 1020 / 230; imgPts[1].y = 95 * 647 / 145;
	//imgPts[2].x = 90 * 1020 / 230; imgPts[2].y = 65 * 647 / 145;
	//imgPts[3].x = 140 * 1020 / 230; imgPts[3].y = 65 * 647 / 145;
	////透视后坐标
	//int road_w = 540; //将透视变换的图片大小改变一下
	//int road_h = 850;
	//objPts[0].x = 50;  objPts[0].y = 780;
	//objPts[1].x = 490; objPts[1].y = 780;
	//objPts[2].x = 50; objPts[2].y = 150;
	//objPts[3].x = 490; objPts[3].y = 150;
	////计算透视变换矩阵
	//Mat H = getPerspectiveTransform(imgPts, objPts);


	////进行透视变换
	//warpPerspective(srcImage, dstImage, H, srcImage.size());
	////画出透视变换后的四个点
	//circle(dstImage, objPts[0], 9, Scalar(0, 0, 255), 3);
	//circle(dstImage, objPts[1], 9, Scalar(0, 0, 255), 3);
	//circle(dstImage, objPts[2], 9, Scalar(0, 0, 255), 3);
	//circle(dstImage, objPts[3], 9, Scalar(0, 0, 255), 3);
	//imwrite("ok.jpg", dstImage);
}

void imgTool_Calibration::DistortionTest(const char* files, int rowCorrnums, int colCorrnums) {

	int BOARDSIZE[2]{ colCorrnums,rowCorrnums };//棋盘格每行每列角点个数
	std::vector<std::vector<Point3f>> objpoints_img;//保存棋盘格上角点的三维坐标
	std::vector<Point3f> obj_world_pts;//三维世界坐标
	std::vector<std::vector<Point2f>> images_points;//保存所有角点
	std::vector<Point2f> img_corner_points;//保存每张图检测到的角点
	std::vector<String> images_path;
	glob(files, images_path);
	Mat img_gray;
	//转世界坐标系
	for (int i = 0; i < BOARDSIZE[1]; i++)
	{
		for (int j = 0; j < BOARDSIZE[0]; j++)
		{
			obj_world_pts.push_back(Point3f(j, i, 0));
		}
	}
	for (int i = 0; i < images_path.size(); i++) {

		//Mat image(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
		Mat image = imread(images_path[i]);
		

		if (image.channels() > 1) {
			cvtColor(image, img_gray, COLOR_BGR2GRAY);
		}
		else {
			img_gray = image;
		}

		bool found_success = findChessboardCorners(img_gray, Size(BOARDSIZE[0], BOARDSIZE[1]), img_corner_points,
			CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
		if (found_success)
		{
			//迭代终止条件
			TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
			//进一步提取亚像素角点
			cornerSubPix(img_gray, img_corner_points, Size(11, 11), Size(-1, -1), criteria);
			////绘制角点
			//drawChessboardCorners(image, Size(BOARDSIZE[0], BOARDSIZE[1]), img_corner_points,
			//	found_success);
			objpoints_img.push_back(obj_world_pts);//从世界坐标系到相机坐标系
			images_points.push_back(img_corner_points);
		}
	}

	//计算内参和畸变系数等 内参矩阵，畸变系数矩阵，旋转量，偏移量
	Mat cameraMatrix, distCoeffs, R, T;
	cv::calibrateCamera(objpoints_img, images_points, img_gray.size(), cameraMatrix, distCoeffs, R, T);

	for (int xi = 0; xi < 4; xi++) {
		if (Params[xi] == 0) {
			Params[xi] = new Mat;
		}
	}
	cameraMatrix.copyTo(*(Mat*)Params[0]);
	distCoeffs.copyTo(*(Mat*)Params[1]);
	R.copyTo(*(Mat*)Params[2]);
	T.copyTo(*(Mat*)Params[3]);
	return;
}

rxsImg imgTool_Calibration::Calibrate(rxsImg* in) {
	//畸变图像校准
	Mat	dst;
	Mat src(in->rows, in->cols, CV_MAKETYPE(CV_8U, in->Channels), in->Data, in->cols * in->Channels);
	Mat cameraMatrix = *(Mat*)Params[0];
	Mat distCoeffs = *(Mat*)Params[1];
	undistort(src, dst, cameraMatrix, distCoeffs);
	imwrite("dest.jpg", dst);
	out.Channels = dst.channels();
	out.cols = dst.cols;
	out.rows = dst.rows;
	out.Len = dst.cols * dst.rows;
	if (out.Data != 0) {
		free(out.Data);
	}
	out.Data = malloc(out.Len);
	if (out.Data != 0) {
		memcpy(out.Data, dst.data, out.Len);
	}
	return out;
}


bool imgTool_Calibration::SaveParam(const char* path) {
	Mat cameraMatrix = *(Mat*)Params[0];
	Mat distCoeffs = *(Mat*)Params[1];
	Mat R = *(Mat*)Params[2];
	Mat T = *(Mat*)Params[3];

	std::ofstream f1(path);
	if (!f1) { return false; }
	f1 << cameraMatrix << std::endl;
	f1 << distCoeffs << std::endl;
	f1 << R << std::endl;
	f1 << T << std::endl;
	f1.close();
	return true;
}

bool imgTool_Calibration::ReadParam() {
	char szIn[1024] = { 0 };
	bool nedsnull = false;
	if (curpath == 0) {
		std::string name = "CalibrationParam.txt";
		curpath = (char*)name.c_str();
		nedsnull = true;
	}
	std::ifstream in(curpath);
	in.read(szIn, 1024);
	for (int xi = 0; xi < 4; xi++) {
		if (Params[xi] == 0) {
			Params[xi] = new Mat;
		}
	}
	if (nedsnull) {
		curpath = 0;
	}
	return true;
}

void imgTool_Calibration::Save() {
	bool nedsnull = false;
	if (curpath == 0) {
		std::string name = "CalibrationParam.txt";
		curpath = (char*)name.c_str();
		nedsnull = true;
	}
	SaveParam(curpath);
	if (nedsnull) {
		curpath = 0;
	}
}



imgTool_Contour::~imgTool_Contour() {
	if (filterContour != 0) {
		delete (std::vector<std::vector<rxsPointI>> *)filterContour;
		filterContour = 0;
	}
}

int imgTool_Contour::Get(FilterCondition fc) {
	Mat srcIn(SinImg->rows, SinImg->cols, CV_MAKETYPE(CV_8U, SinImg->Channels), SinImg->Data, SinImg->cols * SinImg->Channels);
	Mat binary;
	if (fc.GassKd > 0) {
		GaussianBlur(srcIn, srcIn, Size(fc.GassKd, fc.GassKd), 2, 2);
	}
	if (fc.thrvMethod > -1) {
		adaptiveThreshold(srcIn, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C/*ADAPTIVE_THRESH_MEAN_C*/, fc.thrvMethod, 3, fc.thrvAdj);
		//imwrite("binary.jpg", binary);
	}
	else {
		binary = srcIn.clone();
	}
	std::vector<Vec4i>hierachy;
	std::vector<std::vector<Point>> findcontour;
	findContours(binary, findcontour, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	if (filterContour == 0) {
		filterContour = new std::vector<std::vector<Point>>;
	}
	else {
		((std::vector<std::vector<Point>> *)filterContour)->clear();
	}
	int i = findcontour.size();
	for (unsigned x = 0; x < i; x++) {
		size_t pnums = findcontour[x].size();
		if (pnums < fc.PointNum.min) continue;
		if (pnums > fc.PointNum.max) continue;
		double s = contourArea(findcontour[x]);
		if (s < fc.Area.min) continue;
		if (s > fc.Area.max) continue;
		((std::vector<std::vector<Point>> *)filterContour)->push_back(findcontour[x]);
	}
	return ((std::vector<std::vector<Point>> *)filterContour)->size();
}

int imgTool_Contour::Count() {
	return ((std::vector<std::vector<Point>> *)filterContour)->size();
}

rxsPointI* imgTool_Contour::Reader(unsigned i, unsigned& outnums) {
	std::vector<std::vector<Point>>* C = ((std::vector<std::vector<Point>> *)filterContour);
	if (i < (*C).size()) {
		outnums = (*C)[i].size();
		if (curConvertrpip == 0) {
			curConvertrpip = new std::vector<rxsPointI>;
		}
		else {
			((std::vector<rxsPointI> *)curConvertrpip)->clear();
		}
		Point* v = &(*C)[i][0];
		for (int t = 0; t < outnums; t++) {
			rxsPointI nv = { v[t].x,v[t].y };
			((std::vector<rxsPointI> *)curConvertrpip)->push_back(nv);
		}
		return &(*(std::vector<rxsPointI> *)curConvertrpip)[0];
	}
	return 0;
}

rxsRectI imgTool_Contour::ReaderBoundBox(unsigned i) {
	std::vector<std::vector<Point>>* C = ((std::vector<std::vector<Point>> *)filterContour);
	rxsRectI box = { 0,0,0,0,0 };
	if (i < (*C).size()) {
		cv::Rect boundRect = boundingRect(Mat((*C)[i]));
		int Cx = (boundRect.x + boundRect.width / 2);
		int Cy = (boundRect.y + boundRect.height / 2);
		box = { boundRect.x ,boundRect.y,boundRect.width,boundRect.height,0,Cx,Cy };
		return box;
	}
	return box;
}

rxsRectI imgTool_Contour::ReaderMinAreaRect(unsigned i) {
	std::vector<std::vector<Point>>* C = ((std::vector<std::vector<Point>> *)filterContour);
	rxsRectI v = { 0,0,0,0,0 };
	if (i < (*C).size()) {
		RotatedRect box = minAreaRect((*C)[i]);
		Point2f boxPoints[4];
		box.points(boxPoints);
		for (int t = 0; t < 4; t++) {
			v.x = boxPoints[0].x;
			v.y = boxPoints[0].y;
			v.w = box.size.width;
			v.h = box.size.height;
			v.Angle = box.angle;
			v.Cx = box.center.x;
			v.Cy = box.center.y;
			return v;
		}
	}
	return v;
}

