
#include "CLProcessInfo.h"
#include "../_cl_string/CLString.h"

//���_ProEnt32Array����
void CLProcessInfo::clearProEnt32Array()
{
	for (std::vector<PROCESSENTRY32*>::iterator i = _ProEnt32Array.begin(); i != _ProEnt32Array.end(); i++)
	{
		//if ( *i ) delete *i;
		if ( *i ) Pe32M.deleteOne(*i);
	}
	_ProEnt32Array.clear();
	_currentIndex = 1;
}

void CLProcessInfo::clearThrEnt32Array(void)
{
	for (std::vector<THREADENTRY32*>::iterator i = _ThrEnt32Array.begin(); i != _ThrEnt32Array.end(); i++)
	{
		//if (*i) delete *i;
		if ( *i ) Te32M.deleteOne(*i);
	}
	_ThrEnt32Array.clear();
	_currentProID = -1;
}

//���_ModEnt32Array����
void CLProcessInfo::clearModEnt32Array()
{
	for (std::vector<MODULEENTRY32*>::iterator i = _ModEnt32Array.begin(); i != _ModEnt32Array.end(); i++)
	{
		//if (*i) delete *i;
		if ( *i ) Me32M.deleteOne(*i);
	}
	_ModEnt32Array.clear();
	_currentProID = -1;
}

//���_IIDArray����
void CLProcessInfo::clearIIDArray(void)
{
	for (std::vector<IIDINFO*>::iterator i = _IIDArray.begin(); i != _IIDArray.end(); i++)
	{
		if (*i)
		{
// 			for (std::vector<FUNCTION*>::iterator j = (*i)->modFuncArry.begin(); j != (*i)->modFuncArry.end(); j++)
// 			{
// 				if (*j)
// 				{
// 					//delete *j;
// 					FIM.deleteOne(*j);
// 				}
// 			}
			(*i)->modFuncArry.clear();
			//delete *i;			
			IIDM.deleteOne(*i);
		}
	}
	_IIDArray.clear();
	_currentModHandle = 0;
}

LASTERRORID CLProcessInfo::initProEnt32Array(void)
{

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		//MessageBox(nullptr, _T("CreateToolhelp32Snapshot����ʧ�ܣ� \n"), _T("Error"), MB_ICONERROR);
		return GetLastError();
	}

	clearProEnt32Array();

	//PROCESSENTRY32* _lproinfo = new PROCESSENTRY32;
	PROCESSENTRY32* _lproinfo = Pe32M.newOne();
	ZeroMemory(_lproinfo,sizeof(PROCESSENTRY32));
	if (!_lproinfo)
	{
		CloseHandle(hSnapshot);
		return ERROR_OUT_OF_STRUCTURES;
	}
	_lproinfo->dwSize = sizeof(PROCESSENTRY32);
	BOOL bOk = Process32First(hSnapshot, _lproinfo);
	//if (bOk == FALSE) delete _lproinfo;
	if (bOk == FALSE) Pe32M.deleteOne(_lproinfo);
	while (bOk != FALSE)
	{
		_ProEnt32Array.push_back(_lproinfo);
		_currentIndex++;
		//_lproinfo = new PROCESSENTRY32;
		_lproinfo = Pe32M.newOne();
		ZeroMemory(_lproinfo,sizeof(PROCESSENTRY32));
		if (!_lproinfo)
		{
			CloseHandle(hSnapshot);
			return ERROR_OUT_OF_STRUCTURES;
		}
		_lproinfo->dwSize = sizeof(PROCESSENTRY32);
		bOk = Process32Next(hSnapshot, _lproinfo);
	}
	//if (bOk == FALSE) delete _lproinfo;
	if (bOk == FALSE) Pe32M.deleteOne(_lproinfo);
	CloseHandle(hSnapshot);
	return ERROR_SUCCESS;
}

LASTERRORID CLProcessInfo::initThrEnt32Array(int nProID)
{

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nProID);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		//MessageBox(nullptr, _T("CreateToolhelp32Snapshot TH32CS_SNAPTHREAD ����ʧ�ܣ� \n"), _T("Error"), MB_ICONERROR);
		return GetLastError();
	}

	clearThrEnt32Array();
	THREADENTRY32* _lproinfo = Te32M.newOne();
	ZeroMemory(_lproinfo,sizeof(THREADENTRY32));
	if (!_lproinfo)
	{
		CloseHandle(hSnapshot);
		return ERROR_OUT_OF_STRUCTURES;
	}
	_lproinfo->dwSize = sizeof(THREADENTRY32);
	BOOL bOk = Thread32First(hSnapshot, _lproinfo);
	_lproinfo->th32OwnerProcessID = nProID;
	if (bOk == FALSE) Te32M.deleteOne(_lproinfo);
	while (bOk != FALSE)
	{
		if(_lproinfo->th32OwnerProcessID == nProID)
		_ThrEnt32Array.push_back(_lproinfo);
		_lproinfo = Te32M.newOne();
		ZeroMemory(_lproinfo,sizeof(THREADENTRY32));
		if (!_lproinfo)
		{
			CloseHandle(hSnapshot);
			return ERROR_OUT_OF_STRUCTURES;
		}
		_lproinfo->dwSize = sizeof(THREADENTRY32);
		bOk = Thread32Next(hSnapshot, _lproinfo);
		_lproinfo->th32OwnerProcessID = nProID;
	}
	if (bOk == FALSE) Te32M.deleteOne(_lproinfo);
	CloseHandle(hSnapshot);
	_currentProID = nProID;
	return ERROR_SUCCESS;
}

LASTERRORID CLProcessInfo::initModEnt32Array(int nProID)
{

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nProID);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		//MessageBox(nullptr, _T("CreateToolhelp32Snapshot TH32CS_SNAPMODULE ����ʧ�ܣ� \n"), _T("Error"), MB_ICONERROR);
		return GetLastError();
	}

	clearModEnt32Array();
	MODULEENTRY32* _lproinfo = Me32M.newOne();
	ZeroMemory(_lproinfo,sizeof(MODULEENTRY32));
	if (!_lproinfo)
	{
		CloseHandle(hSnapshot);
		return ERROR_OUT_OF_STRUCTURES;
	}
	_lproinfo->dwSize = sizeof(MODULEENTRY32);
    _lproinfo->ProccntUsage = 0;
	BOOL bOk = Module32First(hSnapshot, _lproinfo);
	_lproinfo->th32ProcessID = nProID;
	if (bOk == FALSE) Me32M.deleteOne(_lproinfo);
	while (bOk != FALSE)
	{
		_ModEnt32Array.push_back(_lproinfo);
		_lproinfo = Me32M.newOne();
		ZeroMemory(_lproinfo,sizeof(MODULEENTRY32));
		if (!_lproinfo)
		{
			CloseHandle(hSnapshot);
			return ERROR_OUT_OF_STRUCTURES;
		}
		_lproinfo->dwSize = sizeof(MODULEENTRY32);
		bOk = Module32Next(hSnapshot, _lproinfo);
		_lproinfo->th32ProcessID = nProID;
	}
	if (bOk == FALSE) Me32M.deleteOne(_lproinfo);
	CloseHandle(hSnapshot);
	_currentProID = nProID;
	return ERROR_SUCCESS;
}

LASTERRORID CLProcessInfo::initIIDArray(HMODULE tagMod)
{
    if (_currentProID == -1) return ERROR_INVALID_DATA;
    _currentModHandle = tagMod;
    return initRemoteIIDArray(_currentProID, _currentModHandle);
}

