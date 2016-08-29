
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/service/wallpaper/WallpaperService.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/service/wallpaper/CIWallpaperEngineWrapper.h"
#include "elastos/droid/service/wallpaper/CIWallpaperServiceWrapper.h"
#include "elastos/droid/service/wallpaper/CWallpaperServiceEngineWindow.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/CInputChannel.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/CWindowInsets.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Service::Wallpaper::CIWallpaperEngineWrapper;
using Elastos::Droid::Service::Wallpaper::CIWallpaperServiceWrapper;
using Elastos::Droid::View::CInputChannel;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CWindowInsets;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::ISurfaceHolderCallback2;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::ViewRootImpl;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

//===============================
//WallpaperService::Engine::MReceiver
//===============================

WallpaperService::Engine::MReceiver::MReceiver(
    /* [in] */ Engine* host)
    : mHost(host)
{
}

ECode WallpaperService::Engine::MReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_SCREEN_ON.Equals(action)) {
        mHost->mScreenOn = TRUE;
        mHost->ReportVisibility();
    }
    else if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->mScreenOn = FALSE;
        mHost->ReportVisibility();
    }
    return NOERROR;
}

//===============================
//WallpaperService::Engine::MSurfaceHolder
//===============================

WallpaperService::Engine::MSurfaceHolder::MSurfaceHolder(
    /* [in] */ Engine* host)
    : mHost(host)
{
    mRequestedFormat = IPixelFormat::RGBX_8888;
}

Boolean WallpaperService::Engine::MSurfaceHolder::OnAllowLockCanvas()
{
    return mHost->mDrawingAllowed;
}

ECode WallpaperService::Engine::MSurfaceHolder::OnRelayoutContainer()
{
    AutoPtr<IMessage> msg;
    mHost->mCaller->ObtainMessage(WallpaperService::MSG_UPDATE_SURFACE, (IMessage**)&msg);
    mHost->mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::Engine::MSurfaceHolder::OnUpdateSurface()
{
    AutoPtr<IMessage> msg;
    mHost->mCaller->ObtainMessage(WallpaperService::MSG_UPDATE_SURFACE, (IMessage**)&msg);
    mHost->mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::Engine::MSurfaceHolder::IsCreating(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mIsCreating;
    return NOERROR;
}

ECode WallpaperService::Engine::MSurfaceHolder::SetFixedSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (!mHost->mFixedSizeAllowed) {
        // Regular apps can't do this.  It can only work for
        // certain designs of window animations, so you can't
        // rely on it.
        Logger::E("WallpaperService::Engine::MSurfaceHolder", "Wallpapers currently only support sizing from layout");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    BaseSurfaceHolder::SetFixedSize(width, height);
    return NOERROR;
}

ECode WallpaperService::Engine::MSurfaceHolder::SetKeepScreenOn(
    /* [in] */ Boolean screenOn)
{
    Logger::E("WallpaperService::Engine::MSurfaceHolder", "Wallpapers do not support keep screen on");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//===============================
//WallpaperService::Engine::WallpaperInputEventReceiver
//===============================

ECode WallpaperService::Engine::WallpaperInputEventReceiver::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ Engine* host)
{
    mHost= host;
    return InputEventReceiver::constructor(inputChannel, looper);
}

ECode WallpaperService::Engine::WallpaperInputEventReceiver::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    Boolean handled = FALSE;

    Int32 source;
    event->GetSource(&source);
    if (IMotionEvent::Probe(event) != NULL
            && (source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        AutoPtr<IMotionEventHelper> meh;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meh);
        AutoPtr<IMotionEvent> dup;
        meh->ObtainNoHistory(IMotionEvent::Probe(event), (IMotionEvent**)&dup);
        mHost->DispatchPointer(dup);
        handled = TRUE;
    }

    InputEventReceiver::FinishInputEvent(event, handled);
    return NOERROR;
}

//===============================
//WallpaperService::Engine::MWindow
//===============================

ECode WallpaperService::Engine::MWindow::constructor(
    /* [in] */ IWallpaperServiceEngine* host)
{
    mHost = (Engine*)host;
    return NOERROR;
}

ECode WallpaperService::Engine::MWindow::Resized(
    /* [in] */ IRect* frame,
    /* [in] */ IRect* overscanInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRect* stableInsets,
    /* [in] */ Boolean reportDraw,
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<IMessage> msg;
    mHost->mCaller->ObtainMessageI(WallpaperService::MSG_WINDOW_RESIZED, reportDraw ? 1 : 0, (IMessage**)&msg);
    mHost->mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::Engine::MWindow::Moved(
    /* [in] */ Int32 newX,
    /* [in] */ Int32 newY)
{
    AutoPtr<IMessage> msg;
    mHost->mCaller->ObtainMessageII(WallpaperService::MSG_WINDOW_MOVED, newX, newY, (IMessage**)&msg);
    mHost->mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::Engine::MWindow::DispatchAppVisibility(
    /* [in] */ Boolean visible)
{
    // We don't do this in preview mode; we'll let the preview
    // activity tell us when to run.
    AutoPtr<IWallpaperEngineWrapper> wew = (IWallpaperEngineWrapper*)(mHost->mIWallpaperEngine).Get();

    if (!(wew->mIsPreview)) {
        AutoPtr<IMessage> msg;
        mHost->mCaller->ObtainMessageI(WallpaperService::MSG_VISIBILITY_CHANGED, visible ? 1 : 0, (IMessage**)&msg);
        mHost->mCaller->SendMessage(msg);
    }
    return NOERROR;
}

ECode WallpaperService::Engine::MWindow::DispatchWallpaperOffsets(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float xStep,
    /* [in] */ Float yStep,
    /* [in] */ Boolean sync)
{
    {    AutoLock syncLock(this);
        if (WallpaperService::DEBUG) Logger::V(TAG, "Dispatch wallpaper offsets: %f, %f", x, y);
        mHost->mPendingXOffset = x;
        mHost->mPendingYOffset = y;
        mHost->mPendingXOffsetStep = xStep;
        mHost->mPendingYOffsetStep = yStep;
        if (sync) {
            mHost->mPendingSync = TRUE;
        }
        if (!mHost->mOffsetMessageEnqueued) {
            mHost->mOffsetMessageEnqueued = TRUE;
            AutoPtr<IMessage> msg;
            mHost->mCaller->ObtainMessage(WallpaperService::MSG_WALLPAPER_OFFSETS, (IMessage**)&msg);
            mHost->mCaller->SendMessage(msg);
        }
    }
    return NOERROR;
}

ECode WallpaperService::Engine::MWindow::DispatchWallpaperCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync)
{
    {    AutoLock syncLock(this);
        if (WallpaperService::DEBUG) Logger::V(TAG, "Dispatch wallpaper command: %d, %d", x, y);
        AutoPtr<WallpaperCommand> cmd = new WallpaperCommand();
        cmd->mAction = action;
        cmd->mX = x;
        cmd->mY = y;
        cmd->mZ = z;
        cmd->mExtras = extras;
        cmd->mSync = sync;
        AutoPtr<IMessage> msg;
        mHost->mCaller->ObtainMessage(WallpaperService::MSG_WALLPAPER_COMMAND, (IMessage**)&msg);
        AutoPtr<IInterface> _cmd = IInterface::Probe((IObject*)cmd);
        msg->SetObj(_cmd);
        mHost->mCaller->SendMessage(msg);
    }
    return NOERROR;
}

//===============================
//WallpaperService::WallpaperCommand
//===============================

CAR_INTERFACE_IMPL(WallpaperService::WallpaperCommand, Object, IWallpaperCommand)

WallpaperService::WallpaperCommand::WallpaperCommand()
    : mX(0)
    , mY(0)
    , mZ(0)
    , mSync(FALSE)
{}

//===============================
//WallpaperService::Engine
//===============================

CAR_INTERFACE_IMPL(WallpaperService::Engine, Object, IWallpaperServiceEngine)

WallpaperService::Engine::Engine()
{
}

WallpaperService::Engine::Engine(
    /* [in] */ WallpaperService* host)
    : mInitializing(TRUE)
    , mVisible(FALSE)
    , mScreenOn(TRUE)
    , mReportedVisible(FALSE)
    , mDestroyed(FALSE)
    , mCreated(FALSE)
    , mSurfaceCreated(FALSE)
    , mIsCreating(FALSE)
    , mDrawingAllowed(FALSE)
    , mOffsetsChanged(FALSE)
    , mFixedSizeAllowed(FALSE)
    , mWidth(0)
    , mHeight(0)
    , mFormat(0)
    , mType(0)
    , mCurWidth(0)
    , mCurHeight(0)
    , mWindowFlags(IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE)
    , mWindowPrivateFlags(IWindowManagerLayoutParams::PRIVATE_FLAG_WANTS_OFFSET_NOTIFICATIONS)
    , mCurWindowFlags(mWindowFlags)
    , mCurWindowPrivateFlags(mWindowPrivateFlags)
    , mIsEmulator(FALSE)
    , mIsCircularEmulator(FALSE)
    , mWindowIsRound(FALSE)
    , mOffsetMessageEnqueued(FALSE)
    , mPendingXOffset(0.0f)
    , mPendingYOffset(0.0f)
    , mPendingXOffsetStep(0.0f)
    , mPendingYOffsetStep(0.0f)
    , mPendingSync(FALSE)
    , mHost(host)
{
    CRect::New((IRect**)&mVisibleInsets);
    CRect::New((IRect**)&mWinFrame);
    CRect::New((IRect**)&mOverscanInsets);
    CRect::New((IRect**)&mContentInsets);
    CRect::New((IRect**)&mStableInsets);
    CRect::New((IRect**)&mDispatchedOverscanInsets);
    CRect::New((IRect**)&mDispatchedContentInsets);
    CRect::New((IRect**)&mDispatchedStableInsets);
    CRect::New((IRect**)&mFinalSystemInsets);
    CRect::New((IRect**)&mFinalStableInsets);
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&mLayout);

    AutoPtr<MReceiver> mr = new MReceiver(this);
    mReceiver = (BroadcastReceiver*)(mr.Get());


    AutoPtr<MSurfaceHolder> msh = new MSurfaceHolder(this);
    mSurfaceHolder = (BaseSurfaceHolder*)(msh.Get());

    AutoPtr<IBaseIWindow> mw;
    CWallpaperServiceEngineWindow::New(this, (IBaseIWindow**)&mw);
    mWindow = (BaseIWindow*)(mw.Get());

    CConfiguration::New((IConfiguration**)&mConfiguration);
}

ECode WallpaperService::Engine::constructor()
{
    return NOERROR;
}

ECode WallpaperService::Engine::GetSurfaceHolder(
    /* [out] */ ISurfaceHolder** sh)
{
    VALIDATE_NOT_NULL(sh)
    *sh = mSurfaceHolder;
    REFCOUNT_ADD(*sh)
    return NOERROR;
}

ECode WallpaperService::Engine::GetDesiredMinimumWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    AutoPtr<IWallpaperEngineWrapper> wew = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
    *width = wew->mReqWidth;
    return NOERROR;
}

