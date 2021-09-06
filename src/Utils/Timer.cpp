#include "Timer.h"

#ifdef __linux
	#include <stdio.h>
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <unistd.h>
#endif

#include <iostream>


// ////////////////////////////////////////////////////////////////



void CTimer::StartMeasuringTime()
{
   //time(&m_tStartTime);

   struct rusage usage;
   getrusage(RUSAGE_SELF, &usage); 
   m_tStartTime = usage.ru_utime.tv_sec+usage.ru_utime.tv_usec/1000000.0+
                  usage.ru_stime.tv_sec+usage.ru_stime.tv_usec/1000000.0;
   // m_tStartTime=clock();
}



// ////////////////////////////////////////////////////////////////



double CTimer::ElapsedTime()
{
    // time_t tNow;
    // time(&tNow);
    // return difftime(tNow,m_tStartTime);


    double TimeElapsed, TimeCurrent;
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    TimeCurrent = usage.ru_utime.tv_sec+usage.ru_utime.tv_usec/1000000.0+
                  usage.ru_stime.tv_sec+usage.ru_stime.tv_usec/1000000.0;
    TimeElapsed=TimeCurrent-m_tStartTime;

    if(TimeCurrent<m_tStartTime) 
      return 0;
    else {
      if (TimeCurrent-m_tStartTime<0.000001)
         return 0;
      else
         return (TimeCurrent-m_tStartTime);
    }


    return TimeElapsed;
    // return ((clock()-m_tStartTime))/CLOCKS_PER_SEC;
}

// ////////////////////////////////////////////////////////////////


