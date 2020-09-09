
#pragma once

#ifndef __CL_JSONTASK__
#define __CL_JSONTASK__

//input outside library
#include "../_cl_common/CLCommon.h"
#include "../_jsoncpp/json.h"
#include "../_cl_string/CLString.h"


//ȡ�ýڵ㹤��������,lpkeyΪһ��������
#define GET_JOB_TYPE( lpkey ) LPCSTR lpkey = var->trdParm_json[JTASK_Key_JobType].isString() ? \
	(var->trdParm_json[JTASK_Key_JobType].asCString()): "*δ֪����*" ;
//ȡ����ҳԭʼ����,codeΪһ��������
#define GET_ORG_CODE( code ) byte code = 0;setOrgCode(var,code);
//����0�����˳�
#define RET_FALSE \
	goto Ret0;
//����1�����˳�
#define RET_TRUE \
	goto Ret1;
//������ĩβ�˳���
#define PROC_RETURN \
	Ret1:\
	return 1;\
	Ret0:\
	return 0;
//�õ�Ԫ�ж��Ƿ���Ҫ�����ַ�����
#define logoutu( x ) (isLogOpen() ? logout((x)):0)
//���峣���ַ���ָ�������,ʧ�ܺ��˳�
#define MAKEVAR_LPTR_( varName , varSec )  \
	LPCSTR varName = var->trdParm_json[#varSec].isString() ? \
	var->trdParm_json[#varSec].asCString() : \
	(logoutu( (var->strTmp % _T("ERROR����") << (var->index) <<  _T("������Ԫ��\"")	<< _T( #varSec ) <<_T("\"�ֶβ����ڻ����ʹ���")).string()),0);\
	if(varName == 0)\
		RET_FALSE;	
//���峣���ַ���ָ�������,ʧ�ܺ��˳�
#define MAKEVAR_ARRAY_( varName , varSec , numb ) \
	Json::Value varName;if(var->trdParm_json[#varSec].isArray())\
	varName = var->trdParm_json[#varSec];\
	if(varName.size() != numb ){\
		logoutu( (var->strTmp % _T("ERROR����") << (var->index) <<  _T("������Ԫ��\"")	<< _T( #varSec ) <<_T("\"�ֶε��б����ڻ����ݸ�������")).string()); \
		RET_FALSE;\
	}	

//���������
#define STEPS_PUTOUT( ci, cn ) \
	stepsOut((ci),(cn));

//ѭ����ʼ��
#define CYCLE_BEGIN_() \
	double _s_ticks = CLTime::getTickCountsMicro(),_s_ticks2 = _s_ticks; \
	for (;;) { \
		WHEN_EVENT_QUIT_ARRIVE_BREAK_LOOP(var->quitEvent); \
		WHEN_WM_QUIT_ARRIVE_BREAK_LOOP(); \
		STEPS_PUTOUT(1, 100);

//ѭ����β��
#define CYCLE_END_() \
cycle: \
	_s_ticks = CLTime::getTickCountsMicro();\
	logoutu( var->strTmp.format(200,_T("���: [%s]-><%.6fs> ..."),var->fatherRootKey.string(),_s_ticks - _s_ticks2).string());\
	_s_ticks2 = _s_ticks;\
	}


using namespace std;
using namespace Json;

//����Ĺ̻�key����
#define JTASK_Key_State ("״̬")
#define JTASK_Key_OpemLog ("����")
#define JTASK_Key_JobType ("�ɼ�����")
#define JTASK_Key_Notes ("ע��")
#define JTASK_Key_ExeTrdNumber ("ִ���߳���")
#define JTASK_Key_ExeTimePoint ("ִ��ʱ���")
#define JTASK_Key_ExeTimePoint_Date ("��������")
#define JTASK_Key_ExeTimePoint_Time ("ʱ������")
#define JTASK_Key_ExeTimePoint_Week ("����")
#define JTASK_Key_ExeJobsVar ("���������")
#define JTASK_Key_DataPageUrl ("����ҳ������")
#define JTASK_Key_DataOrgCode ("ԭʼ���ݱ���")


typedef int(__cdecl *JTASK_INFLOGOUT_CALLBACK)(LPCTSTR format); //��������ص�����ָ��
typedef int(__cdecl *JTASK_STEPSPUTOUT_CALLBACK)(DWORD currentIndex, DWORD totalCounts); //��������ص�����ָ��

//�ڵ㹲��ĸ�������,�ڲ���Ԫ����������ݽṹ��
typedef class JTASK_SHARE {
public:
	JTASK_INFLOGOUT_CALLBACK* ppFuncInfputout;//�����Ϣ�ӿ�ָ�������ָ��
	JTASK_STEPSPUTOUT_CALLBACK* ppFuncStepsPutout;//����߳��ڲ���ǰ�������ȵĽӿڱ���ָ��
	UINT* pTrdQuitMsgId;
	HWND* pHWnd;
}*PJTASK_SHARE;

//JTASKU�ڲ�ʹ�õ��߳����ݽṹ������������̳�,����reset()ʵ�ֳ�ʼ��
typedef class JTASK_TRDINF {
public:
	//�������------------------------------------------
	size_t index;//�̱߳�ţ�1��ʼ����
	DWORD nId;//���߳�id	
	HANDLE handle;//���߳̾��	
	HANDLE quitEvent;//�˳��ź��¼����ñ����ɼ���ⲿ�˳��¼�
	DWORD ownerTrdId;//�����߳�id
	UINT quitMsgId;//�߳��˳��������̻߳򴰿ڹ��̷��͵���Ϣ 
	HWND hWnd;//������Ϣ�����ڹ���

	CLString fatherRootKey;//����Ԫ������
	size_t fatherRootKeyIndex;//����Ԫ����������

	//Ԥ���幦�ܱ���--------------------------------
	CLString strTmp;//���߳�ʹ�õ���ʱ����
	//��������--------------------------------------
	int iRet; // -1��ʾû��ִ�������̣�0��ʾִ�г���1��ʾ�ɹ�
	//������Json��������--------------------------------------
	Json::Value trdParm_json;//���ݸ��̵߳�json�������
	
	JTASK_TRDINF(); 
	virtual~JTASK_TRDINF();
	virtual void reset(); 
} *PJTASK_TRDINF;//JTASKU�ڲ�ʹ�õ��߳����ݽṹ��

//�����ڵ㵥Ԫ(��λ������)��,��������������mainProc()����ʵ���Զ��幦��
//1���ɸ�����Ҫ������A���̳���JTASK_TRDINF���ݽṹ�������Զ��������
//2�����幤����ģ����B���̳���ģ����CLJsonTaskUnit<A>������д�����̷���mainProc()������ԽṹA���������������������fillThreadData()��
template<typename JTASK_TRDINF_>
class CLJsonTaskUnit :public CLTaskSvc{
private:
	CLString m_rootKey;
	BOOL m_isSetJson;
	Value m_json;
	const JTASK_SHARE* m_pShare;
	size_t m_unitIndex;
	std::vector<JTASK_TRDINF_*> JTASK_TRDINFLst;
	size_t m_trdNumber;
	BOOL m_isLogOpen;
public:
	//�ڲ��������Ƿ���������
	BOOL isJobsRunning()
	{
		size_t si = JTASK_TRDINFLst.size();

		for (size_t i = 0; i < si; i++) {
			PJTASK_TRDINF p = JTASK_TRDINFLst.at(i);
			lock();
			if (isTrdRunning(p->handle)) {
				unlock();
				return TRUE;
			}
			else unlock();
		}
		return FALSE;
	}
	CLStringRC rootKey() { return m_rootKey; }//ȡ�øýڵ������ֵ
	BOOL isSetJson() const { return m_isSetJson; }//����Ƿ����ù�����
	CLJsonTaskUnit& quitJobs() { _quitAndClearTrdLst(0); return *this; }//�˳���������ⲿΨһ�ֶΣ��˳�����������������Դ������еĻ���
	CLJsonTaskUnit(const JTASK_SHARE* pShare) :m_pShare(pShare)
	{
		_init();
	}
	virtual ~CLJsonTaskUnit()
	{
		_reset();
		_clear();
	}
	//�����ڵ� -1�ڵ���ã� 0ʱ�䷶Χ�⣬1������װ��������2δ��װ������������
	int execute(BOOL timeCheck)
	{
		//�������Ƿ�����
		if (isJobsRunning()) {
			logoutu(CLString().format(200, _T("�ڵ�\"%s\"�����������С�"), rootKey().string()).string());
			return 2;
		}

		//�Ƿ�ڵ����״̬
		if (isUnitAvailable() == FALSE) {
			logoutu(CLString().format(200, _T("�ڵ�\"%s\"�ѽ��á�"), rootKey().string()).string());
			return -1;
		}

		//�Ƿ���ʱ�䷶Χ	
		if (timeCheck == TRUE && _isInTimeRang() == FALSE) {
			logoutu(CLString().format(200, _T("�ڵ�\"%s\"����ִ��ʱ�䷶Χ�⡣"), rootKey().string()).string());
			return 0;
		}

		logoutu(CLString().format(200, _T("�ڵ�\"%s\"���С�"), rootKey().string()).string());

		//��ȡ�߳�����
		if (m_json[JTASK_Key_ExeTrdNumber].isInt() == true) {
			m_trdNumber = m_json[JTASK_Key_ExeTrdNumber].asInt() < 1 ? CLTaskSvc::getCpuCoreCounts() : m_json[JTASK_Key_ExeTrdNumber].asInt();
		}
		else m_trdNumber = CLTaskSvc::getCpuCoreCounts();

		//��װ�̶߳���
		_quitAndClearTrdLst(m_trdNumber);//�˳�,�����б�

		size_t curr = 0, cshould = m_trdNumber; size_t _index = 0;
		auto ite = JTASK_TRDINFLst.begin();
		for (; ite != JTASK_TRDINFLst.end(); )
		{
			(*ite)->reset();
			(*ite)->quitMsgId = m_pShare ? *(m_pShare->pTrdQuitMsgId) : WM_NULL;
			(*ite)->hWnd = (m_pShare && (m_pShare->pHWnd) && *(m_pShare->pHWnd) && ::IsWindow(*(m_pShare->pHWnd))) ? *(m_pShare->pHWnd) : 0;
			(*ite)->ownerTrdId = GetCurrentThreadId();
			(*ite)->trdParm_json = m_json[JTASK_Key_ExeJobsVar];
			(*ite)->fatherRootKey = rootKey();
			(*ite)->fatherRootKeyIndex = m_unitIndex;
			(*ite)->index = ++_index;
			if (FALSE == fillThreadData((*ite), _index, JTASK_TRDINFLst.size()))
				return -1;
			++ite;
		}
		//���������߳�
		curr = CLTaskSvc::start(m_trdNumber,TRUE);
		for (CLTaskSvc::ThreadsTable::const_iterator i = getThreadsTable().cbegin(); i != getThreadsTable().cend(); )
		{
			JTASK_TRDINFLst.at(i->second.nIndex - 1)->handle = i->second.hThread;
			JTASK_TRDINFLst.at(i->second.nIndex - 1)->nId = i->second.tId;
			JTASK_TRDINFLst.at(i->second.nIndex - 1)->quitEvent = i->second.quitEvent;
			++i;
		}
		resume();//��ʾ����

		logoutu(CLString().format(200, curr == cshould ? _T("���������� <%d/%d>,time: %s .") : _T("���������� <%d/%d>,time: %s,WARNING:����������ȫ��")
			, curr, cshould, CLTime().getLocalTime().dateTimeStr(CLString()).string()).string());

		return curr;
	}
	//�ڵ��Ƿ�������������������ñ�Ҫ����
	BOOL isUnitAvailable()
	{
		if (!isSetJson())
			return FALSE;
		if (m_json[JTASK_Key_State].isNull() == true)
			return TRUE;
		else if (m_json[JTASK_Key_State].isBool() == false)
			return TRUE;
		else if (m_json[JTASK_Key_State].asBool() == true)
			return TRUE;
		else
			return FALSE;
	}
	CLJsonTaskUnit& setUnitJson(const Value& json, int index, LPCSTR rootKey = 0)
	{
		m_json = json;
		m_rootKey = rootKey;
		m_unitIndex = index < 0 ? (size_t)0 : (size_t)index;
		if (m_json.type() == Json::objectValue)
			m_isSetJson = TRUE;
		if (isSetJson())
			m_isLogOpen = m_json[JTASK_Key_OpemLog].isBool() ? m_json[JTASK_Key_OpemLog].asBool() : TRUE;
		else m_isLogOpen = TRUE;
		if (rootKey) {
			logoutu(CLString().format(200, _T("����ִ�нڵ�\"%s\"��"), this->rootKey().string()).string());
		}
		
		return *this;
	}
	inline BOOL isLogOpen() const {return m_isLogOpen;}
	static BOOL isTrdRunning(HANDLE hd) { return hd && ::WaitForSingleObject(hd, 0) == WAIT_TIMEOUT; }
	//��json�ļ�����ȡ���������ݵı�����ʽ
	static void setOrgCode(PJTASK_TRDINF var, byte& code)
	{
		assert(var != NULL);
		code = EnCode_UTF8;
		if (var->trdParm_json[JTASK_Key_DataOrgCode].isString()) {
			if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UTF-8") == 0)
				code = EnCode_UTF8;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UTF8") == 0)
				code = EnCode_UTF8;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "ASCII") == 0)
				code = EnCode_ASCII;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UNICODE") == 0)
				code = EnCode_ULE;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UNICODE_BIG_ENDIAN") == 0)
				code = EnCode_UBE;
		}
	}
protected:			
	void _init(){
		m_isSetJson = FALSE;
		m_trdNumber = 1;
		m_unitIndex = -1;
		m_isLogOpen = TRUE;
	}
	void _reset(){}
	void _clear(){
		for (size_t i = 0; i < JTASK_TRDINFLst.size(); i++) {
			JTASK_TRDINF_* p = JTASK_TRDINFLst.at(i);
			if (p)
				delete p;
		}
		JTASK_TRDINFLst.clear();
	}
	BOOL _isInTimeRang()
	{
		if (!isSetJson())return FALSE;
		if (m_json[JTASK_Key_ExeTimePoint].isObject() == false)return FALSE;
		BOOL bd = TRUE, bt = TRUE;
		TIMESTAMP ct = CLTime::getLocalTime_ll();
		Value tjson = m_json[JTASK_Key_ExeTimePoint];
		if (tjson[JTASK_Key_ExeTimePoint_Date].isNull() == false && tjson[JTASK_Key_ExeTimePoint_Date].isArray() == true) {//set time
			long date = (long)(ct / 1000000000);
			Value ary = tjson[JTASK_Key_ExeTimePoint_Date];
			Value::UInt si = ary.size();
			bd = FALSE;
			for (Value::UInt i = 0; i < si; i += 2)
			{
				if (ary[i].isInt() == true) {
					if (date < ary[i].asInt())
						continue;
				}
				if (ary[i + 1].isInt() == true) {
					if (date <= ary[i + 1].asInt()) {
						bd = TRUE; break;;
					}
				}
				else bd = TRUE;
			}
		}
		if (tjson[JTASK_Key_ExeTimePoint_Time].isNull() == false && tjson[JTASK_Key_ExeTimePoint_Time].isObject() == true) {
			long time = (long)(ct % 1000000000);
			int wk = CLTime::getDayOfWeekByStamp_ll(ct);
			Value tp = tjson[JTASK_Key_ExeTimePoint_Time];
			Value::Members member = tp.getMemberNames();
			bt = FALSE;
			CLString szt;
			for (size_t i = 0; bt == FALSE && i < member.size(); i++)
			{
				szt = member.at(i).c_str();
				if (szt.size() >= 2 && szt.find(_T(',')) > -1) {// check ,
					szt.trim().split(_T(','));
					BOOL isyes = FALSE;
					for (size_t k = 0; isyes == FALSE && szt.getVT().size() > k; k++)
					{
						if (wk == _ttoi(szt.getVT().at(k))) { isyes = TRUE; }
					}
					if (isyes == FALSE)continue;
				}
				else if (szt.size() >= 3 && szt.find(_T('-')) > -1) { // check -
					szt.trim().split(_T('-'));
					int k1 = _ttoi(szt.getVT().at(0));
					int k2 = _ttoi(szt.getVT().at(1));
					if (BETWEEN(wk, k1, k2) == FALSE)continue;
				}
				else { if (szt.stoi() != wk)continue; }
				Value ary = tp[szt.getASCII()];
				Value::UInt si = ary.size();
				for (Value::UInt i = 0; i < si; i += 2)
				{
					if (ary[i].isInt() == true) {
						if (time < ary[i].asInt())
							continue;
					}
					if (ary[i + 1].isInt() == true) {
						if (time <= ary[i + 1].asInt()) {
							bt = TRUE; break;;
						}
					}
					else bt = TRUE;
				}
			}
		}
		return (bd && bt) ? TRUE : FALSE;
	}
	//ֻ�˳��̣߳��������б�
	void _quitJobs()
	{
		size_t si = JTASK_TRDINFLst.size();
		for (size_t i = 0; i < si; i++)
		{
			PJTASK_TRDINF p = JTASK_TRDINFLst.at(i);
			HANDLE hd = p->handle;
			lock();
			if (isTrdRunning(p->handle)) {
				::PostThreadMessage(p->nId, WM_QUIT, 0, 0);
				unlock();
				::WaitForSingleObject(hd, INFINITE);
			}
			else unlock();
			p->reset();
		}		

		CLTaskSvc::close();
	}
	//�˳��̣߳������б�,����ָ���ĸ������б����,0��ʾ�˳�������ȫ������
	void _quitAndClearTrdLst(size_t leave  = 0 )
	{
		_quitJobs();
		size_t si = JTASK_TRDINFLst.size();
		if (leave == 0) {
			for (size_t i = 0; i < si; i++)
				delete JTASK_TRDINFLst.at(i);
			JTASK_TRDINFLst.clear();
		}
		else if (leave > si) {
			for (; si < leave; si++) { JTASK_TRDINFLst.push_back(new JTASK_TRDINF_); }
		}
		else if (leave < si) {
			for (size_t i = si - 1; i >= leave; i--) {
				delete JTASK_TRDINFLst.at(i);
				JTASK_TRDINFLst.erase(JTASK_TRDINFLst.begin() + i);
			}
		}
	}
	//������������������أ�
	virtual DWORD run(PCLTaskSvcTrdParam _var);
		
	//�������������Ϣ�ĺ������������Ըı�ʵ�֣�����������������أ�
	virtual int logout(LPCTSTR str) {
		return (m_isLogOpen ? (m_pShare ? ((m_pShare->ppFuncInfputout) ? ((*(m_pShare->ppFuncInfputout)) ? (*(m_pShare->ppFuncInfputout))(str) : FALSE) : FALSE) : FALSE) : FALSE);
	}
	//�������������Ϣ�ĺ������������Ըı�ʵ�֣�����������������أ����Դ��߳�������
	virtual int stepsOut(DWORD currentIndex, DWORD totalCounts) {
		if (m_pShare && (m_pShare->ppFuncStepsPutout) && (*(m_pShare->ppFuncStepsPutout))) { 
			lock(0); int rt = (*(m_pShare->ppFuncStepsPutout))(currentIndex, totalCounts); unlock(0); return rt; 
		}else return 0;
	}
	//����Ҫ���̺����������������أ������̵߳���Ҫ���̺���
	virtual int mainProc(PJTASK_TRDINF var);	
	//����Ҫ���̺����������������أ������߳����ݽṹ�����������������FALSE���������߳���
	virtual BOOL fillThreadData(PJTASK_TRDINF p, size_t ci, size_t cn) {
		return TRUE;
	}
};

template<typename JTASK_TRDINF_>
DWORD CLJsonTaskUnit<JTASK_TRDINF_>::run(PCLTaskSvcTrdParam _var)
{
	PJTASK_TRDINF var = NULL;
	for (size_t i = 0; i < JTASK_TRDINFLst.size(); i++)
	{
		PJTASK_TRDINF p = (PJTASK_TRDINF)JTASK_TRDINFLst.at(i);
		if (p->index == _var->info.nIndex) {
			var = p;
			break;
		}
	}
	assert(var != NULL);
	if (var == NULL) 
		return 0;
	//------------------------------------------------------------------------------------------	
	LPCSTR _szType = ("\0");
	if (var->trdParm_json[JTASK_Key_JobType].isString() == false) {
		logoutu(var->strTmp.format(200, _T("ERROR : \"�ɼ�����\"�ֶ����ݲ����ڻ�Ϊ�գ���%d������Ԫֹͣ���˳���"), var->index).string());
		goto Exit1;
	}
	else
		_szType = var->trdParm_json[JTASK_Key_JobType].asCString();
	logoutu(var->strTmp.format(200, _T("�ڵ�\"%s\"������\"%s\"����%d������Ԫ������"), var->fatherRootKey.string(), _szType, var->index).string());

	//------------------------------------------------------------------------------------------
	int rt = mainProc(var);
	var->iRet = rt;
	//------------------------------------------------------------------------------------------	
Exit1:

	//����������	
	logoutu(var->strTmp.format(200, _T("�ڵ�\"%s\",��%d������Ԫ�˳���"), var->fatherRootKey.string(), var->index).string());
	//�˳�ǰ����һ����Ϣ�������̻߳򴰿ڹ���
	BOOL bp = FALSE;
	if (var->quitMsgId != WM_NULL) {
		if (var->hWnd)
			bp = ::PostMessage(var->hWnd, var->quitMsgId, var->fatherRootKeyIndex, var->index);
		else if (var->ownerTrdId)
			bp = ::PostThreadMessage(var->ownerTrdId, var->quitMsgId, var->fatherRootKeyIndex, var->index);
	}
	return 1;
}

template<typename JTASK_TRDINF_>
int CLJsonTaskUnit<JTASK_TRDINF_>::mainProc(PJTASK_TRDINF var)
{
	assert(var != NULL);
	//------------------------------------------------------------------------------------
	GET_JOB_TYPE(lpJobType);
	CYCLE_BEGIN_();
	logoutu(var->strTmp.format(_T(">>> �ڵ� (%s, %zd, %s) : �����໯����дmainProc()��������Ҫֱ��ʵ�������࣡ ..."),
		var->fatherRootKey.string(), var->index, lpJobType).string());
	throw MessageBox(NULL, _T("�����໯����дmainProc()��������Ҫֱ��ʵ�������࣡"), _T("exception"), MB_ICONERROR);
	Sleep(1000);
	CYCLE_END_();
	PROC_RETURN;
}

//ʵ�ֺ�json���ݸ�ʽ�󶨵Ķ��������๤�ߣ�ʹ�÷�������
//1���ɸ�����Ҫ������A���̳���JTASK_TRDINF���ݽṹ�������Զ��������
//2�����幤����ģ����B���̳���ģ����CLJsonTaskUnit<A>������д�����̷���mainProc()������ԽṹA���������������������fillThreadData()��
//3�����巺�Ͷ���C������ΪCLJsonTask<A,B>,��Cʵ������
template<
	typename JTASK_TRDINF_, 
	template<typename JTASK_TRDINF__> typename JTUNIT = CLJsonTaskUnit
>
class CLJsonTask
{
#define JTUNIT_TYPE JTUNIT<JTASK_TRDINF_>
private:
	//friend class JTUNIT_TYPE;
	CLString m_jsonPath;
	CLString m_jsonStr;
	BOOL m_bJsonEnable;
	Value m_jsonMain;
	Reader m_jsonReader;
	Value::Members m_jsonMember;
	UINT m_trdQuitMsg;
	HWND m_trdQuitMsgTagWnd;
	BOOL m_timeCheckFlag;

	JTASK_INFLOGOUT_CALLBACK m_pFuncInfputout;
	JTASK_STEPSPUTOUT_CALLBACK m_pFuncStepsPutout;

	std::vector<JTUNIT_TYPE*> m_vtStoreUnitLst;
	std::vector<JTUNIT_TYPE*> m_vtUnitLst;
	JTASK_SHARE m_share;

protected:
	inline JTUNIT_TYPE* _getAqUnit(int iIndex = 0)const {
		return (iIndex >= 0 && (size_t)iIndex < m_vtUnitLst.size()) ? m_vtUnitLst.at(iIndex) : 0;
	}
	inline JTUNIT_TYPE* _adJTASKUnit() {
		m_vtStoreUnitLst.push_back(new JTUNIT_TYPE(&m_share));
		return m_vtStoreUnitLst.at(m_vtStoreUnitLst.size() - 1);
	}
	void _init()
	{
		m_pFuncInfputout = 0;
		m_pFuncStepsPutout = 0;
		m_trdQuitMsg = WM_NULL;
		m_bJsonEnable = FALSE;
		m_share.ppFuncInfputout = &m_pFuncInfputout;
		m_share.ppFuncStepsPutout = &m_pFuncStepsPutout;
		m_share.pTrdQuitMsgId = &m_trdQuitMsg;
		m_share.pHWnd = &m_trdQuitMsgTagWnd;
		setTimeCheckFlag(TRUE);
	}
	void _reset(){
	}
	void _clear()	{
		quitJobs();
		for (size_t i = 0; i < m_vtStoreUnitLst.size(); i++)
			delete m_vtStoreUnitLst.at(i);
		m_vtStoreUnitLst.clear();
		m_vtUnitLst.clear();
	}
	JTUNIT_TYPE* _newOneUnit(size_t curSize)	{
		size_t si = m_vtStoreUnitLst.size();
		if (si > curSize)return m_vtStoreUnitLst.at(si - 1);
		else return _adJTASKUnit();
	}
public:
	CLJsonTask(){
		_init();
	}
	virtual ~CLJsonTask(){
		_reset();
		_clear();
	}	
	//���ر���json�ִ�����
	inline CLStringRC getJsomString() { return m_jsonStr; }
	//�����ڲ�json�ṹ����
	inline const Value& getValue() { return m_jsonMain; }
	//ȡ�������õĽڵ����
	inline size_t getUnitCounts() { return m_vtUnitLst.size(); }
	//����Ƿ����ÿ��õ�json����
	inline BOOL isAlready() { return isSetJson(); }
	//������Ϣ�ַ�������ӿ�����
	inline CLJsonTask& setLogoutProc(JTASK_INFLOGOUT_CALLBACK pFuncIn = 0) { m_pFuncInfputout = pFuncIn; return *this; }
	//���ò������Խӿں���
	inline CLJsonTask& setStepsPutoutProc(JTASK_STEPSPUTOUT_CALLBACK pFuncIn = 0) { m_pFuncStepsPutout = pFuncIn; return *this; }
	//�ú���ΪJTASK����һ���Զ�����Ϣ������Ϣ����JTASK�ڲ�ĳ�ڵ㹤�����ÿһ���߳��˳�ʱ��������JTASK�̣߳���ĳ���ڹ��̷��͸���Ϣ��������ھ��tagHwnd���õĻ���
	//������ϢΪWM_NULL(0)�������߳��˳�ʱ��������Ϣ����tagHwnd��ΪNULL����Ե����߳�ֱ���򴰿ڹ��̷�����Ϣ��
	//����Ϣ�Ľ��ն�WPARAM���������ܵ����͸���Ϣ���߳����ڵĽڵ���JTASK�ڲ��������ţ���0��ʼ����LPARAM�����������߳������ڵĹ������е������ţ���1��ʼ��
	inline CLJsonTask& setTrdQuitMsgId(UINT msg = WM_NULL, HWND tagHwnd = 0) { m_trdQuitMsg = msg; m_trdQuitMsgTagWnd = tagHwnd; return *this; }
	//����Ƿ�����������Ժ���ָ��
	inline BOOL isSetLogoutFunc() { return m_pFuncInfputout ? TRUE : FALSE; }
	//����Ƿ��������в����������ָ��
	inline BOOL isSetStepsFunc() { return m_pFuncStepsPutout ? TRUE : FALSE; }
	//�Ƿ�������ʱ�����ǣ���������Ҫ�������ʱ��
	inline BOOL isTimeCheckOpen() const { return m_timeCheckFlag; }
	//����ʱ�����ǣ�TRUEΪ��
	inline CLJsonTask& setTimeCheckFlag(BOOL timeCheckFlag) { m_timeCheckFlag = timeCheckFlag ? TRUE : FALSE; return *this; }
	//ͨ���ļ�����json����
	CLJsonTask& setJsonByLoadFile(LPCTSTR lpFilePath)
	{
		logout(_T("��ȡ�ļ�������Json���ݡ�"));
		if (isAnyoneExecuting())
		{
			logout(_T("WARNING : ���������еĹ����飬���Ա���Json�������ã�")); return *this;
		}
		m_bJsonEnable = FALSE;
		m_jsonPath = lpFilePath;
		m_jsonStr.empty();
		m_jsonMain.clear();
		m_vtUnitLst.clear();
		if (m_jsonPath.fileExists()) {
			CLString m_tmp;
			m_tmp.readFile(m_jsonPath);
			if (!m_jsonReader.parse(m_tmp.getASCII(), m_tmp.getASCII() + strlen(m_tmp.getASCII()), m_jsonMain)) {
				logout(_T("ERROR : Json���ݽ���ʧ��,�������ݸ�ʽ��ȷ�ԣ�")); return *this;
			}
			m_jsonStr = m_tmp;
			if (m_jsonMain.type() == Json::nullValue) {
				logout(_T("ERROR : ��ȡ��Json����Ϊ�����ݣ�")); return *this;
			}
			logout(_T("Json�����Ѷ�ȡ��������ϡ�"));
			m_jsonMember = m_jsonMain.getMemberNames();
			for (auto it = m_jsonMember.begin(); it != m_jsonMember.end(); it++) {
				if (!m_jsonMain[*it].isObject())
					continue;
				JTUNIT_TYPE* p = _newOneUnit(m_vtUnitLst.size());
				m_vtUnitLst.push_back(p);
				p->setUnitJson(m_jsonMain[*it], m_vtUnitLst.size() - 1, (*it).c_str());
			}
			m_bJsonEnable = TRUE;
		}
		else logout(_T("ERROR : ָ����Json�����ļ������ڣ�"));
		return *this;
	}
	//ͨ���ַ�������json����
	CLJsonTask& setJson(LPCTSTR lpJsonStr)
	{
		logout(_T("����Json���ݡ�"));
		if (isAnyoneExecuting())
		{
			logout(_T("WARNING : ���������еĹ����飬���Ա���Json�������ã�")); return *this;
		}
		m_bJsonEnable = FALSE;
		m_jsonStr = lpJsonStr;
		m_jsonPath.empty();
		m_jsonMain.clear();
		m_vtUnitLst.clear();
		if (m_jsonStr.strlen() > 0) {
			if (!m_jsonReader.parse(m_jsonStr.getASCII(), m_jsonStr.getASCII() + strlen(m_jsonStr.getASCII()), m_jsonMain))
			{
				logout(_T("ERROR : Json�ַ������ݽ���ʧ��,�������ݸ�ʽ��ȷ�ԣ�"));
				m_jsonStr.empty();
				return *this;
			}
			if (m_jsonMain.type() == Json::nullValue) {
				logout(_T("ERROR : ��ȡ��Json����Ϊ�����ݣ�")); return *this;
			}
			logout(_T("Json���ݽ���������ϡ�"));
			m_jsonMember = m_jsonMain.getMemberNames();
			for (auto it = m_jsonMember.begin(); it != m_jsonMember.end(); it++) {
				if (!m_jsonMain[*it].isObject())
					continue;
				JTUNIT_TYPE* p = _newOneUnit(m_vtUnitLst.size());
				m_vtUnitLst.push_back(p);
				p->setUnitJson(m_jsonMain[*it], m_vtUnitLst.size() - 1, (*it).c_str());

			}
			m_bJsonEnable = TRUE;
		}
		else logout(_T("ERROR : ָ����Json�����ִ�����Ϊ0 ��"));
		return *this;
	}
	//�ж��Ƿ�������json���ݣ��ɷ���������json���ַ���
	BOOL isSetJson(LPTSTR lpFilePathBuf = 0, size_t nBufSizeInChar = MAX_PATH)
	{
		if (m_bJsonEnable) {
			if (lpFilePathBuf) _tcsncpy_s(lpFilePathBuf, nBufSizeInChar, m_jsonPath.string(), m_jsonPath.strlen());
			return TRUE;
		}
		else return FALSE;
	}
	//ִ�����нڵ�
	CLJsonTask& executeAll()
	{
		if (!isAlready())return *this;
		JTUNIT_TYPE* p = 0;
		for (int i = 0; p = _getAqUnit(i); i++)
			p->execute();
		return *this;
	}
	//-1�ڵ���ã� 0ʱ�䷶Χ�⣬1������װ��������2δ��װ������������
	int execute(int nIndex = 0)
	{
		if (!isAlready())return -1;
		JTUNIT_TYPE* p = 0;
		if (p = _getAqUnit(nIndex))
			return p->execute(m_timeCheckFlag);
		else return -1;
	}
	//-1�ڵ���ã� 0ʱ�䷶Χ�⣬1������װ��������2δ��װ������������
	int execute(LPCTSTR lpKey = 0)
	{
		if (!isAlready())
			return -1;
		JTUNIT_TYPE* p = 0;
		for (int i = 0; ((p = _getAqUnit(i)) != NULL) && (size_t)i < m_vtUnitLst.size(); i++)
			if (p->rootKey() == lpKey)
				return p->execute(m_timeCheckFlag);
		return -1;
	}
	//-1�ڵ���ã� 0ʱ�䷶Χ�⣬1������װ��������2δ��װ������������
	int executeNextUnit(LPCTSTR lpCurrUnitKey = 0, int offi = 1)
	{
		int k = -1;
		if ((k = getUnitIndex(lpCurrUnitKey)) == -1)
			return -1;
		int si = m_vtUnitLst.size();
		k = (offi + k) % si;
		if (k < 0) k = si + k;
		return _getAqUnit(k)->execute(m_timeCheckFlag);
	}
	//-1�ڵ���ã� 0ʱ�䷶Χ�⣬1������װ��������2δ��װ������������
	int executeNextUnit(int ci = -1, int offi = 1)
	{
		if (_getAqUnit(ci) == NULL)
			return -1;
		int si = m_vtUnitLst.size();
		ci = (offi + ci) % si;
		if (ci < 0) ci = si + ci;
		return _getAqUnit(ci)->execute(m_timeCheckFlag);
	}
	//�˳��������Ψһ����������ģʽ��
	CLJsonTask& quitJobs()
	{
		size_t si = m_vtUnitLst.size();
		for (size_t i = 0; i < si; i++)
		{
			m_vtUnitLst.at(i)->quitJobs();
		}
		return *this;
	}
	//�Ƿ��нڵ�����������
	BOOL isAnyoneExecuting(LPTSTR lpMainFlagBuf = 0, size_t nBufSizeInChar = MAX_PATH)
	{
		size_t si = m_vtUnitLst.size();
		for (size_t i = 0; i < si; i++)
		{
			if (m_vtUnitLst.at(i)->isJobsRunning()) {
				if (lpMainFlagBuf != NULL && nBufSizeInChar > 0) {
					_tcscpy_s(lpMainFlagBuf, nBufSizeInChar, m_vtUnitLst.at(i)->rootKey().string());
				}
				return TRUE;
			}
		}
		return FALSE;
	}
	//���ĳ��ָ���ڵ��Ƿ�������
	BOOL isUnitExecuting(LPCTSTR lpMainFlagBuf = 0)
	{
		size_t si = m_vtUnitLst.size();
		for (size_t i = 0; i < si; i++)
		{
			if (m_vtUnitLst.at(i)->rootKey().compareNoCase(lpMainFlagBuf) == 0
				&& m_vtUnitLst.at(i)->isJobsRunning()) {
				return TRUE;
			}
		}
		return FALSE;
	}
	//���ĳ��ָ���ڵ��Ƿ�������
	BOOL isUnitExecuting(int index = 0)
	{
		auto p = _getAqUnit(index);
		return p ? p->isJobsRunning() : FALSE;
	}
	//���ĳ��ָ���ڵ��Ƿ��Ѿ����ڲ������������
	BOOL isUnitAvailable(LPCTSTR lpKey = 0)
	{
		if (!lpKey)return FALSE;
		if (isSetJson() == FALSE)return FALSE;
		JTUNIT_TYPE* p = 0;
		for (int i = 0; p = _getAqUnit(i); i++) { if (p->rootKey().compareNoCase(lpKey) == 0) { return p->isUnitAvailable(); } }
		return FALSE;
	}
	//���ĳ��ָ���ڵ��Ƿ��Ѿ����ڲ������������
	BOOL isUnitAvailable(int index = 0)
	{
		JTUNIT_TYPE* p = _getAqUnit(index);
		return  p ? p->isUnitAvailable() : FALSE;
	}
	//���ĳ��ָ���ڵ��Ƿ��Ѿ����ڲ�������
	BOOL haveSomeUnit(LPCTSTR lpKey = 0)
	{
		if (!lpKey)return FALSE;
		if (isSetJson() == FALSE)return FALSE;
#ifndef UNICODE
		for (auto i = m_jsonMember.begin(); i != m_jsonMember.end(); i++) { if (_stricmp((*i).c_str(), lpKey) == 0)return TRUE; }
#else
		CLString str;
		for (auto i = m_jsonMember.begin(); i != m_jsonMember.end(); i++) { str = lpKey; if (_stricmp((*i).c_str(), str.getASCII()) == 0)return TRUE; }
#endif // !UNICODE

		return FALSE;
	}
	//���� û���ҵ����� NULL
	LPCTSTR getUnitKey(int index = 0)
	{
		auto p = _getAqUnit(index);
		return p ? p->rootKey().string() : 0;
	}
	//����ָ�����ƵĽڵ����ڲ�������ֵ��0��ʼ��û���ҵ�����-1
	int getUnitIndex(LPCTSTR lpKey = 0)
	{
		if (!lpKey)return -1;
		for (size_t i = 0; i < m_vtUnitLst.size(); i++) { if (m_vtUnitLst.at(i)->rootKey() == lpKey)return i; }
		return -1;
	}
	//ȡ��ĳ�ڵ���߳���
	size_t getUnitRunningTrdCounts(int index = 0)
	{
		JTUNIT_TYPE* p = _getAqUnit(index);
		if (!p)return 0;
		size_t ret = 0;
		p->lock();
		for (size_t i = 0; i < p->JTASK_TRDINFLst.size(); i++) {
			if (JTUNIT_TYPE::isTrdRunning(p->JTASK_TRDINFLst.at(i)->handle))
				ret++;
		}
		p->unlock();
		return ret;
	}
	//�������������Ϣ�ĺ���
	virtual int logout(LPCTSTR str) {
		return (m_pFuncInfputout ? (*m_pFuncInfputout)(str) : FALSE);
	}
};

//�ɻ�����ɵĹ����壬δʵ��mainProc���̣�����ʹ�ø���ʵ������
//1���ɸ�����Ҫ������A���̳���JTASK_TRDINF���ݽṹ�������Զ��������
//2�����幤����ģ����B���̳���ģ����CLJsonTaskUnit<A>������д�����̷���mainProc()������ԽṹA���������������������fillThreadData()��
//3�����巺�Ͷ���C������ΪCLJsonTask<A,B>,��Cʵ������
typedef CLJsonTask<JTASK_TRDINF, CLJsonTaskUnit> CLJsonTaskBase;

#endif // !__CL_JSONTASK__