LASTERRORID CLProcessInfo::initIIDArray1(HMODULE tagMod)
{
	//����һ:
	/*
	HMODULE hMod = ::GetModuleHandle(NULL);   //Ҳ����ͨ��LoadLibrary("dllname.dll")��þ��
	IMAGE_DOS_HEADER*   pDosHeader = (IMAGE_DOS_HEADER*)hMod;
	IMAGE_OPTIONAL_HEADER * pOptHeader = (IMAGE_OPTIONAL_HEADER*)   //optional��
	((BYTE*)hMod + pDosHeader->e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER)); //4Ϊ signature���ֽ���
	IMAGE_IMPORT_DESCRIPTOR* pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)      //ȡ�õ�һ��dll
	((BYTE*)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)  //ȡ�õ�ǰdll�еĵ�һ��api
	((BYTE*)hMod + pImportDesc->OriginalFirstThunk);
	*/

	//������
	//��ȡָ��PE�ļ��е�Import��IMAGE_DIRECTORY_DESCRIPTOR�����ָ�� 
	//��ָ����ģ��������������Ϣ
	DWORD size;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	if (!(pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
		ImageDirectoryEntryToData(tagMod, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size)))
	{
		LASTERRORID le = GetLastError();
		return le == ERROR_SUCCESS ?  ERROR_INVALID_DATA : le;
	}
	clearIIDArray();
	for (; pImportDesc->Name; pImportDesc++)
	{
		PIIDINFO _PIID = IIDM.newOne();
		if (!_PIID)return ERROR_OUT_OF_STRUCTURES;
		_PIID->pMainModBaseAddr = (DWORD_PTR)tagMod;
		_PIID->pThisModImportDescStructAddr = pImportDesc;
		_PIID->subModNameAddrInChar = (LPSTR)((PBYTE)tagMod + pImportDesc->Name);
		_PIID->subModNameRelateAddr = pImportDesc->Name;
		_PIID->subHModule = GetModuleHandleA(_PIID->subModNameAddrInChar);
        strcpy_s(_PIID->subModNameCH, MAX_PATH, _PIID->subModNameAddrInChar);
		_PIID->subModIATRelateAddr = pImportDesc->FirstThunk;
		_PIID->subModIATAddr = (PIMAGE_THUNK_DATA)((PBYTE)tagMod + pImportDesc->FirstThunk);
		_PIID->subModRelateOFT = pImportDesc->OriginalFirstThunk;
		_PIID->subModOFT = (PIMAGE_THUNK_DATA)((PBYTE)tagMod + pImportDesc->OriginalFirstThunk);
		PFUNCTION _PFUN = NULL;
		PIMAGE_THUNK_DATA _PTD = _PIID->subModIATAddr;
		PIMAGE_THUNK_DATA _POTD = _PIID->subModOFT;
		PIMAGE_IMPORT_BY_NAME _PBN = NULL;

		while (_PTD->u1.Function)
		{
			//_PFUN = new FUNCTION;
			//_PFUN = FIM.newOne();
			//ZeroMemory(_PFUN,sizeof(FUNCTION));
			FUNCTION fci = {0};
			_PFUN = &fci;			
			if (_PFUN)
			{
				if (_POTD->u1.AddressOfData)
				{
					_PBN = (PIMAGE_IMPORT_BY_NAME)((PBYTE)tagMod + _POTD->u1.AddressOfData);
					_PFUN->functionImportByName = _PBN;
					if (!IsBadReadPtr(_PBN, sizeof(IMAGE_IMPORT_BY_NAME)))
					{
						strcpy_s(_PFUN->functionName,MAX_PATH, (const char*)_PBN->Name);
					}
					//else _PFUN->functionName = _PFUN->functionName;
				}
				_PFUN->functionOrThunkData = _POTD;
				_PFUN->functionThunkData = _PTD;
				_PFUN->functionAddr = (FARPROC)_PTD->u1.Function;
				//TCHAR* ps = (TCHAR*)_PFUN->functionAddr;
				//_PIID->modFuncArry.push_back(_PFUN);
				_PIID->modFuncArry.push_back(fci);
			}
			else return ERROR_OUT_OF_STRUCTURES;
			_POTD++;
			_PTD++;
		}
		_IIDArray.push_back(_PIID);
	}
	_currentModHandle = tagMod;
	return ERROR_SUCCESS;
}

