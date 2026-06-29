
#include "../rxsImgProcess/imgProcess.h"
#include "protected.h"

#define ResultReportManger			(*(rxsResultReportGrpsManger*)resultCollection)



short imgToolKitBlockExt::runProcessActiver(rxsImg* inimg, int nums, rxsResultReport* rr, const char* Fname) {
	int errcode = 0;
	try {
		std::string tn = Fname;
		if (tn == "光度立体法") {

				errcode = PhotometricDefectDetection(Fname, rr);
			
		}
		else if (tn == "颜色识别") {

				errcode = ColorIdentify(Fname, rr);
			
		}
		else if (tn == "QR读码") {

				errcode = QRCodeProcess(inimg, rr);
			
		}
	}
	catch (std::exception& ec) {
		std::string msg = "runProcessActiver-异常\r\n";
		msg += ec.what();
		return -100;
	}
	return errcode;
}


short imgToolKitBlockExt::QRCodeProcess(rxsImg* img, rxsResultReport* rpp) {
	imgTool_CodeDataReader codeReader(img);
	imgTool_Draw drawPanel(img);
	drawPanel.GrayToRGB();
	unsigned Len = 0;
	codeReader.Reader(imgTool_CodeDataReader::QR, Len);
	if (Len > 0) {
		std::string filed[2] = { "ID","条码" };
		rpp->SetFiled(filed, 2, "条码内容");
		char buff[20];
		int row = 0;
		for (unsigned x = 0; x < Len; x++) {
			rxsPointI* pos;
			const char* v = codeReader.Code(x, &pos);
			if (v != 0) {
				row = rpp->AddRow();
				sprintf_s(buff, "%d", row);
				row -= 1;
				rpp->SetValue("ID", buff, row);
				rpp->SetValue("条码", v, row);
				drawPanel.Rectangle(pos[0], pos[3]);
				drawPanel.Save("qrFinsh.jpg");
			}
		}
	}
	return 0;
}

int imgToolKitBlockExt::ColorIdentify(const char* ctkName, rxsResultReport* rr) {
	int ret = 0;
	rxsImg inimg;
	memset(&inimg, 0, sizeof(rxsImg));
	inputClr();
	ImageableSensorUnit* isu = pSystemer->Wrkpd->iabGather->Selected("HkCream");
	if (isu == 0) { return -96; }
	for (unsigned short x = 1; x < 7; x++) {
		CST_LightSource::ColorIndex offCis[4] = {
			(CST_LightSource::ColorIndex)x ,
			(CST_LightSource::ColorIndex)x ,
			(CST_LightSource::ColorIndex)x ,
			(CST_LightSource::ColorIndex)x
		};
		lightSource->brightnessONOFF(offCis);
		_sleep(170);
		isu->Grab(false, 1);
		isu->Stop();
		unsigned pnum = 0;
		void* data = isu->DataGet(pnum);
		inimg.Channels = 1;
		inimg.Data = malloc(pnum);
		if (inimg.Data == 0) {
			//SysLogRecord("PhotometricDefectDetection-图片内存分配失败!");
			return -98;
		}
		memcpy(inimg.Data, data, pnum);
		inimg.Len = pnum;
		inimg.cols = isu->CtrInfo->Width;
		inimg.rows = isu->CtrInfo->Height;

		if (!ColorRecognition(&inimg)) {
			ret = -99;
		}

		free(inimg.Data);
		CST_LightSource::ColorIndex oci[4] = { CST_LightSource::None,CST_LightSource::None, CST_LightSource::None, CST_LightSource::None };
		lightSource->brightnessONOFF(oci);
	}
	return ret;
}


int imgToolKitBlockExt::PhotometricDefectDetection(const char* ctkName, rxsResultReport* rr) {
	int ret = 0;
	rxsImg inimg[4];
	memset(inimg, 0, sizeof(rxsImg) * 4);
	inputClr();
	ImageableSensorUnit* isu = pSystemer->Wrkpd->iabGather->Selected("HkCream");
	if (isu == 0) { return -97; }
	for (unsigned short x = 0; x < 4; x++) {
		CST_LightSource::ColorIndex offCis[4] = { CST_LightSource::None,CST_LightSource::None ,CST_LightSource::None ,CST_LightSource::None };
		offCis[x] = CST_LightSource::White;
		lightSource->brightnessONOFF(offCis);

		_sleep(70);
		isu->Grab(false, 1);
		isu->Stop();
		unsigned pnum = 0;
		void* data = isu->DataGet(pnum);
		inimg[x].Channels = 1;
		inimg[x].Data = malloc(pnum);
		if (inimg[x].Data == 0) {
			//SysLogRecord("PhotometricDefectDetection-图片内存分配失败!");
			return -98;
		}
		memcpy(inimg[x].Data, data, pnum);
		inimg[x].Len = pnum;
		inimg[x].cols = isu->CtrInfo->Width;
		inimg[x].rows = isu->CtrInfo->Height;
		_sleep(30);
	}
	inputPush(&inimg[0], 4);
	int len = 0;
	rxsImg* in = GetInImg(len);
	if (!Photometric(in)) {
		ret = -99;
	}
	for (unsigned short m = 0; m < 4; m++) {
		free(inimg[m].Data);
		inimg[m].Data = 0;
	}
	return ret;
}