
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/view/CChoreographerHelper.h"
#include "elastos/droid/webkit/native/base/TraceEvent.h"
#include "elastos/droid/webkit/native/ui/VSyncMonitor.h"

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::CChoreographerHelper;
using Elastos::Droid::View::EIID_IFrameCallback;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::View::IChoreographerHelper;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Webkit::Base::TraceEvent;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

//=====================================================================
//            VSyncMonitor::InnerChoreographerFrameCallback
//=====================================================================
CAR_INTERFACE_IMPL(VSyncMonitor::InnerChoreographerFrameCallback, Object, IFrameCallback)

VSyncMonitor::InnerChoreographerFrameCallback::InnerChoreographerFrameCallback(
    /* [in] */ VSyncMonitor* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode VSyncMonitor::InnerChoreographerFrameCallback::DoFrame(
    /* [in] */ Int64 frameTimeNanos)
{
    // ==================before translated======================
    // TraceEvent.begin("VSync");
    // mGoodStartingPointNano = frameTimeNanos;
    // onVSyncCallback(frameTimeNanos, getCurrentNanoTime());
    // TraceEvent.end("VSync");

    TraceEvent::Begin(String("VSync"));
    mOwner->mGoodStartingPointNano = frameTimeNanos;
    mOwner->OnVSyncCallback(frameTimeNanos, mOwner->GetCurrentNanoTime());
    TraceEvent::End(String("VSync"));
    return NOERROR;
}

//=====================================================================
//                VSyncMonitor::InnerVSyncTimerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(VSyncMonitor::InnerVSyncTimerRunnable, Object, IRunnable)

VSyncMonitor::InnerVSyncTimerRunnable::InnerVSyncTimerRunnable(
    /* [in] */ VSyncMonitor* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode VSyncMonitor::InnerVSyncTimerRunnable::Run()
{
    // ==================before translated======================
    // TraceEvent.begin("VSyncTimer");
    // final long currentTime = getCurrentNanoTime();
    // onVSyncCallback(currentTime, currentTime);
    // TraceEvent.end("VSyncTimer");

    TraceEvent::Begin(String("VSyncTimer"));
    const Int64 currentTime = mOwner->GetCurrentNanoTime();
    mOwner->OnVSyncCallback(currentTime, currentTime);
    TraceEvent::End(String("VSyncTimer"));
    return NOERROR;
}

//=====================================================================
//                VSyncMonitor::InnerVSyncSyntheticRunnable
//=====================================================================
CAR_INTERFACE_IMPL(VSyncMonitor::InnerVSyncSyntheticRunnable, Object, IRunnable)

VSyncMonitor::InnerVSyncSyntheticRunnable::InnerVSyncSyntheticRunnable(
    /* [in] */ VSyncMonitor* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode VSyncMonitor::InnerVSyncSyntheticRunnable::Run()
{
    // ==================before translated======================
    // TraceEvent.begin("VSyncSynthetic");
    // final long currentTime = getCurrentNanoTime();
    // onVSyncCallback(estimateLastVSyncTime(currentTime), currentTime);
    // TraceEvent.end("VSyncSynthetic");

    TraceEvent::Begin(String("VSyncSynthetic"));
    const Int64 currentTime = mOwner->GetCurrentNanoTime();
    mOwner->OnVSyncCallback(mOwner->EstimateLastVSyncTime(currentTime), currentTime);
    TraceEvent::End(String("VSyncSynthetic"));
    return NOERROR;
}

//=====================================================================
//                             VSyncMonitor
//=====================================================================
const Int32 VSyncMonitor::MAX_AUTO_ONVSYNC_COUNT;
const Int64 VSyncMonitor::NANOSECONDS_PER_SECOND;
const Int64 VSyncMonitor::NANOSECONDS_PER_MILLISECOND;
const Int64 VSyncMonitor::NANOSECONDS_PER_MICROSECOND;

VSyncMonitor::VSyncMonitor()
    : mRefreshPeriodNano(0)
    , mHaveRequestInFlight(FALSE)
    , mTriggerNextVSyncCount(0)
    , mChoreographer(NULL)
    , mVSyncFrameCallback(NULL)
    , mVSyncRunnableCallback(NULL)
    , mGoodStartingPointNano(0)
    , mLastPostedNano(0)
    , mSyntheticVSyncRunnable(NULL)
    , mLastVSyncCpuTimeNano(0)
{
}

ECode VSyncMonitor::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Listener* listener)
{
    // ==================before translated======================
    // this(context, listener, true);

    return VSyncMonitor::constructor(context, listener, TRUE);
}

ECode VSyncMonitor::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Listener* listener,
    /* [in] */ Boolean enableJBVSync)
{
    // ==================before translated======================
    // mListener = listener;
    // float refreshRate = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE))
    //         .getDefaultDisplay().getRefreshRate();
    // if (refreshRate <= 0) refreshRate = 60;
    // mRefreshPeriodNano = (long) (NANOSECONDS_PER_SECOND / refreshRate);
    // mTriggerNextVSyncCount = 0;
    //
    // if (enableJBVSync && Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
    //     // Use Choreographer on JB+ to get notified of vsync.
    //     mChoreographer = Choreographer.getInstance();
    //     mVSyncFrameCallback = new Choreographer.FrameCallback() {
    //         @Override
    //         public void doFrame(long frameTimeNanos) {
    //             TraceEvent.begin("VSync");
    //             mGoodStartingPointNano = frameTimeNanos;
    //             onVSyncCallback(frameTimeNanos, getCurrentNanoTime());
    //             TraceEvent.end("VSync");
    //         }
    //     };
    //     mVSyncRunnableCallback = null;
    // } else {
    //     // On ICS we just hope that running tasks is relatively predictable.
    //     mChoreographer = null;
    //     mVSyncFrameCallback = null;
    //     mVSyncRunnableCallback = new Runnable() {
    //         @Override
    //         public void run() {
    //             TraceEvent.begin("VSyncTimer");
    //             final long currentTime = getCurrentNanoTime();
    //             onVSyncCallback(currentTime, currentTime);
    //             TraceEvent.end("VSyncTimer");
    //         }
    //     };
    //     mLastPostedNano = 0;
    // }
    // mSyntheticVSyncRunnable = new Runnable() {
    //     @Override
    //     public void run() {
    //         TraceEvent.begin("VSyncSynthetic");
    //         final long currentTime = getCurrentNanoTime();
    //         onVSyncCallback(estimateLastVSyncTime(currentTime), currentTime);
    //         TraceEvent.end("VSyncSynthetic");
    //     }
    // };
    // mGoodStartingPointNano = getCurrentNanoTime();

    mListener = listener;
    CHandler::New((IHandler**)&mHandler);

    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&interfaceTmp);
    IWindowManager* winManager = IWindowManager::Probe(interfaceTmp);

    AutoPtr<IDisplay> defaultDisplay;
    winManager->GetDefaultDisplay((IDisplay**)&defaultDisplay);
    Float refreshRate;
    defaultDisplay->GetRefreshRate(&refreshRate);
    if (refreshRate <= 0)
        refreshRate = 60;

    mRefreshPeriodNano = (Int64) (NANOSECONDS_PER_SECOND / refreshRate);
    mTriggerNextVSyncCount = 0;

    if (enableJBVSync && Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        // Use Choreographer on JB+ to get notified of vsync.
        AutoPtr<IChoreographerHelper> helper;
        CChoreographerHelper::AcquireSingleton((IChoreographerHelper**)&helper);
        helper->GetInstance((IChoreographer**)&mChoreographer);
        mVSyncFrameCallback = new InnerChoreographerFrameCallback(this);
        mVSyncRunnableCallback = NULL;
    }
    else {
        // On ICS we just hope that running tasks is relatively predictable.
        mChoreographer = NULL;
        mVSyncFrameCallback = NULL;
        mVSyncRunnableCallback = new InnerVSyncTimerRunnable(this);
        mLastPostedNano = 0;
    }
    mSyntheticVSyncRunnable = new InnerVSyncSyntheticRunnable(this);
    mGoodStartingPointNano = GetCurrentNanoTime();
    return NOERROR;
}

