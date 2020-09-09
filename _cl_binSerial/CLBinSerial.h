#pragma once

#ifndef __CL_BINSERIAL_H__
#define __CL_BINSERIAL_H__

//#include "../../_cl_extension/_cl_extension.h"
#include "windows.h"
#include "tchar.h"
#include "assert.h"
#include "fstream"
using namespace std;
class CLBinSerial;
typedef CLBinSerial *PCLBinSerial;
typedef const CLBinSerial *PCCLBinSerial;
typedef CLBinSerial& CLBinSerialR;
typedef const CLBinSerial& CLBinSerialRC;

class CLBinSerial {
private:
	HANDLE m_hFile;
	long long m_SizeInBytes;
	long long m_readPointer;
	TCHAR m_fileName[MAX_PATH];
	BOOL m_change;
	long long m_counts;
	byte m_attachType;
	inline BOOL checkFile()
	{
		return (m_hFile) ? TRUE : FALSE;
	}
	inline long long movePointer(long long index)//�ƶ�ָ�뵽ĳλ�ã�index����0����������Ϊ��λ
	{
		return SetFilePointer(m_hFile, (long)index*m_SizeInBytes, 0, FILE_BEGIN);
	}
	inline long long getPinter()//ȡ��ָ�룬index����0����������Ϊ��λ
	{
		return SetFilePointer(m_hFile, 0, 0, FILE_CURRENT) / m_SizeInBytes;
	}

public:
	//���캯��������ָ���ļ��������������ÿһ�����ݵ����ݽṹ��Byte�ƵĴ�С��
	CLBinSerial(long long nDataItemSizeInByte);

	virtual ~CLBinSerial();
	//�ر�ԭ���ӣ��Զ�д��ʽ�����ļ����ļ������ھʹ����ļ���
	//���ļ�������·�������ڣ������Զ��ȴ���·����������·��ʧ�ܣ��򲻻���������ļ���
	CLBinSerialR attach(LPCTSTR fileNameToAttach);
	//�ر�ԭ���ӣ���ֻ��ģʽ�����ļ����ļ�������������ʧ�ܡ�
	CLBinSerialR attachToRead(LPCTSTR fileNameToAttach);
	//�Ƿ�����ļ�������Ϊ�ļ��������buf
	BOOL isAttach(LPTSTR attachFileNameBuf = NULL, long long bufSizeInTChar = 0);
	//�Ƿ�ɶ�
	BOOL isRead() const;
	//�Ƿ�ɶ�д
	BOOL isWrite() const;
	//�ر����ӵ��ļ���������û��������ʲôҲ������
	CLBinSerialR detach();
	//��дģʽ�£���������ļ����ݡ�������û��������ʲôҲ������
	CLBinSerialR clearFile();
	//��дģʽ�£����������ݵ������ļ���β��������ı��ȡָ���λ�á�
	CLBinSerialR append(LPCVOID lpData);
	//����ĳλ�õ����ݡ�
	CLBinSerialR set(LPCVOID lpData, long long index);
	//��дģʽ�£����������ݵ������ļ���β��������ı��ȡָ���λ�á�
	CLBinSerialR operator << (LPCVOID lpData);
	CLBinSerialR operator << (byte& data);
	//��ȡ���ݵ�ָ��buf�С��ƶ���ȡָ�뵽��һ������λ�á�
	CLBinSerialR operator >> (LPVOID lpOutputDataBuf);
	//�����ļ��󶨵���Ŀ������û�󶨷���0��
	long long getCounts();
	//�ƶ���ȡָ�뵽ָ��������λ�ã�����ԭλ�õ����������г������������ָ���ƶ���ĩβ��
	long long moveReadPionter(long long newIndex);
	//��ȡ���ݵ�ָ��buf�С��ƶ���ȡָ�뵽��һ������λ�á�
	BOOL getItem(LPVOID lpOutputDataBuf);
	//��ȡ���ݵ�ָ��buf�С�����ı��ȡָ���λ�á�
	//ƥ������Ϊ[varStart,varEnd]֮ǰ��ֵ
	//varPosInByte��ʾ�Ƚ������ڽṹ�е�λ��������byteΪ��λ��ƫ��
	template<typename T>
	long long getItemCmp(LPVOID lpOutputData, T varStart, T varEnd, DWORD varPosInByte = 0)
	{
		assert(lpOutputData != NULL);
		if (!checkFile()) {
			ZeroMemory(lpOutputData, m_SizeInBytes);
			return 0;
		}
		T _var = 0; BOOL isFind = FALSE; DWORD nRead = 0;
		long long ret = 0;
		SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);
		while (::ReadFile(m_hFile, lpOutputData, m_SizeInBytes, &nRead, NULL) && (nRead == m_SizeInBytes)) {
			ret++;
			_var = *((T*)((LPBYTE)lpOutputData + varPosInByte));
			if (BETWEEN(_var, varStart, varEnd)) {
				isFind = TRUE; break;
			}
		}
		movePointer(m_readPointer);
		if (isFind)
			return ret;
		else {
			ZeroMemory(lpOutputData, m_SizeInBytes);
			return 0;
		}
	}

	//��ȡ���ݵ�ָ��buf�С�����ı��ȡָ���λ�á�
	//indexָ����ȡ�ĵڼ�����������������0Ϊ������
	BOOL getItemAt(LPVOID lpOutputDataBuf, long long index);
};
#define TIME_STAMP_POS  0
#define TIME_STAMP_SIZE 8