ECode WallpaperService::Engine::GetDesiredMinimumHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    AutoPtr<IWallpaperEngineWrapper> wew = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
    *height = wew->mReqHeight;
    return NOERROR;
}

ECode WallpaperService::Engine::IsVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReportedVisible;
    return NOERROR;
}

ECode WallpaperService::Engine::IsPreview(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWallpaperEngineWrapper> wew = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
    *result = wew->mIsPreview;
    return NOERROR;
}

ECode WallpaperService::Engine::SetTouchEventsEnabled(
    /* [in] */ Boolean enabled)
{
    mWindowFlags = enabled
            ? (mWindowFlags & ~IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE)
            : (mWindowFlags | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);
    if (mCreated) {
        UpdateSurface(FALSE, FALSE, FALSE);
    }
    return NOERROR;
}

ECode WallpaperService::Engine::SetOffsetNotificationsEnabled(
    /* [in] */ Boolean enabled)
{
    mWindowPrivateFlags = enabled
            ? (mWindowPrivateFlags |
                IWindowManagerLayoutParams::PRIVATE_FLAG_WANTS_OFFSET_NOTIFICATIONS)
            : (mWindowPrivateFlags &
                ~IWindowManagerLayoutParams::PRIVATE_FLAG_WANTS_OFFSET_NOTIFICATIONS);
    if (mCreated) {
        UpdateSurface(FALSE, FALSE, FALSE);
    }
    return NOERROR;
}

ECode WallpaperService::Engine::SetFixedSizeAllowed(
    /* [in] */ Boolean allowed)
{
    mFixedSizeAllowed = allowed;
    return NOERROR;
}

