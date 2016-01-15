
#ifndef __AUDIOTRACK_HELPER_H__
#define __AUDIOTRACK_HELPER_H__

#ifdef __cplusplus
extern "C" 
#endif
int GetMinBufferSize(
    /* [in] */ int sampleRateInHz,
    /* [in] */ int channelConfig,
    /* [in] */ int audioFormat);

#endif //__AUDIOTRACK_HELPER_H__
