#pragma once
#ifndef __CL_PROCESSINFO_H__
#define __CL_PROCESSINFO_H__



#include <windows.h>  
#include <imagehlp.h> 
#pragma comment(lib, "imagehlp.lib ")
#include <tlhelp32.h> 
#include <vector>  
#include <tchar.h>
#include <assert.h>
#include "../_cl_common/CLCommon.h"


using namespace std;

//���溯��ThunkData�����Ϣ�ṹ��
typedef struct _FUNCTION
{
	PIMAGE_IMPORT_BY_NAME functionImportByName;
	PIMAGE_THUNK_DATA functionOrThunkData;
	PIMAGE_THUNK_DATA functionThunkData;
	FARPROC functionAddr;
	CHAR functionName[MAX_PATH];
}FUNCTION, *PFUNCTION;

//����Dll����������Ϣ�ṹ��
typedef struct _IIDINFO
{
	PIMAGE_IMPORT_DESCRIPTOR pThisModImportDescStructAddr;//ģ�鵼�����Ϣ������Ľṹ��ָ��
	DWORD_PTR pMainModBaseAddr;//�õ�������ڵ���ģ�����ַ
	DWORD_PTR subModNameRelateAddr;
	LPCSTR subModNameAddrInChar;
	CHAR subModNameCH[MAX_PATH];
	HMODULE subHModule;
	DWORD_PTR subModRelateOFT;
	PIMAGE_THUNK_DATA subModOFT;
	DWORD_PTR subModIATRelateAddr;
	PIMAGE_THUNK_DATA subModIATAddr;
	std::vector<FUNCTION> modFuncArry;//���뺯����ThunkData�����Ϣ�ṹ��
}IIDINFO, *PIIDINFO;//Dll������е�һ������ģ��������Ϣ�ṹ��

typedef struct _MODIATINFO {
	UINT modIndex;//ģ���ڽ����еĲ���˳���0��ʼ
	MODULEENTRY32 modInfo;//ģ����Ϣ�ṹ��
	std::vector<IIDINFO> iidLst;//��ģ������е������Ϣ�ṹ�����
}MODIATINFO, *PMODIATINFO;//ģ��iat��Ϣ�б�

typedef struct _PROIATINFO {
	PROCESSENTRY32 proInfo;//������Ϣ�ṹ	  
	std::vector<MODIATINFO> modIatLst;//���̵����еļ���ģ����Ϣ�б�
}PROIATINFO, *PPROIATINFO;//IAT����ṹ��

typedef struct _FUNCTIONEXP {
	DWORD_PTR addressOrdinal;
	DWORD_PTR exportOrdinal;
	DWORD_PTR baseOrdinal;
	DWORD_PTR rvaOfFunction;
	FARPROC addressOfFunction;
	CHAR functionName[MAX_PATH];
}FUNCTIONEXP, *PFUNCTIONEXP;

typedef struct _RMTDATA
{
	int type;
	HMODULE tagMod;
	DWORD size;

	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_IMPORT_BY_NAME pImgImpByName;

	TCHAR dllNameTH[MAX_PATH];

	CHAR funcNameCH[MAX_PATH];
	CHAR func2NameCH[MAX_PATH];

	TCHAR msg1TH[MAX_PATH];
	TCHAR msg2TH[MAX_PATH];
	TCHAR msg3TH[MAX_PATH];
	TCHAR msg4TH[MAX_PATH];
	TCHAR msg5TH[MAX_PATH];
	TCHAR msg6TH[MAX_PATH];
	TCHAR msg7TH[MAX_PATH];

	FARPROC m_funcAddr1_MessageBoxA;
	FARPROC m_funcAddr2_MessageBox;
	FARPROC m_funcAddr3_GetModuleHandle;
	FARPROC m_funcAddr4_LoadLibrary;
	FARPROC m_funcAddr5_GetProcAddress;
	FARPROC m_funcAddr6_GetModuleHandleA;
	FARPROC m_funcAddr7_IsBadReadPtr;
}RMTDATA, *PRMTDATA;//Զ��ע��ʱ��д��Զ�̽��̵����ݽṹ��

typedef struct _RMTMODULE
{
	HMODULE tagMod;
	CHAR dllNameCH[MAX_PATH];
}RMTMODULE, *PRMTMODULE;

