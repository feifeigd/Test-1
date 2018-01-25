
#include "message.h"
#include "SocketClient.h"
#include "ByteBuffer.h"


Message::Message():data(NULL){
    
}
Message::~Message()
{
	if (data!=NULL) {
		delete[] data;
	}
}

int Message::datalength()
{
    return ByteBuffer::bytesToInt(length)+13;
}

Message* Message::clone()
{
	Message* pCloneMsg = new Message();
	if (pCloneMsg != nullptr)
	{
		pCloneMsg->HEAD0 = this->HEAD0;
		pCloneMsg->HEAD1 = this->HEAD1;
		pCloneMsg->HEAD2 = this->HEAD2;
		pCloneMsg->HEAD3 = this->HEAD3;
		pCloneMsg->ProtoVersion = this->ProtoVersion;
		memcpy(pCloneMsg->serverVersion, this->serverVersion, sizeof(unsigned char) * 4);
		memcpy(pCloneMsg->mainCmd, this->mainCmd, sizeof(unsigned char) * 4);
		memcpy(pCloneMsg->subCmd, this->subCmd, sizeof(unsigned char) * 4);
		memcpy(pCloneMsg->length, this->length, sizeof(unsigned char) * 4);
		pCloneMsg->data = new char[this->datalength()];
		memcpy(pCloneMsg->data, this->data, datalength());
		pCloneMsg->type = this->type;
		pCloneMsg->type_selfdefine = this->type_selfdefine;
	}
	return pCloneMsg;
}

Message* Message::createMessage(const char* data, size_t dataSize, int mainCmd, int subCmd)
{
	Message* msg = new Message();

	msg->HEAD0=78;
	msg->HEAD1=37;
	msg->HEAD2=38;
	msg->HEAD3=48;
	msg->ProtoVersion=9;

	int a=0;
	msg->serverVersion[3] = (unsigned char)(0xff & a);;
	msg->serverVersion[2] = (unsigned char)((0xff00 & a) >> 8);
	msg->serverVersion[1] = (unsigned char)((0xff0000 & a) >> 16);
	msg->serverVersion[0] = (unsigned char)((0xff000000 & a) >> 24);

	int b=dataSize+4;

	msg->length[3] = (unsigned char)(0xff & b);
	msg->length[2] = (unsigned char)((0xff00 & b) >> 8);
	msg->length[1] = (unsigned char)((0xff0000 & b) >> 16);
	msg->length[0] = (unsigned char)((0xff000000 & b) >> 24);

	int c = mainCmd;
	msg->mainCmd[3] = (unsigned char)(0xff & c);
	msg->mainCmd[2] = (unsigned char)((0xff00 & c) >> 8);
	msg->mainCmd[1] = (unsigned char)((0xff0000 & c) >> 16);
	msg->mainCmd[0] = (unsigned char)((0xff000000 & c) >> 24);

	int d = subCmd;
	msg->subCmd[3] = (unsigned char)(0xff & d);
	msg->subCmd[2] = (unsigned char)((0xff00 & d) >> 8);
	msg->subCmd[1] = (unsigned char)((0xff0000 & d) >> 16);
	msg->subCmd[0] = (unsigned char)((0xff000000 & d) >> 24);

	//    str.append(msg->HEAD0);
	//printf("%d" ,msg->datalength());
	msg->data = new char[msg->datalength()];
	memcpy(msg->data + 0, &msg->HEAD0, 1);
	memcpy(msg->data + 1, &msg->HEAD1, 1);
	memcpy(msg->data + 2, &msg->HEAD2, 1);
	memcpy(msg->data + 3, &msg->HEAD3, 1);
	memcpy(msg->data + 4, &msg->ProtoVersion, 1);
	memcpy(msg->data + 5, &msg->serverVersion, 4);
	memcpy(msg->data + 9, &msg->length, 4);
	memcpy(msg->data + 13, &msg->mainCmd, 4);
	memcpy(msg->data + 17, &msg->subCmd, 4);
	memcpy(msg->data + 21,data, dataSize);
	//memcpy(msg->data+position,bytes+offset,len);
	//msg->data = data;
	return msg;
}