ECode WallpaperService::Engine::OnCreate(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnDestroy()
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* insets)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnOffsetsChanged(
    /* [in] */ Float xOffset,
    /* [in] */ Float yOffset,
    /* [in] */ Float xOffsetStep,
    /* [in] */ Float yOffsetStep,
    /* [in] */ Int32 xPixelOffset,
    /* [in] */ Int32 yPixelOffset)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean resultRequested,
    /* [out] */ IBundle** b)
{
    VALIDATE_NOT_NULL(b)
    *b = NULL;
    return NOERROR;
}

ECode WallpaperService::Engine::OnDesiredSizeChanged(
    /* [in] */ Int32 desiredWidth,
    /* [in] */ Int32 desiredHeight)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnSurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnSurfaceRedrawNeeded(
    /* [in] */ ISurfaceHolder* holder)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnSurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    return NOERROR;
}

ECode WallpaperService::Engine::OnSurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    return NOERROR;
}

ECode WallpaperService::Engine::DispatchPointer(
    /* [in] */ IMotionEvent* event)
{
    Boolean result = FALSE;
    if (event->IsTouchEvent(&result), result) {
        {    AutoLock syncLock(mLock);
            Int32 action;
            if (event->GetAction(&action) == IMotionEvent::ACTION_MOVE) {
                mPendingMove = event;
            } else {
                mPendingMove = NULL;
            }
        }
        AutoPtr<IMessage> msg;
        mCaller->ObtainMessageO(WallpaperService::MSG_TOUCH_EVENT, event, (IMessage**)&msg);
        mCaller->SendMessage(msg);
    } else {
        IInputEvent::Probe(event)->Recycle();
    }
    return NOERROR;
}