typedef DWORD_PTR LASTERRORID; //�����GetlastError�ķ���ֵ���ͣ�

//API�ٳ������ö�ṹ��
typedef struct _APIHOOK32_ENTRY
{
	LPCTSTR     pszAPINameTH; //api�ٳֵ�Ŀ�꺯������
	LPCTSTR     pszCalleeModuleNameTH; //����api���ڵ�dll���ơ�
	FARPROC     pfnOriginApiAddress; //api�ڵ���dll�е���ʵ��ڵ�,��GetProcAddress()ȡ����ʵ��ڵ㡣
	FARPROC     pfnDummyFuncAddress; //�滻�ĵ�����ڵ�ֵ��
	HMODULE     hModCallerModule; //Ҫ����api�滻������Ŀ��ģ��������ΪNULLʱ���滻���˹����屾��dll����������еĽ�����ģ�飨��������ϵͳģ�飩��
}APIHOOK32_ENTRY, *PAPIHOOK32_ENTRY;

typedef FARPROC(WINAPI *PGetProcAddress)(HMODULE, LPCSTR);
typedef HMODULE(WINAPI *PLoadLibraryA)(HMODULE, LPCSTR);
typedef HMODULE(WINAPI *PLoadLibraryW)(HMODULE, LPCTSTR);
typedef HANDLE(WINAPI *PCreateRemoteThread)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD_PTR, LPDWORD);

class CLProcessInfo
{
private:


	//ָ��λ��
	size_t _currentIndex;
	//ϵͳ���н�����Ϣ����
	std::vector<PROCESSENTRY32*> _ProEnt32Array;
	CLMemeryManagerTemplate<PROCESSENTRY32> Pe32M;
	//�����������߳���Ϣ����
	std::vector<THREADENTRY32*> _ThrEnt32Array;
	CLMemeryManagerTemplate<THREADENTRY32> Te32M;
	//��������ģ����Ϣ����
	std::vector<MODULEENTRY32*> _ModEnt32Array;
	CLMemeryManagerTemplate<MODULEENTRY32> Me32M;
	//��������ģ�鵼�����Ϣ����
	std::vector<IIDINFO*> _IIDArray;
	CLMemeryManagerTemplate<IIDINFO> IIDM;
	//CLMemeryManagerTemplate<FUNCTION> FIM;

	int _currentProID;
	HMODULE _currentModHandle;

	//��ʼ��������Ϣ����
	LASTERRORID initProEnt32Array(void);
	//���������Ϣ����
	void clearProEnt32Array(void);
	//����߳�ģ����Ϣ����
	void clearThrEnt32Array(void);
	//�������ģ����Ϣ����
	void clearModEnt32Array(void);
	//��������������Ϣ����
	void clearIIDArray(void);


	static HMODULE getRemoteModuleHandle(HANDLE hProcess, LPCSTR pModuleName, RMTDATA* prdata1, PTHREAD_START_ROUTINE pfnThreadRtn);

	static BOOL getRemoteFuncName(HANDLE hProcess, LPCSTR pFuncName, RMTDATA * prdata1, PTHREAD_START_ROUTINE pfnThreadRtn, PIMAGE_IMPORT_BY_NAME pIIBN);

	static LASTERRORID remoteReleaseDllByRemoHModule(DWORD_PTR dwProcessId, HMODULE hDllRemoteModule, size_t cntUsage);

	// �鿴������ĳĿ��ģ��ĵ�����ҵ���Ӧ��dll�ͺ������滻��(���ĺ���)
	static LASTERRORID _setApiToNewAdr(APIHOOK32_ENTRY* phk, HANDLE hOpenedProcess);

