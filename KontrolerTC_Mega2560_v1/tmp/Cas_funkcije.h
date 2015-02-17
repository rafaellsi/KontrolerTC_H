#ifndef Cas_funkcije_h
#define Cas_funkcije_h


void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec);
boolean IsWeekend(void);

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




#endif
