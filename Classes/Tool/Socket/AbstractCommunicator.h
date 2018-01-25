#ifndef AbstractCommunicator_h__
#define AbstractCommunicator_h__

#include "ICommListener.h"

class AbstractCommunicator
{

public:

	AbstractCommunicator();
	virtual ~AbstractCommunicator();

public:

	//�����Ϣ������
	virtual void addListener(ICommListener* pListener);

	//�Ƴ���Ϣ������
	virtual void removeListener(ICommListener* pListener);

	//������Ϣ������������·�
	virtual void recvMessage(int nMainCmd, int nSubCmd, char* szData);

	//������Ϣ
	virtual void sendMessage(int nMainCmd, int nSubCmd, const char* szData);

protected:

	const char*  constructErrorMessage(int nMainCmd, int nSubCmd, char* szError);

protected:

	//����������
	std::vector<ICommListener*> mVecListener;

};

#endif //AbstractCommunicator_h__