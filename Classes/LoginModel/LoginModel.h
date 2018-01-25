#ifndef LoginModel_h__
#define LoginModel_h__

#include "Observer/Observer.h"
#include <string>

using namespace std;

class LoginModel : public Observable
{

public:

	LoginModel(void);
	~LoginModel(void);

	static LoginModel*  getInstance();
	static void  destroyInstance();

	void updateNotify();


	void processLogin(char* szData);

private:

	static LoginModel*  m_pInstance;

};

#endif // LoginModel_h__
