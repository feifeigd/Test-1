#include "SocketClient.h"
#include "MSAutoReleasePool.h"
#include "NetDef.h"
#include <errno.h>
#include <signal.h>


SocketClient::SocketClient():
	mConnectState(NOT_CONNECTED),
	mRecvBuffer(RECEIVE_BUFFER_SIZE),
	mSendBuffer(SEND_BUFFER_SIZE),
	mCUR_ADDRESS_FAMILY(AF_INET),
	mSocketId(-1),
	mThreadRecvCreated(false),
	mThreadSendCreated(false),
	mSendThreadFinished(false),
	mRecvThreadFinished(false)
{   
    pthread_mutex_init(&mSendQueueMutex, NULL);
	pthread_mutex_init(&mRecvQueueMutex, NULL);
	pthread_mutex_init(&mThreadCondMutex, NULL);
	pthread_cond_init(&mThreadCond, NULL);
}

SocketClient::~SocketClient()
{
	clear();
}

//销毁线程并关闭Socket连接
void SocketClient::clear()
{
	//停止线程
	stopThreads();

	//关闭连接
	closeSocket();

	//清空队列
	pthread_mutex_lock(&mRecvQueueMutex);
	while (!mReceivedMessageQueue.empty()) 
	{
		Message* m = mReceivedMessageQueue.front();
		mReceivedMessageQueue.pop();
		SAFE_DELETE_ELEMENT(m);
	}
	pthread_mutex_unlock(&mRecvQueueMutex);

	pthread_mutex_lock(&mSendQueueMutex);
	while (!mSendMessageQueue.empty()) 
	{
		Message* m = mSendMessageQueue.front();
		mSendMessageQueue.pop();
		SAFE_DELETE_ELEMENT(m);
	}
	pthread_mutex_unlock(&mSendQueueMutex);

	pthread_mutex_destroy(&mSendQueueMutex);
	pthread_mutex_destroy(&mRecvQueueMutex);
	pthread_mutex_destroy(&mThreadCondMutex);
	pthread_cond_destroy(&mThreadCond);
}

void SocketClient::stopThreads()
{
	mRecvThreadFinished = true;
	mSendThreadFinished = true;

	//唤醒发送线程
	pthread_mutex_lock(&mThreadCondMutex);
	pthread_cond_signal(&mThreadCond);
	pthread_mutex_unlock(&mThreadCondMutex);

	//回收线程资源
	if (mThreadRecvCreated)
	{
		pthread_join(mThreadReceive, NULL);
		mThreadRecvCreated = false;
	}
	if (mThreadSendCreated)
	{
		pthread_join(mThreadSend, NULL);
		mThreadSendCreated = false;
	}
}

