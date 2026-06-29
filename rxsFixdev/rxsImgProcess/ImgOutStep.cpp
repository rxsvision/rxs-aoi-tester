#include "private.h"



ImgOutStep::ImgOutStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::IMG_OUT;
	outResultType = BLKOUTRESULTYPE_IMG;
	dyinpdata = new std::vector<FromLinker*>;
}

ImgOutStep::ImgOutStep(int id, const char* name) :StepBlockBase(id, name) {
	_type = StepBlockBase::IMG_OUT;
	outResultType = BLKOUTRESULTYPE_IMG;
	dyinpdata = new std::vector<FromLinker*>;
}

void ImgOutStep::Release() {
	std::vector<FromLinker*>* A = (std::vector<FromLinker*>*) dyinpdata;
	for (int i = 0; i < A->size(); i++) {
		FromLinker* b = (*A)[i];
		free(b);
		(*A)[i] = 0;
	}
	A->clear();
}

ImgOutStep::~ImgOutStep() {
	if (dyinpdata != 0) {
		Release();
		delete (std::vector<FromLinker*>*) dyinpdata;
		dyinpdata = 0;
	}
}

//void ImgOutStep::ImgOutInputParamSet(ImgOutParamer iop) {
//	int len = sizeof(ImgSourceParam);
//	InParamAlloc(&iop, len);
//}
//
//ImgOutParamer* ImgOutStep::ImgOutInputParamGet() {
//	ImgOutParamer* iop = (ImgOutParamer*)InParamsGet();
//	if (iop == 0) {
//		ImgOutParamer scp;
//		memset(&scp, 0, sizeof(ImgOutParamer));
//		ImgOutInputParamSet(scp);
//		iop = (ImgOutParamer*)InParamsGet();
//		if (iop == 0) { return 0; }
//	}
//	return iop;
//}

bool ImgOutStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	//ImgOutParamer* iopp = ImgOutInputParamGet();
	ImgOutParamer* iopp = ParamGet<ImgOutParamer>();
	iopp->srcLinker.id = atoi(p[index].c_str());
	iopp->srcLinker.outChoose = atoi(p[index += 1].c_str());
	index += 1;
	if (l > index) {
		iopp->renderingNums = atoi(p[index].c_str());
	}
	else {
		return false;
	}
	for (int x = 0; x < iopp->renderingNums; x++) {
		FromLinker fmlr = { atoi(p[index += 1].c_str()),atoi(p[index += 1].c_str()) };
		RenderDataPush(&fmlr);
	}
	index += 1;
	if (l > index) {
		iopp->savePath = p[index];
	}
	index += 1;
	if (l > index) {
		iopp->saveName = p[index];
	}

	return true;
}

std::string ImgOutStep::InputParamSaveStringGet() {
	ImgOutParamer* iop = (ImgOutParamer*)InParamsGet();
	stringstream result;
	result << std::to_string(iop->srcLinker.id) << "," << std::to_string(iop->srcLinker.outChoose) << "," << std::to_string(iop->renderingNums);
	for (int i = 0; i < iop->renderingNums; i++) {
		result << "," << std::to_string(iop->renderingIds[i]->id) << "," << std::to_string(iop->renderingIds[i]->outChoose);
	}

	if (iop->savePath.length() > 0) {
		result << "," << iop->savePath;
	}

	if (iop->saveName.length() > 0) {
		result << "," << iop->saveName;
	}
	return result.str();
}

void ImgOutStep::RenderDataClr() {
	Release();
	ImgOutParamer* iop = (ImgOutParamer*)InParamsGet();
	iop->renderingNums = 0;
	iop->renderingIds = 0;
}

void ImgOutStep::RenderDataPush(FromLinker* d) {
	if (d == 0) { return; }
	ImgOutParamer* iop = (ImgOutParamer*)InParamsGet();
	if (iop == 0) { return; }
	FromLinker* data = (FromLinker*)malloc(sizeof(FromLinker));
	if (data == 0) { return; }
	memcpy(data, d, sizeof(FromLinker));
	std::vector<FromLinker*>* A = (std::vector<FromLinker*>*) dyinpdata;
	A->push_back(data);
	iop->renderingNums = A->size();
	iop->renderingIds = &(*A)[0];
}

short ImgOutStep::Run() {
	ImgOutParamer* ip = (ImgOutParamer*)InParamsGet();
	if (ip == 0) {
		return -101;
	}
	StepBlockBase* prev = fromSrc(ip->srcLinker.id);
	const rxsImg* ibp = 0;
	prev->GetResult(&ibp);
	Mat img(ibp->rows, ibp->cols, CV_MAKETYPE(CV_8U, ibp->Channels), ibp->Data, ibp->cols * ibp->Channels);
	cv::Mat rgb;
	if (img.channels() == 1) {
		cvtColor(img, rgb, COLOR_GRAY2RGB);
	}
	else {
		rgb = img;
	}
	for (int x = 0; x < ip->renderingNums; x++) {
		int id = ip->renderingIds[x]->id;
		StepBlockBase* prev = fromSrc(id);
		if ((prev->OutType() & BLKOUTRESULTYPE_IMG) == BLKOUTRESULTYPE_IMG) {

		}
		else {
			rxsResultReport* rrr = 0;
			prev->GetResult(&rrr);
			int rc = rrr->RowCount();
			for (int i = 0; i < rc; i++) {
				const char* v = rrr->GetValue(i, "条码");
				const char* P1 = rrr->GetValue(i, "P1");
				const char* P2 = rrr->GetValue(i, "P2");
				const char* P3 = rrr->GetValue(i, "P3");
				const char* P4 = rrr->GetValue(i, "P4");

				cv::Point p[4];
				ConvertToPoint(P1, &p[0]);
				ConvertToPoint(P2, &p[1]);
				ConvertToPoint(P3, &p[2]);
				ConvertToPoint(P4, &p[3]);
				putText(rgb, v, p[2], HersheyFonts::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 0), 2);
				rectangle(rgb, p[1], p[3], cv::Scalar(0, 255, 0), 2);
			}
		}
	}
	if (ip->savePath.length() > 2) {
		std::string fname = ip->savePath + "/";
		if (ip->saveName.length() > 3) {
			fname += ip->saveName;
		}
		else {
			fname += sbName;
			fname += ".jpg";
		}
		imwrite(fname, rgb);
	}

	rxsImg rgbsave = { 0,0,0,0,0 };
	rgbsave.Channels = rgb.channels();
	rgbsave.cols = rgb.cols;
	rgbsave.rows = rgb.rows;
	rgbsave.Data = rgb.data;
	rgbsave.Len = (rgbsave.cols * rgbsave.rows) * rgbsave.Channels;
	outimgAlloc(&rgbsave);
	return 0;
}

void ImgOutStep::ConvertToPoint(const char* str, void* p) {
	cv::Point* cp = (cv::Point*)p;
	std::vector<std::string> r;
	std::string v;
	for (int x = 0; ; x++) {
		if (str[x] == ',') {
			r.push_back(v);
			v = "";
			continue;
		}
		if (str[x] == 0) {
			r.push_back(v);
			v = "";
			break;
		}
		v.push_back(str[x]);
	}
	if (r.size() < 2) { return; }
	cp->x = std::atoi(r[0].c_str());
	cp->y = std::atoi(r[1].c_str());
}
