#ifndef _VOW_API_MD32_H
#define _VOW_API_MD32_H

//#include "simDebug.h"

#ifdef __cplusplus
extern "C" {
#endif

int TestingInit_IModel (const char *IModel);
int TestingInit_SModel (const char *SModel);
int TestingInit_Table (int isMultipleCmd);
int setPreLearn (const int loopTimes);
int onTesting (short *pMicBuf1, int *rtnCmdID);
// maximum length should not exceed 2^16-1, ex: 4 secs in 16kHz recording
short DCRemove(short *buff, int len, int reset);

//#ifdef _SWITCHTOSIM_
    // for simulation used
  //  int onTestingMFCC (short *MFCC, int *rtnCmdID, int SNR);
//#endif

#ifdef __cplusplus
}
#endif

#endif
