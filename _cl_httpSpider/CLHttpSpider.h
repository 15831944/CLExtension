#pragma once
#ifndef __CL_HTTPSPIDER_H__
#define __CL_HTTPSPIDER_H__

#include "../../_cl_extension/_cl_extension.h"
#include "windows.h"

class CLHttpSpider;
typedef CLHttpSpider *PCLHttpSpider;
typedef const CLHttpSpider *PCCLHttpSpider;
typedef CLHttpSpider& CLHttpSpiderR;
typedef const CLHttpSpider& CLHttpSpiderRC;



class CLHttpSpider{
protected:
	CLString m_rem;
	CLString m_org;
	CLString m_value;
	CLString m_tmp;
	int m_ceng;
	
	virtual LPCTSTR _labelRecrsion(LPCTSTR ptcTag,LPCTSTR ptcRem,std::vector<CLString>& resuleSet,int nlayer,LPCTSTR lpHeader,size_t nAllSize);//��ȡ����
	virtual BOOL _checkSelfClose(LPCTSTR pc,LPCTSTR lpHeader,size_t nAllSize);//�����λ���Ƿ�Ϊ�Ապϱ�ǩ,��<���濪ʼ����
	virtual BOOL _checkNotes(LPCTSTR pc,LPCTSTR &p1,LPCTSTR lpHeader,size_t nAllSize);//��<���濪ʼ
	//��<��ʼ����,����ɹ�p1���ر�ǩ�ʸ�β��֮���ָ�룬p2���ر�ǩ����֮���ָ��
	virtual BOOL _isFlag(LPCTSTR pc,LPCTSTR lpflag,LPCTSTR &p1,LPCTSTR &p2,LPCTSTR lpHeader,size_t nAllSize);
	//��<��ʼ����,����ɹ�p1����<��ָ�룬p2����>֮���ָ��
	virtual BOOL _tatchEnd(LPCTSTR pc,LPCTSTR lpflag,LPCTSTR &p1,LPCTSTR &p2,LPCTSTR lpHeader,size_t nAllSize);
	//��<��ʼ����
	virtual LPCTSTR _deleteFlagPair(LPCTSTR pc,LPCTSTR lpHeader,size_t nAllSize);
public:
	CLHttpSpider();	
	virtual ~CLHttpSpider() {}
/* 
lpRem��ʽ������
<html><body><div>{2}<div>{4}<div>{3}<div>{1,2}<div><table><tr>{1-50,1}<td>{1-5,1}[_value_]
*/
	CLHttpSpiderR set(LPCTSTR lpOrg,LPCTSTR lpRem);
	CLHttpSpiderR setRem(LPCTSTR lpRem);
	CLHttpSpiderR setOrg(LPCTSTR lpOrg);
	//��ȡ���ݵ�������bClearSetsBeforeExtractָ���ڳ�ȡǰ�Ƿ��������
	virtual std::vector<CLString>& extract(std::vector<CLString>& resultSet,BOOL bClearSetsBeforeExtract = TRUE);
};

#endif