class CLMemBinSerial{
public:
	CLMemBinSerial() { init(); }
	CLMemBinSerial(LPCVOID _lpMen, size_t _memBufSize) { setMem(_lpMen, _memBufSize); }
	CLMemBinSerial(LPCVOID _lpMen) { setMem(_lpMen, 0); }
	CLMemBinSerial& setMem(LPCVOID _lpMen, size_t _memBufSize) {		
		assert(_lpMen != NULL);
		init();
		memcpy_s(&lpMen, sizeof(LPBYTE), &_lpMen, sizeof(LPCVOID));
		lpMenC = lpMen;
		m_memBufSize = _memBufSize;
		return *this;
	}
	virtual ~CLMemBinSerial() { init(); }
	inline size_t getOperateCounts() const { return m_operateCounts; }
	inline LPVOID getMemBuf() const { return lpMen; }
	//��ָ�����õ�buf��ʼλ��
	inline CLMemBinSerial& repeat() { return  getMemBuf() == 0 ? *this : setMem(getMemBuf(), getMemBufSize()); }
	template <typename ClassTag> CLMemBinSerial& append(const ClassTag* lpTag, size_t nCounts = 1) {
		if (getMemBufSize() == 0)
			throw std::logic_error("Mem Bin Serial error: The buf writed size is 0!");
		if (lpTag == 0)return *this;
		if (nCounts == 0)nCounts = 1;
		if (getMemBufSize() <= getOperateCounts())
			throw std::logic_error("Mem Bin Serial error: The buf writed size is not long enough!");
		size_t nSi = nCounts * sizeof(ClassTag);
		memcpy_s(lpMenC, getMemBufSize() - getOperateCounts(), lpTag, nSi);
		m_operateCounts += nSi;
		lpMenC += nSi;
		return *this;
	}
	template <typename ClassTag> inline CLMemBinSerial& append(const ClassTag& tag, size_t nCounts = 1) { return append(&tag, nCounts); }
	template <typename ClassTag> inline CLMemBinSerial& operator << (const ClassTag& tag) { return append(&tag, 1); }
	template <typename ClassTag> inline CLMemBinSerial& operator << (const ClassTag* lpTag) { return append(lpTag, 1); }
	inline CLMemBinSerial& operator << (const CHAR* lpString) { return append(lpString); }
	inline CLMemBinSerial& operator << (const WCHAR* lpString) { return append(lpString); }
	inline CLMemBinSerial& append(const CHAR* lpString) { return append(lpString, lpString != 0 ? (strlen(lpString) + 1) : 0); }
	inline CLMemBinSerial& append(const WCHAR* lpString) { return append(lpString, lpString != 0 ? (wcslen(lpString) + 1) : 0); }
	template <typename ClassTag> CLMemBinSerial& putout(ClassTag* lpTag, size_t nCounts = 1) {
		if (lpTag == 0)return *this;
		if (nCounts == 0)nCounts = 1;
		size_t nSi = nCounts * sizeof(ClassTag);
		memcpy_s(lpTag, nSi, lpMenC, nSi);
		m_operateCounts += nSi;
		lpMenC += nSi;
		return *this;
	}
	template <typename ClassTag> inline CLMemBinSerial& putout(ClassTag& tag, size_t nCounts = 1) { return putout(&tag, nCounts); }
	template <typename ClassTag> inline CLMemBinSerial& operator >>(ClassTag& tag) { return putout(&tag, 1); }
	template <typename ClassTag> inline CLMemBinSerial& operator >>(ClassTag* lpTag) { return putout(lpTag, 1); }
	//��ȷ��д��ռ��㹻�󣬺����������Խ����
	inline CLMemBinSerial& operator >> (CHAR* lpString) { return putout(lpString); }
	inline CLMemBinSerial& operator >> (WCHAR* lpString) { return putout(lpString); }
	inline CLMemBinSerial& putout(CHAR* lpString) { return putout(lpString, (strlen((const CHAR*)lpMenC) + 1)); }
	inline CLMemBinSerial& putout(WCHAR* lpString) { return putout(lpString, (wcslen((const WCHAR*)lpMenC) + 1)); }
protected:
	inline void init() {
		lpMenC = lpMen = 0;
		m_memBufSize = 0;
		m_operateCounts = 0;
	}
	inline size_t getMemBufSize() const { return m_memBufSize; }
private:
	LPBYTE lpMen, lpMenC;
	size_t m_memBufSize;
	size_t m_operateCounts;
};

