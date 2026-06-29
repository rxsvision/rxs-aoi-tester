
#include "private.h"
#include <io.h>
#include <stdio.h>
#include <string>

#define RXSAPI	extern "C" __declspec(dllexport)
#pragma warning(disable : 4996)

using namespace std;

typedef struct _segmentinfo {
	string Name;
	void* Content = 0;
}SegInfo;


SysConfigManger::SysConfigManger() {
	rsfPath = new string;
	secedseg = NULL;
	siv = new vector<SegInfo>;
	file = 0;
}

SysConfigManger::~SysConfigManger() {
	unsigned x = 0;
	Close();
	x = (*(vector<SegInfo> *)siv).size();
	for (unsigned j = 0; j < x; j++) {
		delete (vector<string> *)(*(vector<SegInfo> *)siv)[j].Content;
	}
	delete (vector<SegInfo> *)siv;
	siv = 0;
	delete (string*)rsfPath;
}


RXSAPI int SysConfigManger::Open(char *path) {
	if (path != NULL) {
		int plen = strlen(path);
		if (plen > 1) {
			(*(string*)rsfPath) = path;
			const char* path = (*(string*)rsfPath).c_str();
			file = fopen(path, "r");
			if (file != 0) {
				fseek((FILE*)file, 0, SEEK_SET);
				analysis();
				fclose((FILE*)file);
				file = 0;
				return  0;
			}
			return -1;
		}
	}
	return -2;
}

const char *SysConfigManger::operator[](unsigned index) {
	if (secedseg != NULL) {
		SegInfo *segi = (SegInfo*)secedseg;
		vector<string> *a = (vector<string> *)(*segi).Content;
		if ((*a).size() > index)
			return (*a)[index].c_str();
	}
	return NULL;
}

const char *SysConfigManger::operator[](char *key) {
	if (secedseg != NULL) {
		unsigned klen = strlen(key);
		SegInfo *segi = (SegInfo*)secedseg;
		vector<string> *sContent = (vector<string> *)(*segi).Content;
		unsigned cSize = (*sContent).size();
		for (unsigned i = 0; i < cSize; i++) {
			char *ac = (char *)(*sContent)[i].c_str();
			if (strstr(ac, key) != NULL) {
				return &ac[klen + 1];
			}
		}
		//map<string, string> *sContent = (map<string, string> *)(*segi).Content;
		//if (sContent->size() > 0) {
		//	return (*sContent)[key].c_str();
		//}
	}
	return NULL;
}

bool SysConfigManger::Editer(char *key, char *val) {
	if (secedseg != NULL) {
		unsigned klen = strlen(key);
		SegInfo *segi = (SegInfo*)secedseg;
		vector<string> *sContent = (vector<string> *)(*segi).Content;
		unsigned cSize = (*sContent).size();
		for (unsigned i = 0; i < cSize; i++) {
			char *ac = (char *)(*sContent)[i].c_str();
			if (strstr(ac, key) != NULL) {
				string newVal = key;
				(*sContent)[i] = (newVal + ":") + val;
				return true;
			}
		}
	}
	return false;
}

RXSAPI int SysConfigManger::ReadSegSet(char* SegName) {
	if (siv != 0) {
		vector<SegInfo>* a = (vector<SegInfo> *)siv;
		unsigned long ofs = 0;
		unsigned len = 0;
		unsigned x = (*a).size();
		if (x > 0) {
			for (unsigned j = 0; j < x; j++) {
				secedseg = &(*a)[j];
				if (memcmp((*(SegInfo*)secedseg).Name.c_str(), SegName, (*(SegInfo*)secedseg).Name.size()) == 0) {
					return  (*(vector<string> *)(*(SegInfo*)secedseg).Content).size();
				}
			}
		}
	}
	return -1;
}

void SysConfigManger::analysis() {
	FILE* afile = (FILE*)file;
	SegInfo sig;
	char buff[128] = { 0 };
	unsigned slen = 0;
	bool isStart = false, isEnd = false;
	((vector<SegInfo> *)siv)->clear();
	while (fscanf(afile, "%s\n", buff) != EOF) {
		slen = strlen(buff);
		if (slen > 0) {
			if (buff[slen - 1] == '}') {
				isEnd = true;
				(*(vector<SegInfo> *)siv).push_back(sig);
				continue;
			}
			if (buff[slen - 2] == ':' && buff[slen - 1] == '{') {
				buff[slen - 2] = 0;
				buff[slen - 1] = 0;
				sig.Name = buff;
				isStart = true;
				sig.Content = new vector<string>;
				continue;
			}
			(*(vector<string> *)sig.Content).push_back(buff);
		}
	}
}

RXSAPI bool SysConfigManger::Save() {
	file = fopen((*(string*)rsfPath).c_str(), "wt");//新建一个文本文件，已存在的文件将内容清空，只允许写
	if (file != NULL) {
		string wc;
		fwrite(wc.c_str(), 1, wc.size(), (FILE*)file);
		fclose((FILE*)file);
		file = 0;
		return true;
	}
	return false;
}

//void SysConfigManger::writeconvert(char *SegName, inkType::inkString *inDataArray, unsigned DaLen) {
//	wc + SegName;
//	//wc + ":{\n";
//	//unsigned cols = 0;
//	//for (unsigned i = 0; i < DaLen; i++)
//	//{
//	//	cols = inDataArray[i].Length();
//	//	wc + inDataArray[i].chrString();
//	//	wc + "\n";
//	//}
//	//wc + "}\n";
//}


RXSAPI void SysConfigManger::Close() {
	unsigned	x = (*(vector<SegInfo> *)siv).size();
	for (unsigned j = 0; j < x; j++) {
		(*(vector<string> *)(*(vector<SegInfo> *)siv)[j].Content).clear();
	}
	if (file != NULL) {
		fclose((FILE *)file);
		file = NULL;
	}
}