//����ImageDirectoryEntryToData����ָ�룻
typedef PIMAGE_IMPORT_DESCRIPTOR (WINAPI *PImageDirectoryEntryToData)(PVOID, BOOLEAN,USHORT,PULONG);
typedef int (WINAPI *PMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
typedef int (WINAPI *PMessageBoxW)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef int (WINAPI *PMessageBox)(HWND, LPCTSTR, LPCTSTR, UINT);
typedef HMODULE (WINAPI *PLoadLibrary)(LPCTSTR);
typedef HMODULE (WINAPI *PGetModuleHandle)(LPCTSTR);
typedef HMODULE(WINAPI *PGetModuleHandleA)(LPCSTR);
typedef FARPROC(WINAPI *PGetProcAddress)(HMODULE,LPCSTR);
typedef BOOL(WINAPI *PIsBadReadPtr)(const PVOID,UINT_PTR );

DWORD_PTR WINAPI CLProcessInfo::ThreadFunc1_GetImportDesc(PRMTDATA pData)
{ 
    
    HMODULE hDllmodule = 0;
    PGetModuleHandle pGetModuleHandle = (PGetModuleHandle)pData->m_funcAddr3_GetModuleHandle;
    PLoadLibrary pLoadLibrary = (PLoadLibrary)pData->m_funcAddr4_LoadLibrary;
    PGetProcAddress pGetProcAddress = (PGetProcAddress)pData->m_funcAddr5_GetProcAddress;
    PMessageBoxA pMessageBoxA = (PMessageBoxA)pData->m_funcAddr1_MessageBoxA;
    //pMessageBoxA(0, pData->funcNameCH, 0, MB_ICONWARNING);
    PMessageBox pMessageBox = (PMessageBox)pData->m_funcAddr2_MessageBox;
    //pMessageBox(0, pData->dllNameTH, 0, MB_ICONWARNING); 

    if (pData->type == 0)//ȡ��iat�������ṹ��ַ
    {
        if (!(hDllmodule = pGetModuleHandle(pData->dllNameTH)))
            hDllmodule = pLoadLibrary(pData->dllNameTH);
        if (!hDllmodule)
        {
            pMessageBox(NULL, pData->msg1TH, 0, MB_ICONWARNING);
            return NULL;
        }
        PImageDirectoryEntryToData pImageDirectoryEntryToData = (PImageDirectoryEntryToData)pGetProcAddress(hDllmodule, pData->funcNameCH);
        if (!pImageDirectoryEntryToData)
        {
            pMessageBox(NULL, pData->msg2TH, 0, MB_ICONWARNING);
            return NULL;
        }
        pData->pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
            pImageDirectoryEntryToData(pData->tagMod, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &(pData->size));
        return (DWORD_PTR)pData->pImportDesc;
    }
    else if (pData->type == 1)//ȡ��ģ����
    {
        PGetModuleHandleA pGetModuleHandleA = (PGetModuleHandleA)pData->m_funcAddr6_GetModuleHandleA;
        pData->tagMod = pGetModuleHandleA(pData->funcNameCH);
        return (DWORD_PTR)pData->tagMod;
    }
    else if (pData->type == 2)//��ѯģ��ɶ�
    {
        PIsBadReadPtr pIsBadReadPtr = (PIsBadReadPtr)pData->m_funcAddr7_IsBadReadPtr;
        int i = 0;
        for (i = 0; i < MAX_PATH; i++)
        {
            pData->func2NameCH[i] = 0;
        }
        if (!pIsBadReadPtr(pData->pImgImpByName, sizeof(IMAGE_IMPORT_BY_NAME)))
        {
            //strcpy_s(pData->func2NameCH, MAX_PATH, pData->pImgImpByName->Name);
            CHAR* p = (CHAR*) pData->pImgImpByName->Name;
            i = 0;
			if(p)
				while( (pData->func2NameCH[i++] = *p++) != 0 && i < (MAX_PATH-1) );
            /*while (*p != 0 && i <(MAX_PATH-1))
            {
                pData->func2NameCH[i] = *p;
                p++;
                i++;
            }*/
			
        }
        return (DWORD_PTR)pData->func2NameCH;
    }
    else if (pData->type == 3)//δʹ��
    {
        return 0;
    }  
    return 0;
}

DWORD_PTR CLProcessInfo::ThreadFunc1_GetImportDescEnd(void)
{
    return (DWORD_PTR)((size_t)CLProcessInfo::ThreadFunc1_GetImportDescEnd - (size_t)CLProcessInfo::ThreadFunc1_GetImportDesc);
}

LASTERRORID CLProcessInfo::initRemoteIIDArray(LONG dwProcessId, HMODULE tagMod)
{
	HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
	DWORD_PTR cch = 0,cch1 = 0;
	PRMTDATA prdata1 = 0;
    PTHREAD_START_ROUTINE pfnThreadRtn = 0;
    LASTERRORID errorID = ERROR_INVALID_DATA;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 0;
	RMTDATA data1 = {0};   

        if (dwProcessId < 0 || tagMod == 0)
            goto ErrorEnd1;;

        // �����Ҫע�����Ľ��̵ľ��.
        hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessId);
		
        if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

		
        data1.tagMod = tagMod;
        _tcscpy_s(data1.dllNameTH ,MAX_PATH ,_T("imagehlp.dll"));
        strcpy_s(data1.funcNameCH, MAX_PATH, "ImageDirectoryEntryToData");
        
        HMODULE hUser32 = LoadLibrary(_T("User32.dll"));
        data1.m_funcAddr1_MessageBoxA = GetProcAddress(hUser32, "MessageBoxA");
        
#ifdef UNICODE
        data1.m_funcAddr2_MessageBox = GetProcAddress(hUser32, "MessageBoxW");
        data1.m_funcAddr3_GetModuleHandle = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetModuleHandleW");
        data1.m_funcAddr4_LoadLibrary = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
#else
        data1.m_funcAddr2_MessageBox = GetProcAddress(hUser32, "MessageBoxA");
        data1.m_funcAddr3_GetModuleHandle = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetModuleHandleA");
        data1.m_funcAddr4_LoadLibrary = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
#endif
        FreeLibrary(hUser32);
        data1.m_funcAddr5_GetProcAddress = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetProcAddress");
        data1.m_funcAddr6_GetModuleHandleA = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetModuleHandleA");
        data1.m_funcAddr7_IsBadReadPtr = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "IsBadReadPtr");
        _tcscpy_s(data1.msg1TH, MAX_PATH, _T("�޷����� imagehlp.dll ��"));
        _tcscpy_s(data1.msg2TH, MAX_PATH, _T("�޷���ȡ���� ImageDirectoryEntryToData ��ַ��"));

        // ����RMTDATA���ֽ���.
        cch = sizeof(RMTDATA);

        // ��Զ���߳���Ϊ�������ݷ���ռ�.
        prdata1 = (PRMTDATA)VirtualAllocEx(hProcess, NULL, cch, MEM_COMMIT, PAGE_READWRITE);

        if (prdata1 == NULL)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // �����ݽṹ���帴�Ƶ�Զ�̽��̵��ڴ�ռ�.
        if (!WriteProcessMemory(hProcess,(PVOID)prdata1,(PVOID)&data1,cch,NULL))
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // ���㺯�����ֽ���.
		// Debug�汾���Զ����㺯�����С
		// ע�⣺Release�汾����ر��Ż�ѡ�������ע�����ڴ���ʳ���
        cch1 = CLProcessInfo::ThreadFunc1_GetImportDescEnd(); 

        // ��Զ���߳���Ϊ��������ռ�.       
		pfnThreadRtn = (PTHREAD_START_ROUTINE)VirtualAllocEx(hProcess,NULL, cch1,MEM_COMMIT ,PAGE_EXECUTE_READWRITE);
        
        if (pfnThreadRtn == NULL)
        {
            errorID = GetLastError(); 
			goto ErrorEnd1;
        }

        // �������帴�Ƶ�Զ�̽��̵��ڴ�ռ�.
        if (!WriteProcessMemory(hProcess,(PVOID)pfnThreadRtn,(PVOID)CLProcessInfo::ThreadFunc1_GetImportDesc,cch1,NULL))
        {
            errorID = GetLastError();
            goto ErrorEnd1;
        }

        hThread = CreateRemoteThread( hProcess,NULL, 0, pfnThreadRtn, (PVOID)prdata1,0,NULL);
        if (hThread == NULL)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // �ȴ�Զ���߳�,��LoadLiberary���̽���.
        WaitForSingleObject(hThread, INFINITE);
        if (!ReadProcessMemory(hProcess, &(prdata1->pImportDesc), &pImportDesc,sizeof(PIMAGE_IMPORT_DESCRIPTOR), NULL))
        {
             errorID = GetLastError(); 
			 goto ErrorEnd1;
        }

        if (!pImportDesc) 
        {
            errorID = ERROR_INVALID_DATA; //Զ��iat�������ṹָ���ȡʧ��
			goto ErrorEnd1;
        }

        clearIIDArray();//�ɹ������iid����

        IMAGE_IMPORT_DESCRIPTOR importDesc;
        ReadProcessMemory(hProcess, pImportDesc, &importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR), NULL);
	
		for (; importDesc.Name;)
        {
            PIIDINFO _PIID = IIDM.newOne();
            if (!_PIID){
				errorID = ERROR_OUT_OF_STRUCTURES; 
				goto ErrorEnd1;
			}
            _PIID->pMainModBaseAddr = (DWORD_PTR)tagMod;
            _PIID->pThisModImportDescStructAddr = pImportDesc;
            _PIID->subModNameAddrInChar = (LPSTR)((PBYTE)tagMod + importDesc.Name);
            _PIID->subModNameRelateAddr = importDesc.Name;
			ZeroMemory(_PIID->subModNameCH,MAX_PATH);
            const CHAR* p = _PIID->subModNameAddrInChar;
            CHAR c = 0;
            int i = 0;
            do
            {
                c = 0;
                ReadProcessMemory(hProcess, p++, &c, sizeof(CHAR), NULL);
                _PIID->subModNameCH[i++] = c;
            } while (c != 0);

            _PIID->subHModule = getRemoteModuleHandle(hProcess, _PIID->subModNameCH,prdata1, pfnThreadRtn);
            _PIID->subModIATRelateAddr = importDesc.FirstThunk;
            _PIID->subModIATAddr = (PIMAGE_THUNK_DATA)((PBYTE)tagMod + importDesc.FirstThunk);
            _PIID->subModRelateOFT = importDesc.OriginalFirstThunk;
            _PIID->subModOFT = (PIMAGE_THUNK_DATA)((PBYTE)tagMod + importDesc.OriginalFirstThunk);
            PFUNCTION _PFUN = NULL;
            PIMAGE_THUNK_DATA _PTD = _PIID->subModIATAddr;
            PIMAGE_THUNK_DATA _POTD = _PIID->subModOFT;
            PIMAGE_IMPORT_BY_NAME _PBN = NULL;

            IMAGE_THUNK_DATA itd = { 0 };
            IMAGE_THUNK_DATA iotd = { 0 };
            ReadProcessMemory(hProcess, _PTD, &itd, sizeof(IMAGE_THUNK_DATA), NULL);
            ReadProcessMemory(hProcess, _POTD, &iotd, sizeof(IMAGE_THUNK_DATA), NULL);
            while (itd.u1.Function)
            {
               // _PFUN = new FUNCTION;
				//_PFUN = FIM.newOne();
				//ZeroMemory(_PFUN,sizeof(FUNCTION));
				FUNCTION fci = {0};
				_PFUN = &fci;				
                if (_PFUN)
                {                   
                    if (iotd.u1.AddressOfData)
                    {
                        _PBN = (PIMAGE_IMPORT_BY_NAME)((PBYTE)tagMod + iotd.u1.AddressOfData);
                        _PFUN->functionImportByName = _PBN;
                        getRemoteFuncName(hProcess, _PFUN->functionName, prdata1, pfnThreadRtn,_PBN);
                    }
                    _PFUN->functionOrThunkData = _POTD;
                    _PFUN->functionThunkData = _PTD;
                    _PFUN->functionAddr = (FARPROC)itd.u1.Function;
                    //_PIID->modFuncArry.push_back(_PFUN);
					_PIID->modFuncArry.push_back(fci);
                }
                else { errorID = ERROR_OUT_OF_STRUCTURES; goto ErrorEnd1; }
                _POTD++;
                _PTD++;
                ReadProcessMemory(hProcess, _PTD, &itd, sizeof(IMAGE_THUNK_DATA), NULL);
                ReadProcessMemory(hProcess, _POTD, &iotd, sizeof(IMAGE_THUNK_DATA), NULL);
            }
            _IIDArray.push_back(_PIID);
            importDesc.Name = 0;
            pImportDesc++;
            ReadProcessMemory(hProcess, pImportDesc, &importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR), NULL);
        }
        _currentModHandle = tagMod;
        errorID = ERROR_SUCCESS;   
