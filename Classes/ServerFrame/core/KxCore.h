/*
*   Core �����������ļ�
*   ��ƽ̨Ԥ����
*/
#ifndef __KXCORE_H__
#define __KXCORE_H__

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024  //�޸ķ�����ͬʱ���ӵ����������
#endif

#include "KxPlatform.h"
#include "KxObject.h"

#include <assert.h>
#include <algorithm>
#define KXASSERT assert

//#define RunningInServer

//����ƽ̨��ص�ʱ�䣬Socket����
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)

#include <WinSock2.h>
#include <WinSock.h>
#include <Windows.h>
#include <time.h>

// ͨѶID
typedef SOCKET KXCOMMID;
typedef int kxSockLen;

// ��д�����Ƿ������
#define SOCKERR_RW_RETRIABLE(e)			\
	((e) == WSAEWOULDBLOCK ||			\
	    (e) == WSAEINTR)

// connect�����Ƿ������
#define SOCKERR_CONNECT_RETRIABLE(e)	\
	((e) == WSAEWOULDBLOCK ||			\
	    (e) == WSAEINTR ||				\
	    (e) == WSAEINPROGRESS ||		\
	    (e) == WSAEINVAL)

// accept�����Ƿ������
#define SOCKERR_ACCEPT_RETRIABLE(e)		\
	SOCKERR_RW_RETRIABLE(e)

// connect�����Ƿ񱻾ܾ�
#define SOCKERR_CONNECT_REFUSED(e)		\
	((e) == WSAECONNREFUSED)

#ifndef snprintf
#define snprintf  sprintf_s
#endif // snprintf

#define stlocaltime(pTime,ptm) localtime_s(ptm,pTime)
#define strncpys strncpy_s
#define sstrdup _strdup
#define KXINVALID_COMMID (INVALID_SOCKET)

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval * val, struct timezone * zone);

#pragma comment(lib, "ws2_32.lib")

#else

#define KXINVALID_COMMID (-1)

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/time.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include <signal.h>
#include <stdarg.h>


// ��д�����Ƿ������
#define SOCKERR_RW_RETRIABLE(e)			\
	((e) == EINTR || (e) == EAGAIN ||	\
	(e) == EWOULDBLOCK)

// connect�����Ƿ������
#define SOCKERR_CONNECT_RETRIABLE(e)	\
	((e) == EINTR || (e) == EINPROGRESS)

// accept�����Ƿ������
#define SOCKERR_ACCEPT_RETRIABLE(e)		\
	((e) == EINTR || (e) == EAGAIN ||	\
	(e) == ECONNABORTED)

// connect�����Ƿ񱻾ܾ�
#define SOCKERR_CONNECT_REFUSED(e)		\
	((e) == ECONNREFUSED)

typedef int KXCOMMID;
typedef socklen_t kxSockLen;
#define strncpys strncpy
#define sstrdup strdup
#define vsnprintf_s vsnprintf
#define stlocaltime(pTime,ptm) localtime_r(pTime,ptm)
#endif

#endif