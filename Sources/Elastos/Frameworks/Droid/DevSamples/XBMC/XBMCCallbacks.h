
#ifndef __IXBMCCALLBACKS_H__
#define __IXBMCCALLBACKS_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/Vector.h>

using Elastos::Utility::Etl::Vector;

class IElAudioTrackInterface
{
public:
    void* (*CreateElAudioTrack)(
        int streamType, int sampleRateInHz, int channelConfig,
        int audioFormat, int bufferSizeInBytes, int mode);

    void (*DeleteElAudioTrack)(void* track);

    void (*Play)(void* track);

    void (*Stop)(void* track);

    void (*Flush)(void* track);

    void (*Release)(void* track);

    int (*Write)(void* track, void* audioData, int offsetInBytes, int sizeInBytes);

    int (*GetPlayState)(void* track);

    int (*SetStereoVolume)(void* track, float leftVolume, float rightVolume);

    int (*GetPlaybackHeadPosition)(void* track);

    int (*GetMinBufferSize)(int sampleRateInHz, int channelConfig, int audioFormat);
};

class ScanResult
{
public:
    char ssid[255];
    int security; //-1:NULL 0:psk 1:web
};

class IElWifiInterface
{
public:
    bool (*Start)();
    bool (*Stop)();
    Vector<ScanResult*>* (*ScanResults)();
    int (*AddNetwork)(const char* ssid, int security, const char* password);
    bool (*SaveConfig)();
    bool (*ReloadConfig)();
};

typedef void* eobject;

class DroidEnv
{
public:
    eobject (*CreateSurfaceTexture)(int texName);
    void (*SurfaceTextureSetDefaultBufferSize)(eobject object, int width, int height);
    void (*SurfaceTextureUpdateTexImage)(eobject object);
    void (*SurfaceTextureDetachFromGLContext)(eobject object);
    void (*SurfaceTextureAttachToGLContext)(eobject object, int texName);
    void (*SurfaceTextureGetTransformMatrix)(eobject object, float* mtx);
    int64_t (*SurfaceTextureGetTimestamp)(eobject object);
    void (*SurfaceTextureRelease)(eobject object);

    eobject (*CreateSurface)(eobject surfaceTexture);
    bool (*SurfaceIsValid)(eobject object);
    int (*SurfaceGetNativeWindow)(eobject object);

    void (*AddRefObject)(eobject object);
    void (*ReleaseObject)(eobject object);
};

class IXBMCCallbacks
{
public:
    IElAudioTrackInterface* mElAudioTrack;
    IElWifiInterface* mElWifi;
    DroidEnv* mEnv;

    void (*StartBrowser)();
    void (*StartCalculator)();
    void (*FinishActivity)();
    bool (*ConnectEthernet)(bool connect);
};

IXBMCCallbacks* CreateXBMCCallbacks();

#endif //__IXBMCCALLBACKS_H__
