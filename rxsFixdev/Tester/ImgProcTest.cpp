#include "uintTester.h"


#ifdef TEST_IP
#include "../rxsImgProcess/imgProcess.h"
#include <iostream>
#include <string>
#include <io.h>
#include <vector>

using namespace std;



void getFiles(std::string path, std::vector<std::string>& files)
{

	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			//如果是目录,迭代之（即文件夹内还有文件夹）  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				//文件名不等于"."&&文件名不等于".."
				//.表示当前目录
				//..表示当前目录的父目录
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			//如果不是,加入列表  
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


int main() {

	//imgprocToolbase imgpb("E:\\VM_processerSOL\\qr.png", 0);
	imgprocToolbase imgpb("C:\\Users\\Administrator\\Desktop\\unknown1.png", 0);
	rxsImg* rximg = imgpb.OutHistoryGet(0);
	rxsResultReport rpp;
	rxsImg outi;
	memset(&outi, 0, sizeof(rxsImg));
	LoadThridSolutionAndExer("E:\\VM_processerSOL\\cardoor_chk.sol", "", rximg, &rpp, &outi);
	

}







#endif // TEST_IP                                                                                                                                                                                                                                                                                              