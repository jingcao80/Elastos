
#ifndef __ELASTOS_DROID_GALAXY4_RENDERSCRIPT_H__
#define __ELASTOS_DROID_GALAXY4_RENDERSCRIPT_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class RenderScript
    : public Object
{
public:
    enum Priority {
        LOW = IProcess::THREAD_PRIORITY_BACKGROUND + (5 * IProcess::THREAD_PRIORITY_LESS_FAVORABLE),
        NORMAL = IProcess::THREAD_PRIORITY_DISPLAY,
    };

    /**
     * @deprecated in API 16
     * Class which is used to describe a pixel format for a graphical buffer.
     * This is used to describe the intended format of the display surface.
     *
     * The configuration is described by pairs of minimum and preferred bit
     * depths for each component within the config and additional structural
     * information.
     */
    class SurfaceConfig
        : public Object
    {
    public:
        SurfaceConfig()
            : mDepthMin(0)
            , mDepthPref(0)
            , mStencilMin(0)
            , mStencilPref(0)
            , mColorMin(8)
            , mColorPref(8)
            , mAlphaMin(0)
            , mAlphaPref(0)
            , mSamplesMin(1)
            , mSamplesPref(1)
            , mSamplesQ(1.0)
        {}

        SurfaceConfig(
            /* [in] */ SurfaceConfig* sc)
            : mDepthMin(sc->mDepthMin)
            , mDepthPref(sc->mDepthPref)
            , mStencilMin(sc->mStencilMin)
            , mStencilPref(sc->mStencilPref)
            , mColorMin(sc->mColorMin)
            , mColorPref(sc->mColorPref)
            , mAlphaMin(sc->mAlphaMin)
            , mAlphaPref(sc->mAlphaPref)
            , mSamplesMin(sc->mSamplesMin)
            , mSamplesPref(sc->mSamplesPref)
            , mSamplesQ(sc->mSamplesQ)
        {}

    public:
        Int32 mDepthMin;
        Int32 mDepthPref;
        Int32 mStencilMin;
        Int32 mStencilPref;
        Int32 mColorMin;
        Int32 mColorPref;
        Int32 mAlphaMin;
        Int32 mAlphaPref;
        Int32 mSamplesMin;
        Int32 mSamplesPref;
        Float mSamplesQ;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ SurfaceConfig* sc);

    CARAPI Validate();

    CARAPI SetPriority(
        /* [in] */ Priority p);

    CARAPI SetSurface(
        /* [in] */ ISurfaceHolder* sur,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI Destroy();

    CARAPI_(Int64) nDeviceCreate();

    CARAPI_(void) nDeviceDestroy(
        /* [in] */ Int64 dev);

    CARAPI_(Int64) nContextCreateGL(
        /* [in] */ Int64 dev,
        /* [in] */ Int32 ver,
        /* [in] */ Int32 sdkVer,
        /* [in] */ Int32 colorMin,
        /* [in] */ Int32 colorPref,
        /* [in] */ Int32 alphaMin,
        /* [in] */ Int32 alphaPref,
        /* [in] */ Int32 depthMin,
        /* [in] */ Int32 depthPref,
        /* [in] */ Int32 stencilMin,
        /* [in] */ Int32 stencilPref,
        /* [in] */ Int32 samplesMin,
        /* [in] */ Int32 samplesPref,
        /* [in] */ Float samplesQ,
        /* [in] */ Int32 dpi);

    CARAPI_(void) nContextSetSurface(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ISurface* sur);

    CARAPI_(void) nContextFinish();

    CARAPI_(void) nContextDeinitToClient(
        /* [in] */ Int64 con);

    CARAPI nContextDestroy();

    CARAPI_(void) nContextSetPriority(
        /* [in] */ Int32 p);

public:
    Int64 mDev;
    Int64 mContext;
    Object mRWLock;

    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<SurfaceConfig> mSurfaceConfig;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_RENDERSCRIPT_H__