	//��̬ȡ�ú�����ַ
	//static PCreateRemoteThread pCreateRemoteThread();
	static BOOL CALLBACK _enumFuncCallBack(HWND hwnd, LPARAM lParam);
public:
	//���캯��������ʱ���Զ�������ǰʱ��Ľ����б�
	CLProcessInfo();
	virtual ~CLProcessInfo();
	//������ж��������ͷ��ڴ�
	void clearAllInfo(void);
	//ȡ���ڲ����̶���
	const std::vector<PROCESSENTRY32*>& info(void) const;
	//ȡ���ڲ�ģ�����
	const std::vector<MODULEENTRY32*>& modInfo(void) const;
	//ȡ���ڲ��̶߳���
	const std::vector<THREADENTRY32*>& thrInfo(void) const;
	//ȡ���ڲ���������
	const std::vector<IIDINFO*>& iidInfo(void) const;
	//��ʼ���߳���Ϣ���У�����GetLastError()���롣
	LASTERRORID initThrEnt32Array(int nProID = 0);
	//��ʼ��ģ����Ϣ���У�����GetLastError()���롣
	LASTERRORID initModEnt32Array(int nProID = 0);
	//��ʼ���������Ϣ���У�Զ�̽��̷�ʽ���ڲ����뱣�����Զ�̽��̵�id��������GetLastError()���롣
	LASTERRORID initIIDArray(HMODULE tagMod);
	//��ʼ���������Ϣ���У�Զ�̽��̷�ʽ��������GetLastError()���롣
	LASTERRORID initRemoteIIDArray(LONG dwProcessId, HMODULE tagMod);
	//��ʼ���������Ϣ���У���ǰ���̷�ʽ������GetLastError()���롣
	LASTERRORID initIIDArray1(HMODULE tagMod);
	//�Ե�ǰ�ڲ������Զ�̽���id��ж��Զ�̽����ж�Ӧ���Ƶ�dll�������ڲ�id�Ѳ���������������ڲ�������Ϣ������GetLastError()���롣
	LASTERRORID remoteReleaseDllByName(LPCTSTR pDllName);

	//Զ������ʱ��������庯����
	static DWORD_PTR WINAPI ThreadFunc1_GetImportDesc(PRMTDATA pData);
	//������������ֲ���庯���Ĵ���ε��ֽڴ�С��
	static DWORD_PTR WINAPI ThreadFunc1_GetImportDescEnd(void);
	//����������Ϣ���С������ǰ�ڲ�����Ľ���id��Ӧ�Ľ��̻�ģ������Ӧ�ľ���Ѳ����ڣ����������ڲ�ά���Ķ���������Ϣ��
	CLProcessInfo& updateInfo(void);
	//��߽���DebugȨ��
	static LASTERRORID enablePrivilege(LPCTSTR seName = SE_DEBUG_NAME);

	//�ɽ���id���ڲ��Ľ����б����ҳ���Ӧ�Ľ�����Ϣ�ṹ�壬����ǰ�ɵ���updateInfo()����ά���Ľ����б�
	BOOL findProcessStructFromInnerProListByProID(DWORD_PTR dProcessID, PROCESSENTRY32& outStruct);
	//�ɽ��������ڲ��Ľ����б��еĵ�һ���ҳ��Ľ�����Ϣ�ṹ�壬����ǰ�ɵ���updateInfo()����ά���Ľ����б�
	BOOL findProcessStructFromInnerProListByNameFirst(LPCSTR lpProName, PROCESSENTRY32& outStruct);
	//�ɽ��������ڲ��Ľ����б��е���һ���ҳ��Ľ�����Ϣ�ṹ�壬����ǰ�ɵ���updateInfo()����ά���Ľ����б�
	BOOL findProcessStructFromInnerProListByNameNext(LPCSTR lpProName, PROCESSENTRY32& outStruct);

