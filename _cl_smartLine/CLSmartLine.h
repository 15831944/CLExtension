#pragma once
#ifndef __CL_SMARTLINE_H__
#define __CL_SMARTLINE_H__

#include "windows.h"
#include "shlobj.h"
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <queue>
#include <assert.h>
#include <conio.h>
#include <iOS>
#include <TlHelp32.h>    
#include <atlstr.h>    
#include <locale.h> 
#include <map>
#include "../_cl_common/CLCommon.h"

#define SL_TYPE_NO       (0x0)  //ģʽ���ͣ����л����������ߣ���������չ���� 
#define SL_TYPE_DIFF     (0x1)  //ģʽ���ͣ����� �ƶ���ֵ   ����  
#define SL_TYPE_AVER     (0x2)  //ģʽ���ͣ����� �ƶ�ƽ��ֵ ���� 
//#define SL_TYPE_ALL      (SL_TYPE_AVER|SL_TYPE_DIFF)  //ģʽ���ͣ����� �ƶ�ƽ��ֵ �� �ƶ���ֵ ����   
#define SL_TYPE_VARI     (0x4|SL_TYPE_AVER)  //ģʽ���ͣ����� ���� ����  
#define SL_TYPE_STDD     (0x8|SL_TYPE_VARI)  //ģʽ���ͣ����� ��׼�� ����  
#define SL_TYPE_NOCYCLE  (0x10)  //ģʽ��չ���ͣ����������������� ���ò�ѭ��ģʽ������ģʽ������ģʽҪƽ���ƶ��������ݣ��ǳ�����cpu����������   
class CLSmartLine{
public:
	using valueType = double;
	using sizeType = long;
	using flagType = int;
protected:
	/*valueType* lst;
	valueType* avr;
	valueType* dif;
	valueType* var;
	valueType* stdd;*/
	vector<valueType> lst;
	vector<valueType> avr;
	vector<valueType> dif;
	vector<valueType> var;
	vector<valueType> stdd;
	flagType flag;
	sizeType nMax;
	sizeType ci;
	sizeType divN;
	valueType total;
	valueType defV;
	void _init();
	void _release();
	void _construct(sizeType nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO,valueType _defV = 0);
#ifdef _DEBUG
	//std::vector<VT> lstCheck;
#endif
public:
	CLSmartLine();
	CLSmartLine(sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO,valueType _defV = 0);
	//CLSmartLine(const CLSmartLine& tag);
	CLSmartLine(const valueType* _pv,sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	CLSmartLine(const std::vector<valueType>& _pv,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	void construct(sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO,valueType _defV = 0);
	void construct(const valueType* _pv,sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	void construct(const std::vector<valueType>& _pv,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	virtual ~CLSmartLine();	
	void reset();//����
	void release();//�ͷ��ڴ沢����
	valueType& operator [](sizeType i);//���ص��ǻ����������ݣ�������չ����
	valueType& at(sizeType i){ return (*this)[i];}
	//ȡ����Ч��ȫ��������
	sizeType index(sizeType offset = 0){  
		if(nMax <= 0 )goto err;
		if(flag & SL_TYPE_NOCYCLE){
			sizeType i = (ci + offset);
			return ( (i > (nMax - 1)) ? (nMax - 1) : ( (i < 0) ? (nMax + i%nMax) : i ) );
		}
		else {sizeType i = (ci+offset)%nMax;return (i >= 0) ? i:(nMax + i);}
err:
		::MessageBox(NULL,_T("MOVEAVERAGE�ࡰindex�������Ƿ�������û�й��� ��"),_T("MOVEAVERAGE Alert"),MB_ICONERROR);
		throw std::logic_error("MOVEAVERAGE�ࡰindex�������Ƿ�������û�й��� ��");
		return -1;
	};
	//����һ�����ݵ������У����������ͼ�����չ����
	sizeType push(valueType v);
	//���������ƶ��ܺ�ֵ
	inline valueType sum() const { return total; }

	////��������ƽ��ֵ
	//inline valueType aver() const {return (avr) ? avr[ci] : 0;}		
	////���ػ����������������б�û�й����ʼ������¿���ΪNULL
	//inline const valueType* vlst() const {return lst;}
	////�����ƶ�ƽ��ֵ���������б�����ΪNULL
	//inline const valueType* avrLst() const {return avr;}
	////�����ƶ���ֵ���������б�����ΪNULL
	//inline const valueType* difLst() const {return dif;}
	////�����ƶ��������������б�����ΪNULL
	//inline const valueType* varLst() const {return var;}
	////�����ƶ���׼�����������б�����ΪNULL
	//inline const valueType* stddLst() const {return stdd;}
	////��������ֵ
	//inline valueType cv() const {return (lst)? lst[ci] : 0; } 
	////�������²�ֵ
	//inline valueType diff() const {return (dif) ? dif[ci] : 0;}
	////�������·���ֵ
	//inline valueType vari() const {return (var) ? var[ci] : 0;}
	////�������±�׼��ֵ
	//inline valueType stddev() const {return (stdd) ? stdd[ci] : 0;}

	
	//���ػ����������������б�û�й����ʼ������¿���ΪNULL
	inline const valueType* vlst() const { return lst.data();}
	//�����ƶ�ƽ��ֵ���������б�����ΪNULL
	inline const valueType* avrLst() const { return avr.data(); }
	//�����ƶ���ֵ���������б�����ΪNULL
	inline const valueType* difLst() const { return dif.data(); }
	//�����ƶ��������������б�����ΪNULL
	inline const valueType* varLst() const { return var.data(); }
	//�����ƶ���׼�����������б�����ΪNULL
	inline const valueType* stddLst() const { return stdd.data(); }
	//��������ֵ
	inline valueType cv() const { return lst[ci]; }
	//��������ƽ��ֵ
	inline valueType aver() const { return avr[ci]; }
	//�������²�ֵ
	inline valueType diff() const { return dif[ci]; }
	//�������·���ֵ
	inline valueType vari() const { return var[ci]; }
	//�������±�׼��ֵ
	inline valueType stddev() const { return stdd[ci]; }

	//��������������ݸ���
	inline sizeType maxCounts() const {return nMax;}
	//���ػ������ߵ�Ĭ�����ֵ
	inline valueType defaultValue() const {return defV;}
	//���ض������ͱ�ǩ��SL_TYPE_NO SL_TYPE_AVER SL_TYPE_DIFF SL_TYPE_ALL SL_TYPE_NOCYCLE һ�ֻ������ϣ�
	inline sizeType typeFlag() const {return flag;}
	//�����ƶ�ƽ��ģʽ�µĿ��
	inline sizeType moveStep() const {return divN;}
	//�����ƶ�ƽ��ģʽ���ڵ�ǰindex֮�������ȫ����0��������б�
	const valueType* zeroAvrLstEnd();
	//�����ƶ���ֵģʽ���ڵ�ǰindex֮�������ȫ����0��������б�
	const valueType* zeroDifLstEnd();
	const valueType* zeroVarLstEnd();
	const valueType* zeroStddLstEnd();
	
	void dumpVlst()const;
	void dumpAvrLst()const;
	void dumpDifLst()const;
	void dumpVariLst()const;
	void dumpStddLst()const;
	void dumpAllLst()const;

	//���ڲ���������ȫ����nMax�ߴ練�򡣺���ʹ�ú�push���������ң�һ��ֻ���ڲ����������ִ�и÷�����
	CLSmartLine& reverse();
};
typedef CLSmartLine &CLSmartLineR,*PCLMoveAverage;
typedef const CLSmartLineR CLSmartLineRC;
typedef const PCLMoveAverage PCCLMoveAverage;

#endif