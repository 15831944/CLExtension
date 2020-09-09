#pragma once
#ifndef __CL_ARRAYTEMPLATE_H__
#define __CL_ARRAYTEMPLATE_H__

#include "windows.h"
#include <vector>
#include <stack>

using namespace std;

#ifndef __CL_VALUEPACK__
#define __CL_VALUEPACK__
//������ֵ���͵Ķ��󻯷�װ��ģ��
template<class T>
struct CLValuePacking {	
	using value_type = T;
	using ref = value_type&;
	using refc = const value_type&;
	value_type v;
	CLValuePacking() {}
	CLValuePacking(value_type _v): v (_v){}
	template<class T2>
	CLValuePacking(const CLValuePacking<T2>& _v) : v(_v) {}
	operator value_type& () { return v; }
	template<class T2>
	operator T2 () const { return T2(v); }
	value_type operator%(int v2) const { return int(v) % v2; }
	ref operator%=(int v2) { return v = int(v) % v2; }
	template<class T2>
	ref operator=(T2 v2) { return v = value_type(v2); }
	//����
	ref operator--() {
		return --v;
	}
	ref operator++() {
		return ++v;
	}
	value_type operator--(int) {
		return v--;
	}
	value_type operator++(int) {
		return v++;
	}
};
template<class T1, class T2>
T1 operator%(T1 v, const CLValuePacking<T2>& t) {
	return int(v) % int(t);
}


//ʵ���� ������ֵ�Ͱ�װ�� ģ����
#define CLVPack(valueClass,definedClassName)\
template class CLValuePacking<valueClass>;\
typedef CLValuePacking<valueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

#endif

