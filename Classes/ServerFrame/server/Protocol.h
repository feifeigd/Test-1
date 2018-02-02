/*
* ͨѶЭ�� ��Ҫ����������Ͱ�ͷ
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define PASSWD_SIZE 32

enum SERVER_MAIN_CMD
{
	SERVER_MAIN
};

enum SERVER_SUB_CMD
{
	SERVER_SUB_OFFLINE,
};

enum MAIN_CMD
{
	CMD_HEARTBEART = 0,
	CMD_LOGIN_SERVER = 1,      // ��¼
};

enum LOGIN_CMD
{
	CMD_C2S_BEIGN = 0,
	CMD_C2S_REGISTER,
	CMD_C2S_LOGIN,
	CMD_C2S_END,

	CMD_S2C_BGINE = 100,
	CMD_S2C_REGISTER,
	CMD_S2C_LOGIN,
	CMD_S2C_END,
};

struct LOGIN_DATA
{
	int accountId;
};

#endif