ECode WallpaperService::Engine::UpdateSurface(
    /* [in] */ Boolean forceRelayout,
    /* [in] */ Boolean forceReport,
    /* [in] */ Boolean redrawNeeded)
{
    if (mDestroyed) {
        Logger::W(TAG, "Ignoring updateSurface: destroyed");
    }

    Boolean fixedSize = FALSE;
    Int32 myWidth = mSurfaceHolder->GetRequestedWidth();
    if (myWidth <= 0) {
        myWidth = IViewGroupLayoutParams::MATCH_PARENT;
    }
    else {
        fixedSize = TRUE;
    }
    Int32 myHeight = mSurfaceHolder->GetRequestedHeight();
    if (myHeight <= 0) {
        myHeight = IViewGroupLayoutParams::MATCH_PARENT;
    }
    else {
        fixedSize = TRUE;
    }

    const Boolean creating = !mCreated;
    const Boolean surfaceCreating = !mSurfaceCreated;
    const Boolean formatChanged = mFormat != mSurfaceHolder->GetRequestedFormat();
    Boolean sizeChanged = mWidth != myWidth || mHeight != myHeight;
    Boolean insetsChanged = !mCreated;
    const Boolean typeChanged = mType != mSurfaceHolder->GetRequestedType();
    const Boolean flagsChanged = mCurWindowFlags != mWindowFlags ||
            mCurWindowPrivateFlags != mWindowPrivateFlags;

    AutoPtr<IWallpaperEngineWrapper> wew = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
    Boolean shownReported = wew->mShownReported;
    if (forceRelayout || creating || surfaceCreating || formatChanged || sizeChanged
        || typeChanged || flagsChanged || redrawNeeded || !shownReported) {
        if (WallpaperService::DEBUG) {
            Logger::V(TAG, "Changes: creating=%s format=%s size=%s",
                creating ? "TRUE" : "FALSE",
                formatChanged ? "TRUE" : "FALSE",
                sizeChanged ? "TRUE" : "FALSE" );
        }
        // try {
        mWidth = myWidth;
        mHeight = myHeight;
        mFormat = mSurfaceHolder->GetRequestedFormat();
        mType = mSurfaceHolder->GetRequestedType();

        mLayout->SetX(0);
        mLayout->SetY(0);
        IViewGroupLayoutParams::Probe(mLayout)->SetWidth(myWidth);
        IViewGroupLayoutParams::Probe(mLayout)->SetHeight(myHeight);
        mLayout->SetFormat(mFormat);

        mCurWindowFlags = mWindowFlags;
        mLayout->SetFlags(mWindowFlags
                | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
                | IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE);

        mCurWindowPrivateFlags = mWindowPrivateFlags;
        mLayout->SetPrivateFlags(mWindowPrivateFlags);

        mLayout->SetMemoryType(mType);
        mLayout->SetToken(mWindowToken);

        if (!mCreated) {
            // Retrieve watch round and outset info
            AutoPtr<IInterface> _windowService;
            mHost->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&_windowService);
            AutoPtr<IWindowManager> windowService = IWindowManager::Probe(_windowService);

            AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Window);
            const AutoPtr<ITypedArray> windowStyle;
            mHost->ObtainStyledAttributes(attrIds.Get(), (ITypedArray**)&windowStyle);
            const AutoPtr<IDisplay> display;
            windowService->GetDefaultDisplay((IDisplay**)&display);

            Int32 displayid;
            display->GetDisplayId(&displayid);
            const Boolean shouldUseBottomOutset = displayid == IDisplay::DEFAULT_DISPLAY;

            Boolean hasValue = FALSE;
            if (shouldUseBottomOutset && (windowStyle->HasValue(
                    R::styleable::Window_windowOutsetBottom, &hasValue), hasValue)) {
                if (mOutsetBottom == NULL) {
                    CTypedValue::New((ITypedValue**)&mOutsetBottom);
                }
                Boolean result;
                windowStyle->GetValue(R::styleable::Window_windowOutsetBottom,
                        mOutsetBottom, &result);
            } else {
                mOutsetBottom = NULL;
            }
            AutoPtr<IResources> res;
            mHost->GetResources((IResources**)&res);
            res->GetBoolean(R::bool_::config_windowIsRound, &mWindowIsRound);
            windowStyle->Recycle();

            // detect emulator
            mIsEmulator = Build::HARDWARE.Contains("goldfish");
            //TODO some one make mistake here, should write in IViewRootImpl.car file
            //IViewRootImpl::PROPERTY_EMULATOR_CIRCULAR
            SystemProperties::GetBoolean(
                /*IViewRootImpl::PROPERTY_EMULATOR_CIRCULAR*/String("ro.emulator.circular"), FALSE, &mIsCircularEmulator);

            // Add window
            AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)(mIWallpaperEngine.Get());
            mLayout->SetType(_mIWallpaperEngine->mWindowType);
            mLayout->SetGravity(IGravity::START|IGravity::TOP);
            AutoPtr<ICharSequence> title;
            CString::New(String("WallpaperService")/*WallpaperService.this.getClass().getName()*/, (ICharSequence**)&title);
            mLayout->SetTitle(title.Get());
            mLayout->SetWindowAnimations(R::style::Animation_Wallpaper);
            CInputChannel::New((IInputChannel**)&mInputChannel);
            Int32 i;
            mSession->AddToDisplay((IIWindow*)mWindow, mWindow->mSeq, mLayout.Get(), IView::VISIBLE,
                IDisplay::DEFAULT_DISPLAY, mContentInsets, mInputChannel, (IRect**)&mContentInsets, (IInputChannel**)&mInputChannel, &i);
            if (i < 0) {
                Logger::W(TAG, "Failed to add window while updating wallpaper surface.");
                return E_NULL_POINTER_EXCEPTION;
            }
            mCreated = TRUE;

            mInputEventReceiver = new WallpaperInputEventReceiver();
            mInputEventReceiver->constructor(mInputChannel, Looper::GetMyLooper(), this);
        }

        mSurfaceHolder->mSurfaceLock->Lock();
        mDrawingAllowed = TRUE;

        AutoPtr<CWindowManagerLayoutParams> _mLayout = (CWindowManagerLayoutParams*)(mLayout.Get());
        if (!fixedSize) {
            AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)(mIWallpaperEngine.Get());
            _mLayout->mSurfaceInsets->Set(_mIWallpaperEngine->mDisplayPadding);
        } else {
            _mLayout->mSurfaceInsets->Set(0, 0, 0, 0);
        }

        AutoPtr<IRect> winFrame, overscanInsets, contentInsets, visibleInsets, stableInsets;
        AutoPtr<IConfiguration> configuration;
        AutoPtr<ISurface> surface;

        Int32 relayoutResult;
        mSession->Relayout((IIWindow*)mWindow, mWindow->mSeq, mLayout.Get(), mWidth, mHeight,
            IView::VISIBLE, 0, mWinFrame, mOverscanInsets, mContentInsets, mVisibleInsets,
            mStableInsets, mConfiguration, mSurfaceHolder->mSurface, (IRect**)&winFrame,
            (IRect**)&overscanInsets, (IRect**)&contentInsets, (IRect**)&visibleInsets,
            (IRect**)&stableInsets,(IConfiguration**)&configuration,
            &relayoutResult, (ISurface**)&surface);

        if (mWinFrame != NULL && winFrame != NULL)
            mWinFrame->Set(winFrame);
        if (mOverscanInsets != NULL && overscanInsets != NULL)
            mOverscanInsets->Set(overscanInsets);
        if (mContentInsets != NULL && contentInsets != NULL)
            mContentInsets->Set(contentInsets);
        if (mVisibleInsets != NULL && visibleInsets != NULL)
            mVisibleInsets->Set(visibleInsets);
        if (mStableInsets != NULL && stableInsets != NULL)
            mStableInsets->Set(stableInsets);
        if (mConfiguration != NULL && configuration != NULL)
            mConfiguration->SetTo(configuration);

        if (mSurfaceHolder->mSurface != NULL && surface != NULL) {
            mSurfaceHolder->mSurface->TransferFrom(surface);
        }

        if (WallpaperService::DEBUG) {
            String str1, str2;
            IObject::Probe(mSurfaceHolder->mSurface)->ToString(&str1);
            IObject::Probe(mWinFrame)->ToString(&str2);
            Logger::V(WallpaperService::TAG, "New surface: %s, frame=%s", str1.string(), str2.string());
        }

        Int32 w;
        mWinFrame->GetWidth(&w);
        Int32 h;
        mWinFrame->GetHeight(&h);

        if (!fixedSize) {
            AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)(mIWallpaperEngine.Get());
            const AutoPtr<IRect> padding = _mIWallpaperEngine->mDisplayPadding;
            Int32 left, right, top, bottom;
            padding->GetLeft(&left);
            padding->GetRight(&right);
            padding->GetTop(&top);
            padding->GetBottom(&bottom);
            w += left + right;
            h += top + bottom;

            Int32 left2, right2, top2, bottom2;
            mOverscanInsets->GetLeft(&left2);
            mOverscanInsets->SetLeft(left2 + left);
            mOverscanInsets->GetTop(&top2);
            mOverscanInsets->SetTop(top2 + top);
            mOverscanInsets->GetRight(&right2);
            mOverscanInsets->SetRight(right2 + right);
            mOverscanInsets->GetBottom(&bottom2);
            mOverscanInsets->SetBottom(bottom2 + bottom);

            Int32 left3, right3, top3, bottom3;
            mContentInsets->GetLeft(&left3);
            mContentInsets->SetLeft(left3 + left);
            mContentInsets->GetTop(&top3);
            mContentInsets->SetTop(top3 + top);
            mContentInsets->GetRight(&right3);
            mContentInsets->SetRight(right3 + right);
            mContentInsets->GetBottom(&bottom3);
            mContentInsets->SetBottom(bottom3 + bottom);

            Int32 left4, right4, top4, bottom4;
            mStableInsets->GetLeft(&left4);
            mStableInsets->SetLeft(left4 + left);
            mStableInsets->GetTop(&top4);
            mStableInsets->SetTop(top4 + top);
            mStableInsets->GetRight(&right4);
            mStableInsets->SetRight(right4 + right);
            mStableInsets->GetBottom(&bottom4);
            mStableInsets->SetBottom(bottom4 + bottom);
        }

        if (mCurWidth != w) {
            sizeChanged = TRUE;
            mCurWidth = w;
        }
        if (mCurHeight != h) {
            sizeChanged = TRUE;
            mCurHeight = h;
        }

        Boolean result;
        IObject::Probe(mDispatchedOverscanInsets)->Equals(mOverscanInsets, &result);
        insetsChanged |= !result;
        IObject::Probe(mDispatchedContentInsets)->Equals(mContentInsets, &result);
        insetsChanged |= !result;
        IObject::Probe(mDispatchedStableInsets)->Equals(mStableInsets, &result);
        insetsChanged |= !result;

        mSurfaceHolder->SetSurfaceFrameSize(w, h);
        mSurfaceHolder->mSurfaceLock->UnLock();

        Boolean isValid = FALSE;
        mSurfaceHolder->mSurface->IsValid(&isValid);
        if (!isValid) {
            ReportSurfaceDestroyed();
            if (WallpaperService::DEBUG) Logger::V(WallpaperService::TAG, "Layout: Surface destroyed");
            return E_NULL_POINTER_EXCEPTION;
        }

        Boolean didSurface = FALSE;

        // try {
        mSurfaceHolder->UngetCallbacks();

        if (surfaceCreating) {
            mIsCreating = TRUE;
            didSurface = TRUE;
            if (WallpaperService::DEBUG) {
                String str1, str2;
                mSurfaceHolder->ToString(&str1);
                this->ToString(&str2);
                Logger::V(WallpaperService::TAG, "onSurfaceCreated(%s): %s", str1.string(), str2.string());
            }
            OnSurfaceCreated(mSurfaceHolder);
            //ask other man to modify return value: BaseSurfaceHolder:: CARAPI_(Vector<AutoPtr<ISurfaceHolderCallback> >&) GetCallbacks();
            AutoPtr<ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
            if (callbacks != NULL) {
                for (Int32 i = 0; i < callbacks->GetLength(); i++) {
                    AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                    c->SurfaceCreated(mSurfaceHolder);
                }
            }
        }

        redrawNeeded |= creating || (relayoutResult
            & IWindowManagerGlobal::RELAYOUT_RES_FIRST_TIME) != 0;

        if (forceReport || creating || surfaceCreating || formatChanged || sizeChanged) {
            if (WallpaperService::DEBUG) {
                //TODO
                // RuntimeException e = new RuntimeException();
                // e.fillInStackTrace();
                Logger::W(WallpaperService::TAG, "forceReport=%s creating=%s formatChanged=%s sizeChanged=%s",
                    forceReport ? "TRUE" : "FALSE",
                    creating ? "TRUE" : "FALSE",
                    formatChanged ? "TRUE" : "FALSE",
                    sizeChanged ? "TRUE" : "FALSE");
            }
            if (WallpaperService::DEBUG) {
                String str1, str2;
                mSurfaceHolder->ToString(&str1);
                this->ToString(&str2);
                Logger::V(WallpaperService::TAG, "onSurfaceChanged(%s, %d, %d, %d): %s",
                    str1.string(), mFormat, mCurWidth, mCurHeight, str2.string());
            }
            didSurface = TRUE;
            OnSurfaceChanged(mSurfaceHolder, mFormat, mCurWidth, mCurHeight);
            //ask other man to modify return value: BaseSurfaceHolder:: CARAPI_(Vector<AutoPtr<ISurfaceHolderCallback> >&) GetCallbacks();
            AutoPtr<ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
            if (callbacks != NULL) {
                for (Int32 i = 0; i < callbacks->GetLength(); i++) {
                    AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                    c->SurfaceChanged(mSurfaceHolder, mFormat, mCurWidth, mCurHeight);
                }
            }
        }

        if (insetsChanged) {
            mDispatchedOverscanInsets->Set(mOverscanInsets);
            mDispatchedContentInsets->Set(mContentInsets);
            mDispatchedStableInsets->Set(mStableInsets);
            const Boolean isRound = (mIsEmulator && mIsCircularEmulator) || mWindowIsRound;
            mFinalSystemInsets->Set(mDispatchedOverscanInsets);
            mFinalStableInsets->Set(mDispatchedStableInsets);
            if (mOutsetBottom != NULL) {
                AutoPtr<IResources> resources;
                mHost->GetResources((IResources**)&resources);
                const AutoPtr<IDisplayMetrics> metrics;
                resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
                Float dimension;
                mOutsetBottom->GetDimension(metrics, &dimension);
                Int32 b;
                AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)(mIWallpaperEngine.Get());
                _mIWallpaperEngine->mDisplayPadding->GetBottom(&b);
                mFinalSystemInsets->SetBottom((Int32)dimension + b);
            }
            AutoPtr<IWindowInsets> insets;
            CWindowInsets::New(mFinalSystemInsets, NULL, mFinalStableInsets, isRound, (IWindowInsets**)&insets);
            OnApplyWindowInsets(insets);
        }

        if (redrawNeeded) {
            OnSurfaceRedrawNeeded(mSurfaceHolder);
            //ask other man to modify return value: BaseSurfaceHolder:: CARAPI_(Vector<AutoPtr<ISurfaceHolderCallback> >&) GetCallbacks();
            AutoPtr<ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
            if (callbacks != NULL) {
                for (Int32 i = 0; i < callbacks->GetLength(); i++) {
                    AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                    AutoPtr<ISurfaceHolderCallback2> c2 = ISurfaceHolderCallback2::Probe(c);
                    if (c2 != NULL) {
                        c2->SurfaceRedrawNeeded(mSurfaceHolder);
                    }
                }
            }
        }

        if (didSurface && !mReportedVisible) {
            // This wallpaper is currently invisible, but its
            // surface has changed.  At this point let's tell it
            // again that it is invisible in case the report about
            // the surface caused it to start running.  We really
            // don't want wallpapers running when not visible.
            if (mIsCreating) {
                // Some wallpapers will ignore this call if they
                // had previously been told they were invisble,
                // so if we are creating a new surface then toggle
                // the state to get them to notice.
                if (WallpaperService::DEBUG) {
                    String str;
                    this->ToString(&str);
                    Logger::V(WallpaperService::TAG, "onVisibilityChanged(true) at surface: %s", str.string());
                }
                OnVisibilityChanged(TRUE);
            }
            if (WallpaperService::DEBUG) {
                String str;
                this->ToString(&str);
                Logger::V(WallpaperService::TAG, "onVisibilityChanged(false) at surface: %s", str.string());
            }
            OnVisibilityChanged(FALSE);
        }

        // } finally {
        mIsCreating = FALSE;
        mSurfaceCreated = TRUE;
        if (redrawNeeded) {
            mSession->FinishDrawing(mWindow);
        }
        mIWallpaperEngine->ReportShown();
        // }
    // } catch (RemoteException ex) {
    // }
        if (WallpaperService::DEBUG) {
            Int32 x, y, width, height;
            mLayout->GetX(&x);
            mLayout->GetY(&y);
            AutoPtr<IViewGroupLayoutParams> vlp = IViewGroupLayoutParams::Probe(mLayout);
            vlp->GetWidth(&width);
            vlp->GetHeight(&height);
            Logger::V(WallpaperService::TAG, "Layout: x=%d y=%d w=%d h=%d", x, y, width, height);
        }
    }
    return NOERROR;
}