	//��ģ�������ڲ���Mod�б����ҳ���Ӧ��ģ����Ϣ�ṹ�壬����ǰ�᣺�����ʼ����ģ������initModEnt32Array()��
	BOOL findModuleStructFromInnerModListByHandle(HMODULE hModule, MODULEENTRY32& outStruct);
	//��ģ���������ڲ���Mod�б����ҳ���Ӧ��ģ����Ϣ�ṹ�壬����ǰ�᣺�����ʼ����ģ������initModEnt32Array()��
	BOOL findModuleStructFromInnerModListByModName(LPCSTR lpModuleName, MODULEENTRY32& outStruct);
	//��ģ���������ڲ���iid�б����ҳ���Ӧ�ĵ����ģ�����Ϣ�ṹ�壬,dll�����ַ�����������ֽ��룬���ܲ���UNICODE������ǰ�᣺�����ʼ����ģ������initModEnt32Array()��
	BOOL findIIDStructFromInnerIIDListByDllName(LPCSTR lpDllName, IIDINFO& outStruct);
	//��ģ�������ڲ���iid�б����ҳ���Ӧ�ĵ����ģ�����Ϣ�ṹ�壬����ǰ�᣺�����ʼ������������initIIDArray()��
	BOOL findIIDStructFromInnerIIDListByDllHandle(HMODULE hModule, IIDINFO& outStruct);
	//��API���������ڲ���iid�б����ҳ���Ӧ��api�������Ϣ�ṹ�壬����ǰ�᣺�����ʼ������������initIIDArray()��
	BOOL findApiStructFromInnerIIDListByApiName(LPCSTR lpApiName, FUNCTION& outStruct);
	//������һ�γ�ʼ��ģ����Ϣ���к��Ӧ���ڲ�����ID��Ҳ���ǵ���initThrEnt32Array��initThrEnt32Array������Ǹ�����id������û�г�ʼ��ģ���б����ֵΪ-1.
	int currentInnerProcessID(void) const;
	//������һ�γ�ʼ���������Ϣ���к��Ӧ���ڲ�ģ������Ҳ���ǵ���initIIDArray������Ǹ�ģ��������û�г�ʼ����������ֵΪNULL��
	HMODULE currentInnerModuleHandle(void) const;
	
	//�����ͨ������id����������������̱�ȡ�ý�����Ϣ�ṹ�����ݣ������ؽ�����ģ������
	static HMODULE getProcessInfo(DWORD_PTR dwProcessId, LPCTSTR szProcessName, PROCESSENTRY32& out_ProcessEntry32Str);
	typedef struct _str_proinfo_pack {
		PROCESSENTRY32 pro = { sizeof(PROCESSENTRY32) };
		MODULEENTRY32 mainMod = { sizeof(MODULEENTRY32) };
		THREADENTRY32 mainThread = { sizeof(THREADENTRY32) };
		HEAPLIST32 mainHeapList = { sizeof(HEAPLIST32) };
		std::vector<MODULEENTRY32> subMods;
		std::vector<THREADENTRY32> subThreads;
		std::vector<HEAPLIST32> subHeapList;
	}PROINFO,*PPROINFO;
	typedef std::vector<PROINFO> PROINFOTABLE;
	//�����ͨ�����̽��������������̱�ȡ�ý�����Ϣ�ṹ�����ݣ������ؽ�����Ϣ�ṹ��ö���б�
	static PROINFOTABLE& enumProcessInfo(LPCTSTR szProcessName, PROINFOTABLE& out_ProcessInfoTable);
	//���ؽ���ָ����ģ����Ϣ�ṹ�����ݣ���������ģ�������ѯ��ģ����ڷ���ģ���������򷵻�NULL��
	static HMODULE getModuleInfoOfProcess(DWORD_PTR dwProcessId, LPCTSTR szDllName, MODULEENTRY32& out_ModuleEntry32Str);
	//���ļ���ȡ�ý���ID����Ҫ�������̱�
	static DWORD_PTR getProcessIDByName(LPCTSTR lpszProcessName);
	//�ɽ���idȡ�ý���ȫ·������ʧ�ܷ���false
	static BOOL getProcessFileNameByPid(DWORD_PTR dwPid, LPTSTR out_ProcessFullPathName, DWORD_PTR nSize);

	//���� ͨ������Զ���̸߳��������̼���ָ����Dll
	static DWORD_PTR remoteloadDll(DWORD_PTR dwProcessId, LPCTSTR lpszDllPath);
	//���� ͨ������Զ���߳�ж���������̼��ص�Dll��cntUsage��ʾdll���ٵ����ô�������Ϊ0��ʾ��ȫж��dll
	static DWORD_PTR remoteReleaseDll(DWORD_PTR dwProcessId, LPCTSTR szDllName, DWORD_PTR cntUsage = 0);

