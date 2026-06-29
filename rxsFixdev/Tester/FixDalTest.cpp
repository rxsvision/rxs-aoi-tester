
#include "uintTester.h"
#include <string.h>
#include <iostream>

unsigned EXT_FLOB_Test;


#ifdef TEST_FD
#include "../rxsFixdal/rxsFixdal.h"
#include "../rxsFixdal/DesignFilesImport.h"
#include "../rxsVisualization/rxsVisualization.h"





void CadImportTester();

void SysInitTester();



int main() {


	//EXT_FLOB_Test = 1;
	//int f = GlbTester();
	//std::cout << EXT_FLOB_Test << "," << f << std::endl;





	//CadImportTester();

	SysInitTester();



	//rxsVisualizationDescriptionFile rvdf;

	////rvdf.reLoader("test.cfwer");

	//rvdf.Save("test.cfwer");

}






void CadImportTester() {
	CADImport	cdi;
	//cdi.Load("C:\\Users\\Administrator\\Desktop\\QtUi\\TestFile\\3view.dwg", 0);
	//cdi.Load("C:\\Users\\Administrator\\Desktop\\QtUi\\TestFile\\test.dwg", 0);
	cdi.Load("C:\\Users\\Administrator\\Desktop\\QtUi\\TestFile\\DJ023-MJ002-0.DXF", 0);
	//cdi.Load("C:\\Users\\Administrator\\Desktop\\QtUi\\TestFile\\SMA-400-2020-3-20.dwg", 0);
}


void SysInitTester() {
	rxsMainFrame* SR = rxsSystemReady();
	//rxsMainFrame* rmf = rxsMainFrameGet();
	//if (rmf == 0) {
	//	std::cout << "InitTester::rmf NULL\n";
	//	return;
	//}

	WorkProcessDal* wpd = WorkProcessDalGet();
	if (wpd == 0) {
		std::cout << "InitTester::wpd NULL\n";
		return;
	}
	wpd->modelConvert("C:\\Users\\Administrator\\Desktop\\cfg\\0x_model.pcd", "C:\\Users\\Administrator\\Desktop\\cfg\\out\\0x_model.pcd");
	wpd->modelConvert("C:\\Users\\Administrator\\Desktop\\cfg\\1x_model.pcd", "C:\\Users\\Administrator\\Desktop\\cfg\\out\\1x_model.pcd");
	//int i = wpd->AutoAllLightTakephoto();
	//std::cout << i << std::endl;


	//wpd->ImgProcessFlowerSave();

	//wpd->ImgProcessFlowerLoad();


	//while (true) {
	//	_sleep(500);
	//}
	//rmf->WorkStart(0);
	//while (true) {
	//	_sleep(500);
	//}
	rxsSystemRelease();
}





#endif // TEST_FD
