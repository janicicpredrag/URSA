#if !defined(TIME_H)
#define TIME_H

#include <time.h>


class CTimer
{
public:
   CTimer() {} ;
   virtual ~CTimer() {};

   void StartMeasuringTime();
   double ElapsedTime();

//   time_t m_tStartTime;

   double m_tStartTime;

//   clock_t m_tStartTime;
};


#endif // !defined(TIME_H)