ECode WallpaperService::Engine::Attach(
    /* [in] */ IIWallpaperEngineWrapper* wrapper)
{
    if (WallpaperService::DEBUG) {
        String str1, str2;
        this->ToString(&str1);
        IObject::Probe(wrapper)->ToString(&str2);
        Logger::V(WallpaperService::TAG, "attach: %s wrapper=", str1.string(), str2.string());
    }
    if (mDestroyed) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mIWallpaperEngine = wrapper;
    mCaller = ((IWallpaperEngineWrapper*)wrapper)->mCaller;
    mConnection = ((IWallpaperEngineWrapper*)wrapper)->mConnection;
    mWindowToken = ((IWallpaperEngineWrapper*)wrapper)->mWindowToken;
    mSurfaceHolder->SetSizeFromLayout();
    mInitializing = TRUE;
    mSession = CWindowManagerGlobal::GetWindowSession();

    mWindow->SetSession(mSession);

    AutoPtr<IInterface> obj;
    mHost->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> powerm = IPowerManager::Probe(obj);
    powerm->IsScreenOn(&mScreenOn);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    AutoPtr<IIntent> intent;
    mHost->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);

    if (WallpaperService::DEBUG) {
        String str;
        this->ToString(&str);
        Logger::V(WallpaperService::TAG, "onCreate(): %s", str.string());
    }
    OnCreate(mSurfaceHolder);

    mInitializing = FALSE;
    mReportedVisible = FALSE;
    UpdateSurface(FALSE, FALSE, FALSE);
    return NOERROR;
}