void SocketClient::startSocket(std::string host, int port)
{
	if (mConnectState == CONNECTED)
	{
		printf("============SocketClient::starSocket:already connected============");
		return;
	}
	else if(mConnectState == CONNECTING)
	{
		printf("============SocketClient::starSocket:already connecting============");
		return;
	}
	else if(mConnectState == TO_CONNECT)
	{
		printf("============SocketClient::starSocket:already to connect============");
		return;
	}
	else if (mConnectState == NOT_CONNECTED)
	{
		if(host.length() < 1 || port == 0) 
		{
			printf("============SocketClient::starSocket:ip or port invalid============");
			return;
		}

		mConnectState = TO_CONNECT;

		//Window平台需调用WSAStartup进行Socket初始化
#ifdef _WIN32
		WSADATA  wsd;
		WSAStartup(MAKEWORD(2,2), &wsd) ;
#endif

		// 格式化端口号;
		char szPort[16] = {0};
		sprintf(szPort, "%d", port);

		// 获取服务器参数信息;
		addrinfo hint, *srvAddrInfo;
		memset(&hint, 0, sizeof(addrinfo));
		hint.ai_socktype = SOCK_STREAM;
		hint.ai_family = AF_UNSPEC;
		int ret = getaddrinfo(host.c_str(), szPort, &hint, &srvAddrInfo);		// 返回0-成功 非0-出错;
		if (ret != 0)
			return;

		// 检查服务器配置类型(IPv4/IPv6)，客户端对应生成相同类型的配置;
		addrinfo* loopAdr = srvAddrInfo;
		for (; loopAdr != nullptr; loopAdr = loopAdr->ai_next)
		{
			switch (loopAdr->ai_family)
			{
			case AF_INET:
				{
					memset(&mSockAddr,0,sizeof(sockaddr_in));
					mSockAddr.sin_family = AF_INET;
					mSockAddr.sin_port = htons(port);
                    struct sockaddr_in* sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(loopAdr->ai_addr);
                    mSockAddr.sin_addr.s_addr = sockaddr_ipv4->sin_addr.s_addr;//inet_addr(host.c_str());
					mCUR_ADDRESS_FAMILY = mSockAddr.sin_family;
				}
				break;

			case AF_INET6:
				{
					memset(&mSockAddr_v6,0,sizeof(sockaddr_in6));
					mSockAddr_v6.sin6_family = AF_INET6;
					mSockAddr_v6.sin6_port = htons(port);
					mSockAddr_v6.sin6_addr = in6addr_any;
                    struct sockaddr_in6* sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(loopAdr->ai_addr);
                    mSockAddr_v6.sin6_addr = sockaddr_ipv6->sin6_addr;
					//inet_pton(AF_INET6, host.c_str(), &(mSockAddr_v6.sin6_addr));
					mCUR_ADDRESS_FAMILY = mSockAddr_v6.sin6_family;
				}
				break;

			default:
				return;
			}
            
            if (mCUR_ADDRESS_FAMILY == AF_INET || mCUR_ADDRESS_FAMILY == AF_INET6)
                break;
		}

		//初始化连接参数
		/*unsigned long dwServerIP = inet_addr(host.c_str());
		unsigned short wPort = port;
		memset(&mSockAddr,0,sizeof(mSockAddr));
		mSockAddr.sin_family = AF_INET;
		mSockAddr.sin_port = htons(wPort);
		mSockAddr.sin_addr.s_addr = dwServerIP;*/
	}

	//创建发送线程
	if(!mThreadSendCreated)
	{
		if(pthread_create( &mThreadSend, NULL,ThreadSendMessage, this) != 0)
		{
			printf("============SocketClient::starSocket:send thread create error!============");
			return;
		}
		mThreadSendCreated = true;
	}
	else
	{
		printf("============SocketClient::starSocket:send thread already createad!============");
	}

	//创建接收线程 rain mark: 接收线程放在主线程中创建，防止出现线程同步问题导致崩溃
	if( !mThreadRecvCreated )
	{
		if(pthread_create(&mThreadReceive, NULL,ThreadReceiveMessage, this) != 0)
		{
			printf("============SocketClient::starSocket:recv thread create error!============");
		}
		else
		{
			mThreadRecvCreated = true;
		}
	}
}

//关闭Socket连接
void SocketClient::closeSocket()
{
	mConnectState = NOT_CONNECTED;
	if( mSocketId != -1){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		closesocket(mSocketId);
#else
		close(mSocketId);
#endif
		mSocketId = -1;
	}
}

void SocketClient::sleepThread(long milli)
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Sleep(milli);
#else
	usleep(milli);
#endif

}