//�������ļ�����
class CLFileBinSerial:public fstream {
protected:
	unsigned char* pBuf = NULL;
	size_t bufSize = 0;
	CLFileBinSerial(const CLFileBinSerial&) = delete;
	CLFileBinSerial& operator= (const CLFileBinSerial&) = delete;
public:
	CLFileBinSerial(){}
	CLFileBinSerial(LPCTSTR file,int mode)
		:fstream(file,mode | ios::binary){
		seekg(ios::beg);
	}
	CLFileBinSerial(LPCTSTR file,bool isWrite = false) 
		:fstream(file, (isWrite ? (ios::app|ios::out|ios::binary):(ios::in | ios::binary))) {
		seekg(ios::beg);
	}
	bool open(LPCTSTR file, bool isWrite = false) {
		if (is_open())
			close();
		fstream::open(file, (isWrite ? (ios::app | ios::out | ios::binary) : (ios::in | ios::binary)));
		if (is_open()) {
			seekg(ios::beg);
			return true;
		}
		else return false;
	}
	virtual ~CLFileBinSerial() { 
		if (is_open())
			close();
		if (pBuf)
			delete[] pBuf, pBuf = NULL, bufSize = 0;
	}
	//˳�򽫶�������д�뵽�ļ����У����������������ݴ�С����¼����֤��ȡʱ���ݵ������ԣ��⽫��ʧ���ֿռ����ڼ�¼��������Ĵ�С��
	template<class T>
	CLFileBinSerial& operator << (const T& v) {
		size_t st = sizeof(T);
		if (st < 0xFF) {
			unsigned char si = (unsigned char)st;
			write((const char*)& si, 1);
			write((const char*)& v, si);
		}
		else {
			const unsigned char si = 0xFF;
			write((const char*)& si, 1);
			write((const char*)& st, sizeof(size_t));
			write((const char*)& v, st);
		}
		return *this;
	}
	//˳���ļ�����������������ڴ��У�������Լ�¼�����ݴ�С����ȡ���뱣������֤��ȡ���������ԣ��⽫��ʧ���ֿռ����ڼ�¼��������Ĵ�С��
	template<class T>
	CLFileBinSerial& operator >> (T& v) {
		ZeroMemory(&v, sizeof(T));
		unsigned char si;
		read((char*)& si, 1);	
		if (si == 0x5) {
			float ff;
			read((char*)&ff, sizeof(float));
			//v = ff;
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, &ff, stt);
		}else if (si == 0x6) {
			double db;
			read((char*)& db, sizeof(double));
			//v = db;
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, &db, stt);
		}else if(si == 0x7) {
			long double db2;
			read((char*)& db2, sizeof(long double));
			//v = db2;
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, &db2, stt);
		}else if (si == 0xFF) {
			size_t st;
			read((char*)& st, sizeof(size_t));
			if (pBuf == NULL) {
				pBuf = new unsigned char[bufSize = st];
			}
			else if (bufSize < st) {
				delete[] pBuf;
				pBuf = new unsigned char[bufSize = st];
			}
			read((char*)pBuf, st);
			size_t stt = min(sizeof(T), st);
			memcpy_s(&v, stt, pBuf, stt);
		}
		else {
			//v = 0;
			char buf[256];
			read(buf, si);
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, buf, stt);
		}
		return *this;
	}

	CLFileBinSerial& operator << (const float& v) {
		unsigned char si = 0x5;
		write((const char*)& si, 1);
		write((const char*)& v, sizeof(float));
		return *this;
	}
	CLFileBinSerial& operator << (const double& v) {
		unsigned char si = 0x6;
		write((const char*)& si, 1);
		write((const char*)& v, sizeof(double));
		return *this;
	}
	CLFileBinSerial& operator << (const long double& v) {
		unsigned char si = 0x7;
		write((const char*)& si, 1);
		write((const char*)& v, sizeof(long double));
		return *this;
	}
	CLFileBinSerial& operator << (const char* lpString) {
		size_t si = strlen(lpString) + 1;
		write((const char*)& si, sizeof(size_t));
		write(lpString, sizeof(char) * (si));
		return *this;
	}
	CLFileBinSerial& operator << (const wchar_t* lpString) {
		size_t si = wcslen(lpString) + 1;
		write((const char*)& si, sizeof(size_t));
		write((const char*)lpString, sizeof(wchar_t) * (si));
		return *this;
	}
	CLFileBinSerial& operator >> (char*& lpString) {
		size_t si = 0;
		read((char*)& si, sizeof(size_t));
		read(lpString, sizeof(char) * (si));
		return *this;
	}
	CLFileBinSerial& operator >> (wchar_t*& lpString) {
		size_t si = 0;
		read((char*)& si, sizeof(size_t));
		read((char*)lpString, sizeof(wchar_t) * (si));
		return *this;
	}
};

#endif