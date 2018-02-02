#ifndef LoginModel_h__
#define LoginModel_h__

#include "Observer/Observer.h"
#include <string>

using namespace std;

enum LOGIN_MODEL
{
	CMD_LOGIN = 1
};

class LoginModel : public Observable
{

public:

	LoginModel(void);
	~LoginModel(void);

	static LoginModel*  getInstance();
	static void  destroyInstance();


	void processLogin(int nMainCmd, int nSubCmd, char* szData);

private:

	static LoginModel*  m_pInstance;

};

#endif // LoginModel_h__
