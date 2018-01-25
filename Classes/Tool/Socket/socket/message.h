#ifndef MESSAGE_H
#define MESSAGE_H

#include "Util.h"

class Message
{
public:
	
    
    char HEAD0;
    char HEAD1;
    char HEAD2;
    char HEAD3;
    char ProtoVersion;
    
    unsigned char serverVersion[4];
	unsigned char length[4];
	unsigned char mainCmd[4];
	unsigned char subCmd[4];
    /**
      * 消息的数据
      */
    char* data;
	
	int type;
	unsigned int type_selfdefine;
	
	
	Message();
    int datalength();
	~Message();

	// 实现一个深拷贝;
	Message* clone();

	static Message* createMessage(const char* data, size_t dataSize, int mainCmd, int subCmd);
};
#endif

