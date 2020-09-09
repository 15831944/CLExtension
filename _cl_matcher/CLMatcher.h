#pragma once
#ifndef __CL_MATCHER_H__
#define __CL_MATCHER_H__

#include "windows.h"
#include "../_cl_string/CLString.h"

class CLMatcher;
typedef CLMatcher* PCLMatcher;
typedef const PCLMatcher PCLCMatcher;
typedef CLMatcher& CLMatcherR;
typedef const CLMatcherR CLMatcherRC;

#define MAX_LEN 256
typedef struct _str_kwUnit{
	TCHAR kwd[MAX_LEN];//�ؼ���
	byte nCharCounts;//�ؼ����ַ�����
	byte nCp;//�ڲ�ʹ�õ���ʱ����
	LPCTSTR lpFirst;//��һ��ƥ���λ��
	size_t nMatch;//ƥ�����
	_str_kwUnit(){reset();}
	_str_kwUnit& reset(){ZeroMemory(this,sizeof(_str_kwUnit));return *this;}
}KwUnit,*PKwUnit;
typedef struct _str_KeyWordsLst{ 			
	_str_kwUnit unit[MAX_LEN];//�ؼ�������
	byte nUnitCounts;//�ؼ�������
	_str_KeyWordsLst(){}
	_str_KeyWordsLst& reset(){ZeroMemory(this,sizeof(_str_KeyWordsLst));return *this; }
}KeyWordsLst,*PKeyWordsLst;

//�б�ƥ�乤����
class CLMatcher{
protected:
	KeyWordsLst m_kwLst;  
	LPCTSTR pOrgHead;
	TCHAR m_currentChar;
	CLString m_splitLst;

public:
	//Ĭ�Ϲ��캯�����ڲ������汾��ʱ��
	CLMatcher();
	virtual~CLMatcher() {};
	//��������ڲ����ݺ�����
	CLMatcherR reset();
	//�����ַ���Դ
	CLMatcherR setOrg(LPCTSTR text);
	//���ùؼ����б��ؼ��ּ���,�������Զ�ִ�йؼ��ֵ�trim������
	CLMatcherR setKeyLst(LPCTSTR keyString);
	//ִ��ȫ��ƥ�䣬ִ��ǰ�����setOrg��setKeyLst������ã�ִ�к����getResultȡ�ý��
	CLMatcherR match();
	//��match��������ã�ȡ�ùؼ�����Ϣ�ṹ�壬�ɴ�unit��ȡ�ý��
	const KeyWordsLst& getResult() const;
	//ƥ���
	float matchRat() const;
    //�ؼ��ָ���
	UINT getKeyWordCounts() const;
};

#endif


