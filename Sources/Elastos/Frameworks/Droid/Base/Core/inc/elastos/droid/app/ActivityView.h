
#ifndef __ELASTOS_DROID_APP_ACTIVITY_VIEW_H__
#define __ELASTOS_DROID_APP_ACTIVITY_VIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::View::ViewGroup;

// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Os::IBinder;
// using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEvent;
// using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ITextureView;
// using Elastos::Droid::View::ITextureViewSurfaceTextureListener;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceTextureListener;
using Elastos::Droid::View::IView;
// using Elastos::Droid::View::IViewGroup;
// using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IDisplayMetrics;

using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace App {

/** @hide */
class ActivityView
    : public ViewGroup
    , public IActivityView
{
public:
    class ActivityContainerCallback
        : public Object
        , public IActivityContainerCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityContainerCallback();

        virtual ~ActivityContainerCallback();

        CARAPI constructor(
            /* [in] */ IActivityView* activityView);

        CARAPI SetVisible(
            /* [in] */ IBinder* container,
            /* [in] */ Boolean visible);

        CARAPI OnAllActivitiesComplete(
            /* [in] */ IBinder* container);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IWeakReference> mActivityViewWeakReference;
    };

private:

    class AllActivitiesCompleteRunnable
        : public Runnable
    {
    public:
        AllActivitiesCompleteRunnable(
            /* [in] */ IActivityViewCallback* callback,
            /* [in] */ IActivityView* view);

        CARAPI Run();

    private:
        AutoPtr<IActivityViewCallback> mCallback;
        AutoPtr<IActivityView> mActivityView;
    };

    class ActivityViewSurfaceTextureListener
        : public Object
        , public ISurfaceTextureListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityViewSurfaceTextureListener(
            /* [in] */ ActivityView* host);

        CARAPI OnSurfaceTextureAvailable(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI OnSurfaceTextureSizeChanged(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI OnSurfaceTextureDestroyed(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [out] */ Boolean* bval);

        CARAPI OnSurfaceTextureUpdated(
            /* [in] */ ISurfaceTexture* surfaceTexture);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        ActivityView* mHost;
    };

    class ActivityContainerWrapper
        : public Object
    {
    public:
        ActivityContainerWrapper(
            /* [in] */ IIActivityContainer* container);

        virtual ~ActivityContainerWrapper();

        CARAPI AttachToDisplay(
            /* [in] */ Int32 displayId);

        CARAPI SetSurface(
            /* [in] */ ISurface* surface,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 density);

        CARAPI StartActivity(
            /* [in] */ IIntent* intent,
            /* [out] */ Int32* status);

        CARAPI StartActivityIntentSender(
            /* [in] */ IIIntentSender* intentSender,
            /* [out] */ Int32* status);

        CARAPI CheckEmbeddedAllowed(
            /* [in] */ IIntent* intent);

        CARAPI CheckEmbeddedAllowedIntentSender(
            /* [in] */ IIIntentSender* intentSender);

        CARAPI GetDisplayId(
            /* [out] */ Int32* status);

        CARAPI InjectEvent(
            /* [in] */ IInputEvent* event,
            /* [out] */ Boolean* bval);

        CARAPI ReleaseResources();

        void Finalize();

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        AutoPtr<IIActivityContainer> mActivityContainer;
        AutoPtr<ICloseGuard> mGuard;// = CloseGuard.get();
        Boolean mOpened; // Protected by mGuard.
    };

public:
    CAR_INTERFACE_DECL()

    ActivityView();

    virtual ~ActivityView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* bval);

    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* bval);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI IsAttachedToDisplay(
        /* [out] */ Boolean* bval);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent);

    CARAPI StartActivity(
        /* [in] */ IIntentSender* intentSender);

    CARAPI StartActivity(
        /* [in] */ IPendingIntent* pendingIntent);

    CARAPI ReleaseResources();

    /**
     * Set the callback to use to report certain state changes.
     * @param callback The callback to report events to.
     *
     * @see ActivityViewCallback
     */
    CARAPI SetCallback(
        /* [in] */ IActivityViewCallback* callback);

protected:
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    Boolean InjectInputEvent(
        /* [in] */ IInputEvent* event);

    CARAPI AttachToSurfaceWhenReady();

private:
    friend class ActivityViewSurfaceTextureListener;
    friend class ActivityContainerWrapper;

    static const String TAG;;
    static const Boolean DEBUG;

    AutoPtr<IDisplayMetrics> mMetrics;
    AutoPtr<ITextureView> mTextureView;
    AutoPtr<ActivityContainerWrapper> mActivityContainer;
    AutoPtr<IActivity> mActivity; // TODO weak ref ??
    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<ISurface> mSurface;
    Int32 mLastVisibility;
    AutoPtr<IActivityViewCallback> mActivityViewCallback;

    // Only one IIntentSender or Intent may be queued at a time. Most recent one wins.
    AutoPtr<IIIntentSender> mQueuedPendingIntent;
    AutoPtr<IIntent> mQueuedIntent;
};


} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIVITY_VIEW_H__