ErrorEnd1:
     // �رվ��. 
        if (pfnThreadRtn != NULL)
            VirtualFreeEx(hProcess, (PVOID)pfnThreadRtn, 0, MEM_RELEASE);
        if (prdata1 != NULL)
            VirtualFreeEx(hProcess, (PVOID)prdata1, 0, MEM_RELEASE);
        if (hThread != NULL)
            CloseHandle(hThread);
        if (hProcess != NULL)
            CloseHandle(hProcess);
    return errorID;
}

HMODULE CLProcessInfo::getRemoteModuleHandle(HANDLE hProcess, LPCSTR pModuleName, RMTDATA* prdata1, PTHREAD_START_ROUTINE pfnThreadRtn)
{
    HANDLE hThread = NULL;
    DWORD_PTR cch = sizeof(HMODULE);
    HMODULE ret = 0;
    if (hProcess <= 0 || pModuleName == 0)
        return ret;
    
        int type = 1;
		CHAR funcNameCH[MAX_PATH] = {0};
        strcpy_s(funcNameCH, MAX_PATH, pModuleName);

        if (!WriteProcessMemory(hProcess,(PVOID)&(prdata1->type),(PVOID)&type,sizeof(int),NULL))
        {
            goto ErrorEnd1;
        }
		if (!WriteProcessMemory(hProcess,(PVOID)(prdata1->funcNameCH),(PVOID)funcNameCH,sizeof(CHAR)*MAX_PATH,NULL))
		{
			goto ErrorEnd1;
		}
        // ����Զ���̣߳���ͨ��Զ���̵߳����û���DLL�ļ�. 
        hThread = CreateRemoteThread(hProcess,NULL,0,pfnThreadRtn,(PVOID)prdata1,0,NULL);
        if (hThread == NULL)
        {
            goto ErrorEnd1;
        }
        WaitForSingleObject(hThread, INFINITE);
        if (!ReadProcessMemory(hProcess, (PVOID)&(prdata1->tagMod) , &ret, sizeof(HMODULE), NULL))
        {
            goto ErrorEnd1;
        }
ErrorEnd1:
	 {
        if (hThread != NULL)
            CloseHandle(hThread);
    }
    return ret;
}

BOOL CLProcessInfo::getRemoteFuncName(HANDLE hProcess, LPCSTR pFuncName, RMTDATA* prdata1, PTHREAD_START_ROUTINE pfnThreadRtn, PIMAGE_IMPORT_BY_NAME pIIBN)
{
    HANDLE hThread = NULL;
    BOOL errorID = FALSE;
    if (hProcess <= 0 || pFuncName == 0)
        return errorID;

        int type = 2;
		if (!WriteProcessMemory(hProcess,(PVOID)&(prdata1->type),(PVOID)&type,sizeof(int),NULL))
			goto ErrorEnd1;
        if (!WriteProcessMemory(hProcess,(PVOID)&(prdata1->pImgImpByName),(PVOID)&pIIBN,sizeof(PIMAGE_IMPORT_BY_NAME),NULL))
			goto ErrorEnd1;
        // ����Զ���̣߳���ͨ��Զ���̵߳����û���DLL�ļ�. 
        hThread = CreateRemoteThread(hProcess,NULL,0,pfnThreadRtn,(PVOID)prdata1,0,NULL);
        if (hThread == NULL)
			goto ErrorEnd1;

        // �ȴ�Զ���߳�,��LoadLiberary���̽���.
        WaitForSingleObject(hThread, INFINITE);

        if (!ReadProcessMemory(hProcess, (PVOID)&(prdata1->func2NameCH), (PVOID)pFuncName, sizeof(CHAR)*MAX_PATH, NULL))
			goto ErrorEnd1;

        errorID = TRUE;
ErrorEnd1:
	{
        if (hThread != NULL)
            CloseHandle(hThread);
    }
    return errorID;
}

//PCreateRemoteThread CLProcessInfo::pCreateRemoteThread(){
//	HMODULE hKer = 0;
//	if(( hKer = GetModuleHandle(_T("kernel32.dll"))) == NULL)
//		hKer = LoadLibrary(_T("kernel32.dll"));
//	return (PCreateRemoteThread)GetProcAddress(hKer,"CreateRemoteThread");
//}

CLProcessInfo::CLProcessInfo()
{
	initProEnt32Array();
}

CLProcessInfo::~CLProcessInfo()
{
	clearAllInfo();
}

void CLProcessInfo::clearAllInfo(void)
{
	clearProEnt32Array();
	clearModEnt32Array();
	clearIIDArray();
	clearThrEnt32Array();
}

const std::vector<PROCESSENTRY32*>& CLProcessInfo::info(void) const
{
	return _ProEnt32Array;
}

const std::vector<MODULEENTRY32*>& CLProcessInfo::modInfo(void) const
{
	return _ModEnt32Array;
}

const std::vector<THREADENTRY32*>& CLProcessInfo::thrInfo(void) const
{
	return _ThrEnt32Array;
}

const std::vector<IIDINFO*>& CLProcessInfo::iidInfo(void) const
{
	return _IIDArray;
}

CLProcessInfo& CLProcessInfo::updateInfo(void)
{
	initProEnt32Array();
	PROCESSENTRY32 _proInfo;
	if(!findProcessStructFromInnerProListByProID(_currentProID, _proInfo))
	{
		clearThrEnt32Array();
		clearModEnt32Array();
		clearIIDArray();
		_currentProID = -1;
		_currentModHandle = 0;
		return *this;
	}
	if (_currentProID >= 0) 
	{ 		
		initModEnt32Array(_currentProID); 
		initThrEnt32Array(_currentProID);
		MODULEENTRY32 _mod;
		if (_currentModHandle != 0 )
		{
			if(!findModuleStructFromInnerModListByHandle(_currentModHandle, _mod))
				_currentModHandle = modInfo().at(0)->hModule;
			int i =  (_currentProID == GetCurrentProcessId() ?  initIIDArray1(_currentModHandle): initIIDArray(_currentModHandle));
			if (i != ERROR_SUCCESS)
			{
				return  *this;
			}
		}
	}  
	return  *this;
}

LASTERRORID CLProcessInfo::enablePrivilege(LPCTSTR seName)
{
	HANDLE hToken;
	DWORD_PTR fOk ;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
        return GetLastError();
	
    TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
    if (!LookupPrivilegeValue(NULL, seName, &tp.Privileges[0].Luid))
    {
        fOk = GetLastError();
        CloseHandle(hToken);
        return fOk;
    }
		
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;		
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		
    fOk = GetLastError();		
    CloseHandle(hToken);
    return fOk;	
}

