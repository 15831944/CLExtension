#pragma once
#ifndef __CL_TIME_H__
#define __CL_TIME_H__

#include "windows.h"
#include <time.h>
#include "../_cl_common/CLCommon.h"
#include "../_cl_string/CLString.h"


//ʱ�����ݲ���������
#define CLTIMESTRINGLENGTH 35

#define CTT_CREATEBYNULL         0//�շ�ʽ����
#define CTT_CREATEBYLOCALTIME    1//local��ʽ����
#define CTT_CREATEBYUTCTIME      2//utc��ʽ����
#define CTT_CREATEBYTIMESTAMP    3//ʱ�����ʽ����

class CLTime;
typedef CLTime CLTT;
typedef CLTime* PCLTime;
typedef const PCLTime PCLCTime;
typedef CLTime& CLTimeR;
typedef const CLTimeR CLTimeRC;

typedef long long TIMESTAMP,*PTIMESTAMP;//�����ʱ������ͣ���һ��long long���͵���ʾ�����ֵ��eg: 20150521183526142,��ʾ2015��5��21��18��35��26��142����


//�����ڸ�ֵ
#define DATE_COVER (TIMESTAMP(1000000000)) 
//TIMESTAMPֵʱ�䣬����date���� 20170101093030000 -> 20170101000000000
#define LEAVE_DATE( stamp ) (TIMESTAMP(stamp)/DATE_COVER*DATE_COVER)
//TIMESTAMPֵʱ�䣬��ȡdate���� 20170101093030000 -> 20170101
#define EXTR_DATE( stamp ) (TIMESTAMP(stamp)/DATE_COVER)
//TIMESTAMPֵʱ�䣬��ȡtime���� 20170101093030123 -> 93030123
#define EXTR_TIME( stamp ) (TIMESTAMP(stamp)%DATE_COVER)
//TIMESTAMPֵʱ�䣬��ȡtime���֣�����������,��������λ����Ϊ0�� 20170101093030123 -> 93030000
#define EXTR_TIMENO( stamp ) (TIMESTAMP(stamp)%DATE_COVER/1000i64*1000i64)
//TIMESTAMPֵʱ�䣬��ȡtime���֣�����������,����������λ�� 20170101 093030 123 -> 93030
#define EXTR_TIMES( stamp ) (TIMESTAMP(stamp)%DATE_COVER/1000i64)
//TIMESTAMPֵʱ�䣬��ȡyear���� 20170101093030000 -> 2017
#define EXTR_YEAR( stamp ) (long(TIMESTAMP(stamp)/10000000000000i64))
//TIMESTAMPֵʱ�䣬��ȡmonth���� 20171021093030000 -> 10
#define EXTR_MON( stamp )  (long(TIMESTAMP(stamp)%10000000000000i64/100000000000i64))
//TIMESTAMPֵʱ�䣬��ȡday���� 20170121093030000 -> 21
#define EXTR_DAY( stamp )  (long(TIMESTAMP(stamp)%100000000000i64/1000000000i64))
//TIMESTAMPֵʱ�䣬��ȡhour���� 20170101093030000 -> 09
#define EXTR_HOUR( stamp ) (long(TIMESTAMP(stamp)%1000000000i64/10000000i64))
//TIMESTAMPֵʱ�䣬��ȡminute���� 20170101092530000 -> 25
#define EXTR_MINU( stamp ) (long(TIMESTAMP(stamp)%10000000i64/100000i64))
//TIMESTAMPֵʱ�䣬��ȡsecond���� 20170101092530127 -> 30
#define EXTR_SEC( stamp )  (long(TIMESTAMP(stamp)%100000i64/1000i64))
//TIMESTAMPֵʱ�䣬��ȡmillisecond���� 20170101093030315 -> 315
#define EXTR_MIS( stamp )  (long(TIMESTAMP(stamp)%1000i64))
//�������TIMESTAMPֵʱ�䣬�ֱ���ȡdate��time������ϳ��µ�ʱ��ֵ��
//20170101093030000 + 20140202101350102 = 20170101101350102
#define COMB_DATE_TIME(date,time) (((TIMESTAMP)(date)/DATE_COVER*DATE_COVER) +((TIMESTAMP)(time)%DATE_COVER))
//��ϸ�ʱ����ֵʱ�䣬��һ��TIMESTAMPֵʱ�䡣
// 11��15��21 ��876�� ->  111521876
#define MAKE_STAMP_TIME(h,mi,s,ms) ((TIMESTAMP)(h)*10000000i64 +(TIMESTAMP)(mi)*100000i64 +(TIMESTAMP)(s)*1000i64 +(ms))
//��ϸ�������ֵʱ�䣬��һ��TIMESTAMPֵʱ�䡣
// 2017-11-21         ->  20171121000000000
#define MAKE_STAMP_DATE(y,m,d) ((TIMESTAMP)(y)*10000000000000i64 +(TIMESTAMP)(m)*100000000000i64 +(TIMESTAMP)(d)*1000000000i64)
//��ϸ� ������ ʱ���� ֵʱ�䣬��һ��TIMESTAMPֵʱ�䡣
// 2017-11-21 10��15��21 ��876��  ->  20171121101521876
#define MAKE_STAMP(y,m,d,h,mi,s,ms) ((TIMESTAMP)ms + ((TIMESTAMP)s*1000i64) + ((TIMESTAMP)mi*100000i64) + ((TIMESTAMP)h*10000000i64) + ((TIMESTAMP)d*1000000000i64) + ((TIMESTAMP)m*100000000000i64) + ((TIMESTAMP)y*10000000000000i64))