ECode WallpaperService::Engine::DoDesiredSizeChanged(
    /* [in] */ Int32 desiredWidth,
    /* [in] */ Int32 desiredHeight)
{
    if (!mDestroyed) {
        if (WallpaperService::DEBUG) {
            String str;
            this->ToString(&str);
            Logger::V(WallpaperService::TAG, "onDesiredSizeChanged(%d,%d):", desiredWidth, desiredHeight, str.string());
        }
        AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
        _mIWallpaperEngine->mReqWidth = desiredWidth;
        _mIWallpaperEngine->mReqHeight = desiredHeight;
        OnDesiredSizeChanged(desiredWidth, desiredHeight);
        DoOffsetsChanged(TRUE);
    }
    return NOERROR;
}

ECode WallpaperService::Engine::DoDisplayPaddingChanged(
    /* [in] */ IRect* padding)
{
    if (!mDestroyed) {
        if (WallpaperService::DEBUG) {
            String str1, str2;
            IObject::Probe(padding)->ToString(&str1);
            this->ToString(&str2);
            Logger::V(WallpaperService::TAG, "onDisplayPaddingChanged(%s): ", str1.string(), str2.string());
        }
        AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
        Boolean isEquals;
        IObject::Probe(_mIWallpaperEngine->mDisplayPadding)->Equals(padding, &isEquals);
        if (!isEquals) {
            _mIWallpaperEngine->mDisplayPadding->Set(padding);
            UpdateSurface(TRUE, FALSE, FALSE);
        }
    }
    return NOERROR;
}

ECode WallpaperService::Engine::DoVisibilityChanged(
    /* [in] */ Boolean visible)
{
    if (!mDestroyed) {
        mVisible = visible;
        ReportVisibility();
    }
    return NOERROR;
}

ECode WallpaperService::Engine::ReportVisibility()
{
    if (!mDestroyed) {
        Boolean visible = mVisible && mScreenOn;
        if (mReportedVisible != visible) {
            mReportedVisible = visible;
            if (WallpaperService::DEBUG) {
                String str;
                this->ToString(&str);
                Logger::V(WallpaperService::TAG, "onVisibilityChanged(%s): %s",
                    visible ? "TRUE" : "FALSE", str.string());
            }
            if (visible) {
                // If becoming visible, in preview mode the surface
                // may have been destroyed so now we need to make
                // sure it is re-created.
                DoOffsetsChanged(FALSE);
                UpdateSurface(FALSE, FALSE, FALSE);
            }
            OnVisibilityChanged(visible);
        }
    }
    return NOERROR;
}