//�������� ��ά��ȫ������ ģ���ࣨע�⣺������Ӧ�����ƴ������ֵ�򿽱�������ڲ���ָ̬�������Ӱ�죬������������ͷţ�
template <class T>
class CLArrayTemplate {
public:
	using obj = CLArrayTemplate<T>;
	using pObj = CLArrayTemplate<T>*;
	using pObjc = const CLArrayTemplate<T>*;
	using ref = CLArrayTemplate<T> &;
	using refc = const CLArrayTemplate<T> &;
	using vlist = initializer_list<T>;
	using data = vector<CLArrayTemplate<T>*>;
	using pdata = vector<CLArrayTemplate<T>*>*;
protected:
	class VPackage {//��װ��
	public:
		T* m_value;	//�ڵ�ֵ����
		inline VPackage()
			:m_value(new T)
		{
		}
		inline ~VPackage() {
			if (m_value != 0) {
				delete m_value;
				m_value = 0;
			}
		}
	};
	VPackage m_vPack;
	pdata m_sublst;//�ڵ���Ӷ����
	byte m_createByLst;
	stack<pObj> objLibrary;//mem pool
	void _release() {
		//�ͷ��ڴ��
		_releaseLib();
		//�ͷű���������
		if (m_sublst != NULL) {
			for (size_t i = 0, si = m_sublst->size(); i < si; i++) {
				auto pc = m_sublst->at(i);
				if (pc != 0) {
					_deleteOne(pc);
				}
			}
			delete m_sublst;
			m_sublst = NULL;
		}
	};
	void _releaseLib() {
		while (!objLibrary.empty()) //�ͷſ�
		{
			pObj pc = objLibrary.top();
			objLibrary.pop();
			_deleteOne(pc);
		}
	};
	virtual pObj _newObj() {
		pObj pnew;
		if (!objLibrary.empty()) {
			pnew = objLibrary.top();
			objLibrary.pop();
			pnew->m_vPack.VPackage::VPackage();
			pnew->m_createByLst = false;
			return pnew;
		}
		else {
			pnew = (pObj)malloc(sizeof(obj));
			pnew->CLArrayTemplate::CLArrayTemplate();
			return pnew;
		}
	}
	virtual void _storeOne(void* p) {
		((pObj)p)->clear();//��������
		((pObj)p)->m_vPack.VPackage::~VPackage();//��������
		objLibrary.push((pObj)p);
	}
	virtual void _deleteOne(void* p) {
		((pObj)p)->CLArrayTemplate::~CLArrayTemplate();
		free(p);
	}
	inline size_t _getObjSizeof() const {
		return  _getClassSizeof() + (m_vPack.m_value ? sizeof(T) : 0);
	}
	inline virtual size_t _getClassSizeof() const {
		return sizeof(obj);
	}
	inline void* _newData() { return new data; }
	void _copyList(const pdata& tag_sublst) {
		size_t lsi = (tag_sublst != 0 ? tag_sublst->size() : 0);
		if (lsi > 0) {
			if (!m_sublst)
				m_sublst = (pdata)_newData();
			size_t si = m_sublst->size();
			if (si < lsi) {
				m_sublst->resize(lsi, (pObj)0);
			}
			for (size_t i = 0; i < lsi; i++)
			{
				auto& p = m_sublst->at(i);
				auto& pt = tag_sublst->at(i);
				if (pt != 0) {
					if (p == 0)
						p = _newObj();
					*(p->m_vPack.m_value) = *(pt->m_vPack.m_value);
				}
			}
		}
	}
	void _clear() {
		if (m_sublst != NULL) {
			for (size_t i = 0, si = m_sublst->size(); i < si; i++) {
				auto& pc = m_sublst->at(i);
				if (pc != 0) {
					_storeOne(pc);
				}
			}
			m_sublst->clear();
		}
	};
public:	
	//ȡ�ö����ڴ���ռ�ô�С,������ʹ�õ��ڴ���ѻ������ڴ���б��õ��ڴ�
	size_t getMemerySize() const {
		size_t sii = getUsedMemerySize();
		sii += getSpareMemerySize();
		return sii;
	}
	//�ͷŽṹ����ռ���ڴ�,������ʹ�õ��ڴ���ѻ������ڴ���б��õ��ڴ�
	void releaseMemery() {
		_release();
	}
	//ȡ�������ڴ���б����ı����ڴ��ռ�ô�С������������ʹ�õ��ڴ棩
	size_t getSpareMemerySize() const {
		size_t sii = 0;
		stack<pObj> stk2 = objLibrary;
		while (!stk2.empty())
		{
			sii += (stk2.top()->getMemerySize());
			stk2.pop();
		}
		return sii;
	}
	//�ͷ������ڴ���б����ı����ڴ��ռ�ô�С������������ʹ�õ��ڴ棩
	void releaseSpareMemery() {
		_releaseLib();
	}
	//ȡ�ö�����ʹ�õ��ڴ�ռ�ô�С�������������ڴ���б��õ��ڴ棩
	size_t getUsedMemerySize()const {
		size_t sii = _getObjSizeof();
		if (m_sublst != NULL) {
			sii += (m_sublst->capacity() * sizeof(pObj));
			for (size_t i = 0, si = m_sublst->size(); i < si; ++i)
			{
				pObj p = (m_sublst->at(i));
				if (p)
					sii += p->getMemerySize();
			}
		}
		return sii;
	}
	inline CLArrayTemplate()
		:m_sublst(0), m_createByLst(false) {
	}
	template<class otherT>
	inline CLArrayTemplate(otherT v)
		: m_sublst(0), m_createByLst(false) {
		*m_vPack.m_value = v;
	}
	inline CLArrayTemplate(refc tag)
		: m_sublst(0), m_createByLst(false) {
		*this = tag;
	}
	inline CLArrayTemplate(const vlist& _Ilist)
		: m_sublst(0), m_createByLst(true) {
		*this = _Ilist;
	}
	//�������� �ͷ����ж����ӽڵ���ڴ�
	inline virtual ~CLArrayTemplate() {
		_release();
	}
	inline ref operator =(T v) {
		return (m_createByLst = false, *m_vPack.m_value = v), * this;
	}
	ref operator =(refc tag) {
		if (!tag.m_createByLst)
			m_createByLst = false, * m_vPack.m_value = (*tag.m_vPack.m_value);
		_copyList(tag.m_sublst);
		return *this;
	}
	ref operator =(const vlist& _Ilist) {
		size_t lsi = _Ilist.size();
		if (lsi > 0) {
			if (!m_sublst)
				m_sublst = (pdata)_newData();
			auto si = m_sublst->size();
			if (si < lsi) {
				m_sublst->resize(lsi, (pObj)0);
			}
			for (size_t i = 0; i < lsi; i++)
			{
				auto& p = m_sublst->at(i);
				if (p == 0)
					p = _newObj();
				*(p->m_vPack.m_value) = *(_Ilist.begin() + i);
			}
		}
		return *this;
	}
	//�������ֱ�Ӳ���i��ʵ�������Ա����at��[]����ʹ�ã���δ������õ�Ԫ֮ǰ��
	//�����ͻὫ����׼����������̬�����ڴ棩����������ı����еĶ���
	void generate(int i) {
		size_t _i = (i < 0 ? 0 : i);
		if (!m_sublst)m_sublst = new data;
		if (m_sublst->size() >= _i) {
			for (size_t i = 0; i < _i; i++)
			{
				auto& pc = m_sublst->at(i);
				if (pc == 0)
					pc = _newObj();
			}
			return;
		}
		m_sublst->resize(_i, (pObj)0);
		for (size_t i = 0; i < _i; i++)
		{
			auto& pc = m_sublst->at(i);
			if (pc == 0)
				pc = _newObj();
		}
		return;
	}
	//�ͻ�ɾ�������������󣬱���ָ���ĸ���,�����ʼ�����еĶ���
	void resize(int i) {
		size_t _i = (i < 0 ? 0 : i);
		if (!m_sublst)
			m_sublst = new data;
		size_t si = m_sublst->size();
		if (si > _i) {
			for (size_t i = si; i > _i; --i)
			{
				auto& pc = m_sublst->at(i - 1);
				if (pc != 0) {
					_storeOne(pc);
					pc = 0;
				}
			}
			m_sublst->resize(_i);
			return;
		}
		else if (si < _i)
			m_sublst->resize(_i, 0);
		return;
	}

