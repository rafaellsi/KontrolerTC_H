#ifndef Cas_funkcije_h
#define Cas_funkcije_h


void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec);
boolean IsWeekend(void);
boolean  PreveriCas(void);
unsigned long getNtpTime(void);

//--------------------------------------------------------------------------------
// Naredi string v obliki HH:MM:SS
//  - izpSec:    - true, izpiše tudi secunde
//               - false, brez sekund   
void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec = true)
{   
  if (izpSec)
    sprintf(cas, "%02d:%02d:%02d", hour(tcas), minute(tcas), second(tcas));
  else 
    sprintf(cas, "%02d:%02d", hour(tcas), minute(tcas));
}

//--------------------------------------------------------------------------------
//
boolean IsWeekend(void)
{
  if (weekday() == NED || weekday() == SOB)
    return(true);
  return(false);  
}


boolean PreveriCas(void)
{
  int n = 0;
  boolean timeIsOk = false;
  unsigned long ntpTime = 0;

  
 while (ntpTime == 0 && n < 3) {
 
    ntpTime = getNtpTime();
    n++;
    if (ntpTime > 0) {
      if (abs(now() - ntpTime) > 5) {
        RTC.set(ntpTime);   // set the RTC and the system time to the received value
        setTime(ntpTime);
        timeIsOk = true;
        break;
      }
    }
  }
  return(timeIsOk);
}


#endif