ECode WallpaperService::Engine::DoOffsetsChanged(
    /* [in] */ Boolean always)
{
    if (mDestroyed) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!always && !mOffsetsChanged) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Float xOffset = 0, yOffset = 0, xOffsetStep = 0, yOffsetStep = 0;
    Boolean sync = FALSE;
    {    AutoLock syncLock(mLock);
        xOffset = mPendingXOffset;
        yOffset = mPendingYOffset;
        xOffsetStep = mPendingXOffsetStep;
        yOffsetStep = mPendingYOffsetStep;
        sync = mPendingSync;
        mPendingSync = FALSE;
        mOffsetMessageEnqueued = FALSE;
    }

    if (mSurfaceCreated) {
        if (mReportedVisible) {
            if (WallpaperService::DEBUG) {
                String str;
                this->ToString(&str);
                Logger::V(WallpaperService::TAG, "Offsets change in %s: %f, %f", str.string(), xOffset, yOffset);
            }
            AutoPtr<IWallpaperEngineWrapper> _mIWallpaperEngine = (IWallpaperEngineWrapper*)mIWallpaperEngine.Get();
            const Int32 availw = _mIWallpaperEngine->mReqWidth - mCurWidth;
            const Int32 xPixels = availw > 0 ? -(Int32)(availw * xOffset + .5f) : 0;
            const Int32 availh = _mIWallpaperEngine->mReqHeight - mCurHeight;
            const Int32 yPixels = availh > 0 ? -(Int32)(availh * yOffset + .5f) : 0;
            OnOffsetsChanged(xOffset, yOffset, xOffsetStep, yOffsetStep, xPixels, yPixels);
        }
        else {
            mOffsetsChanged = TRUE;
        }
    }

    if (sync) {
        if (WallpaperService::DEBUG) Logger::V(WallpaperService::TAG, "Reporting offsets change complete");
        ECode ec = mSession->WallpaperOffsetsComplete(mWindow);
        if (FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode WallpaperService::Engine::DoCommand(
    /* [in] */ IWallpaperCommand* handlecmd)
{
    AutoPtr<WallpaperCommand> cmd = (WallpaperCommand*)handlecmd;
    AutoPtr<IBundle> result;
    if (!mDestroyed) {
        OnCommand(cmd->mAction, cmd->mX, cmd->mY, cmd->mZ,
            cmd->mExtras, cmd->mSync, (IBundle**)&result);
    }
    else {
        result = NULL;
    }
    if (cmd->mSync) {
        if (WallpaperService::DEBUG) Logger::V(WallpaperService::TAG, "Reporting command complete");
        ECode ec = mSession->WallpaperCommandComplete(mWindow, result);
        if (FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode WallpaperService::Engine::ReportSurfaceDestroyed()
{
    if (mSurfaceCreated) {
        mSurfaceCreated = FALSE;
        mSurfaceHolder->UngetCallbacks();
        //ask other man to modify return value: BaseSurfaceHolder:: CARAPI_(Vector<AutoPtr<ISurfaceHolderCallback> >&) GetCallbacks();
        AutoPtr<ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
        if (callbacks != NULL) {
            for (Int32 i = 0; i < callbacks->GetLength(); i++) {
                AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                c->SurfaceDestroyed(mSurfaceHolder);
            }
        }
        if (WallpaperService::DEBUG) {
            String str1, str2;
            mSurfaceHolder->ToString(&str1);
            this->ToString(&str2);
            Logger::V(WallpaperService::TAG, "onSurfaceDestroyed(%s): %s", str1.string(), str2.string());
        }
        OnSurfaceDestroyed(mSurfaceHolder);
    }
    return NOERROR;
}

ECode WallpaperService::Engine::Detach() {
    if (mDestroyed) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mDestroyed = TRUE;

    if (mVisible) {
        mVisible = FALSE;
        if (WallpaperService::DEBUG) {
            String str;
            this->ToString(&str);
            Logger::V(WallpaperService::TAG, "onVisibilityChanged(false): %s", str.string());
        }
        OnVisibilityChanged(FALSE);
    }

    ReportSurfaceDestroyed();

    if (WallpaperService::DEBUG) {
        String str;
        this->ToString(&str);
        Logger::V(WallpaperService::TAG, "onDestroy(): %s", str.string());
    }
    mHost->OnDestroy();

    mHost->UnregisterReceiver(mReceiver);

    if (mCreated) {
        if (WallpaperService::DEBUG) {
            AutoPtr<ISurface> surface;
            mSurfaceHolder->GetSurface((ISurface**)&surface);
            String str1, str2;
            IObject::Probe(surface)->ToString(&str1);
            this->ToString(&str2);
            Logger::V(WallpaperService::TAG, "Removing window and destroying surface %s of: %s", str1.string(), str2.string());
        }

        if (mInputEventReceiver != NULL) {
            mInputEventReceiver->Dispose();
            mInputEventReceiver = NULL;
        }

        ECode ec = mSession->Remove(mWindow);
        if (FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
        mSurfaceHolder->mSurface->ReleaseResources();
        mCreated = FALSE;

        // Dispose the input channel after removing the window so the Window Manager
        // doesn't interpret the input channel being closed as an abnormal termination.
        if (mInputChannel != NULL) {
            mInputChannel->Dispose();
            mInputChannel = NULL;
        }
    }
    return NOERROR;
}

//===============================
//WallpaperService::IWallpaperEngineWrapper
//===============================

CAR_INTERFACE_IMPL_4(WallpaperService::IWallpaperEngineWrapper, Object, IIWallpaperEngine, IHandlerCallerCallback, IBinder, IIWallpaperEngineWrapper)

WallpaperService::IWallpaperEngineWrapper::IWallpaperEngineWrapper()
    : mWindowType(0)
    , mIsPreview(FALSE)
    , mShownReported(FALSE)
    , mReqWidth(0)
    , mReqHeight(0)
{
    CRect::New((IRect**)&mDisplayPadding);
}

ECode WallpaperService::IWallpaperEngineWrapper::constructor(
    /* [in] */ IWallpaperService* context,
    /* [in] */ IIWallpaperConnection* conn,
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 windowType,
    /* [in] */ Boolean isPreview,
    /* [in] */ Int32 reqWidth,
    /* [in] */ Int32 reqHeight,
    /* [in] */ IRect* padding)
{
    mHost = (WallpaperService*)context;
    AutoPtr<ILooper> looper;
    IContext::Probe(context)->GetMainLooper((ILooper**)&looper);
    CHandlerCaller::New(IContext::Probe(context), looper.Get(), this, TRUE, FALSE, (IHandlerCaller**)&mCaller);
    mConnection = conn;
    mWindowToken = windowToken;
    mWindowType = windowType;
    mIsPreview = isPreview;
    mReqWidth = reqWidth;
    mReqHeight = reqHeight;
    mDisplayPadding->Set(padding);

    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(WallpaperService::DO_ATTACH, (IMessage**)&msg);
    mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::SetDesiredSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageII(WallpaperService::DO_SET_DESIRED_SIZE, width, height, (IMessage**)&msg);
    mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::SetDisplayPadding(
    /* [in] */ IRect* padding)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(WallpaperService::DO_SET_DISPLAY_PADDING, padding, (IMessage**)&msg);
    mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::SetVisibility(
    /* [in] */ Boolean visible)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageI(WallpaperService::MSG_VISIBILITY_CHANGED, visible ? 1 : 0, (IMessage**)&msg);
    mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::DispatchPointer(
    /* [in] */ IMotionEvent* event)
{
    if (mEngine != NULL) {
        mEngine->DispatchPointer(event);
    }
    else {
        IInputEvent::Probe(event)->Recycle();
    }
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::DispatchWallpaperCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras)
{
    if (mEngine != NULL) {
        mEngine->mWindow->DispatchWallpaperCommand(action, x, y, z, extras, FALSE);
    }
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::ReportShown()
{
    if (!mShownReported) {
        mShownReported = TRUE;
        ECode ec = mConnection->EngineShown(this);
        if (FAILED(ec)) {
            Logger::W(WallpaperService::TAG, "Wallpaper host disappeared");
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::Destroy()
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(WallpaperService::DO_DETACH, (IMessage**)&msg);
    mCaller->SendMessage(msg);
    return NOERROR;
}

ECode WallpaperService::IWallpaperEngineWrapper::ExecuteMessage(
    /* [in] */ IMessage* message)
{
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WallpaperService::DO_ATTACH: {
            ECode ec = mConnection->AttachEngine(this);
            if (FAILED(ec)) {
                Logger::W(WallpaperService::TAG, "Wallpaper host disappeared");
                return E_REMOTE_EXCEPTION;
            }
            AutoPtr<IWallpaperServiceEngine> engine;
            mHost->OnCreateEngine((IWallpaperServiceEngine**)&engine);
            mEngine = (Engine*)engine.Get();
            mHost->mActiveEngines->Add(engine);
            engine->Attach(this);
            return NOERROR;
        }
        case WallpaperService::DO_DETACH: {
            mHost->mActiveEngines->Remove((IObject*)mEngine);
            mEngine->Detach();
            return NOERROR;
        }
        case WallpaperService::DO_SET_DESIRED_SIZE: {
            Int32 arg1, arg2;
            message->GetArg1(&arg1);
            message->GetArg2(&arg2);
            mEngine->DoDesiredSizeChanged(arg1, arg2);
            return NOERROR;
        }
        case WallpaperService::DO_SET_DISPLAY_PADDING: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IRect> padding = IRect::Probe(obj);
            mEngine->DoDisplayPaddingChanged(padding);
        }
        case WallpaperService::MSG_UPDATE_SURFACE:
            mEngine->UpdateSurface(TRUE, FALSE, FALSE);
            break;
        case WallpaperService::MSG_VISIBILITY_CHANGED: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (WallpaperService::DEBUG) {
                String str;
                mEngine->ToString(&str);
                Logger::V(WallpaperService::TAG, "Visibility change in %s: %d", str.string(), arg1);
            }
            mEngine->DoVisibilityChanged(arg1 != 0);
            break;
        }
        case WallpaperService::MSG_WALLPAPER_OFFSETS: {
            mEngine->DoOffsetsChanged(TRUE);
            break;
        }
        case WallpaperService::MSG_WALLPAPER_COMMAND: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<WallpaperCommand> cmd = (WallpaperCommand*)(IObject::Probe(obj));
            mEngine->DoCommand(cmd);
            break;
        }
        case WallpaperService::MSG_WINDOW_RESIZED: {
            Int32 arg1;
            message->GetArg1(&arg1);
            const Boolean reportDraw = arg1 != 0;
            mEngine->UpdateSurface(TRUE, FALSE, reportDraw);
            mEngine->DoOffsetsChanged(TRUE);
            break;
        }
        case WallpaperService::MSG_WINDOW_MOVED: {
            // Do nothing. What does it mean for a Wallpaper to move?
            break;
        }
        case WallpaperService::MSG_TOUCH_EVENT: {
            Boolean skip = FALSE;
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IMotionEvent> ev = IMotionEvent::Probe(obj);
            Int32 action;
            ev->GetAction(&action);
            if (action == IMotionEvent::ACTION_MOVE) {
                {    AutoLock syncLock(this);
                    if (mEngine->mPendingMove == ev) {
                        mEngine->mPendingMove = NULL;
                    }
                    else {
                        // this is not the motion event we are looking for....
                        skip = TRUE;
                    }
                }
            }
            if (!skip) {
                if (WallpaperService::DEBUG) {
                    String str;
                    IObject::Probe(ev)->ToString(&str);
                    Logger::V(WallpaperService::TAG, "Delivering touch event: %s", str.string());
                }
                mEngine->OnTouchEvent(ev);
            }
            IInputEvent::Probe(ev)->Recycle();
            break;
        }
        default :
            Logger::W(WallpaperService::TAG, "Unknown message type %d", what);
    }
    return NOERROR;
}

//===============================
//WallpaperService::IWallpaperServiceWrapper
//===============================

CAR_INTERFACE_IMPL_3(WallpaperService::IWallpaperServiceWrapper, Object, IIWallpaperService, IBinder, IIWallpaperServiceWrapper)

WallpaperService::IWallpaperServiceWrapper::IWallpaperServiceWrapper()
{
}

ECode WallpaperService::IWallpaperServiceWrapper::constructor(
    /* [in] */ IWallpaperService* context)
{
    mTarget = context;
    return NOERROR;
}

ECode WallpaperService::IWallpaperServiceWrapper::Attach(
    /* [in] */ IIWallpaperConnection* conn,
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 windowType,
    /* [in] */ Boolean isPreview,
    /* [in] */ Int32 reqWidth,
    /* [in] */ Int32 reqHeight,
    /* [in] */ IRect* padding)
{
    return CIWallpaperEngineWrapper::New(mTarget, conn, windowToken,
        windowType, isPreview, reqWidth, reqHeight, padding,
        (IIWallpaperEngineWrapper**)&mWallpaperEngineWrapper);
}

//===============================
//WallpaperService
//===============================

const String WallpaperService::TAG("WallpaperService");
const Boolean WallpaperService::DEBUG = FALSE;

const Int32 WallpaperService::DO_ATTACH;
const Int32 WallpaperService::DO_DETACH;
const Int32 WallpaperService::DO_SET_DESIRED_SIZE;
const Int32 WallpaperService::DO_SET_DISPLAY_PADDING;

const Int32 WallpaperService::MSG_UPDATE_SURFACE;
const Int32 WallpaperService::MSG_VISIBILITY_CHANGED;
const Int32 WallpaperService::MSG_WALLPAPER_OFFSETS;
const Int32 WallpaperService::MSG_WALLPAPER_COMMAND;
const Int32 WallpaperService::MSG_WINDOW_RESIZED;
const Int32 WallpaperService::MSG_WINDOW_MOVED;
const Int32 WallpaperService::MSG_TOUCH_EVENT;

CAR_INTERFACE_IMPL(WallpaperService, Service, IWallpaperService)

WallpaperService::WallpaperService()
{
    CArrayList::New((IArrayList**)&mActiveEngines);
}

ECode WallpaperService::constructor()
{
    return NOERROR;
}

ECode WallpaperService::OnCreate()
{
    return Service::OnCreate();
}

ECode WallpaperService::OnDestroy()
{
    FAIL_RETURN(Service::OnDestroy());
    Int32 size;
    mActiveEngines->GetSize(&size);
    for (Int32 i=0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mActiveEngines->Get(i, (IInterface**)&obj);
        AutoPtr<IWallpaperServiceEngine> engine = IWallpaperServiceEngine::Probe(obj);
        engine->Detach();
    }
    mActiveEngines->Clear();
    return NOERROR;
}

ECode WallpaperService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    CIWallpaperServiceWrapper::New(this, b);
    return NOERROR;
}

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos
