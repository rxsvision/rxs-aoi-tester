#include "private.h"

#include "../dmtx/dmtx.h"
#pragma comment(lib,"../dmtx/libdmtx.lib")


using namespace cv;








std::vector<SCResult>	cv_wechat_qrdecode(rxsImg* in, unsigned num, bool roiEnb, rxsRectI roiArea, float scale) {

	//º”‘ÿÕº∆¨Ω‚¬Î
	Ptr<wechat_qrcode::WeChatQRCode> detector;
	std::string detect_prototxt = "E:/opencv-4.9.0/build/downloads/wechat_qrcode/detect.prototxt";
	std::string detect_caffe_model = "E:/opencv-4.9.0/build/downloads/wechat_qrcode/detect.caffemodel";
	std::string sr_prototxt = "E:/opencv-4.9.0/build/downloads/wechat_qrcode/sr.prototxt";
	std::string sr_caffe_model = "E:/opencv-4.9.0/build/downloads/wechat_qrcode/sr.caffemodel";
	std::vector<String> strDecoded;
	try
	{
		detector = makePtr<wechat_qrcode::WeChatQRCode>(detect_prototxt, detect_caffe_model,
			sr_prototxt, sr_caffe_model);

	}
	catch (const std::exception& e)
	{
		std::cout <<
			"\n---------------------------------------------------------------\n"
			"Failed to initialize WeChatQRCode.\n"
			"Please, download 'detector.*' and 'sr.*' from\n"
			"https://github.com/WeChatCV/opencv_3rdparty/tree/wechat_qrcode\n"
			"and put them into the current directory.\n"
			"---------------------------------------------------------------\n";
		std::cout << e.what() << std::endl;
	}

	std::vector<SCResult>  scrr;
	Mat img(in->rows, in->cols, CV_MAKETYPE(CV_8U, in->Channels), in->Data, in->cols * in->Channels);
	std::vector<Mat> vPoints;
	double start = getTickCount();
	strDecoded = detector->detectAndDecode(img, vPoints);
	for (int i = 0; i < strDecoded.size(); i++)
	{
		//Point pt1 = Point((int)vPoints[i].at<float>(0, 0), (int)vPoints[i].at<float>(0, 1));
		//Point pt2 = Point((int)vPoints[i].at<float>(1, 0), (int)vPoints[i].at<float>(1, 1));
		//Point pt3 = Point((int)vPoints[i].at<float>(2, 0), (int)vPoints[i].at<float>(2, 1));
		//Point pt4 = Point((int)vPoints[i].at<float>(3, 0), (int)vPoints[i].at<float>(3, 1));
		//line(img, pt1, pt2, Scalar(0, 255, 0), 2);
		//line(img, pt2, pt3, Scalar(0, 255, 0), 2);
		//line(img, pt3, pt4, Scalar(0, 255, 0), 2);
		//line(img, pt4, pt1, Scalar(0, 255, 0), 2);
		//putText(img, strDecoded[i], pt1, 0, 0.5, Scalar(255, 0, 0), 2);

		rxsPointI p[4] = {
			{(int)vPoints[i].at<float>(0, 0),(int)vPoints[i].at<float>(0, 1)},
			{(int)vPoints[i].at<float>(1, 0),(int)vPoints[i].at<float>(1, 1)},
			{(int)vPoints[i].at<float>(2, 0),(int)vPoints[i].at<float>(2, 1)},
			{(int)vPoints[i].at<float>(3, 0),(int)vPoints[i].at<float>(3, 1)},
		};

		SCResult scr = { strDecoded[i],p[0],p[1],p[2],p[3] };
		scrr.push_back(scr);
	}

	double end = getTickCount();
	double run_time = (end - start) / getTickFrequency();
	double fps = 1 / run_time;
	return scrr;
}


std::vector<SCResult> dmtx_dmdecode(rxsImg* in, unsigned num, bool roiEnb, rxsRectI roiArea, float scale) {
	DmtxMessage* msg;
	DmtxRegion* reg;
	Mat tage;
	if (roiEnb) {
		Mat image1(in->rows, in->cols, CV_MAKETYPE(CV_8U, in->Channels), in->Data, in->cols * in->Channels);
		Mat imageROI(image1, Rect(roiArea.x, roiArea.y, roiArea.w, roiArea.h));
		tage = imageROI.clone();
	}
	else {
		tage = Mat(in->rows, in->cols, CV_MAKETYPE(CV_8U, in->Channels), in->Data, in->cols * in->Channels);
	}
	Mat fin;
	if (scale != 1.0f) {
		resize(tage, fin, cv::Size(tage.cols * scale, tage.rows * scale));
	}
	else {
		fin = tage;
	}
	std::vector<SCResult>  scrr;
	if (!fin.data) {
		cout << "Load image failed!" << endl;
		return scrr;
	}
	imwrite("scin.jpg", fin);
	DmtxImage* img;
	img = dmtxImageCreate(fin.data, fin.cols, fin.rows, DmtxPack8bppK);//DmtxPack24bppRGB
	DmtxDecode* dec = dmtxDecodeCreate(img, 1);
	reg = dmtxRegionFindNext(dec, NULL);
	if (reg != NULL) {
		msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
		if (msg != NULL) {
			SCResult r;
			r.code = (char*)msg->output;
			r.pos[0].x = reg->leftLoc.X;
			r.pos[0].y = reg->leftLoc.Y;
			r.pos[1].x = reg->leftLoc.X;
			r.pos[1].y = reg->leftLoc.Y;
			r.pos[2].x = reg->bottomLoc.X;
			r.pos[2].y = reg->bottomLoc.Y;
			r.pos[3].x = reg->bottomLoc.X;
			r.pos[3].y = reg->bottomLoc.Y;
			scrr.push_back(r);
			cout << msg->output << endl;
			dmtxMessageDestroy(&msg);
		}
		dmtxRegionDestroy(&reg);
	}
	dmtxDecodeDestroy(&dec);
	dmtxImageDestroy(&img);
	return scrr;
}
