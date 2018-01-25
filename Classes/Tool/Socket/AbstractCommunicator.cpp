#include "AbstractCommunicator.h"


AbstractCommunicator::AbstractCommunicator()
{
	
}

AbstractCommunicator::~AbstractCommunicator()
{

}

void AbstractCommunicator::addListener(ICommListener* pListener)
{
	if (nullptr == pListener)
	{
		return;
	}
	auto iter = find(mVecListener.begin(), mVecListener.end(), pListener);
	if(iter == mVecListener.end())
		mVecListener.push_back(pListener);
}

void AbstractCommunicator::removeListener(ICommListener* pListener)
{
	if (nullptr == pListener)
	{
		return;
	}
	auto iter = find(mVecListener.begin(), mVecListener.end(), pListener);
	if(iter != mVecListener.end())
	{
		mVecListener.erase(iter);
	}
}

void AbstractCommunicator::sendMessage(int nCmdID, int nSubCmd, const char* szData)
{

}

void AbstractCommunicator::recvMessage(int nCmdID, int nSubCmd, char* szData)
{	
	//����ͨ�������쳣
	if (szData == NULL || strlen(szData) == 0)
	{
		printf("-----recvMessage: data is empty!-----");
		return;
	}

	//����ͨ�������쳣
	//Json::Reader read;
	//Json::Value root;
	//if (!read.parse(szData, root))
	//{
	//	printf("-----recvMessage: json decode error!-----");
	//	return;
	//}

	//��������ɷ���Ϣ, �����쳣����
	for (size_t i = 0; i < mVecListener.size(); i++)
	{
		mVecListener.at(i)->onRecvMessage(nCmdID, nSubCmd, szData);
	}
}

const char* AbstractCommunicator::constructErrorMessage(int nCmdID, int nSubCmd, char* szError)
{
	Json::FastWriter  writer;
	Json::Value val;
	val["result"] = 1;
	val["msg"] = szError;
	std::string  json_file=writer.write(val);	
	recvMessage(nCmdID, nSubCmd, (char*)json_file.c_str());
	return json_file.c_str();
}