BOOL CLProcessInfo::findProcessStructFromInnerProListByProID(DWORD_PTR dProcessID, PROCESSENTRY32 & outStruct)
{
	if (_ProEnt32Array.size() == 0 || &outStruct == nullptr || dProcessID < 0) return false;
    size_t i = 0;
	PROCESSENTRY32* _p = nullptr;
	while (i < _ProEnt32Array.size())
	{
        _p = _ProEnt32Array.at(i);
		i++;
		if (_p->th32ProcessID == dProcessID)
		{			
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findProcessStructFromInnerProListByNameFirst(LPCSTR lpProName, PROCESSENTRY32& outStruct)
{
	if ( _ProEnt32Array.size() == 0 || &outStruct == nullptr || lpProName == nullptr) return false;
	_currentIndex = 0;
	PROCESSENTRY32* _p = nullptr;
	while (_currentIndex < _ProEnt32Array.size())
	{
        _p = _ProEnt32Array.at(_currentIndex);
		_currentIndex++;
		if (_tcsicmp(_p->szExeFile, lpProName) == 0)
		{
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findProcessStructFromInnerProListByNameNext(LPCSTR lpProName, PROCESSENTRY32 & outStruct)
{
	if ( &outStruct == nullptr || lpProName == nullptr) return false;
	PROCESSENTRY32* _p = nullptr;
	while (_currentIndex < _ProEnt32Array.size())
	{
        _p = _ProEnt32Array.at(_currentIndex);
		_currentIndex++;
		if (_tcsicmp(_p->szExeFile, lpProName) == 0)
		{
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findModuleStructFromInnerModListByHandle(HMODULE hModule, MODULEENTRY32 & outStruct)
{
	if (_ModEnt32Array.size() == 0 || &outStruct == nullptr || hModule == 0) return false;
    size_t i = 0;
	MODULEENTRY32* _p = nullptr;
	while (i < _ModEnt32Array.size())
	{
        _p = _ModEnt32Array.at(i);
		i++;
		if (_p->hModule == hModule)
		{
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findModuleStructFromInnerModListByModName(LPCSTR lpModuleName, MODULEENTRY32 & outStruct)
{
	if (_ModEnt32Array.size() == 0 || &outStruct == nullptr || lpModuleName == nullptr) return false;
    size_t i = 0;
	MODULEENTRY32* _p = nullptr;
	while (i < _ModEnt32Array.size())
	{
        _p = _ModEnt32Array.at(i);
		i++;
		if (_tcsicmp(_p->szModule, lpModuleName) == 0)
		{
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findIIDStructFromInnerIIDListByDllName(LPCSTR lpDllName, IIDINFO & outStruct)
{
	if (_IIDArray.size() == 0 || &outStruct == nullptr || lpDllName == nullptr) return false;
    size_t i = 0;
	IIDINFO* _p = nullptr;
	while (i < _IIDArray.size())
	{
        _p = _IIDArray.at(i);
		i++;
		if (_stricmp(_p->subModNameCH, lpDllName) == 0)
		{
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findIIDStructFromInnerIIDListByDllHandle(HMODULE hModule, IIDINFO & outStruct)
{
	if (_IIDArray.size() == 0 || &outStruct == nullptr || hModule == 0) return false;
	size_t i = 0;
	IIDINFO* _p = nullptr;
    while (i < _IIDArray.size())
    {
        _p = _IIDArray.at(i);
		i++;
		if (_p->subHModule == hModule)
		{
			outStruct = *_p;
			return true;
		}
	}
	return false;
}

BOOL CLProcessInfo::findApiStructFromInnerIIDListByApiName(LPCSTR lpApiName, FUNCTION& outStruct)
{
	if (_IIDArray.size() == 0 || &outStruct == nullptr || lpApiName == nullptr) return false;
    size_t i = 0;
	IIDINFO* _p = nullptr;
	FUNCTION* _f = nullptr;
    while (i < _IIDArray.size())
    {
        _p = _IIDArray.at(i);
		i++;
        size_t j = 0;
		while (j < _p->modFuncArry.size())
		{
            _f = &(_p->modFuncArry.at(j));
			j++;
			if (!_f->functionName)continue;
			if (_stricmp(_f->functionName, lpApiName) == 0)
			{
				outStruct = *_f;
				return true;
			}
		}		
	}
	return false;
}

int CLProcessInfo::currentInnerProcessID(void) const
{
	return _currentProID;
}

HMODULE CLProcessInfo::currentInnerModuleHandle(void) const
{
    return _currentModHandle;
}

/*************************************
* BOOL CreateRemoteThreadByRometeloadDll(DWORD_PTR dwProcessId, LPCTSTR lpszDllPath)
* ���� ͨ������Զ���̸߳��������̼���Dll
*
* ���� DWORD_PTR dwProcessId Ŀ�����PID
*   LPCTSTR lpszLibName Dll��·��
* �����Ƿ�ɹ�
**************************************/
DWORD_PTR CLProcessInfo::remoteloadDll(DWORD_PTR dwProcessId, LPCTSTR lpszDllPath)
{
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    PTSTR   pszLibFileRemote = NULL;
    SIZE_T cch = 0;
    PTHREAD_START_ROUTINE pfnThreadRtn = 0;
    LASTERRORID errorID = ERROR_INVALID_DATA;
    if (!lpszDllPath || dwProcessId < 0)
        return errorID;
    if (_tcslen(lpszDllPath) == 0) 
        return errorID;
    
    
        // �����Ҫע�����Ľ��̵ľ��.
        hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessId);
		
        if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // ����DLL·������Ҫ���ֽ���.
        //cch = sizeof(TCHAR) + lstrlen(lpszDllPath);
        cch = sizeof(TCHAR) *(_tcslen(lpszDllPath) + 1);
        // ��Զ���߳���Ϊ·��������ռ�.
        pszLibFileRemote = (PTSTR)VirtualAllocEx(hProcess,NULL,cch,MEM_COMMIT,
            PAGE_EXECUTE_READWRITE //PAGE_READWRITE
            );

        if (pszLibFileRemote == NULL)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // ��DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�.
        if (!WriteProcessMemory(hProcess,(PVOID)pszLibFileRemote,(PVOID)lpszDllPath,cch,NULL))
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // ���LoadLibraryA��Kernel32.dll�е�������ַ. 
        pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle(_T("Kernel32")),
#ifdef UNICODE
            "LoadLibraryW"
#else
            "LoadLibraryA"
#endif
            );

        if (pfnThreadRtn == NULL)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // ����Զ���̣߳���ͨ��Զ���̵߳����û���DLL�ļ�. 
        hThread = CreateRemoteThread(hProcess,NULL,0,pfnThreadRtn,(PVOID)pszLibFileRemote,0,NULL);
        if (hThread == NULL)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }

        // �ȴ�Զ���߳�,��LoadLiberary���̽���.
        WaitForSingleObject(hThread, INFINITE);
        errorID = ERROR_SUCCESS;  
ErrorEnd1:
	{
        // �رվ��. 
        if (pszLibFileRemote != NULL)
            VirtualFreeEx(hProcess, (PVOID)pszLibFileRemote, 0, MEM_RELEASE);
        if (hThread != NULL)
            CloseHandle(hThread);
        if (hProcess != NULL)
            CloseHandle(hProcess);
    }
    return errorID;
}

DWORD_PTR CLProcessInfo::remoteReleaseDll(DWORD_PTR dwProcessId, LPCTSTR szDllName,DWORD_PTR cntUsage)
{
    if (!dwProcessId)return ERROR_INVALID_DATA;
    if (_tcslen(szDllName) == 0)return ERROR_INVALID_DATA;
    if (enablePrivilege() != ERROR_SUCCESS) return ERROR_INVALID_ACCESS;
    MODULEENTRY32 _out_struct;
    if(getModuleInfoOfProcess(dwProcessId, szDllName, _out_struct) != ERROR_SUCCESS) return ERROR_INVALID_ACCESS;
    return remoteReleaseDllByRemoHModule(
        dwProcessId,
        _out_struct.hModule, 
        cntUsage ==  0 ? _out_struct.ProccntUsage : (cntUsage < _out_struct.ProccntUsage ? cntUsage: _out_struct.ProccntUsage)
        );
}

typedef HHOOK(*PFN) ( DWORD_PTR dwThreadID,WPARAM wParam, LPARAM lParam);
HHOOK CLProcessInfo::setHookByDll(HMODULE &out_hHookDll, LPCTSTR in_lpszHookDllPath, LPCSTR in_lpszFucNameInHookDll ,DWORD_PTR dwThrId,WPARAM wParam, LPARAM lParam)
{
    if (!in_lpszHookDllPath) return 0;
    HMODULE hHookDll = LoadLibrary(in_lpszHookDllPath);
    if (hHookDll == NULL)
	{	
		DWORD nLastError =  GetLastError();
		LPTSTR lpMsgBuf = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,nLastError,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR)&lpMsgBuf,0, NULL);
		TCHAR inf[100] ={0};
		if(lpMsgBuf == NULL){
			_stprintf_s(inf,_T("LastError: %d ,�ô������޷�������"),nLastError);
		}
		else {
			_stprintf_s(inf, _T("LastError: %d < %s >"), nLastError, lpMsgBuf);
			delete[] lpMsgBuf;
		}
		::MessageBox(0,inf,0,MB_ICONERROR);
		return 0;
	}
    out_hHookDll = hHookDll;
    PFN pfnInsertDll = (PFN)GetProcAddress(hHookDll,in_lpszFucNameInHookDll == NULL ? "InsertDll":in_lpszFucNameInHookDll);
    if (!pfnInsertDll)
		return 0;
    return pfnInsertDll(dwThrId,wParam,lParam);
}

HMODULE CLProcessInfo::getProcessInfo(DWORD_PTR dwProcessId, LPCTSTR szProcessName, PROCESSENTRY32& out_ProcessEntry32Str)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, dwProcessId);
	if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
	PROCESSENTRY32 _struct = { 0 };
	HMODULE _hMod = 0;
	_struct.dwSize = sizeof(PROCESSENTRY32);
	BOOL bOk = Process32First(hSnapshot, &_struct);
	while (bOk != FALSE)
	{
		if (_struct.th32ProcessID == dwProcessId || _tcsicmp(szProcessName, _struct.szExeFile) == 0)
		{
			if (&out_ProcessEntry32Str)out_ProcessEntry32Str = _struct;
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
			MODULEENTRY32 mod = { sizeof(MODULEENTRY32) };
			Module32First(hSnapshot, &mod);
			CloseHandle(hSnapshot);
			_hMod = mod.hModule;
			break;
		}
		bOk = Process32Next(hSnapshot, &_struct);
	}
	CloseHandle(hSnapshot);
	return _hMod;
}
CLProcessInfo::PROINFOTABLE& CLProcessInfo::enumProcessInfo(LPCTSTR szProcessName, PROINFOTABLE& out_ProcessInfoTable)
{
	out_ProcessInfoTable.clear();
	if (!szProcessName)
		return out_ProcessInfoTable;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS| TH32CS_SNAPTHREAD, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE) 
		return out_ProcessInfoTable;
	PROCESSENTRY32 pro = { sizeof(PROCESSENTRY32) };
	MODULEENTRY32 mod = { sizeof(MODULEENTRY32) };
	THREADENTRY32 trd = { sizeof(THREADENTRY32) };
	HEAPLIST32 leplist = { sizeof(HEAPLIST32) };
	BOOL bOk = Process32First(hSnapshot, &pro);
	while (bOk != FALSE)
	{
		if (_tcsicmp(szProcessName, pro.szExeFile) == 0)
		{
			PROINFO info;
			info.pro = pro; 

			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE| TH32CS_SNAPMODULE32, pro.th32ProcessID);
			BOOL bOk = Module32First(hSnap, &mod);
			int fist = 0;
			while (bOk != FALSE)
			{
				if (mod.th32ProcessID == pro.th32ProcessID)
				{					
					if (fist++ == 0)
						info.mainMod = mod;
					else
						info.subMods.emplace_back(mod);
				}
				bOk = Module32Next(hSnap, &mod);
			}
			CloseHandle(hSnap);
			
			bOk = Thread32First(hSnapshot, &trd);
			fist = 0;
			while (bOk != FALSE)
			{
				if (trd.th32OwnerProcessID == pro.th32ProcessID)
				{
					if (fist++ == 0)
						info.mainThread = trd;
					else
						info.subThreads.emplace_back(trd);
				}
				bOk = Thread32Next(hSnapshot, &trd);
			}

			hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, pro.th32ProcessID);
			bOk = Heap32ListFirst(hSnap,&leplist);
			fist = 0;
			while (bOk != FALSE)
			{
				if (leplist.th32ProcessID == pro.th32ProcessID)
				{
					if (fist++ == 0)
						info.mainHeapList = leplist;
					else
						info.subHeapList.emplace_back(leplist);
				}
				bOk = Heap32ListNext(hSnap, &leplist);
			}
			CloseHandle(hSnap);
			out_ProcessInfoTable.emplace_back(info);
		}
		bOk = Process32Next(hSnapshot, &pro);
	}
	CloseHandle(hSnapshot);
	return out_ProcessInfoTable;
}


HMODULE CLProcessInfo::getModuleInfoOfProcess(DWORD_PTR dwProcessId, LPCTSTR szDllName, MODULEENTRY32 & out_ModuleEntry32Str)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
    MODULEENTRY32 _struct = { 0 };
    HMODULE _hMod = 0;
    _struct.dwSize = sizeof(MODULEENTRY32);
    BOOL bOk = Module32First(hSnapshot, &_struct);
	_struct.th32ProcessID = dwProcessId;
    while (bOk != FALSE)
    {
        if (_tcsicmp(szDllName, _struct.szModule) == 0)
        {
            if (&out_ModuleEntry32Str)out_ModuleEntry32Str = _struct;
            _hMod = _struct.hModule;
            break;
        }
        bOk = Module32Next(hSnapshot, &_struct);
		_struct.th32ProcessID = dwProcessId;
    }
    CloseHandle(hSnapshot);
    return _hMod;
}

DWORD_PTR CLProcessInfo::getProcessIDByName(LPCTSTR lpszProcessName)
{
	PROCESSENTRY32 st;
	CLProcessInfo::getProcessInfo(0,lpszProcessName,st);
	return st.th32ProcessID;
}

BOOL CLProcessInfo::getProcessFileNameByPid(DWORD_PTR dwPid,LPTSTR out_ProcessFullPathName,DWORD_PTR nSize)
{
	HMODULE hProcess = (HMODULE)OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if(!hProcess || hProcess == INVALID_HANDLE_VALUE)return FALSE;
	DWORD_PTR rt = GetModuleFileName(hProcess,out_ProcessFullPathName,nSize);
	CloseHandle(hProcess);
	if(rt)return TRUE;
	else return FALSE;
}

//���� ͨ������Զ���߳�ж���������̵�Dll��hDllModule����ΪԶ�̾����
LASTERRORID CLProcessInfo::remoteReleaseDllByRemoHModule(DWORD_PTR dwProcessId, HMODULE hDllRemoteModule, size_t cntUsage)
{
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    //HMODULE* pszLibFileRemote = NULL;
    DWORD_PTR cch = sizeof(HMODULE);
    PTHREAD_START_ROUTINE pfnThreadRtn = 0;
    LASTERRORID errorID = ERROR_INVALID_DATA;
    if (dwProcessId < 0 || hDllRemoteModule <= 0)
        return errorID;

		DWORD_PTR _thisId = GetCurrentProcessId();
        // �����Ҫע�����Ľ��̵ľ��.
        hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessId);
		
        if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }
        

        // ���LoadLibraryA��Kernel32.dll�е�������ַ. 
        pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"FreeLibrary");
        if (pfnThreadRtn == NULL)
        {
            errorID = GetLastError(); goto ErrorEnd1;
        }
     

        // ����Զ���̣߳���ͨ��Զ���̵߳����û���DLL�ļ�. 
        if (cntUsage == 65535)  //ϵͳģ��
        {
            errorID = ERROR_ACCESS_DENIED; 
            goto ErrorEnd1;
        }
        for (size_t i = 0; i < cntUsage; i++)
        {
            hThread = CreateRemoteThread(hProcess,NULL,0,pfnThreadRtn,(PVOID)hDllRemoteModule,0,NULL);
            if (hThread == NULL)
            {
                errorID = GetLastError(); goto ErrorEnd1;
            }
            // �ȴ�Զ���߳�,��LoadLiberary���̽���.
            WaitForSingleObject(hThread, INFINITE);
            if (hThread != NULL)
            {
                CloseHandle(hThread);
                hThread = NULL;
            }
            errorID = ERROR_SUCCESS;
        }
ErrorEnd1:
	{
        if (hThread != NULL)
            CloseHandle(hThread);
        if (hProcess != NULL)
            CloseHandle(hProcess);
    }
    return errorID;
}

//���� ͨ������Զ���߳�ж���������̵�Dll��
LASTERRORID CLProcessInfo::remoteReleaseDllByName(LPCTSTR pDllName)
{  
    LASTERRORID errorID = ERROR_INVALID_DATA;
    if (!pDllName)
        return errorID;
    if (_tcslen(pDllName) == 0)
        return errorID;
    TCHAR p[MAX_PATH] = { 0 };
    _tcscpy_s(p, MAX_PATH, pDllName);
    updateInfo();
    MODULEENTRY32 me32;
    if (!findModuleStructFromInnerModListByModName(p, me32))
        return ERROR_FILE_EXISTS;
    errorID = remoteReleaseDllByRemoHModule(me32.th32ProcessID, me32.hModule, me32.ProccntUsage);
    if(errorID == ERROR_SUCCESS) updateInfo();
    return errorID;
}


// �鿴������ĳĿ��ģ��ĵ�����ҵ���Ӧ��dll�ͺ������滻��(���ĺ���)
LASTERRORID CLProcessInfo::_setApiToNewAdr(APIHOOK32_ENTRY* phk,HANDLE hOpenedProcess) 
{ 
	DWORD size;

	//��ȡָ��PE�ļ��е�Import��IMAGE_DIRECTORY_DESCRIPTOR�����ָ�� 
	//��ָ����ģ��������������Ϣ
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) 
		ImageDirectoryEntryToData(phk->hModCallerModule,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&size); 

	if (pImportDesc == NULL) 
		return ERROR_SUCCESS; 

	//���Ҽ�¼,������û��������Ҫ��DLL 

	LPCSTR pszDllName;
	CHAR p[MAX_PATH] = { 0 };
#ifdef UNICODE 
	::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)phk->pszCalleeModuleNameTH, -1, p, MAX_PATH, NULL, NULL);
#else
	strcpy_s(p, MAX_PATH, (LPSTR)phk->pszCalleeModuleNameTH);
#endif

	for (;pImportDesc->Name;pImportDesc++) 
	{ 
		pszDllName = (LPCSTR)((PBYTE)phk->hModCallerModule+pImportDesc->Name);
		if (lstrcmpiA(pszDllName, p) == 0)
			break; //�����ģ�����ҵ��ˣ�Ҫ�滻�������ڵ�ģ���������ٱ���
	} 

	if (pImportDesc->Name == NULL) 
	{ 
		return ERROR_SUCCESS; //�����ģ����û���ҵ���Ҫ�滻�������ڵ�ģ������ֱ���˳�
	} 

	//Ѱ��������Ҫ�ĺ��� 

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) ((PBYTE)phk->hModCallerModule+pImportDesc->FirstThunk);//ȡ��iat�ṹ�б�Ķ���
	SIZE_T dwWrite = 0;
	for (;pThunk->u1.Function;pThunk++) 
	{ 
		//ppfn��¼����IAT������Ӧ�ĺ����ĵ�ַ 

		FARPROC * ppfn= (FARPROC *)&(pThunk->u1.Function);	//ȡ����ʵ��api��ڵ�ֵ	 
		if (*ppfn == phk->pfnOriginApiAddress)  //�Ƚϸ���ʵapi���ֵ����GetProcAddressȡ�õ�api��ڵ��Ƿ����
		{   
			//�����ַ��ͬ��Ҳ�����ҵ���������Ҫ�ĺ���������pThunk->u1.Functionֵ�ĸ�д������ָ������������ĺ��� 
			//BOOL rt = WriteProcessMemory(hOpenedProcess, ppfn, &(phk->pfnDummyFuncAddress), sizeof(phk->pfnDummyFuncAddress), &dwWrite);
			DWORD oldFlag = 0,oldFlag2 = 0,lee = 0;
			BOOL rtVp = VirtualProtect(&(pThunk->u1.Function),sizeof(pThunk->u1.Function),PAGE_EXECUTE_READWRITE,&oldFlag);
			if(rtVp == FALSE){
				return lee = GetLastError();
			}
			BOOL rt = WriteProcessMemory(hOpenedProcess, &(pThunk->u1.Function), &(phk->pfnDummyFuncAddress), sizeof(pThunk->u1.Function), &dwWrite);
			if(rt == FALSE){
				return lee = GetLastError();
				//CLString().getLastErrorMessageBoxExceptSucceed(le,_T("WriteProcessMemory ʧ�ܴ���"));
			}
			rtVp = VirtualProtect(&(pThunk->u1.Function),sizeof(pThunk->u1.Function),oldFlag,&oldFlag2);
			if(rtVp == FALSE){
				lee = GetLastError();
			}
			return ERROR_SUCCESS; 
		} 
	} 
	return ERROR_SUCCESS;
} 

