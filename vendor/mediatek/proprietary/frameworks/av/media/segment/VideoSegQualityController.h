
#ifndef VIDEO_SEG_QUALITY_CONTROLLER_H_
#define VIDEO_SEG_QUALITY_CONTROLLER_H_

#include <utils/Errors.h>
#include <media/stagefright/MetaData.h>


namespace android {

#define FPS_MEM_THRESHOLD_DEFAULT				40*1024*1024
#define FPS_MIN_MEM_THRESHOLD_DEFAULT			20*1024*1024
#define BITRATE_MEM_THRESHOLD_DEFAULT			20*1024*1024
#define BITRATE_MIN_MEM_THRESHOLD_DEFAULT		5*1024*1024

class MPEG4SegWriter;

/******************************************************************************
*
*******************************************************************************/
class VideoSegQualityController
{
public:
	VideoSegQualityController(MPEG4SegWriter *owner, const sp<MediaSource> &source);
	virtual ~VideoSegQualityController();

private:
	VideoSegQualityController(const VideoSegQualityController &);
	VideoSegQualityController &operator = (const VideoSegQualityController &);


/******************************************************************************
*  Operations in class VideoSegQualityController
*******************************************************************************/
public:
	void init(MetaData *param);
	status_t configParams(MetaData *param);
	void propertyGetParams(MetaData *param);
	void adjustQualityIfNeed(/*Track* track,*/const int64_t memory_size = 0);
	void adjustForMemory(const int64_t memory_size);

private:
	MPEG4SegWriter *mOwner;
	sp<MediaSource> mSource;
	int32_t mVideoInitFPS;
	int32_t mVideoEncoder;
	int32_t mVideoWidth;
	int32_t mVideoHeight;
	int32_t mVideoBitRate;
	int64_t mVideoDynamicFPSLowThreshold;  // sam remove
	int64_t mVideoDynamicFPSHighThreshold;
	int64_t mVideoDynamicBitrateLowThreshold;  // sam remove
	int64_t mVideoDynamicBitrateHighThreshold;

	int32_t mEnableForMemory;
	int32_t mVideoBitrateLowPercentage;
	int32_t mVideoDynamicAutoFPSDropRate;

	bool 	mBitrateAdjusted;
	bool 	mFPSAdjustedForMem;
	bool 	mFPSAdjustedForTemp;
};

};

#endif // VIDEO_QUALITY_CONTROLLER_H_