//ʱ�����ݲ���������
class CLTime{
protected:

	SYSTEMTIME m_at;
	int m_isLocalTime;

	SYSTEMTIME m_sysTimeStart,m_sysTimeEnd;

	LARGE_INTEGER m_largeIntegerStart,m_largeIntegerEnd;

	byte m_startTimeFlag,m_startTimeExFlag;

	CLTimeR init();


public:
	//Ĭ�Ϲ��캯�����ڲ������汾��ʱ��
	CLTime();
	//����������CTT_CREATEBYTIMESTAMP��CTT_CREATEBYLOCALTIME��CTT_CREATEBYUTCTIME
	CLTime(UINT uType,unsigned long long timeStamp = 0);
	CLTime(int dYears,int dMonths,int dDays,int dHours,int dMinutes,int dSeconds,int dMMSeconds=0);
	CLTime(int dYears,int dMonths,int dDays);
	CLTime(int dHours,int dMinutes,int dSeconds,int dMMSeconds);
	CLTime(const SYSTEMTIME& _in_sysTime);
	CLTime(const CLTime& tagCLTime);
	virtual ~CLTime();

	//SYSTEMTIME�ṹ�帴��Tm�ṹ��
#define setST2Tm( Tm , systemTime ) \
	{\
		(Tm).tm_sec = (systemTime).wSecond;\
		(Tm).tm_min = (systemTime).wMinute;\
		(Tm).tm_hour = (systemTime).wHour;\
		(Tm).tm_mday = (systemTime).wDay;\
		(Tm).tm_mon = (systemTime).wMonth-1;\
		(Tm).tm_year = (systemTime).wYear-1900;\
		(Tm).tm_wday = (systemTime).wDayOfWeek;\
	}
	//Tm�ṹ�帴�Ƶ�SYSTEMTIME
#define setTm2ST( systemTime , Tm )\
	{\
		(systemTime).wDay = (Tm).tm_mday;\
		(systemTime).wDayOfWeek = (Tm).tm_wday;\
		(systemTime).wHour = (Tm).tm_hour;\
		(systemTime).wMinute = (Tm).tm_min;\
		(systemTime).wMonth = (Tm).tm_mon+1;\
		(systemTime).wSecond = (Tm).tm_sec;\
		(systemTime).wYear = (Tm).tm_year+1900;\
	}
	//TIMESTAMP�ṹ�帴��Tm�ṹ��,��ʱ���Զ�����������
#define setTS2Tm( Tm , timeStamp ) \
	{\
	(Tm).tm_sec = EXTR_SEC(timeStamp);\
	(Tm).tm_min = EXTR_MINU(timeStamp);\
	(Tm).tm_hour = EXTR_HOUR(timeStamp);\
	(Tm).tm_mday = EXTR_DAY(timeStamp);\
	(Tm).tm_mon = EXTR_MON(timeStamp)-1;\
	(Tm).tm_year = EXTR_YEAR(timeStamp)-1900;\
	(Tm).tm_wday = CLTime::getDayOfWeekByKimLarsson((Tm).tm_year+1900,(Tm).tm_mon+1,(Tm).tm_mday);\
	}
	//Tm�ṹ�帴��TIMESTAMP�ṹ�壬��Ҫ�������Ӻ������ݣ�������������
#define setTm2TS( timeStamp , Tm , miniSec ) \
	{(timeStamp) = (TIMESTAMP)MAKE_STAMP((Tm).tm_year+1900,(Tm).tm_mon+1,(Tm).tm_mday,(Tm).tm_hour,(Tm).tm_min,(Tm).tm_sec) , miniSec);}

	//ȡ�ñ����ʱ������״̬�Ƿ��Ǳ���ʱ�䣬����Ƿ���1�����Ƿ���0��û�м�¼����-1
	int isLocalTime() const;

	//��ֵ��������ʱ���¼ֵ������Ϊ�� ʱ���(�ú���ʹ�õ�ʱ����Ǳ�׼����ʱ��� ������ �Զ����longlongʱ��ֵ)
	CLTime& set(unsigned long long timeStamp);

	//��ֵ��������ʱ���¼ֵ������Ϊ�� �� �� �� ʱ �� �� ���� ��intֵ
	CLTime& set(int dYears,int dMonths,int dDays,int dHours=0,int dMinutes=0,int dSeconds=0,int dMMSeconds=0);

	//��ֵ��������ʱ���¼ֵ�������� ͬ��������
	CLTime& set(const CLTime& _in_cltime);
	CLTime& set(const SYSTEMTIME& _in_cltime);
	
	//����=���㣬������ͬ��������
	CLTime& operator=(const CLTime& _in_cltime);
	CLTime& operator=(const SYSTEMTIME& _in_cltime);
	CLTime& operator=(int time);
	CLTime& operator=(long time);
	CLTime& operator=(TIMESTAMP time);

	//����ʱ�䣬���ڲ��ı�
	CLTime& setTime(int dHours,int dMinutes,int dSeconds,int dMMSeconds=0);
	CLTime& setTime(const SYSTEMTIME& sysTime);
	CLTime& setTime(const CLTime& cTime);

	//ͨ��һ������ʱ�������ʽ��TIMESTAMPֵ��Ϊʱ���������ʱ�䡣
	CLTime& setByStamp(TIMESTAMP cTimeStamp);
	CLTime& setTimeByStamp(TIMESTAMP cTimeStamp);
	CLTime& setDateByStamp(TIMESTAMP cTimeStamp);

	//���ַ���������ʱ�������ȥ����ʱ�䣻���ú�������ı�����λ��ֵ��
	//����˵����hPos,hN �ֱ��ʾ����Сʱ�����ֵ����ַ��������ַ����е�λ����0Ϊ��ʼ�������͸��������õ��ַ�������
	//hPos < 0��hN < 0ʱ����Сʱ�������������Բ��޸�ԭֵ;
    //hN = 0ʱ��Сʱ�������������Բ���0��ֵ��
	//hPos >= 0��hN > 0ʱ��Сʱ�������Ž�������Ӧ��ֵ��
	//miPos����֣�sPos�����룬misPos������룬������ͬhPos˵����
	//��������Сʱֵ������Χ(0~23),�����ȡ����ֵ֮��ȡ�������
	//ע�⣺1��ÿһ���������õ��ַ�������಻�ܳ���8�������������
	//      2��ÿ��������Ӧ��λ�ü���ʱ����ע��ascii���뷽ʽʱ����ռ2λ�������
	//eg�� ��18��30��54�� ���η�ʽΪ ("18��30��54",0,2,3,2,6,2,-1,-1) 
	CLTimeR setTimeByString(LPCTSTR timeString,int hPos = 0,int hN = 2,int miPos = 3,int miN = 2,int sPos = 6,int sN = 2,int misPos = 0,int misN = 0);
	CLTimeR setDateByString(LPCTSTR dateString,int yearPos = 0,int yearN = 4,int monPos = 5,int monN = 2,int dayPos = 8,int dayN = 2);

	//���ַ���������ʱ�������ȥȡ��һ��TIMESTAMPʱ�䣻���ú�������ı�����λ��ֵ��
	//����˵����hPos,hN �ֱ��ʾ����Сʱ�����ֵ����ַ��������ַ����е�λ����0Ϊ��ʼ�������͸��������õ��ַ�������
	//hPos < 0��hN < 0ʱ����Сʱ�������������Բ��޸�ԭֵ;
	//hN = 0ʱ��Сʱ�������������Բ���0��ֵ��
	//hPos >= 0��hN > 0ʱ��Сʱ�������Ž�������Ӧ��ֵ��
	//miPos����֣�sPos�����룬misPos������룬������ͬhPos˵����
	//��������Сʱֵ������Χ(0~23),�����ȡ����ֵ֮��ȡ�������
	//ע�⣺1��ÿһ���������õ��ַ�������಻�ܳ���8�������������
	//      2��ÿ��������Ӧ��λ�ü���ʱ����ע��ascii���뷽ʽʱ����ռ2λ�������
	//eg�� ��18��30��54�� ���η�ʽΪ ("18��30��54",0,2,3,2,6,2,-1,-1) 
	static TIMESTAMP getTimeByString_ll(LPCTSTR timeString,int hPos = 0,int hN = 2,int miPos = 3,int miN = 2,int sPos = 6,int sN = 2,int misPos = 0,int misN = 0);
	static TIMESTAMP getDateByString_ll(LPCTSTR dateString,int yearPos = 0,int yearN = 4,int monPos = 5,int monN = 2,int dayPos = 8,int dayN = 2);

	//�������ڣ�ʱ�䲻�ı䣨�������汾���Զ������µ���������
	CLTime& setDate(int dYears,int dMonths,int dDays);
	CLTime& setDate(const SYSTEMTIME& sysTime);
	CLTime& setDate(const CLTime& cTime);	

	const SYSTEMTIME& dataConst() const;//ȡ�����ݽṹ�壨ֻ����ʽ��ȡ���ݱ�����
	CLTimeR getLocalTime();//ȡ�ñ���ʱ��
	CLTimeR getSystemTime();//ȡ��UTCʱ��

	//�õ������ִ���szType��ʾ�ָ������÷��ţ�Ĭ�ϸ�ʽxxxx-xx-xx,����ֵΪoutputTagObj���󣬶��ǵ����߱���
	CLString& dateStr( CLString& outputTagObj,LPCTSTR szType = NULL);
	static CLString& dateStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szType = NULL);

	//�õ�ʱ���ִ���szType��ʾ�ָ������÷��� ��Ĭ�ϸ�ʽxx:xx:xx,����ֵΪoutputTagObj���󣬶��ǵ����߱���
	CLString& timeStr( CLString& outputTagObj,LPCTSTR szType = NULL);
	static CLString& timeStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szType = NULL);

	//�õ����ں�ʱ���ִ���szType��ʾ�ָ������÷��ţ�Ĭ�ϸ�ʽxxxx-xx-xx (x) xx:xx:xx,����ֵΪoutputTagObj���󣬶��ǵ����߱���
	CLString& dateTimeStr( CLString& outputTagObj,LPCTSTR szDType=_T("-"),LPCTSTR szTType=_T(":"));
	static CLString& dateTimeStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szDType=_T("-"),LPCTSTR szTType=_T(":"));

	//ȡ��ʱ������������ʱ�䣬ֻ�ܼ��㵽1970���Ժ�
	CLTime& getNewTime(int dYears=0,int dMonths=0,int dDays=0,int dHours=0,int dMinutes=0,int dSeconds=0,int dMinisecond=0);
	static TIMESTAMP getNewTime_ll(TIMESTAMP orgStamp,int dYears=0,int dMonths=0, int dDays=0,int dHours=0,int dMinutes=0,int dSeconds=0,int dMinisecond=0);

	//ȡ��ʱ������������ʱ�䣬ֻ�ܼ���1970���Ժ������ʱ�䡣
	CLTime& getNewTimeByDays(int dDays=0);

	//�ȽϺ������Ƚ�����ʱ������ں�ʱ�䲿��,ʱ����ڷ���1����ȷ���0��С�ڷ���-1
	int compare(const CLTime& tagTime) const;
	int compare(const SYSTEMTIME& tagSysTime) const;
	int compare(int y,int m,int d,int h,int mi,int s,int mis) const;
	static int compare(int y1,int m1,int d1,int h1,int mi1,int s1,int mis1,
		int y2,int m2,int d2,int h2,int mi2,int s2,int mis2);

	//�ȽϺ������Ƚ�����ʱ������ڲ���,ʱ����ڷ���1����ȷ���0��С�ڷ���-1
	int compareDate(const CLTime& tagTime) const;
	int compareDate(const SYSTEMTIME& tagSysTime) const;
	int compareDate(int y,int m,int d) const;
	static int compareDate(int y,int m,int d,int y2,int m2,int d2);

	//�ȽϺ������Ƚ�����ʱ���ʱ�䲿��,ʱ����ڷ���1����ȷ���0��С�ڷ���-1
	int compareTime(const CLTime& tagTime) const;
	int compareTime(const SYSTEMTIME& tagSysTime) const;
	int compareTime(int h,int mi,int s,int mis) const;
	static int compareTime(int h,int mi,int s,int mis,int h2,int mi2,int s2,int mis2);

	//����windows�µ�ʱ����Ҫ�����洢ʱ���ָ�������
	static double calcTimeDelta(const SYSTEMTIME* beforetime,const SYSTEMTIME* behandtime);
	static double calcTimeDelta(const SYSTEMTIME& beforetime,const SYSTEMTIME& behandtime);
	static double calcTimeDelta(CLTimeRC beforetime,CLTimeRC behandtime);

	//����ʱ���ʱ������㣬������
	static double calcTimeDelta(int hf,int mf,int sf,int msf,int hb,int mb,int sb,int msb);
	static double calcTimeDelta_ll(TIMESTAMP beforetime,TIMESTAMP behandtime);

	//��ʼ��ʱ����ʱ�������������ȵ��ô˺���Timingϵ�к����������Ex��������΢��������
	CLTime& timingStart(void);
	CLTime& timingStartEx(void);

	//ȡ���Լ�ʱ��ʼ�𵽵�ǰ����ʱ�̵�ϵͳ����ʱ�䣬����Ϊ��λ��Ex��������΢��������
	//����isNeedResetStartTimeָ���Ƿ�����startTime���µĵ�ǰֵ��Ĭ��ΪFalse����
	double timingGetSpentTime(BOOL isNeedResetStartTime = FALSE);
	double timingGetSpentTimeEx(BOOL isNeedResetStartTimeEx = FALSE);
	double timingDumpSpentTimeC(BOOL isNeedResetStartTime = FALSE,LPCTSTR lpHdr = _T("\nTimeSpent="),LPCTSTR lpEnd = _T("s\n"));
	double timingDumpSpentTimeExC(BOOL isNeedResetStartTimeEx = FALSE,LPCTSTR lpHdr = _T("\nTimeSpent="),LPCTSTR lpEnd = _T("s\n"));

	//��Zeller�㷨�������������0�����գ�1~6��Ӧ��һ~������
	static int getDayOfWeekByZeller(int year,int month,int day);
	//��KimLason���㷨�������������0�����գ�1~6��Ӧ��һ~������
	static int getDayOfWeekByKimLarsson(int year,int month,int day);

	//���������
	inline static int getDayOfWeekByStamp_ll(TIMESTAMP time){ return getDayOfWeekByZeller((int)EXTR_YEAR(time), (int)EXTR_MON(time), (int)EXTR_DAY(time));}

	//���������ʱ��ֵ����������ʱ��֮��
	BOOL between(CLTimeRC timeA,CLTimeRC timeB);
	BOOL between(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB);
	BOOL between(int y1,int m1,int d1,int h1,int mi1,int s1,int mis1,
		int y2,int m2,int d2,int h2,int mi2,int s2,int mis2);

	//�����ʱ��ֵ������ʱ��֮��
	BOOL betweenTime(CLTimeRC timeA,CLTimeRC timeB);
	BOOL betweenTime(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB);
	BOOL betweenTime(int h1,int mi1,int s1,int mis1,int h2,int mi2,int s2,int mis2);

	//���������ֵ����������֮��
	BOOL betweenDate(CLTimeRC timeA,CLTimeRC timeB);
	BOOL betweenDate(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB);
	BOOL betweenDate(int y1,int m1,int d1,int y2,int m2,int d2);

	//��һ��longlong���͵�ʱ�䣬��С������ֵһ�£���ֳ������ʱ�䡣
	static void stampToTime_ll(TIMESTAMP stamp,int *y,int *m,int *d,int *h,int *mi,int *s,int *ms);
	//���ڲ���������ݷ���һ��longlong���͵�ʱ�䣬��С������ֵһ�¡�
	TIMESTAMP getStamp() const;
	//ȡ�ñ���ʱ�䣬������һ��longlong���͵�ʱ�����ݡ�
	static TIMESTAMP getLocalTime_ll();
	#define _getLocalTimell() CLTime::getLocalTime_ll()