LASTERRORID CLProcessInfo::hookWindowsAPI(APIHOOK32_ENTRY* phk) 
{
	assert(phk != NULL);
	if (phk->pszAPINameTH == NULL)
		return ERROR_INVALID_DATA; 
	if (phk->pszCalleeModuleNameTH == NULL) 
		return ERROR_INVALID_DATA; 
	if (phk->pfnOriginApiAddress == NULL) 
		return ERROR_INVALID_DATA; 
	HANDLE hOpenedProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,GetCurrentProcessId());
	if(hOpenedProcess == 0 || hOpenedProcess == INVALID_HANDLE_VALUE){
		return GetLastError();
	}
	if (phk->hModCallerModule == NULL) 
	{ 
		//return TRUE;	//����ʱ������ȫ������
		MEMORY_BASIC_INFORMATION mInfo; 
		HMODULE hModHookDLL; 
		HANDLE hSnapshot; 
		MODULEENTRY32 me = {sizeof(MODULEENTRY32)};  

		//���¹���ȡ�ù���dll�ľ��
		VirtualQuery(_setApiToNewAdr,&mInfo,sizeof(mInfo));   //�ڼ�����Dll�Ľ������ҵ��ҹ���������ģ��HookDll����Ϣ
		hModHookDLL=(HMODULE)mInfo.AllocationBase; 

		//���¹���ȡ�õ�ǰdll���ڽ��̵�����ģ����Ϣ
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0); //����������Ϣ���ն���(�ú������Ի�����н��̣���ǰ���̣��̵߳ȵĿ���)
		BOOL bOk = Module32First(hSnapshot,&me);       //�ɿ��ն���ȡ�õ�һ��ģ��������Ϣ
		while (bOk) 
		{ 
			if (me.hModule != hModHookDLL)               //���ǹ���ģ��͹ҽ�
			{ 
				phk->hModCallerModule = me.hModule; //��ֵ���´ξͲ���������
				DWORD_PTR le = _setApiToNewAdr(phk,hOpenedProcess);
				if(ERROR_SUCCESS != le){
					CloseHandle(hOpenedProcess);
					return le; 
				} 
			} 
			else
			{
				//MessageBox(NULL,"�����ҽ�","Inf",0);
			}
			bOk = Module32Next(hSnapshot,&me);         //ָ����һģ�飬ִ�б�������
		} 	 
	} 
	else 
	{ 
		DWORD_PTR le = _setApiToNewAdr(phk,hOpenedProcess);
		if(ERROR_SUCCESS != le){
			CloseHandle(hOpenedProcess);
			return le; 
		} 
	} 
	CloseHandle(hOpenedProcess);
	return ERROR_SUCCESS; 
} 

