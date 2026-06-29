#pragma once


namespace inkType
{
	class __declspec(dllexport) inkLog
	{
	public:
		inkLog();
		~inkLog();
		void PushParam(char *path);
		void Write(char *Msg);
		bool Read(char *dtmo, char *dtmt);
	private:
		void reftime();
		char	sysspan[9];
		char    hms[9];
		char	*ipath;
		unsigned filefind(char *, char *, char **outArray);
		char *Query(char *fn);
	};



	typedef struct _inkfileinfo {
		char *FullPath;
		char *Name;
		unsigned long Size;
	}inkFileInfo;

	class _declspec(dllexport) inkFile
	{
	public:
		inkFileInfo FileInfo;
		inkFile();
		inkFile(char *path, char *mode);
		~inkFile();
		bool Read(char **outString, unsigned long outLen);
		bool ReadLine(char **outString, unsigned long outLen);
		bool Write(char *inString, unsigned long inLen);
		bool Open(char *path, char *mode);
		void Close();
		bool ReName(char *newFilepath);
		bool Remove();
		bool Copy(char *newFilepath);
	private:
		void *rwBuff;
		unsigned long cSize;
		void *file;
		unsigned char flag;
		void sinit();
		void getsize();
		void getname();
		void setfullpath(char *path);
		unsigned getlinecontent(char *out, char LEC);
	};
}