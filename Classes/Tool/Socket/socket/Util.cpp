#include "Util.h"
#include <stdlib.h>
#include "ByteBuffer.h"
#include <vector>
#include <sys/stat.h>


std::string byteToHexStr(unsigned char *byte_arr, int arr_len)
{
	std::string  hexstr;
	for (int i=0;i<arr_len;i++)
	{
		char hex1;
		char hex2;
		int value=byte_arr[i]; //????unsigned char?????????????????????????
		int v1=value/16;
		int v2=value % 16;
		//??????????
		if (v1>=0&&v1<=9)
			hex1=(char)(48+v1);
		else
			hex1=(char)(55+v1);

		//????????????
		if (v2>=0&&v2<=9)
			hex2=(char)(48+v2);
		else
			hex2=(char)(55+v2);

		//???????????
		hexstr+= hex1;
		hexstr+= hex2;
		
	}
	return hexstr;

}



double distance(double x1, double y1, double x2, double y2) {
	double dx = x1 - x2;
	double dy = y1 - y2;
	if (dx == 0) {
		return dy > 0 ? dy : -dy;
	}
	if (dy == 0) {
		return dx > 0 ? dx : -dx;
	}
	return sqrt(dx * dx + dy * dy);
}


const char* fullpathFromRelatePath(const char* relatePath){	
	const char* relate = relatePath[0]=='/'?(relatePath+1):relatePath;
	//const char *path =CCFileUtils::fullPathFromRelativePath(relate);
	return relate;
}

/**
 存档路径
 **/
std::string getWriteableFullPath(const char* relatePath){
	const char* relate = relatePath[0]=='/'?(relatePath+1):relatePath;
	//string s =  CCFileUtils::getWriteablePath();
	//s.append(relate);
	return relate;
}
/**
 writeFileData : pszfullFilepath 必须是存档路径
 **/
bool writeSaveData(const char* relatePath,char* buf,int len){
	std::string pszfullFilepath = getWriteableFullPath(relatePath);
	FILE *fp = fopen(pszfullFilepath.c_str(),"wb");
	if( fp!=NULL){
		size_t nn = fwrite(buf, 1, len, fp);	
		fclose(fp);
		
		if( nn!= len){
			//printf("writeFileData(%s) error writed size(%d) < len(%d) \n ",pszfullFilepath.c_str(),(int)nn,len);
			return false;
		}
		return true;
	}
	//printf("writeFileData(%s) error fopen fail \n ",pszfullFilepath.c_str() );
	return false;
}
ByteBuffer* readSaveData(const char* relatePath){	
	std::string pszfullFilepath = getWriteableFullPath(relatePath);
	ByteBuffer * pBuffer = NULL;	
	int size = 0;
	do 
	{
		// read the file from hardware
		FILE *fp = fopen(pszfullFilepath.c_str(), "rb");
		if (!fp)
		{
			break;
		}
		fseek(fp,0,SEEK_END);
		size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		pBuffer = new ByteBuffer(size);
		int ret = fread(pBuffer->getBuffer(), size, 1,fp);
		if( ret!=1){
			delete pBuffer;
			pBuffer=NULL;
		}
		
		fclose(fp);
	} while (0);
	
	return pBuffer;    
}

void deleteSaveData(const char* relatePath){
	std::string pszfullFilepath = getWriteableFullPath(relatePath);
	remove(pszfullFilepath.c_str());
}

ByteBuffer* getFileData(const char* pszFileName, bool isfullPath)
{
	std::string temp = pszFileName;
	if (!isfullPath) {
		//temp = ResourceDataManager::getInstance()->fullPath(temp);
//		printf("getFileData temp = %s\n", temp.c_str());
	}
	unsigned long size = 0;
	char* pBuffer = NULL;//(char*)CCFileUtils::getFileData(temp.c_str(),"rb",&size);
//	printf("Util:: getFileData() %s,data = %p,size = %lld\n ",pszFileName,pBuffer,size);
	ByteBuffer* p = NULL;
	if( size>0 && pBuffer){
		p =  new ByteBuffer(pBuffer,0,size);
	}
//	printf("Util:: getFileData() %s,data = %p,size = %d,ByteBuffer = %p\n ",pszFileName,pBuffer,size,p);
	if( pBuffer){
		delete[] pBuffer;
	}
//	printf("Util:: getFileData() end\n ");
	
		
	return p;
}
unsigned char* getFileData(const char* pszFileName, bool isfullPath,unsigned int* size)
{
	std::string temp = pszFileName;
	if (!isfullPath) {
		//temp = ResourceDataManager::getInstance()->fullPath(temp);
//		printf("getFileData temp = %s\n", temp.c_str());
	}
	//unsigned long lsize;
	unsigned char* pBuffer = NULL;//(unsigned char*)CCFileUtils::getFileData(temp.c_str(),"rb", &lsize);
	//*size = lsize;
	//	printf("Util:: getFileData() %s,data = %p,size = %d\n ",pszFileName,pBuffer,size);
	
	
	return pBuffer;
}
static int randC = 0;
std::string createRandString(int len,bool filter){
	std::string s;
	s.resize(len+10);
	char c;
	for (int i = 0; i < len; i++) {
		randC = 1;//(int) ((System::currentTimeMillis() >> (i * ((randC % 4) + 1))) % 36);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		Sleep(randC);
#else
		sleep(randC);
#endif
			
		if (randC < 10) {
			randC = 48 + randC;
		} else {
			randC = 97 + (randC - 10);
		}
		c = (char)randC;
		if( filter){
			if( c == 'g' || c=='G') c = '9';
			else if( c == 'm' || c=='M') c = '5';
		}            
		if (randC % 2 == 0)
			s.push_back(c);				
		else
			s.insert( s.begin(),1,c);
	}
	return s;
}

void makeDirectory(const std::string& path){
	std::string root = "";
	std::string temp = path;
	size_t index = temp.find("/");
	
	while (index != std::string::npos) {
		std::string dir = temp.substr(0, index);
		root.append(dir);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#else
		mkdir(root.c_str(), 511);
#endif
		temp = temp.substr(index + 1, temp.length()-index-1);
		index = temp.find("/");
		root.append("/");
	}
	root.append(temp);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#else
	mkdir(root.c_str(), 511);
#endif
	//	int a = access(root.c_str(), 0);
	//	if (a == 0) {
	//		//printf("创建多级目录: 成功 path=%s\n", root.c_str());
	//	}else {
	//		//printf("创建多级目录: 失败 path=%s\n", root.c_str());
//	}
}

unsigned long msNextPOT(unsigned long x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}

std::vector<std::string> splitStr(const std::string &str){
	size_t beginIndex = 0;
	std::vector<std::string> value;
	for (size_t i=0; i<str.size();i++) {
		if (str[i]==',') {
			std::string s = i> beginIndex ? str.substr(beginIndex, i - beginIndex) : "";
			beginIndex = i+1;
			value.push_back(s);
		}
	}
	return value;
}