//�ṹ���У�ԭ��ַ���µ�ַ���������¹ҽӻָ�ԭ״
LASTERRORID CLProcessInfo::unHookWindowsAPI(APIHOOK32_ENTRY* phk) 
{ 
	assert(phk != NULL);
	APIHOOK32_ENTRY tmp;
	tmp = *phk;
	tmp.pfnOriginApiAddress = phk->pfnDummyFuncAddress; 
	tmp.pfnDummyFuncAddress = phk->pfnOriginApiAddress; 
	tmp.hModCallerModule = NULL;
	return CLProcessInfo::hookWindowsAPI(&tmp); 
}   

BOOL CLProcessInfo::is64BitSystem()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || 
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
			return TRUE;
	else
		return FALSE;
}
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL CLProcessInfo::is64BitProcess(DWORD_PTR nProID,BOOL& _out_bIs64BitPro)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,nProID);
	BOOL rt = FALSE;
	if(hProcess == 0 || hProcess == INVALID_HANDLE_VALUE){
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION ,FALSE,nProID);
		if(hProcess == 0 || hProcess == INVALID_HANDLE_VALUE){
			return FALSE;
		}
	}	
	rt = IsWow64Process(  hProcess, &_out_bIs64BitPro );
	_out_bIs64BitPro = !_out_bIs64BitPro;
	CloseHandle(hProcess);
	return rt;
}

