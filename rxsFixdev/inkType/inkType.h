#pragma once

namespace inkType
{

	class  __declspec(dllexport) inkReger {
	public:
		inkReger(char *snbuff, unsigned snLen);
		~inkReger();
		typedef struct _regcodeseg {
			char rCodeo[14];
			char rCodet[14];
			char rCodeth[14];
			char rCodef[14];
		}inkRegCode;
		void GetCode(char buff[45]);
		bool RegCode(inkRegCode *irgc);
		// return 0 = 永久有效， = 1 期限注册， = -1 未注册（无效注册码）
		char HasReg(char *outExpirydate, unsigned &outHashours);
		void GetRegCode(char mcode[45], inkRegCode *irgc, unsigned Rtimedist = 0);
	private:
		unsigned	Expirydh;
		void *data;
		unsigned rn, ry, rr, rh;
		unsigned char *table, *Htable;
		char retBuf[256];
		void InitTable();
		void GetCode();
		float Calucavgd(unsigned s, unsigned l);
		bool DeCode(char *regCode);
		char Getmapindex(char val);
	};

	class __declspec(dllexport) inkString
	{
	public:
		inkString(unsigned ds = 100);
		~inkString();
		//inkString *operator+(inkString str);
		inkString *operator+(char *str);
		inkString *operator=(char *str);
		char operator[](unsigned  int);
		char *chrString();
		unsigned Length();
		void SetLength(unsigned len);
		bool InsertStr(unsigned Startindex, char *str);
		bool RemoveStr(unsigned Startindex, char *str);
		void Format(unsigned int StartIndex, const char *fstr, ...);
	private:
		char *Data;
		unsigned length;
		unsigned Datasize;
		unsigned Nouselen;
		void Resetdata();
		void kvscmp(char *str);
	};

	class  __declspec(dllexport) inkMatrix
	{
	protected:
		typedef struct _matrix
		{
			void *Data;
			unsigned long Cols, Rows;
			unsigned ElementSize;
		}Matrix;
	private:
		Matrix  matrix;
	public:
		inkMatrix(unsigned iRows, unsigned iCols, unsigned iElesize);
		~inkMatrix();
		// iRow,iCol  行索引，列索引
		void* GetValue(unsigned int iRow, unsigned int iCol);
		inkMatrix* operator+(inkMatrix ma);
		inkMatrix* operator-(inkMatrix ma);
		inkMatrix* operator*(inkMatrix ma);
		void Setelement(unsigned int iRow, unsigned int iCol, void *Value);
		//Value 特定字符串格式有效x,x,x,x,x,x,x,x.... 未完成...保留
		void Setelement(char *Value);
	private:

	};

	class  __declspec(dllexport) inkNtree
	{
	protected:
		typedef struct _tnode
		{
			unsigned Serirow, Sericol;
			void *Data;
			_tnode	*Parent;
			_tnode	*Left, *Right;
			_tnode	*FirstChild, *LastChild;
		}TreeNode;
	public:
		inkNtree();
		~inkNtree();
		void InsertNode();
		void DeleteNode();
		TreeNode* FindNode();
		TreeNode* Root;
	private:

	};
	class __declspec(dllexport) inkList
	{
	public:
		typedef struct _node
		{
			unsigned index;
			unsigned DataLen;
			void *Data;
			struct _node *Next;
		}inkNode;
		inkList();
		~inkList();
		void Insert(void *Data, unsigned len);
		inkNode *operator[](unsigned  index);
		unsigned Count();
		void Clear();
	protected:
		inkNode *Head;
	private:
		unsigned CurCount;
		inkNode* CurNode;
	};
	class  __declspec(dllexport) inkHashtable
	{
	protected:

	public:
		inkHashtable(unsigned ds = 10);
		~inkHashtable();
		void *operator[](const char *);
		void *operator[](int);
		bool Forech(unsigned index, char **outKey, char **outVal);
		void Insert(const char*key, void *Data, unsigned dlen);
		void Insert(int key, void *Data, unsigned dlen);
		void Clear();
		unsigned long Size();
	private:
		typedef struct _kv
		{
			char *key;
			void *value;
		}Kv;
		unsigned long len;
		int* manger_ils;
		unsigned klen, ilsLen, index;
		long base_value;
		const char *tempkey;
		int			itempkey;
		unsigned char hasvflag;
		long calculatebv(const char* key, unsigned *len);
		void *findvfk(unsigned index, const char* key);
		void *findvfk(unsigned index, int key);
		bool getmaplocation(unsigned index, unsigned *i, unsigned long *sumcount);
	};