Int64 VSyncMonitor::GetVSyncPeriodInMicroseconds()
{
    // ==================before translated======================
    // return mRefreshPeriodNano / NANOSECONDS_PER_MICROSECOND;

    return mRefreshPeriodNano / NANOSECONDS_PER_MICROSECOND;
}

ECode VSyncMonitor::Stop()
{
    // ==================before translated======================
    // mTriggerNextVSyncCount = 0;

    mTriggerNextVSyncCount = 0;
    return NOERROR;
}

ECode VSyncMonitor::RequestUpdate()
{
    // ==================before translated======================
    // mTriggerNextVSyncCount = MAX_AUTO_ONVSYNC_COUNT;
    // postCallback();

    mTriggerNextVSyncCount = MAX_AUTO_ONVSYNC_COUNT;
    PostCallback();
    return NOERROR;
}

ECode VSyncMonitor::SetVSyncPointForICS(
    /* [in] */ Int64 goodStartingPointNano)
{
    // ==================before translated======================
    // mGoodStartingPointNano = goodStartingPointNano;

    mGoodStartingPointNano = goodStartingPointNano;
    return NOERROR;
}

Boolean VSyncMonitor::IsVSyncSignalAvailable()
{
    // ==================before translated======================
    // return mChoreographer != null;

    return (mChoreographer != NULL);
}

Int64 VSyncMonitor::GetCurrentNanoTime()
{
    // ==================before translated======================
    // return System.nanoTime();

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nanoTime = 0;
    system->GetNanoTime(&nanoTime);
    return nanoTime;
}

