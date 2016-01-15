
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_GLTHREADMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_GLTHREADMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IConditionVariable;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Core::Object;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class GLThreadManager
    : public Object
    , public IGLThreadManager
{
private:
    /**
     * Container object for Configure messages.
     */
    class ConfigureHolder
        : public Object
    {
    public:
        ConfigureHolder(
            /* [in] */ IConditionVariable* condition,
            /* [in] */ ICollection* surfaces,
            /* [in] */ ICaptureCollector* collector);
    public:
        AutoPtr<IConditionVariable> mCondition;
        AutoPtr<ICollection> mSurfaces;
        AutoPtr<ICaptureCollector> mCollector;
    };

    class MyHandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallback(
            /* [in] */ GLThreadManager* host);

        CARAPI HandleMessage(
           /* [in] */ IMessage* msg,
           /* [out] */ Boolean* result);
    private:
        GLThreadManager* mHost;
        Boolean mCleanup;
        Boolean mConfigured;
        Boolean mDroppingFrames;
    };

public:
    CAR_INTERFACE_DECL()

    GLThreadManager();

    virtual ~GLThreadManager() {}

    CARAPI constructor();

    /**
     * Create a new GL thread and renderer.
     *
     * @param cameraId the camera id for this thread.
     * @param facing direction the camera is facing.
     * @param state {@link CameraDeviceState} to use for error handling.
     */
    CARAPI constructor(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 facing,
        /* [in] */ ICameraDeviceState* state);

    /**
     * Start the thread.
     *
     * <p>
     * This must be called before queueing new frames.
     * </p>
     */
    CARAPI Start();

    /**
     * Wait until the thread has started.
     */
    CARAPI WaitUntilStarted();

    /**
     * Quit the thread.
     *
     * <p>
     * No further methods can be called after this.
     * </p>
     */
    CARAPI Quit();

    /**
     * Queue a new call to draw into the surfaces specified in the next available preview
     * request from the {@link CaptureCollector} passed to
     * {@link #setConfigurationAndWait(java.util.Collection, CaptureCollector)};
     */
    CARAPI QueueNewFrame();

    /**
     * Configure the GL renderer for the given set of output surfaces, and block until
     * this configuration has been applied.
     *
     * @param surfaces a collection of {@link android.view.Surface}s to configure.
     * @param collector a {@link CaptureCollector} to retrieve requests from.
     */
    CARAPI SetConfigurationAndWait(
        /* [in] */ ICollection* surfaces,
        /* [in] */ ICaptureCollector* collector);

    /**
     * Get the underlying surface to produce frames from.
     *
     * <p>
     * This returns the surface that is drawn into the set of surfaces passed in for each frame.
     * This method should only be called after a call to
     * {@link #setConfigurationAndWait(java.util.Collection)}.  Calling this before the first call
     * to {@link #setConfigurationAndWait(java.util.Collection)}, after {@link #quit()}, or
     * concurrently to one of these calls may result in an invalid
     * {@link android.graphics.SurfaceTexture} being returned.
     * </p>
     *
     * @return an {@link android.graphics.SurfaceTexture} to draw to.
     */
    CARAPI GetCurrentSurfaceTexture(
        /* [out] */ ISurfaceTexture** outsur);

    /**
     * Ignore any subsequent calls to {@link #queueNewFrame(java.util.Collection)}.
     */
    CARAPI IgnoreNewFrames();

    /**
     * Wait until no messages are queued.
     */
    CARAPI WaitUntilIdle();

    /**
     * Re-enable drawing new frames after a call to {@link #ignoreNewFrames()}.
     */
    CARAPI AllowNewFrames();

private:
    String TAG;
    static const Boolean DEBUG;

    friend class MyHandlerCallback;

    static const Int32 MSG_NEW_CONFIGURATION = 1;
    static const Int32 MSG_NEW_FRAME = 2;
    static const Int32 MSG_CLEANUP = 3;
    static const Int32 MSG_DROP_FRAMES = 4;
    static const Int32 MSG_ALLOW_FRAMES = 5;

    AutoPtr<ICaptureCollector> mCaptureCollector;

    AutoPtr<ICameraDeviceState> mDeviceState;

    AutoPtr<ISurfaceTextureRenderer> mTextureRenderer;

    AutoPtr<IRequestHandlerThread> mGLHandlerThread;

    AutoPtr<IRequestThreadManagerFpsCounter> mPrevCounter;

    AutoPtr<IHandlerCallback> mGLHandlerCb;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_GLTHREADMANAGER_H__