	//�����ڲ�ֵ������ 
	inline T& operator()() { return *m_vPack.m_value; }

	//���������������������������reset();
	inline void clear() {
		_clear();
	}
	//����ʹ�ö��������һ��������һ����ʹ������ѧ����,��ֵ����ת��
	inline operator T() const {
		return *m_vPack.m_value;
	}
	//����ʹ�ö��������һ��������һ����ʹ������ѧ����,�ɱ�����ת��
	inline operator T& () {
		return *m_vPack.m_value;
	}
	//ȡ�����ĵ�i������i<0�Զ�ȡ0��i>�������ʱ�Զ����ݣ����i��Զ�������Խ�����
	ref at(size_t _i) {
		if (!m_sublst)
			m_sublst = new data;
		if (m_sublst->size() < _i + 1) {
			m_sublst->resize(_i + 1, (pObj)0);
			auto& pc = m_sublst->at(_i);
			pc = _newObj();
			return *pc;
		}
		auto& pc = m_sublst->at(_i);
		if (pc == 0)
			pc = _newObj();
		return *pc;
	}
	template<class Ti>
	inline ref at(const Ti& i) {
		return at(i < 0 ? 0 : size_t(i));
	}
	//ȡ�����ĵ�i������i<0�Զ�ȡ0��i>�������ʱ�Զ����ݣ����i��Զ�������Խ�����
	inline ref operator[](size_t i) { return at(i); }
	template<class Ti>
	inline ref operator[](const Ti& i) { return at(i); }
	//ȡ�����ĵ�Ԫ��
	inline size_t size() const {
		return m_sublst != NULL ? m_sublst->size() : 0;
	}
	//������ĩβ����Ԫ��
	inline ref push_back(T v) {
		return at(size()) = v, *this;
	}
};

//ʵ���� �������͵� ��ά��ȫ���� ģ���ࣨע�⣺������Ӧ�����ƴ������ֵ�򿽱�������ڲ���ָ̬�������Ӱ�죬������������ͷţ�
#define CLArrayObj(objClass,definedClassName)\
template class CLArrayTemplate<objClass>;\
typedef CLArrayTemplate<objClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

