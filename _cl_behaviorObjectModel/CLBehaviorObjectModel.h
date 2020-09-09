#pragma once
#ifndef __CL_BEHAVOBJMDL_H__
#define __CL_BEHAVOBJMDL_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
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
#include "../_cl_matrix/CLMatrix.h"

using namespace std;

class CLBhvObjUnit;
typedef CLBhvObjUnit *PCLBhvObjUnit;
typedef const CLBhvObjUnit *PCCLBhvObjUnit;
typedef CLBhvObjUnit& CLBhvObjUnitR;
typedef const CLBhvObjUnit& CLBhvObjUnitRC;

typedef unsigned long long UID;
typedef long long COUNTS;
typedef double valueType;
typedef std::vector<valueType> VTV;
class CLBhvObjManager;

typedef CLMatrixT<double> CLMatrixBhv;

class CLBhvObjUnit{
	friend class CLBhvObjManager;
private:
	COUNTS thisBhvCounts;	
protected:
	UID uid;
	valueType linkPower;
	VTV inputVector;
	VTV outputVector;
	CLMatrixBhv transMatrix,midMatrixA,midMatrixB;
	std::vector<UID> subLst;
	CLBhvObjManager* pFather;

	CLBhvObjUnitR getSubUt(size_t index);
	CLBhvObjUnitR setFather(CLBhvObjManager* _pFather){pFather = _pFather;return *this;}
	CLBhvObjUnitR reset();
	inline CLBhvObjUnitR setUid(UID _uid){this->uid = _uid;return *this;};
public:
	CLBhvObjUnit();
	virtual ~CLBhvObjUnit();
	
	inline UID getUid() const{return uid;}//ȡ��id
	inline CLBhvObjUnitR setSingleTransMatrix(const CLMatrixBhv& _transMatrix){this->transMatrix = _transMatrix;return *this;}//����Ԫ���ӱ任����
	CLBhvObjUnitR trigger(const VTV& _inputVector);//������Ϊ
	//CLBhvObjUnitR learning(const VTV& _inputVector,const VTV& _outputVector);//ѧϰ����
	CLBhvObjUnitR learning();//ѧϰ����������������������úõ�����
	CLBhvObjUnitR dumpStruct(int itype = 0);//��ʾ��Ϊ�ڲ��ṹ
	inline BOOL isRegiste(){ return uid != 0 ? TRUE : FALSE; }//��Ϊ��Ԫ�Ƿ���ע��
	inline const VTV& getOutputVector(){ return outputVector; }//ȡ���������
	inline const VTV& getInputVector(){ return inputVector; }//ȡ����������
	CLBhvObjUnitR setInputVector(size_t nSi,valueType v1,...);//�����������������������1������������
	CLBhvObjUnitR setOutputVector(size_t nSi,valueType v1,...);//����������������������1������������
	inline CLBhvObjUnitR pushIV(){ midMatrixA.add_row(inputVector);return *this;}
	inline CLBhvObjUnitR pushOV(){ midMatrixB.add_row(outputVector);return *this;}	
	inline CLBhvObjUnitR clearMidMatrixA(){midMatrixA.clear();return *this;}
	inline CLBhvObjUnitR clearMidMatrixB(){midMatrixB.clear();return *this;}
    CLBhvObjUnitR registe();
	CLBhvObjUnitR unRegiste();
};

#define MAXBOU_SETTS 0xFFF
class CLBhvObjManager{
	friend class CLBhvObjUnit;
private:	
protected:
	size_t maxStroeSets;
	byte* pRemember;
	PCLBhvObjUnit pBOStore;
	int setNewUid(UID uid,BOOL setting = TRUE);
	BOOL registe(CLBhvObjUnitR obj);
	BOOL unRegiste(CLBhvObjUnitR obj);
public:
	CLBhvObjManager(size_t _maxStroeSets = MAXBOU_SETTS);
	virtual ~CLBhvObjManager();
	CLBhvObjManager resize(size_t _newMaxStroeSets);
	UID getNewUid();
	CLBhvObjUnitR getUt(UID uid);
};

#endif