	class  __declspec(dllexport) inkVector
	{
	private:
		void *Data;
		unsigned dfbknum;
		unsigned bksz;
		unsigned count;
		void rebuilddata();
	public:
		inkVector(unsigned blkSize);
		~inkVector();
		void Push(void *Data);
		void Pop(unsigned index);
		void *operator[](unsigned  index);
		unsigned long Size();
	};


	class  __declspec(dllexport) inkObject {
	private:
		void *DataBuff;
		unsigned	dfLen;
	public:
		char *toString();
	};


	enum RowFlag { ADD = 0, DEL, UPDATE, NOMARL };
	typedef RowFlag	ColumnFlag;

	class  __declspec(dllexport) inkColumns
	{
	public:
		typedef struct _inkcolumn
		{
			inkString	Name;
			unsigned	Index;
			ColumnFlag	Flag;
		}inkColumn;
		unsigned long Count;
		inkColumns();
		~inkColumns();
		inkColumn *operator[](unsigned  index);
		inkColumn *operator[](char  *name);
		bool Add(char* Name);
		bool Del(unsigned index);
	private:
		void *Data;
		void *MapIndexName;
		unsigned colsize;
		void clear();
		void remarkdata();
		void updateindex(unsigned index);
		void insertdata(inkColumn ikCol);
	};


	class  __declspec(dllexport) inkRows
	{
	public:
		typedef struct _onerow {
			inkObject *Group;
			RowFlag	Rowflag;
		}Row;
		void *operator[](unsigned  Colindex);
		void *operator[](char*  Colname);
		inkRows(inkColumns *cols, unsigned long RowCount);
		~inkRows();
		unsigned long Count;
		bool Add();
		bool Del();
	private:
		Row	*Rows;
		inkColumns *columns;
		unsigned getindexfromColname(char *ColName);
	};

	class  __declspec(dllexport) inkDataTable
	{
	public:
		inkDataTable();
		~inkDataTable();
		inkString	TableName;
		void Add(void *ColsDataArray, unsigned Adlen);
		void Updata(unsigned ir, unsigned ic, void* val);
		void *operator[](unsigned  index);
		const char *GetData(unsigned iRows, unsigned iCols);
		void Clear();
		unsigned RowsCount, ColsCount;
		void Sort(unsigned WichCol = 0);
	private:
		unsigned long rows;
		void *Columns;
		void *Rows;
	};

	class __declspec(dllexport) BitMange
	{
	private:
		void *buff;
		unsigned blen, arLen, bsLen;
		char caluarindex(unsigned index, unsigned *ari);
		void init();
	public:
		BitMange(unsigned bitLen = 8);
		~BitMange();
		void SetBitValue(unsigned index, char value);
		bool ReadBitval(unsigned bitIndex);
		bool HasFlagOn(unsigned &FirstbitIndex, unsigned char mVal = 1);
	};



	class  __declspec(dllexport) Coded
	{
	public:
		Coded();
		~Coded();
		//mode = 0 为 base64 ,=1 utf-8, = 2 MD5.....
		void SetEncodeMode(unsigned char mode);
		char *EnCode(char *SrcStr, unsigned slen);
		char *DeCode(char *EnStr, unsigned slen);
	private:
		char* result;
		void initresult(unsigned lens);
		unsigned char emode;
		char* base64encode(char *srcStr, unsigned slen);
		char* base64decode(char *enStr, unsigned slen);
		typedef struct _mdfinfo
		{
			//strBaye的长度
			unsigned int strlength;
			//A,B,C,D的临时变量
			unsigned int atemp;
			unsigned int btemp;
			unsigned int ctemp;
			unsigned int dtemp;
		}MDPARAM;
		MDPARAM mdp;
		char* getmd5(char *srcStr, unsigned slen);
		unsigned int* add(char* str, unsigned length);
		void mainLoop(unsigned int M[]);
		void ConvertHex(int a, char *outStr);
	};
}