#define EXTR_TIMESTAMP_TO_SPRINTF_YMDHMSM(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t), EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t),EXTR_MIS(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_YMDHMS(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t), EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_YMDHM(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t), EXTR_HOUR(t), EXTR_MINU(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_HMSM(t) EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t),EXTR_MIS(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_YMD(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_HMS(t) EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t)
	//ȡ��ϵͳʱ�䣬������һ��longlong���͵�ʱ�����ݡ�
	static TIMESTAMP getSystemTime_ll();
	#define _getSystemTimell() CLTime::getSystemTime_ll()
	//��������ʱ���ʱ���ĸ���λ�ܺͣ�eg����xx�꣬��xx�£���xx�գ���xxʱ����xx�֣���xx�룬��xx����
	static int calcDiffTimeSum( TIMESTAMP startTime, TIMESTAMP endTime ,OUT double* pNweek = 0,OUT double* pNyear = 0,OUT double* pNmon = 0,OUT double* pNday = 0,
		OUT double* pNhour = 0,OUT double* pNmin = 0,OUT double* pNsec = 0, OUT double* pNminiSec = 0);
	inline CLTimeR calcDiffTimeSum( TIMESTAMP difTime ,OUT double* pNweek = 0,OUT double* pNyear = 0,OUT double* pNmon = 0,OUT double* pNday = 0,
		OUT double* pNhour = 0,OUT double* pNmin = 0,OUT double* pNsec = 0, OUT double* pNminiSec = 0){
			calcDiffTimeSum( this->getStamp(),difTime ,pNweek,pNyear,pNmon,pNday,pNhour,pNmin,pNsec,pNminiSec);
			return *this;
	}
	inline CLTimeR calcDiffTimeSum( CLTimeRC difTime ,OUT double* pNweek = 0,OUT double* pNyear = 0,OUT double* pNmon = 0,OUT double* pNday = 0,
		OUT double* pNhour = 0,OUT double* pNmin = 0,OUT double* pNsec = 0, OUT double* pNminiSec = 0){
		calcDiffTimeSum( this->getStamp(),difTime.getStamp() ,pNweek,pNyear,pNmon,pNday,pNhour,pNmin,pNsec,pNminiSec);
		return *this;
	}
	//��������ʱ��ĵ���ʱ��eg��xx��xx��xx��xxʱxx��xx��xx����
	static int calcDiffTime( TIMESTAMP startTime, TIMESTAMP endTime ,OUT long* pyear = 0,OUT long* pmon = 0,OUT long* pday = 0,
		OUT long* phour = 0,OUT long* pmin = 0,OUT long* psec = 0, OUT long* pminiSec = 0);
	inline CLTimeR calcDiffTime( TIMESTAMP difTime ,OUT long* pyear = 0,OUT long* pmon = 0,OUT long* pday = 0,
		OUT long* phour = 0,OUT long* pmin = 0,OUT long* psec = 0, OUT long* pminiSec = 0){
			calcDiffTime( this->getStamp(),difTime ,pyear,pmon,pday,phour,pmin,psec,pminiSec);
			return *this;
	}
	inline CLTimeR calcDiffTime( CLTimeRC difTime ,OUT long* pyear = 0,OUT long* pmon = 0,OUT long* pday = 0,
		OUT long* phour = 0,OUT long* pmin = 0,OUT long* psec = 0, OUT long* pminiSec = 0){
			calcDiffTime( this->getStamp(),difTime.getStamp() ,pyear,pmon,pday,phour,pmin,psec,pminiSec);
			return *this;
	}
	//ȡ��΢�뼶��ʱ���������λΪ�룬windows xp����֧�֡�
	static double getTickCountsMicro() {
		static LARGE_INTEGER freq = { 0 };
		if (freq.QuadPart == 0)
			if (QueryPerformanceFrequency(&freq) == FALSE)
				return 0.0;
		LARGE_INTEGER counter;
		return QueryPerformanceCounter(&counter) ? (((double)counter.QuadPart) / ((double)freq.QuadPart)) : 0.0;
	}
	//�ɼ���ʱ������������̻���Ҫ��ʣ��ʱ��,����ֵ��ʣ��ʱ�䣨�룩����������ǰ���timingStartEx()����
	// curIndex ��ǰ�±�, startIndex ��ʼ�±�, endIndex �����±ִ꣨������ȡ curIndex < endIndex��,  stepLen ����,
	// ������ֽ���ַ������壨�֣� lpOutStringBuf = 0,�ַ��������С���֣� size_t bufSize = 0
	double timingGetRestOfTimeEx(long endIndex, long startIndex = 0, long curIndex = 0, long stepLen = 1, LPTSTR lpOutStringBuf = 0, size_t bufSize = 0);
};
#endif