//������ֵ���� ��ά��ȫ������ ģ����
template<class T>
class CLArrayTemplateV :public CLArrayTemplate<CLValuePacking<T>> {
public:
	using obj = CLArrayTemplateV<T>;
	using obj_base = CLArrayTemplate<CLValuePacking<T>>;
	using pObj = CLArrayTemplateV<T>*;
	using pObj_base = CLArrayTemplate<CLValuePacking<T>>*;
	using ref_base = CLArrayTemplate<CLValuePacking<T>> &;
	using refc_base = const CLArrayTemplate<CLValuePacking<T>> &;
	using ref = CLArrayTemplateV<T> &;
	using refc = const CLArrayTemplateV<T> &;
	using vlist = initializer_list<CLValuePacking<T>>;
protected:
	virtual pObj _newObj() {
		pObj pnew;
		if (!obj_base::objLibrary.empty()) {
			pnew = (pObj)obj_base::objLibrary.top();
			obj_base::objLibrary.pop();
			pnew->clear();//��������
			pnew->reset();//���ö���
			return pnew;
		}
		else {
			pnew = (pObj)malloc(sizeof(obj));
			pnew->CLArrayTemplateV::CLArrayTemplateV();
			return pnew;
		}
	}
	virtual void _storeOne(void* p) {
		obj_base::objLibrary.push((pObj)p);
	}
	virtual void _deleteOne(void* p) {
		((pObj)p)->CLArrayTemplateV::~CLArrayTemplateV();
		free(p);
	}
	inline virtual size_t _getClassSizeof() const {
		return sizeof(obj);
	}
public:
	//����ֵ�Ƿ���ڣ�����������1��ʼ��û�ҵ�����0��startIndex��ʾ��������ʼ������0��ʼ��
	size_t find(const T& tag, size_t startIndex = 0) {
		for (size_t i = startIndex, si = obj_base::size(); i < si; i++)
		{
			auto& pc = obj_base::m_sublst->at(i);
			if (pc)
				if ((*(pc))() == tag)
					return i + 1;
		}
		return 0;
	}
	inline void reset() {
		obj_base::m_createByLst = false;
		(*obj_base::m_vPack.m_value) = 0;
	}
	//���캯�� ����ʼ��ֵΪ
	inline CLArrayTemplateV()
	{
		(*obj_base::m_vPack.m_value).v = 0;
	}
	template<class otherT>
	inline CLArrayTemplateV(otherT v)
	{
		(*obj_base::m_vPack.m_value).v = v;
	}
	CLArrayTemplateV(refc tag)
	{
		if (tag.m_createByLst)
			obj_base::m_createByLst = true, (*obj_base::m_vPack.m_value).v = 0;
		else
			(*obj_base::m_vPack.m_value).v = (*tag.m_vPack.m_value).v;
		obj_base::_copyList(tag.m_sublst);
	}
	CLArrayTemplateV(const vlist& _Ilist)
	{
		obj_base::operator = (_Ilist);
		obj_base::m_createByLst = true;
		(*obj_base::m_vPack.m_value).v = 0;
	}
	inline virtual ~CLArrayTemplateV() {
		obj_base::_release();
	}
	template<class otherT>
	inline ref operator =(otherT value) {
		return (obj_base::m_createByLst = false, (*obj_base::m_vPack.m_value).v = T(value)), * this;
	}
	//inline ref operator =(T value) {return (obj_base::m_createByLst = false, (*obj_base::m_vPack.m_value).v = value), * this;}	
	ref operator =(refc tag) {
		if (tag.m_createByLst)
			obj_base::m_createByLst = true, (*obj_base::m_vPack.m_value).v = 0;
		else
			(*obj_base::m_vPack.m_value).v = (*tag.m_vPack.m_value).v;
		obj_base::_copyList(tag.m_sublst);
		return *this;
	}
	inline ref operator =(const vlist& _Ilist) {
		return obj_base::operator = (_Ilist), * this;
	}
	//����ʹ�ö��������һ��������һ����ʹ������ѧ����,��ֵ����ת��
	inline operator T() const {
		return (*obj_base::m_vPack.m_value).v;
	}
	//����ʹ�ö��������һ��������һ����ʹ������ѧ����,�ɱ�����ת��
	inline operator T& () {
		return (*obj_base::m_vPack.m_value).v;
	}
	inline ref at(size_t i) {
		return (ref) * &(obj_base::at(i));
	}
	template<class Ti>
	inline ref at(const Ti& i) {
		return (ref) * &(obj_base::at(i < 0 ? 0 : size_t(i)));
	}
	//ȡ�����ĵ�i������i<0�Զ�ȡ0��i>�������ʱ�Զ����ݣ����i��Զ�������Խ�����
	inline ref operator[](size_t i) { return (ref) * &(obj_base::at(i)); }
	template<class Ti>
	inline ref operator[](const Ti& i) { return (ref) * &(obj_base::at(i < 0 ? 0 : size_t(i))); }