void SocketClient::connectSocket()
{
	if (mConnectState == CONNECTED)
	{
		return; //已经连接上，直接返回
	}
	else if (mConnectState == TO_CONNECT)
	{
		mConnectState = CONNECTING;

		int ret = -2;

		// 兼容IPv6;
		//mSocketId = ::socket(AF_INET, SOCK_STREAM, 0);
		mSocketId = ::socket(mCUR_ADDRESS_FAMILY, SOCK_STREAM, 0);

		if (mSocketId == -1)
		{
			printf("============SocketClient::connectSocket:socket create error!============");
			mConnectState = NOT_CONNECTED;
			return;
		}
		
		//设置非阻塞标记
		int noblock = 1;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		ret = ::ioctlsocket(mSocketId, FIONBIO, (u_long*)(&noblock));
#else
		ret = ::ioctl(mSocketId, FIONBIO, &noblock);
#endif

		if (ret != 0)
		{
			printf("============SocketClient::connectSocket:ioctlsocket error!============");
			mConnectState = NOT_CONNECTED;
			return;
		}
		else
		{
			// 由于采用非阻塞连接，connect会立刻返回-1，但并不是说发生了错误
			// 兼容IPv6;
			//ret = ::connect(mSocketId, (sockaddr*)&mSockAddr, sizeof(mSockAddr));
			ret = -2;
			switch (mCUR_ADDRESS_FAMILY)
			{
			case AF_INET:
				ret = ::connect(mSocketId, (sockaddr*)&mSockAddr, sizeof(sockaddr_in));
				break;

			case AF_INET6:
				ret = ::connect(mSocketId, (sockaddr*)&mSockAddr_v6, sizeof(sockaddr_in6));
				break;

			default:
				break;
			}
			
		}
		
		if (ret == 0)
		{
			printf("============SocketClient::connectSocket:connect success============");
			mConnectState = CONNECTED;
		}
		else if (ret == -1) //非阻塞连接中
		{
			bool isError = false;
			if (getErrno() == ENETUNREACH) //网络不可达
			{
				isError = true;
				printf("============SocketClient::connectSocket:network is unreachable!============");
			}
			else if (getErrno() == EHOSTUNREACH) //主机不可达
			{
				isError = true;
				printf("============SocketClient::connectSocket:network is hostdown!============");
			}

			if (isError)
			{
				mConnectState = NOT_CONNECTED;
				return;
			}
		}
		else
		{
			printf("============SocketClient::connectSocket:connect error ret=%d============", ret);
			mConnectState = NOT_CONNECTED;
			return;
		}
	}
	else if (mConnectState == CONNECTING)
	{
		timeval tv;
		tv.tv_sec = CONNECT_TIMEOUT_SECONDS;
		tv.tv_usec = 0;

		fd_set fdwrite;
		FD_ZERO(&fdwrite);
		FD_SET(mSocketId, &fdwrite);

		int ret = ::select(mSocketId + 1, 0, &fdwrite, 0, &tv);
		if (ret == 0)
		{
			printf("============SocketClient::connectSocket:select timeout errno=%d============", getErrno());

			//超时判定失去连接
			mConnectState = NOT_CONNECTED;
		}
		else if (ret < 0)
		{
			printf("============SocketClient::connectSocket:select error ret=%d============", ret);
			mConnectState = NOT_CONNECTED;
		}
		else
		{
			if (FD_ISSET(mSocketId, &fdwrite))
			{
				printf("============SocketClient::connectSocket:connect success============");
				mConnectState = CONNECTED;
			}
			else
			{
				printf("============SocketClient::connectSocket:mSocketID is not in the fdwrite============");
				mConnectState = NOT_CONNECTED;
			}
		}
	}
}

void SocketClient::threadSend()
{
	while (!mSendThreadFinished)
	{
		if (mConnectState == NOT_CONNECTED)
		{
			sleepThread(50);
			continue;
		}

		connectSocket();

		if (mConnectState == CONNECTED)
		{
			if (mSendMessageQueue.empty() && mSendBuffer.getPosition() <= 0)
			{
				//队列为空且缓冲区数据为空就wait，避免空转消耗CPU
				pthread_mutex_lock(&mThreadCondMutex);
				pthread_cond_wait(&mThreadCond, &mThreadCondMutex);
				pthread_mutex_unlock(&mThreadCondMutex);
			}
			
			if (mSendMessageQueue.empty() && mSendBuffer.getPosition() <= 0)
			{
				//呃。。这时候被唤醒，说明线程要被销毁了
			}
			else
			{
				timeval tv;
				tv.tv_sec = THREAD_SEND_TIMEOUT_SECONDS;
				tv.tv_usec = 0;

				fd_set fdexceptions;
				fd_set fdwrite;
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdexceptions);
				FD_SET(mSocketId, &fdwrite);
				FD_SET(mSocketId, &fdexceptions);

				int nRet = ::select(mSocketId + 1, 0, &fdwrite, 0, &tv);

				if (nRet == 0)	//正常超时
				{
					//log("============SocketClient::threadSend:select timeout============");
					continue;
				}
				else if (nRet < 0)	//出错了
				{
					printf("============SocketClient::threadSend:select error ret=%d!============", nRet);

					// 发生异常,停掉recv线程和send线程
					mSendThreadFinished = true;
					mRecvThreadFinished = true;
				}
				else
				{
					bool isBufferReady = false;

					pthread_mutex_lock(&mSendQueueMutex);

					//判断缓冲区是否有未发完的数据
					if (mSendBuffer.getPosition() > 0)
					{
						isBufferReady = true;
					}
					else
					{
						Message* msg = mSendMessageQueue.front();
						printf("============SocketClient::threadSend:send message length=%d============", msg->datalength());

						//判断缓冲区是否满了
						if(msg->datalength() + mSendBuffer.getPosition() > mSendBuffer.getLimit())
						{
							printf("============SocketClient::threadSend:send buffer full error!============");

							isBufferReady = false;

							//缓冲区满了，停掉recv线程和send线程
							mSendThreadFinished = true;
							mRecvThreadFinished = true;
						}
						else
						{
							//缓冲区未满，则将消息内容写入到缓冲区待发送
							mSendBuffer.put(msg->data, 0, msg->datalength());

							isBufferReady = true;
						}
					}
					
					if (isBufferReady)
					{
						mSendBuffer.flip();
						int bufferSize = mSendBuffer.getLimit();
						int nSentBytes = ::send(mSocketId, (char *)mSendBuffer.getBuffer(), mSendBuffer.getLimit(), 0);

						if (nSentBytes >= 0)
						{
							if (nSentBytes != 0)
							{
								//如果没发完，下次要从缓冲区发送数据
								mSendBuffer.setPosition(mSendBuffer.getPosition() + nSentBytes);
								mSendBuffer.compact();
							}

							if (nSentBytes == bufferSize)
							{
								//发送完毕后再从队列中移除消息
								Message* msg = mSendMessageQueue.front();
								mSendMessageQueue.pop();
								SAFE_DELETE_ELEMENT(msg);
							}
						}
						else
						{
							if(getErrno() == EWOULDBLOCK ||
							   getErrno() == EINTR ||
							   getErrno() == EINPROGRESS ||
							   getErrno() == EAGAIN ||
							   getErrno() == ENOBUFS)
							{
								//可恢复错误
								printf("============SocketClient::threadSend:send recovery errorno=%d============", getErrno());
							}
							else
							{
								printf("============SocketClient::threadSend:send errorno=%d!!!============", getErrno());

								// 发生异常,停掉recv线程和transfer线程
								mSendThreadFinished = true;
								mRecvThreadFinished = true;
							}
						}
					}
					pthread_mutex_unlock(&mSendQueueMutex);
				}
			}
		}

	}//while end

	mConnectState = NOT_CONNECTED;

	printf("============SocketClient::threadSend:exit============");
}