typedef struct EnumFunArg  
{     
	HWND      hWnd;  
	DWORD_PTR    dwProcessId;     
}EnumFunArg,*LPEnumFunArg;  
BOOL CALLBACK CLProcessInfo::_enumFuncCallBack(HWND hwnd, LPARAM lParam)  
{ 
	EnumFunArg  *pArg = (LPEnumFunArg)lParam;     
	DWORD  processId;  
	GetWindowThreadProcessId(hwnd, &processId);      
	if( processId == pArg->dwProcessId)    
	{     
		pArg->hWnd = hwnd;  
		return TRUE;      
	}  
	return FALSE;     
}
HWND CLProcessInfo::getHwndByProID(DWORD_PTR processID)  
{  
	HWND retWnd=NULL;    
	EnumFunArg wi;    
	wi.dwProcessId   =processID;    
	wi.hWnd   =  NULL;    
	::EnumWindows(_enumFuncCallBack,(LPARAM)&wi);  
	if(wi.hWnd){retWnd=wi.hWnd;	}   
	return retWnd;  
} 

LASTERRORID CLProcessInfo::getProIATInfo( DWORD_PTR processID,PROIATINFO& inf )
{
	//���ǰ�Ľ����������
	ZeroMemory(&(inf.proInfo),sizeof(PROCESSENTRY32));
	inf.modIatLst.clear();
	//����������������������
	CLProcessInfo pi;
	LASTERRORID rtl = pi.initModEnt32Array(processID);
	if(rtl != ERROR_SUCCESS)
		return rtl;
	BOOL rtb = pi.findProcessStructFromInnerProListByProID(processID,inf.proInfo);
	if(rtb == FALSE)
		return ERROR_INVALID_DATA;
	size_t si = pi.modInfo().size();
	inf.modIatLst.reserve(si);//���汸��
	MODIATINFO modIat = {0};
	IIDINFO iidInf = {0};
	FUNCTION funcInfo = {0};
	for(UINT i=0;i< si;i++){		
		inf.modIatLst.push_back(modIat);//�ȹ���
		PMODIATINFO pm = &(inf.modIatLst[i]);
		pm->modIndex = i;
		pm->modInfo = *(pi.modInfo()[i]);
		GetCurrentProcessId() == processID ? rtl = pi.initIIDArray1(pm->modInfo.hModule) : rtl = pi.initIIDArray(pm->modInfo.hModule);
		if(rtl != ERROR_SUCCESS)
			continue;
		size_t isi = pi.iidInfo().size();
		pm->iidLst.reserve(isi);		
		for (size_t j=0;j<isi;j++)
		{
			pm->iidLst.push_back(iidInf);
			//pm->iidLst[j] = pi.iidInfo()[j];
			PIIDINFO piid = &(pm->iidLst[j]);
			const PIIDINFO piidc = pi.iidInfo()[j];
			piid->pThisModImportDescStructAddr = piidc->pThisModImportDescStructAddr;//ģ�鵼�����Ϣ������Ľṹ��ָ��
			piid-> pMainModBaseAddr = piidc-> pMainModBaseAddr;//�õ�������ڵ���ģ�����ַ
			piid-> subModNameRelateAddr = piidc-> subModNameRelateAddr;
			piid-> subModNameAddrInChar = piidc-> subModNameAddrInChar;
			strcpy_s(piid->subModNameCH,MAX_PATH,piidc->subModNameCH);
			piid-> subHModule = piidc-> subHModule;
			piid-> subModRelateOFT = piidc-> subModRelateOFT;  
			piid-> subModOFT = piidc-> subModOFT;
			piid-> subModIATRelateAddr = piidc-> subModIATRelateAddr;
			piid-> subModIATAddr = piidc-> subModIATAddr;
			//std::vector<PFUNCTION> modFuncArry;//���뺯����ThunkData�����Ϣ�ṹ��
			size_t fsi = piidc->modFuncArry.size();
			piid->modFuncArry.reserve(fsi);
			for(size_t k =0;k<fsi;k++){
				piid->modFuncArry.push_back(funcInfo);
				memcpy_s(&(piid->modFuncArry[k]),sizeof(FUNCTION),&(piidc->modFuncArry[k]),sizeof(FUNCTION));
			}
			
		}
	}
	return ERROR_SUCCESS;
}

LASTERRORID CLProcessInfo::emunExportFunction(LPCVOID tagMod,std::vector<FUNCTIONEXP>& inf,LPCSTR  familyFlag,BOOL isFullMatch){
	inf.clear();
	BOOL useFlagFilter = ((familyFlag == NULL || familyFlag[0] == 0)? FALSE:TRUE);
	DWORD size;
	PIMAGE_EXPORT_DIRECTORY pds;
	if (!(pds = (PIMAGE_EXPORT_DIRECTORY)
		ImageDirectoryEntryToData((PVOID)tagMod, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size)))
	{
		LASTERRORID le = GetLastError();
		return le == ERROR_SUCCESS ?  ERROR_INVALID_DATA : le;
	}

	LPCSTR lpDLLName = ((LPCSTR)((PBYTE)tagMod + pds->Name));
	DWORD_PTR* AddressOfFunctions = (DWORD_PTR*)((PBYTE)tagMod + pds->AddressOfFunctions);
	DWORD_PTR* AddressOfNames = (DWORD_PTR*)((PBYTE)tagMod + pds->AddressOfNames);
	WORD* AddressOfNameOrdinals = (WORD*)((PBYTE)tagMod + pds->AddressOfNameOrdinals);
	FUNCTIONEXP fnexp;
	if(useFlagFilter == FALSE){
		inf.reserve(pds->NumberOfFunctions);
		for(DWORD_PTR i=0; i < pds->NumberOfFunctions ;i++){	
			ZeroMemory(&fnexp,sizeof(FUNCTIONEXP));
			fnexp.addressOrdinal = i;		
			fnexp.baseOrdinal = pds->Base;
			fnexp.exportOrdinal = pds->Base + i;
			fnexp.rvaOfFunction = AddressOfFunctions[i];
			fnexp.addressOfFunction = (FARPROC)((PBYTE)tagMod + fnexp.rvaOfFunction);		
			inf.push_back(fnexp);
		}	
		for(DWORD_PTR i=0; i < pds->NumberOfNames ;i++){		
			WORD idc = AddressOfNameOrdinals[i];
			LPCSTR lpFN = (LPCSTR)((PBYTE)tagMod + AddressOfNames[i]);
			if(lpFN)strcpy_s(inf[idc].functionName,MAX_PATH,lpFN);
			else inf[idc].functionName[0]=0;
		}
	}else{
		for(DWORD_PTR i=0; i < pds->NumberOfNames ;i++){		
			LPCSTR lpFN = (LPCSTR)((PBYTE)tagMod + AddressOfNames[i]);
			if( isFullMatch == FALSE ? 
				strstr(lpFN,familyFlag) == lpFN : 
				strcmp(lpFN,familyFlag) == 0  
				){
				ZeroMemory(&fnexp,sizeof(FUNCTIONEXP));					
				strcpy_s(fnexp.functionName,MAX_PATH,lpFN);
				WORD idc = AddressOfNameOrdinals[i];
				fnexp.addressOrdinal = idc;		
				fnexp.baseOrdinal = pds->Base;
				fnexp.exportOrdinal = pds->Base + idc;
				fnexp.rvaOfFunction = AddressOfFunctions[idc];
				fnexp.addressOfFunction = (FARPROC)((PBYTE)tagMod + fnexp.rvaOfFunction);		
				inf.push_back(fnexp);
				if(isFullMatch)
					return ERROR_SUCCESS;
			}
		}
		if(inf.size() == 0)
			return ERROR_INVALID_FUNCTION;
	}
	return ERROR_SUCCESS;
}

HWND CLProcessInfo::getConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	TCHAR pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
	// WindowTitle.
	TCHAR pszOldWindowTitle[MY_BUFSIZE]; // Contains original
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
	_stprintf_s(pszNewWindowTitle,_T("%d/%d"),GetTickCount(),GetCurrentProcessId());
	SetConsoleTitle(pszNewWindowTitle);
	Sleep(10);
	hwndFound=FindWindow(NULL, pszNewWindowTitle);
	SetConsoleTitle(pszOldWindowTitle);
	return(hwndFound);
}