	//����

	obj operator--(int) {
		obj a = *this;
		++((*obj_base::m_vPack.m_value).v);
		return a;
	}
	obj operator++(int) {
		obj a = *this;
		++((*obj_base::m_vPack.m_value).v);
		return a;
	}
	inline ref operator--() {
		return --(*obj_base::m_vPack.m_value).v, * this;
	}
	inline ref operator++() {
		return ++(*obj_base::m_vPack.m_value).v, * this;
	}
	inline int operator%(int v) {
		return (((int)((*obj_base::m_vPack.m_value).v)) % v);
	}
};

//ʵ���� ������ֵ���͵� ��ά��ȫ���� ģ����
#define CLArrayV(baseValueClass,definedClassName)\
template class CLArrayTemplateV<baseValueClass>;\
typedef CLArrayTemplateV<baseValueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

template<class T>
class CLSingleArrayV :public vector<T> {
public:
	using obj = CLSingleArrayV<T>;
	using pObj = CLSingleArrayV<T>*;
	using ref = CLSingleArrayV<T> &;
	using refc = const CLSingleArrayV<T> &;
	using vlist = initializer_list<T>;
protected:
	T m_value;
	void _init(const T& v) { m_value = v; }
public:
	inline ref reset() { return m_value = 0, *this; }
	size_t getMemerySize() const {
		return sizeof(CLSingleArrayV) + sizeof(T) * vector<T>::capacity();
	}
	inline CLSingleArrayV() { _init(0); }
	inline CLSingleArrayV(const T& v) { _init(v); }
	inline CLSingleArrayV(const vlist& list) {
		_init(0);
		vector<T>::operator=(list);
	}
	inline ~CLSingleArrayV() {}
	inline operator T() const {
		return m_value;
	}
	inline operator T& () {
		return m_value;
	}
	inline T& operator ()() {
		return m_value;
	}
	inline T& operator =(const T& v) {
		return m_value = v, m_value;
	}
	inline T& operator =(const vlist& list) {
		vector<T>::operator=(list);
		return m_value;
	}	
	inline T& operator--() {
		return --m_value, m_value;
	}
	inline T& operator++() {
		return ++m_value, m_value;
	}
	inline obj operator--(int) {
		obj a(m_value);
		--m_value;
		return a;
	}
	inline obj operator++(int) {
		obj a(m_value);
		++m_value;
		return a;
	}
	inline int operator%(int v) {
		return (((int)m_value) % v);
	}	
};

template<class T>
class CLSingleSafeArrayV :public CLSingleArrayV<T> {
public:
	inline CLSingleSafeArrayV() {}
	inline CLSingleSafeArrayV(const T& v)
		:CLSingleArrayV<T>(v){ }
	inline CLSingleSafeArrayV(const CLSingleArrayV<T>::vlist& list)
		: CLSingleArrayV<T>(list) {	}
	inline T& at(size_t i) {
		//size_t csi = vector<T>::size();
		if (i >= vector<T>::size()) {
			vector<T>::resize(i + 1, 0);
		}
		return vector<T>::operator[](i);
	}
	template<class Ti>
	inline T& at(const Ti& i) {
		return at(i < 0 ? size_t(0) : size_t(i));
	}
	inline T& operator[](size_t i) {
		return at(i);
	}
	template<class Ti>
	inline T& operator[](const Ti& i) {
		return at(i < 0 ? size_t(0) : size_t(i));
	}
};

//ʵ���� ������ֵ���͵� һά�ǰ�ȫ���� ģ����
#define CLArrayVN(baseValueClass,definedClassName)\
template class CLSingleArrayV<baseValueClass>;\
typedef CLSingleArrayV<baseValueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

//ʵ���� ������ֵ���͵� һά��ȫ���� ģ����
#define CLArrayVS(baseValueClass,definedClassName)\
template class CLSingleSafeArrayV<baseValueClass>;\
typedef CLSingleSafeArrayV<baseValueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

#endif