	//���� ͨ�����Ӽ�������Dll��lpszFucName��ʾҪ���õ�dll�еĵ������������������NULL��Ĭ�ϵ���"InsertDll"��
	//���ع��ӹ��̾��������dll���
	//��������Ϊ:setHookByDll() -> Loadliberary(Dll) -> GetProcAddress(InsertDLL) -> InsertDLL() ->
	//SetWindowHookEx() -> Dllmain -> SetWindowsAPIHook()
	static HHOOK setHookByDll(HMODULE &out_hHookDll, LPCTSTR lpszDllPath, LPCSTR lpszFucName = NULL, DWORD_PTR dwThrId = 0, WPARAM wParam = 0, LPARAM lParam = 0);
	//static DWORD_PTR releaseHook(DWORD_PTR dwProcessId, LPCTSTR lpszDllPath, WPARAM hTargetModle, LPARAM lParam);

	//API��ַ�滻������phk->hModCallerModule == NULL�͹���������ģ��
	static LASTERRORID hookWindowsAPI(APIHOOK32_ENTRY* phk);
	//��ԭApiԭʼ��ַ,��������ģ��ж��
	static LASTERRORID unHookWindowsAPI(APIHOOK32_ENTRY* phk);

	//�ж�ϵͳ�Ƿ���64λ��
	static BOOL is64BitSystem();
	//�жϽ����Ƿ���64λ��,����TRUE���_out_bIs64BitPro��ֵ����Ч��_out_bIs64BitProΪTRUE��ʾ����λ64λ����
	static BOOL is64BitProcess(DWORD_PTR nProID, BOOL& _out_bIs64BitPro);
	//�ɽ���id��������ھ����û�д��ڷ���NULL��ͨ��ö�����д��ڶԱȺ󷵻�ƥ��ֵ��
	static HWND getHwndByProID(DWORD_PTR processID);

	//ȡ�ý���(��Զ�̽���)����Ϣ�����������е�ģ����Ϣ���ͽ���������ģ��ĵ������Ϣ��ʧ�ܷ���ʧ��ʱ���GetLastError()ֵ��
	//_out_priIatInfo���ں�����ʹ��ǰ�Զ�����Ҳ����Ҫ�ֶ��ͷ��ڴ档
	//�ڹ�������������κδ��󶼻᷵�أ���ʱ������������_out_priIatInfo�п��ܱ�����һ�����Ѿ���ȡ������Ϣ�����ǲ�����ȫ��
	//ע�⣺�ú������ܻ����Ĵ�����ʱ�䣬��Զ�̽�����ͨ������ע���ȡ����IAT���������ڵ���ǰ��������Ȩ�޺ͱ��鿴�߽��̵Ľ��̱���״̬��������ΪȨ�޲������޷��򿪽��̻�ȡԶ����Ϣ��
	static LASTERRORID getProIATInfo(DWORD_PTR processID, PROIATINFO& _out_priIatInfo);

	//�оٽ������Ѽ��ص�ģ�鵼�����е����к�����_baseAddr��Ŀ��ģ������Ҳ������ģ�����ַ���ڴ澵�����ַ����_out_Sets��һ�������ѯ����Ľ�����������ʼ������
	//_familyFlag��ʾ�������ͷ�����磺Nt, Zw , Rtl, _, __, A, Abc �ȵȾ��ɣ���
	//����_familyFlagֵ�󣬽����ֻ���ش��и����ͷ�ĺ��������ͷ���ִ�Сд������_familyFlag = NULL��""��"\0"ʱ�������ͷ������������ȡ���е���������Ҳ��������������������
	//��_familyFlagΪָ����ĳһ�ض�������ʱ�򣬺������ܵȼ��ڲ�ѯĳ����һ��������ʱӦ������_isFullMatch = TRUE ��ʾ���ͷ������ȫƥ�䣬���ҵ������������أ�����ö�ٺ���ֵ��
	//���û���ҵ��κν����Ҳ���ǽ����_out_Sets��������Ϊ0��������ERROR_INVALID_FUNCTION��������ö�ٳɹ�������ERROR_SUCCESS��
	static LASTERRORID emunExportFunction(LPCVOID _baseAddr, std::vector<FUNCTIONEXP>& _out_Sets, LPCSTR  _familyFlag = NULL, BOOL _isFullMatch = FALSE);


	static HWND getConsoleHwnd(void); //ȡ�õ�ǰ��̨����Ĵ��ھ��

};

#endif