ECode VSyncMonitor::OnVSyncCallback(
    /* [in] */ Int64 frameTimeNanos,
    /* [in] */ Int64 currentTimeNanos)
{
    // ==================before translated======================
    // assert mHaveRequestInFlight;
    // mHaveRequestInFlight = false;
    // mLastVSyncCpuTimeNano = currentTimeNanos;
    // if (mTriggerNextVSyncCount >= 0) {
    //     mTriggerNextVSyncCount--;
    //     postCallback();
    // }
    // if (mListener != null) {
    //     mListener.onVSync(this, frameTimeNanos / NANOSECONDS_PER_MICROSECOND);
    // }

    assert (mHaveRequestInFlight);
    mHaveRequestInFlight = FALSE;
    mLastVSyncCpuTimeNano = currentTimeNanos;
    if (mTriggerNextVSyncCount >= 0) {
        --mTriggerNextVSyncCount;
        PostCallback();
    }
    if (mListener != NULL) {
        mListener->OnVSync(this, frameTimeNanos / NANOSECONDS_PER_MICROSECOND);
    }
    return NOERROR;
}

ECode VSyncMonitor::PostCallback()
{
    // ==================before translated======================
    // if (mHaveRequestInFlight) return;
    // mHaveRequestInFlight = true;
    // if (postSyntheticVSync()) return;
    // if (isVSyncSignalAvailable()) {
    //     mChoreographer.postFrameCallback(mVSyncFrameCallback);
    // } else {
    //     postRunnableCallback();
    // }

    if (mHaveRequestInFlight)
        return NOERROR;

    mHaveRequestInFlight = TRUE;
    if (PostSyntheticVSync())
        return NOERROR;

    if (IsVSyncSignalAvailable()) {
        mChoreographer->PostFrameCallback(mVSyncFrameCallback);
    }
    else {
        PostRunnableCallback();
    }
    return NOERROR;
}

Boolean VSyncMonitor::PostSyntheticVSync()
{
    // ==================before translated======================
    // final long currentTime = getCurrentNanoTime();
    // // Only trigger a synthetic vsync if we've been idle for long enough and the upcoming real
    // // vsync is more than half a frame away.
    // if (currentTime - mLastVSyncCpuTimeNano < 2 * mRefreshPeriodNano) return false;
    // if (currentTime - estimateLastVSyncTime(currentTime) > mRefreshPeriodNano / 2) return false;
    // mHandler.post(mSyntheticVSyncRunnable);
    // return true;

    const Int64 currentTime = GetCurrentNanoTime();
    // Only trigger a synthetic vsync if we've been idle for long enough and the upcoming real
    // vsync is more than half a frame away.
    if (currentTime - mLastVSyncCpuTimeNano < 2 * mRefreshPeriodNano) return FALSE;
    if (currentTime - EstimateLastVSyncTime(currentTime) > mRefreshPeriodNano / 2) return FALSE;

    Boolean postRet = FALSE;
    mHandler->Post(mSyntheticVSyncRunnable, &postRet);
    return postRet;
}

Int64 VSyncMonitor::EstimateLastVSyncTime(
    /* [in] */ Int64 currentTime)
{
    // ==================before translated======================
    // final long lastRefreshTime = mGoodStartingPointNano +
    //         ((currentTime - mGoodStartingPointNano) / mRefreshPeriodNano) * mRefreshPeriodNano;
    // return lastRefreshTime;

    const Int64 lastRefreshTime = mGoodStartingPointNano +
        ((currentTime - mGoodStartingPointNano) / mRefreshPeriodNano) * mRefreshPeriodNano;
    return lastRefreshTime;
}

ECode VSyncMonitor::PostRunnableCallback()
{
    // ==================before translated======================
    // assert !isVSyncSignalAvailable();
    // final long currentTime = getCurrentNanoTime();
    // final long lastRefreshTime = estimateLastVSyncTime(currentTime);
    // long delay = (lastRefreshTime + mRefreshPeriodNano) - currentTime;
    // assert delay > 0 && delay <= mRefreshPeriodNano;
    //
    // if (currentTime + delay <= mLastPostedNano + mRefreshPeriodNano / 2) {
    //     delay += mRefreshPeriodNano;
    // }
    //
    // mLastPostedNano = currentTime + delay;
    // if (delay == 0) mHandler.post(mVSyncRunnableCallback);
    // else mHandler.postDelayed(mVSyncRunnableCallback, delay / NANOSECONDS_PER_MILLISECOND);

    assert (!IsVSyncSignalAvailable());
    const Int64 currentTime = GetCurrentNanoTime();
    const Int64 lastRefreshTime = EstimateLastVSyncTime(currentTime);
    Int64 delay = (lastRefreshTime + mRefreshPeriodNano) - currentTime;
    assert (delay > 0 && delay <= mRefreshPeriodNano);

    if (currentTime + delay <= mLastPostedNano + mRefreshPeriodNano / 2) {
        delay += mRefreshPeriodNano;
    }

    mLastPostedNano = currentTime + delay;
    Boolean postRet = FALSE;
    if (delay == 0)
        mHandler->Post(mVSyncRunnableCallback, &postRet);
    else
        mHandler->PostDelayed(mVSyncRunnableCallback, delay / NANOSECONDS_PER_MILLISECOND, &postRet);

    return NOERROR;
}

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


