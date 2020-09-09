
#ifndef _CL_IOCPMGR_HPP_
#define _CL_IOCPMGR_HPP_

#include "../_cl_common/CLCommon.h"
#include <windows.h>
#include <tchar.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <vector>
#include <iostream>
#include "WS2tcpip.h"
#pragma comment(lib, "WS2_32.lib")

//�����ڴ��֧��-----------------------------------
//#undef CLMP_USE_LOCK_TYPE
//#define CLMP_USE_LOCK_TYPE 1
#include "../_cl_objectMemPool/CLMemPool.h" //��δʹ���ڴ����ע�͵�

#ifndef newObj //����ʹ�õ��ĺ꣬�����ڴ�ض��帲��
#define newObj( x )  new x
#define deleteObj( x ) delete x
#define giveUpObj( x ) delete x
#endif

#define ACCEPT_SOCKET_NUM  20 //��ʼ������
#define DEF_PORT 20011
#define DEF_IPADDR _T("127.0.0.1")
//������¼�ö��ֵ
typedef enum _OVLP_TYPE
{
	CPKEY_NULL = 0x0,
	CPKEY_ACP = 0x1,
	CPKEY_CLOSE = 0x2,
	CPKEY_RECV = 0x4,
	CPKEY_SEND = 0x8,
} OVLP_TYPE, *POVLP_TYPE;
//�󶨼����׽���Ŀ�����Ϣ�ṹ��
typedef class LISTENER_SOCKET
{
public:
	SOCKET m_sListen;                            // ������Socket
	LPFN_ACCEPTEX m_lpfnAcceptEx;                // �ͼ���sok�󶨵Ŀ��ƺ���
	LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptSockAddrs;    // �ͼ���sok�󶨵Ŀ��ƺ���
	TCHAR m_ip[50];							               // �ü���socket������Ŀ��ip��ַ
	USHORT m_port;										   // �ü���socket������Ŀ��ip��ַ��Ӧ�Ķ˿�
	LISTENER_SOCKET(){reset();}
	~LISTENER_SOCKET(){reset();}
	void reset(){
		m_sListen = INVALID_SOCKET;
		m_lpfnAcceptEx = 0;
		m_lpfnGetAcceptSockAddrs = 0;
		_tcscpy_s(m_ip, DEF_IPADDR);
		m_port = DEF_PORT;
	}
	BOOL isListening() const {
		return m_sListen != INVALID_SOCKET && m_lpfnAcceptEx && m_lpfnGetAcceptSockAddrs;
	}
} *PLISTENER_SOCKET;
//�ڲ�iocpʹ�õ��ص��ṹ����չ���࣬����ʵ��resetBuffer()�������������û���ռ�
typedef class OVLP_BASE
{
public:
	OVERLAPPED   m_Overlapped;				// ÿһ���ص�I/O���������Ҫ��һ��
	SOCKET       m_sSock;					// ���I/O������ʹ�õ�Socket��ÿ�����ӵĶ���һ����
	SOCKADDR_IN  m_addr;			        // �Զ˵�ַ�ṹ
	OVLP_TYPE	 m_olType;               // ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
	WSABUF       m_wsaBuf;					// �洢���ݵĻ��������������ص��������ݲ�����
	const LISTENER_SOCKET* pSokPack;	    // �����ṹ��ָ��
	OVLP_BASE(void){		
		resetOverlapped();
		m_sSock = INVALID_SOCKET;
	}
	virtual ~OVLP_BASE(void){
		//ol->close(); //close����Ӧ�����ͷ�ǰ�ֶ�����
	}
	void resetOverlapped(){
		ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
	}
	//�ú���Ӧ�ñ���д,��ɻ���ռ�WSABUF m_wsaBuf������
	virtual void resetBuffer() = 0;
	void close() {
		if (m_sSock != INVALID_SOCKET) {
			shutdown(m_sSock, SD_BOTH);
			closesocket(m_sSock);
		}
		m_sSock = INVALID_SOCKET;
	}
	void reset() {
		resetBuffer();
		resetOverlapped();
		close();
	}
	void inheritSocket(OVLP_BASE* oltag) {
		if (oltag)
			m_sSock = oltag->m_sSock, oltag->m_sSock = INVALID_SOCKET;
	}
} *POVLP_BASE;
//ÿһ���ͻ��˶�Ӧ��io����
typedef class CLIENT_IOINFO
{
public:
	SOCKET                   m_Socket;              // ÿһ���ͻ������ӵ�Socket
	SOCKADDR_IN              m_ClientAddr;          // ����ͻ��˵ĵ�ַ
	std::vector<OVLP_BASE*>& m_arrayIoContext;      // ���飬���пͻ���IO�����Ĳ�����Ҳ����˵����ÿһ���ͻ���Socket�ǿ���������ͬʱͶ�ݶ��IO�����
	CLIENT_IOINFO():
		m_arrayIoContext(*newObj(std::vector<OVLP_BASE*>))
	{
		m_arrayIoContext.clear();
		m_Socket = INVALID_SOCKET;
		m_ClientAddr = { 0 };
	}
	~CLIENT_IOINFO() {
		if (m_Socket != INVALID_SOCKET) {
			shutdown(m_Socket, SD_BOTH);
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		m_arrayIoContext.clear();
		giveUpObj(&m_arrayIoContext);
	}
} *PCLIENTOVLP;

#define OVLP_DEFAULT_BUFFER_LEN 4096 //4kb

//ʵ�־���Ļ��������������ص��ṹ�࣬buffer��СOVLP_DEFAULT_BUFFER_LEN���ֽڡ�
template<int _bufSize = OVLP_DEFAULT_BUFFER_LEN>
class OVLP :public OVLP_BASE
{
public:
	char m_szBuffer[_bufSize]; // ��Ӧ����WSABUF��Ļ�����
	OVLP() {
		resetBuffer();
	}
	virtual void resetBuffer()
	{
		ZeroMemory(m_szBuffer, _bufSize);
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = _bufSize;
	}
};

//iocpģ�ͷ������ģ���࣬��������̳в����عؼ�������ʵ�ֶ�̬
template<typename _MY_OVLP = OVLP<>>
class CLIocpServeBase : protected CLTaskSvc
{	
public:
	using OVLP = _MY_OVLP;
	using POVLP = _MY_OVLP*;

	CLIocpServeBase()
	{
		setTrdCounts();
		initLock(0);
		createIOCP();
		if (getIOCP() == 0 || getIOCP() == INVALID_HANDLE_VALUE)
			throw std::logic_error("iocp manager create fail!");
	}
	virtual ~CLIocpServeBase()
	{
		closeServe();
		closeIOCP();
	}
	//ע��һ��������ַ�Ͷ˿ڣ����������ж����еļ�����������ַ�Ѿ����������Ӽ�����ַ��ֻ�������м����б�
	BOOL startListen(USHORT port, LPCTSTR ip = DEF_IPADDR)
	{
		addListen(port, ip);
		return preStartListen(port, ip) ? initWinSockListener() : FALSE;
	}
	//���������Ѿ��������������еļ���
	BOOL startListen()
	{
		return initWinSockListener();
	}
	//���ù������߳�����0��ʾ�Զ�
	void setTrdCounts(size_t n = 0) {
		m_trdCounts = (n == 0 ? getCpuCoreCounts() : n);
	}
	//ȡ�������еĹ������߳���
	size_t getRunningTrdCounts() const {
		return CLTaskSvc::getActivateThreadsNum();
	}
	//����һ��������ַ�Ͷ˿ڣ�����startListen֮ǰ���ӣ��رշ�������м�����ַ���˿��б����գ��ٴ�������������������
	BOOL addListen(USHORT port = DEF_PORT, LPCTSTR ip = DEF_IPADDR)
	{
		for (auto i = m_ListenLst.cbegin(); i != m_ListenLst.cend(); i++)
		{
			if (i->m_port == port && ip && _tcscmp(i->m_ip, ip) == 0)
				return FALSE;
		}
		LISTENER_SOCKET lsPack;
		lsPack.m_port = port;
		_tcscpy_s(lsPack.m_ip, ip);
		m_ListenLst.push_back(lsPack);
		return 1;
	}
	//�رռ�������
	BOOL closeServe()
	{
		postClose();//�����أ�����iocp�ر���Ϣ����ɶ�

		lock();
		//2:��յȴ�accept���׽���m_vecAcps
		auto iter = m_vecAcps.begin();
		for (; iter != m_vecAcps.end(); )
		{
			OVLP_BASE* p = *iter;
			if (p) {
				p->close();
				deleteObj(p);
			}
			++iter;
		}
		m_vecAcps.clear();
		//3:��������ӵ��׽���m_vecContInfo����ջ���
		auto iter2 = m_vecContInfo.begin();
		for (; iter2 != m_vecContInfo.end(); )
		{
			OVLP_BASE* ol = iter2->second;
			if (ol) {
				ol->close();
				deleteObj(ol);
			}
			++iter2;
		}
		m_vecContInfo.clear();

		for (size_t i = 0; i < m_ListenLst.size(); i++)
		{
			if (m_ListenLst[i].m_sListen != INVALID_SOCKET)
				closesocket(m_ListenLst[i].m_sListen);
			m_ListenLst[i].m_sListen = INVALID_SOCKET;
		}
		m_ListenLst.clear();

		if (wsaData.wVersion > 0)
		{
			WSACleanup();
			wsaData = { 0 };
		}
		unlock();
		return TRUE;
	}
	//����Ƿ�����
	BOOL isListening() const
	{
		return CLTaskSvc::getActivateThreadsNum() ? TRUE : FALSE;
	}
private:
	//iocp���
	HANDLE m_hIOCompletionPort = 0;
	//�����׽��ֶ���
	std::vector<LISTENER_SOCKET> m_ListenLst;
	WSADATA wsaData = { 0 };
	//�ȴ�accept���׽��֣���Щ�׽�����û��ʹ�ù��ģ�����ΪACCEPT_SOCKET_NUM��ͬʱ����ACCEPT_SOCKET_NUM���׽��ֵȴ�accept
	std::vector<OVLP_BASE*> m_vecAcps;
	//�ѽ������ӵ���Ϣ��ÿ���ṹ����һ���׽��֡����ͻ���ͽ��ջ��壬�Լ��Զ˵�ַ
	//std::vector<OVLP_BASE*> m_vecContInfo;
	std::unordered_map<SOCKET, OVLP_BASE*> m_vecContInfo;

	//�����߳�����������
	size_t m_trdCounts = 1;

	//�����Ա�����startWorkersThreads֮ǰ���ã�����false��ִ��startWorkersThreads
	virtual BOOL preStartWorkersThreads(size_t n) { return TRUE; }
	//���������߳���,�������������̵߳�����
	int startWorkersThreads(size_t n = 0)
	{
		return preStartWorkersThreads(n) ? start(n == 0 ? 2 * getCpuCoreCounts() : n) : 0;
	}
	//��ʼ������һ�������׽��ֽ�������ϵͳ
	BOOL bindListener(LISTENER_SOCKET* sokPack)
	{
		// ��ʼ��Socket
		// ������Ҫ�ر�ע�⣬���Ҫʹ���ص�I/O�Ļ����������Ҫʹ��WSASocket����ʼ��Socket
		// ע�������и�WSA_FLAG_OVERLAPPED����
		sokPack->m_sListen = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sokPack->m_sListen == INVALID_SOCKET)
		{
			int res = WSAGetLastError();
			logError(res, _T("bindListener,WSASocketW error:"));
			return FALSE;
		}

		if (!CreateIoCompletionPort((HANDLE)sokPack->m_sListen, getIOCP(), (ULONG_PTR)CPKEY_ACP, 0))
		{
			closesocket(sokPack->m_sListen);
			sokPack->m_sListen = INVALID_SOCKET;
			int res = GetLastError();
			logError(res, _T("CreateIoCompletionPort error:"));
			return FALSE;
		}

		sockaddr_in ServerAddress;
		// ����ַ�ṹ��Ϣ
		ZeroMemory(&ServerAddress, sizeof(ServerAddress));
		ServerAddress.sin_family = AF_INET;
		// �������ѡ����κ�һ�����õĵ�ַ���������Լ�ָ����һ��IP��ַ
		if (sokPack->m_ip == 0 || (sokPack->m_ip[0] == 0))
			ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			InetPton(AF_INET, sokPack->m_ip, &ServerAddress.sin_addr);
		ServerAddress.sin_port = htons(sokPack->m_port);
		int res;
		// �󶨶˿�
		if (::bind(sokPack->m_sListen, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
		{
			lock(0);
			//cout << "bind(" << sokPack->m_ip << ":" << sokPack->m_port << ") error, " << (res = WSAGetLastError(), CLString().getLastErrorString(res)) << endl;
			cout << "bind(" << sokPack->m_ip << ":" << sokPack->m_port << ") error: " << (res = WSAGetLastError()) << endl;
			unlock(0);
			closesocket(sokPack->m_sListen);
			sokPack->m_sListen = INVALID_SOCKET;
			return FALSE;
		}
		lock(0);
		cout << "bind " << sokPack->m_ip << ":" << sokPack->m_port << " ok!" << endl;
		unlock(0);
		// ��ʼ����
		if (listen(sokPack->m_sListen, SOMAXCONN) == SOCKET_ERROR)
		{
			lock(0);
			cout << "listen(" << sokPack->m_ip << ":" << sokPack->m_port << ") error: " << (res = WSAGetLastError()) << endl;
			unlock(0);
			closesocket(sokPack->m_sListen);
			sokPack->m_sListen = INVALID_SOCKET;
			return FALSE;
		}
		lock(0);
		cout << "listen " << sokPack->m_ip << ":" << sokPack->m_port << " ok!" << endl;
		unlock(0);

		if (!getFuncAddr(sokPack))
		{
			throw std::logic_error("func addr get fail!");
			return FALSE;
		}

		//����acceptex�͹������߳�һ����ĳ�ʼ���ܶ�
		for (size_t i = 0; i < m_trdCounts; i++)
		{
			//��accept
			OVLP_BASE* ol = newObj(OVLP);
			ol->pSokPack = sokPack;
			if (!postAccept(ol))
			{
				ol->close();
				deleteObj(ol);
				return FALSE;
			}
			else {
				lock();
				m_vecAcps.push_back(ol);
				unlock();
			}
		}
		return TRUE;
	}
	//��ʼ����������
	BOOL initWinSockListener()
	{
		// ��ʼ��Socket��
		if (wsaData.wVersion == 0)
			WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (getActivateThreadsNum() == 0)
			if (startWorkersThreads(m_trdCounts) == 0)
				return FALSE;
		BOOL rt = FALSE;
		for (size_t i = 0; i < m_ListenLst.size(); i++)
		{
			if (bindListener(&(m_ListenLst.at(i))) != FALSE)
				rt = TRUE;
		}
		return rt;
	}
	//��ȡ�󶨵Ŀ��ƺ���ָ��
	BOOL getFuncAddr(LISTENER_SOCKET* sokPack)
	{
		GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID�������ʶ��AcceptEx���������
		DWORD dwBytes = 0;
		if (SOCKET_ERROR == WSAIoctl(
			sokPack->m_sListen,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx,
			sizeof(GuidAcceptEx),
			&sokPack->m_lpfnAcceptEx,
			sizeof(sokPack->m_lpfnAcceptEx),
			&dwBytes, NULL, NULL))
		{
			int res = WSAGetLastError();
			logError(res, _T("WSAIoctl get AcceptEx function addr error:"));
			return FALSE;
		}
		//��ȡGetAcceptexSockAddrs����ָ��
		GUID GuidGetAcceptexSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
		if (SOCKET_ERROR == WSAIoctl(
			sokPack->m_sListen,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidGetAcceptexSockAddrs,
			sizeof(GuidGetAcceptexSockAddrs),
			&sokPack->m_lpfnGetAcceptSockAddrs,
			sizeof(sokPack->m_lpfnGetAcceptSockAddrs),
			&dwBytes, NULL, NULL))
		{
			int res = WSAGetLastError();
			logError(res, _T("WSAIoctl get GetAcceptexSockAddrs function addr error:"));
			return FALSE;
		}
		return TRUE;
	}
	//����ڲ�iocp����ز���{--------------------------
	HANDLE CLIocpServeBase::getIOCP() const { return m_hIOCompletionPort; }
	BOOL CLIocpServeBase::closeIOCP()
	{
		BOOL bResult = TRUE;
		if (getIOCP() == 0 || getIOCP() == INVALID_HANDLE_VALUE) {}
		else bResult = CloseHandle(getIOCP());
		m_hIOCompletionPort = NULL;
		return(bResult);
	}
	BOOL CLIocpServeBase::createIOCP(int nMaxConcurrency = 0)
	{
		if (getIOCP() == 0 || getIOCP() == INVALID_HANDLE_VALUE)
			m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
		assert(getIOCP() != NULL);
		return(getIOCP() != NULL);
	}
	BOOL CLIocpServeBase::postStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0, OVERLAPPED* po = NULL)
	{
		BOOL fOk = PostQueuedCompletionStatus(getIOCP(), dwNumBytes, CompKey, po);
		assert(fOk);
		return(fOk);
	}
	//}------------------------------------------------
private:
	//�߳���������
	virtual DWORD run(PCLTaskSvcTrdParam var)
	{
		while (true)
		{
			DWORD NumberOfBytes = 0;
			ULONG_PTR CompletionKey = 0;
			OVERLAPPED* ol_ = NULL;
			BOOL rt = GetQueuedCompletionStatus(getIOCP(), &NumberOfBytes, &CompletionKey, &ol_, WSA_INFINITE);
			if (CompletionKey == CPKEY_CLOSE) //�յ��˳���Ϣֱ���˳�
				break;
			OVLP* ol = ol_ ? ((OVLP*)(((char*)ol_) - sizeof(char*))) : NULL; //������Ҫ�������ͷָ���λ�ã�����ַǨ�Ƴ�һ�����ָ��ռ���ܵõ���ʵ�ĵ�ַ��
			if (ol) {
				if (FALSE != rt)
				{
					int res = preDoGetStatus((OVLP*)ol, NumberOfBytes, var);
					if (res < 0)break;
					else if (res == 0)continue;
					else if (doGetStatus((OVLP*)ol, NumberOfBytes, var) == FALSE) //����falseֱ���˳������߳�
						break;
				}
				else
				{
					int res = WSAGetLastError();
					int pres = preDoGetQueuedCompletionStatusError(res, (OVLP*)ol, NumberOfBytes, var);
					if (pres < 0)break;
					else if (pres == 0)continue;
					else if (doGetQueuedCompletionStatusError(res, (OVLP*)ol, NumberOfBytes, var) == FALSE) //����falseֱ���˳������߳�
						break;
				}
			}
			else {
				throw std::runtime_error("Work thread get a unsupported completion status!");
			}
		}
		return 1;
	}
	//���ظú������Դ������ɶ˿�ȡ����Ϣ�ɹ�����ע�⣺���ú��� ����ֵ =  FALSE �����߳̽����밲ȫ�˳�����
	BOOL doGetStatus(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		BOOL rt = TRUE;		
		if (NumberOfBytes == 0 && (ol->m_olType == CPKEY_RECV || ol->m_olType == CPKEY_SEND))
		{
			if (ol->m_olType == CPKEY_RECV)
				rt = doRecvWithNullData(ol, var);
			else if (ol->m_olType == CPKEY_SEND)
				rt = doSendWithNullData(ol, var);
		}
		else
		{
			switch (ol->m_olType)
			{
			case CPKEY_ACP:
			{
				rt = doAccept(ol, NumberOfBytes,  var);
			}
			break;
			case CPKEY_RECV:
			{
				rt = doRecv(ol, NumberOfBytes,  var);
			}
			break;
			case CPKEY_SEND:
			{
				rt = doSend(ol, NumberOfBytes,  var);
			}
			break;
			default:
				rt = doDefaultStatus(ol, NumberOfBytes,  var);
				break;
			}
		}
		return rt;
	}
	//����Ӧʵ�ֵ����ط�������ȫ�˳��߳������ϢͶ�͹���,Ͷ��recv����
	void postClose()
	{
		//1�����IOCP�̶߳��У��˳��߳�,�ж��ٸ��̷߳��Ͷ��ٸ�PostQueuedCompletionStatus��Ϣ
		for (size_t i = 0; i < getThreadsNum(); i++)
		{
			if (FALSE == postStatus(CPKEY_CLOSE))
			{
				logError(WSAGetLastError(), _T("postClose,PostQueuedCompletionStatus error:"));
			}
		}
	}
	void addOvlpToContInfo(OVLP_BASE* ol) {
		if (!ol)return;
		lock();
		auto it = m_vecContInfo.find(ol->m_sSock);
		if (it != m_vecContInfo.end()) {
			if (ol != it->second) {
				MessageBoxA(NULL, "same OVLP obj is in vector!", "CLIocpServeBase Alert", MB_ICONERROR);
				throw std::runtime_error("same OVLP obj is in vector!");
			}
		}
		else m_vecContInfo[ol->m_sSock] = ol;
		unlock();
	}
	//����accept����,NumberOfBytes=0��ʾû���յ���һ֡���ݣ�>0��ʾ�յ���һ֡����
	BOOL doAccept(OVLP_BASE* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		if (!preDoAccept((OVLP*)ol, NumberOfBytes,  var))
			return FALSE;
		//��֧���ڻ�ȡԶ�˵�ַ��
		//�������CPKEY_ACPͬʱ�յ���һ֡���ݣ����һ֡�����ڰ���Զ�˵�ַ��
		//���û���յ���һ֡���ݣ���ͨ��getpeername��ȡԶ�˵�ַ
		BOOL bAccept = FALSE;
		SOCKADDR_IN* ClientAddr = NULL;
		SOCKADDR_IN* LocalAddr = NULL;
		int remoteLen = sizeof(SOCKADDR_IN);
		if (NumberOfBytes > 0)
		{
			//���ܵ����ݷֳ�3���֣���1�����ǿͻ��˷��������ݣ���2�����Ǳ��ص�ַ����3������Զ�˵�ַ��
			if (ol->pSokPack->m_lpfnGetAcceptSockAddrs)
			{				
				int localLen = sizeof(SOCKADDR_IN);
				ol->pSokPack->m_lpfnGetAcceptSockAddrs(
					ol->m_wsaBuf.buf,
					ol->m_wsaBuf.len - (sizeof(SOCKADDR_IN) + 16) * 2,
					sizeof(SOCKADDR_IN) + 16,
					sizeof(SOCKADDR_IN) + 16,
					(LPSOCKADDR*)&LocalAddr,
					&localLen,
					(LPSOCKADDR*)&ClientAddr,
					&remoteLen);
				bAccept = doGetAcceptSockAddrs(LocalAddr, ClientAddr, (OVLP*)ol, NumberOfBytes,  var);
			}
			else
				throw std::runtime_error("m_lpfnGetAcceptSockAddrs is not initialize!");
		}
		else 
		{
			//δ�յ���һ֡����
			int funcRet = ::getpeername(ol->m_sSock, (sockaddr*)ClientAddr, &remoteLen);
			bAccept = doGetPeerName(funcRet, ClientAddr, (OVLP*)ol, var);
		}
		if(bAccept)
			bAccept = ncDoAccept(LocalAddr, ClientAddr, (OVLP*)ol, NumberOfBytes, var);
		if (bAccept) { //����·�ɵ���һ��������Ϣ
			lock();
			OVLP_BASE* pol = newObj(OVLP);
			if (*(char**)pol == 0)
				throw std::runtime_error("OVLP_BASE obj is crash!");
			pol->inheritSocket(ol);
			if (ClientAddr)pol->m_addr = *ClientAddr;
			else pol->m_addr = { 0 };
			pol->pSokPack = ol->pSokPack;
			//�����ֻ��ȡrecv��ͬʱ����recv��send�������λƫ�ƣ��û�����ʵ��
			if (CreateIoCompletionPort((HANDLE)pol->m_sSock, getIOCP(), CPKEY_RECV | CPKEY_SEND, 0) == getIOCP())
			{
				addOvlpToContInfo(pol);
				if (!postRecv(pol)){
					deleteLink(pol);
				}
			}
			else
			{
				//��ʧ�ܣ��رվ��
				pol->close();
				deleteObj(pol);
			}
			unlock();
		}
		else //����Ӧ�ùر�����
			ol->close();
		postAccept(ol);		
		return TRUE;
	}	
protected:
	//�߳�����ʱ�������е��ڲ���ʼ�����̺��������뷵��TRUE���Ż�ִ�к�����run������ֱ�ӽ���exist�����Գ�ʼ��һЩ�̶߳����������var���湩��������ʹ�á�
	virtual BOOL init(PCLTaskSvcTrdParam var) { return TRUE; }
	//�߳��˳�ʱ��������к������̷߳���ֵ�ɸú�����������������ʵ���߳��ͷ�ǰ������var�б���Ķ���var���̶߳���ġ�
	virtual DWORD exist(DWORD runReturnValue, PCLTaskSvcTrdParam var) { return runReturnValue; }
	//��������socket�б����Ƴ�socket���ͷſռ�,��Ӧ���ͷŵ��Ǻ���������Ҫ�����ӣ������Ͽ��������
	BOOL deleteLink(OVLP_BASE* ol)
	{
		if (!ol)
			return false;		
		lock();		
		if (ol->m_sSock != INVALID_SOCKET) {
			shutdown(ol->m_sSock, SD_BOTH);
			closesocket(ol->m_sSock);
		}
		auto it = m_vecContInfo.find(ol->m_sSock);
		if (it != m_vecContInfo.end()) {			
			m_vecContInfo.erase(it);
		}		
		deleteObj(ol);
		unlock();		
		return true;
	}
	//���ظú������������ɶ˿�ȡ����Ϣ����ǰ��Ԥ��������ע�⣺����ֵ= -1��ȫ�˳�,=0ֱ�ӽ�����һ����Ϣ��ȡ����doGetStatus��=1����doGetStatus��������
	virtual int preDoGetStatus(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) { return 1; }
	//���ظú������������ɶ˿�ȡ����Ϣ����ǰ��Ԥ��������ע�⣺����ֵ= -1��ȫ�˳�,=0ֱ�ӽ�����һ����Ϣ��ȡ����doGetQueuedCompletionStatusError��=1����doGetQueuedCompletionStatusError��������
	virtual int preDoGetQueuedCompletionStatusError(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) { return 1; }
	//���ظú������Դ������ɶ˿�ȡ����Ϣʧ������ע�⣺���ú��� ����ֵ =  FALSE �����߳̽���ȥ��ȫ�˳�����
	virtual BOOL doGetQueuedCompletionStatusError(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		switch (errorID)
		{
		case ERROR_NETNAME_DELETED:
		{
			doGetQueuedCompletionStatusError_NetNameDeleted(errorID, ol, NumberOfBytes,  var);
		}
		break;
		default:
			return doGetQueuedCompletionStatusError_default(errorID, ol, NumberOfBytes,  var);
		}
		return TRUE;
	}
	//���ظú������������ɶ˿�ȡ��Accept��Ϣ����ǰ��Ԥ�������񡣷��� TRUE ����ִ��accept
	virtual BOOL preDoAccept(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) { return TRUE; }
	//���ظú������������ɶ˿�ȡ��Accept��Ϣ�����Ĵ������������Դ�����ͻ����������ѽ������ӵ���Ϣ��
	//�������FALSE�����Accept��һ����ʱ��Ϣ�����걾���������󣬽��رձ������ӣ����ٽ��ܺ���Recv�����������紦��GET����ȼ�ʱ��Ϣ��
	//�����ڲ�Ӧ����ʾ�����κ��ͷ�ol�Ĳ�����Ӧ��ͨ������FALSE�ɿ��ִ�к���������
	virtual BOOL ncDoAccept(SOCKADDR_IN* localAddr, SOCKADDR_IN* clientAddr, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {return TRUE;}
	//Ͷ��accept����,�ڲ�����б���socket���ͷţ��ж����Ӻ�AcceptEx����
	BOOL postAccept(OVLP_BASE* ol)
	{
		const LISTENER_SOCKET* pso = ol->pSokPack;
		ol->resetBuffer();
		ol->resetOverlapped();
		ol->close();//�о�Ӧ�ùر�
		ol->m_sSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (ol->m_sSock == INVALID_SOCKET)
		{
			int res = WSAGetLastError();
			logError(res, _T("postAccept,WSASocket error:"));
			throw std::runtime_error("post accept socket create false!");
			return FALSE;
		}
		//���ｨ����socket�����ͶԶ˽������ӣ��ջ����m_vecContInfo�б�
		//����acceptex��accept socket������ɶ˿ڣ�����ʼ�����¼�����
		//������Ҫ����Overlapped��newһ��OVLP
		//AcceptEx��m_listen�ļ����¼���m_listen�Ѿ�������ɶ˿ڣ���Ȼol->m_sSock�Ѿ�������
		//��δʹ�ã����ڲ���Ϊol->m_sSock����ɶ˿ڡ���AcceptEx�¼���������Ϊol->m_sSock��IOCP
		DWORD byteReceived = 0;
		ol->m_olType = CPKEY_ACP;
		if (FALSE == pso->m_lpfnAcceptEx(
			pso->m_sListen,
			ol->m_sSock,
			ol->m_wsaBuf.buf,
			ol->m_wsaBuf.len - (sizeof(SOCKADDR_IN) + 16) * 2,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&byteReceived,
			&ol->m_Overlapped))
		{
			DWORD res = WSAGetLastError();
			if (ERROR_IO_PENDING != res)
			{
				logError(res, _T("postAccept,AcceptEx error:"));
				ol->close();
				throw std::runtime_error("post accept socket false!");
				return FALSE;
			}
		}
		return TRUE;
	}
	//Ͷ��recv����
	BOOL postRecv(OVLP_BASE* ol)
	{
		DWORD BytesRecvd = 0;
		DWORD dwFlags = 0;
		ol->resetOverlapped();
		ol->resetBuffer();
		ol->m_olType = CPKEY_RECV;
		int recvnum = WSARecv(ol->m_sSock, &ol->m_wsaBuf, 1, &BytesRecvd, &dwFlags, &ol->m_Overlapped, NULL);
		if (recvnum != 0)
		{
			int res = WSAGetLastError();
			if (WSA_IO_PENDING != res)
			{
				logError(res, _T("postRecv,WSARecv error:"));
				return FALSE;
			}
		}
		return TRUE;
	}
	//Ͷ��send����
	BOOL postSend(OVLP_BASE* ol, DWORD lenToSendBytes)
	{
		DWORD BytesSend = 0;
		DWORD dwFlags = 0;
		ol->resetOverlapped();
		ol->m_olType = CPKEY_SEND;
		ol->m_wsaBuf.len = lenToSendBytes;
		int recvnum = WSASend(ol->m_sSock, &ol->m_wsaBuf, 1, &BytesSend, dwFlags, &ol->m_Overlapped, NULL);
		if (recvnum != 0)
		{
			int res = WSAGetLastError();
			if (WSA_IO_PENDING != res)
			{
				logError(res, _T("postSend,WSASend error:"));
				ol->resetBuffer();
				return FALSE;
			}
		}
		return TRUE;
	}
	//�����ⲿ�ͻ����Ͽ�����Ͽ��¼�
	virtual void doGetQueuedCompletionStatusError_NetNameDeleted(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		if (ol)
		{
			if (ol->m_olType == CPKEY_ACP) {//���ӽ׶ζϿ����ط�һ���ȴ��Ľ�����Ϣ
				postAccept(ol);
			}
			else {
				//ol->close();
				//deleteObj(ol);
				deleteLink(ol);
			}
		}
	}
	//����GetStatus����ʱ���Error = NetNameDeleted������������¼����ڲ���switch�����ɣ�ע�⣺���ú��� ����ֵ =  FALSE �����߳̽���ȥ��ȫ�˳�����
	virtual BOOL doGetQueuedCompletionStatusError_default(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		logError(errorID, _T("workThread GetQueuedCompletionStatus error:"));
		switch (errorID)
		{
		case ERROR_NETNAME_DELETED:
			break;
		default:
			break;
		}
		return TRUE;
	}
	//�����Ա�����startListen֮ǰ���ã�����false��ִ��start
	virtual BOOL preStartListen(USHORT port, LPCTSTR ip) { return TRUE; }
	//�����Դ���GetAcceptSockAddrs���ú�ĺ���,�ú���������ܳɹ���Կͻ��˵�Ӧ�𣨱��뷵��TRUE���ܼ���������һ����Ϣ,����FALSE�򲻽��������RECV���̣�
	virtual BOOL doGetAcceptSockAddrs(SOCKADDR_IN* localAddr, SOCKADDR_IN* clientAddr, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		return TRUE;
	}
	//�����Դ���getpeername���ú�ĺ���
	virtual BOOL doGetPeerName(int funcRet, SOCKADDR_IN* clientAddr, OVLP* ol, PCLTaskSvcTrdParam var)
	{
		if (funcRet == SOCKET_ERROR) {
			logError(WSAGetLastError(), _T("getpeername error:"));
		}
		return TRUE;
	}
	//����recv������󣬱�����ʾ����postRecv��Ϊָ����socket����һ������
	virtual BOOL doRecv(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		//��ʹ���첽�������ݱ���ʵ�ִ˺�������Ϊ
		MessageBox(NULL, _T("doRecv() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doRecv method need to operator!");
		return TRUE;
	}
	//����send������󣬱�����ʾ����postRecv��Ϊָ����socket����һ������
	virtual BOOL doSend(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		//��ʹ���첽���ͱ���ʵ�ִ˺�������Ϊ
		MessageBox(NULL, _T("doSend() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doSend method need to operator!");
		return TRUE;
	}
	//����Default���󣬱�����ʾ����postRecv��Ϊָ����socket����һ������
	virtual BOOL doDefaultStatus(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		postRecv(ol);
		return TRUE;
	}
	//����NumberOfBytes = 0������,Ҳ���ǿͻ�������������Ͽ����ӵ��������Ӧ�ô����socket�Ͽ������������
	virtual BOOL doRecvWithNullData(OVLP* ol,PCLTaskSvcTrdParam var) {
		//��ʹ���첽�������ݱ���ʵ�ִ˺�������Ϊ
		MessageBox(NULL, _T("doRecvWithNullData() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doRecvWithNullData method need to operator!");
		return TRUE;
	}
	virtual BOOL doSendWithNullData(OVLP* ol, PCLTaskSvcTrdParam var) {
		//��ʹ���첽���������ʵ�ִ˺�������Ϊ
		MessageBox(NULL, _T("doSendWithNullData() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doSendWithNullData method need to operator!");
		return TRUE;
	}
	//���socket������Ϣ
	virtual void logError(int errorId, LPCTSTR inf1 = 0, LPCTSTR inf2 = 0) {}
};

#endif

