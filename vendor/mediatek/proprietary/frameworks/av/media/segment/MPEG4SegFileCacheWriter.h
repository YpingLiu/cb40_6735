
#ifndef MPEG4_SEG_FILE_CACHE_WRITER_H_
#define MPEG4_SEG_FILE_CACHE_WRITER_H_

namespace android {

#define PERFORMANCE_PROFILE //for performance  profile
#define DEFAULT_FILE_CACHE_SIZE 128*1024

#define TEST_BAD_PERFORMANCE //for moniter the sdcard write bitrate

class MPEG4SegWriter;

class MPEG4SegFileCacheWriter{
public:
	MPEG4SegFileCacheWriter(int fd, size_t cachesize = DEFAULT_FILE_CACHE_SIZE);

	virtual ~MPEG4SegFileCacheWriter();

	bool isFileOpen();

	void reset(int fd);

	size_t write(const void *data, size_t size, size_t num);

	int seek(off64_t offset, int refpos);

	int close();

	bool getFile();

	void setOwner(MPEG4SegWriter *owner);
#ifdef PERFORMANCE_PROFILE
	void getPerformanceInfo(int64_t* total_time = NULL, int64_t* max_time = NULL, int64_t* times_of_write = NULL);
#endif
private:

	inline status_t flush();

	void* mpCache;

	size_t mCacheSize;

	size_t mDirtySize;

	int mFd;

	bool mFileOpen;

	//bool mWriteDirty;

	MPEG4SegWriter* mOwner;
	size_t mClusterAlignLeftSize;
	long mClusterSize;

#ifdef PERFORMANCE_PROFILE
	int64_t mTotaltime;
	int64_t mMaxtime;
	int64_t mTimesofwrite;
#ifdef TEST_BAD_PERFORMANCE
	int64_t mTestDelayFreq;
	int64_t mTestDelayTimeUs;
#endif
#endif

};

}  // namespace android

#endif // MPEG4_FILE_CACHE_WRITER_H_