void SocketClient::threadReceive()
{
	fd_set fdreads, fdexceptions;
	while (!mRecvThreadFinished)
	{
		if (mConnectState != CONNECTED)
		{
			sleepThread(50);
			continue;
		}

		FD_ZERO(&fdreads);
		FD_ZERO(&fdexceptions);
		FD_SET(mSocketId, &fdreads);
		FD_SET(mSocketId, &fdexceptions);

		timeval tv;
		tv.tv_sec = THREAD_RECV_TIMEOUT_SECONDS;
		tv.tv_usec = 0;
		int nRet = ::select(mSocketId + 1, &fdreads, 0, &fdexceptions, &tv);

		if (nRet == 0)	//正常超时
		{
			//log("============SocketClient::threadReceive:select timeout errno=%d============", getErrno());
			continue;
		}
		else if (nRet < 0)	//出错了
		{
			printf("============SocketClient::threadReceive:select errorno=%d============", nRet);

			// 发生异常,停掉recv线程和send线程
			mSendThreadFinished = true;
			mRecvThreadFinished = true;
		}
		else
		{
			if (FD_ISSET(mSocketId, &fdexceptions))
			{
				printf("============SocketClient::threadReceive:fdexceptions============", nRet);

				// 发生异常,停掉recv线程和send线程
				mRecvThreadFinished = true;
				mSendThreadFinished = true;
				
			}
			else if (FD_ISSET(mSocketId, &fdreads))
			{

				if(mRecvBuffer.remaining() > 0) //判断接收消息缓冲区是否已满
				{
					int nRecvedBytes = ::recv(mSocketId, mRecvBuffer.getBuffer() + mRecvBuffer.getPosition(),
						mRecvBuffer.remaining(), 0);

					pthread_mutex_lock(&mRecvQueueMutex);

					if (nRecvedBytes < 0) //接收数据失败
					{
						printf("============SocketClient::threadReceive:recv recvedBytes=%d============", nRecvedBytes);
						printf("============SocketClient::threadReceive:recv errorno=%d============", getErrno());

						mSendThreadFinished = true;
						mRecvThreadFinished = true;

					}
					else if(nRecvedBytes == 0) //服务器关闭连接
					{
						printf("============SocketClient::threadReceive:server close connection!!============");
						printf("============SocketClient::threadReceive:server close connection errorno=%d============", getErrno());

						mSendThreadFinished = true;
						mRecvThreadFinished = true;
					}
					else //正常接收数据
					{
						printf("============SocketClient::threadReceive:receive bytes:%d============", nRecvedBytes);

						mRecvBuffer.setPosition(mRecvBuffer.getPosition()+ nRecvedBytes);
						mRecvBuffer.flip();
						int tmpOffset = 17;

						while(mRecvBuffer.remaining() > tmpOffset)
						{
							int pos = mRecvBuffer.position;
							int length = mRecvBuffer.getLength(9) - 4; //服务器返回数据长度多加4

// 							CCLOG("+++++++++++++++length:%d+++++++++++++++",length);
// 							CCLOG("+++++++++++++++pos:%d+++++++++++++++",pos);
// 							CCLOG("+++++++++++++++limit:%d+++++++++++++++",mRecvBuffer.getLimit());
// 							CCLOG("+++++++++++++++capacity:%d+++++++++++++++",mRecvBuffer.getCapacity());

							if(mRecvBuffer.remaining() >=  tmpOffset + length){

								Message* message = new Message(); //memory leak?

								message->type = 1;
								message->HEAD0 = mRecvBuffer.getByte();
								message->HEAD1 = mRecvBuffer.getByte();
								message->HEAD2 = mRecvBuffer.getByte();
								message->HEAD3 = mRecvBuffer.getByte();
								message->ProtoVersion = mRecvBuffer.getByte();
								mRecvBuffer.getAsBytes(message->serverVersion);
								mRecvBuffer.getAsBytes(message->length);
								mRecvBuffer.getAsBytes(message->mainCmd);
								mRecvBuffer.getAsBytes(message->subCmd);

								char* tmp = new char[length + 1];
								mRecvBuffer.get(tmp, 0, length);
								tmp[length] = '\0';
								message->data = tmp;

								mReceivedMessageQueue.push(message);

								printf("============SocketClient::threadReceive:receive message:mainCmd=%d, subCmd=%d, length=%d============"
									, ByteBuffer::bytesToInt(message->mainCmd), ByteBuffer::bytesToInt(message->subCmd), ByteBuffer::bytesToInt(message->length));
								printf("============SocketClient::threadReceive:m_receivedMessageQueue size=%d============", mReceivedMessageQueue.size());

							}
							else if(length > mRecvBuffer.getCapacity())
							{

								printf("============SocketClient::threadReceive:message.length > m_cbRecvBuf.getCapacity()!!!============");

								mSendThreadFinished = true;
								mRecvThreadFinished = true;
								break;
								
							}else {

								mRecvBuffer.position = pos;
								printf("============mRecvBuffer.position = pos!!!============");
								break;
							}
						}

						mRecvBuffer.compact();
					}

					pthread_mutex_unlock(&mRecvQueueMutex);
				}
				else
				{
					printf("============SocketClient::threadReceive:m_cbRecvBuf.remaining()<=0!!!============");
					
					mSendThreadFinished = true;
					mRecvThreadFinished = true;
				}
			}
		}
	}

	mConnectState = NOT_CONNECTED;

	//唤醒发送线程
	pthread_mutex_lock(&mThreadCondMutex);
	pthread_cond_signal(&mThreadCond);
	pthread_mutex_unlock(&mThreadCondMutex);

	printf("============SocketClient::threadReceive:exit============");
}

