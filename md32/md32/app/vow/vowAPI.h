#ifndef _VOW_API_H
#define _VOW_API_H

#if 0 //AP Side Only
struct VOW_TestInfo {
	int mode;
	char	*SPath;
	char	*IPath;
	char	*infoDir;
	int	rtnSModelSize1, rtnSModelSize2, rtnSModelSize3;
	int	rtnIModelSize;
	int	rtnNoiseAndInfoSize;
	char	*rtnSModel1, *rtnSModel2, *rtnSModel3;
	char	*rtnIModel;
	char	*rtnNoiseAndInfo;
};

int getSizes (struct VOW_TestInfo *vowInfo);
int TestingInitAP (struct VOW_TestInfo *vowInfo);
#endif

int TestingInit_IModel (const char *IModel, const char *NoiseAndInfo);
int TestingInit_SModel (const char *SModel);
int TestingInit_Table ();
int setPreLearn (const int loopTimes);
int onTesting (short *pMicBuf1);


#endif