SOCKET_CONNECT_STATE SocketClient::getConnectState()
{
	return mConnectState;
}

void SocketClient::pushSendMessage(Message* msg)
{    
	pthread_mutex_lock(&mSendQueueMutex);
	mSendMessageQueue.push(msg);
	pthread_mutex_unlock(&mSendQueueMutex);

	//唤醒发送线程
	pthread_mutex_lock(&mThreadCondMutex);
	pthread_cond_signal(&mThreadCond);
	pthread_mutex_unlock(&mThreadCondMutex);
}

Message* SocketClient::popReceivedMessage()
{
	Message* msg = nullptr;
	pthread_mutex_lock(&mRecvQueueMutex);
	if( mReceivedMessageQueue.size()>0)
	{
		msg = mReceivedMessageQueue.front();
		mReceivedMessageQueue.pop();
	}
	pthread_mutex_unlock(&mRecvQueueMutex);
	return msg;
}

int SocketClient::getErrno()
{
	return errno;
}

bool SocketClient::isNetAvailable()
{
	return mConnectState != NOT_CONNECTED;
}

void* SocketClient::ThreadReceiveMessage(void *p)
{
	SocketClient* socketClient = static_cast<SocketClient*>(p);
	socketClient->threadReceive();
	return (void*)0;
}

void* SocketClient::ThreadSendMessage(void *p)
{
	SocketClient* socketClient = static_cast<SocketClient*>(p);
	socketClient->threadSend();
	return (void*)0;
}

