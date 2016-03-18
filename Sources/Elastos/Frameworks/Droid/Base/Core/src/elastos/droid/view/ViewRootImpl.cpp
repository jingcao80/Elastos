
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/Surface.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/view/DragEvent.h"
#include "elastos/droid/view/ViewTreeObserver.h"
#include "elastos/droid/view/Choreographer.h"
#include "elastos/droid/view/CInputChannel.h"
#include "elastos/droid/view/CInputQueue.h"
#include "elastos/droid/view/FocusFinder.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/ThreadedRenderer.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CSurface.h"
#include "elastos/droid/view/CWindowInsets.h"
#include "elastos/droid/view/CViewRootImplW.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/CAccessibilityInteractionController.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/CAccessibilityInteractionConnection.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/graphics/CPointF.h"
#include "elastos/droid/graphics/PixelFormat.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/widget/CScroller.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::CCompatibilityInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::PixelFormat;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Internal::View::IRootViewSurfaceTaker;
using Elastos::Droid::Internal::View::EIID_IRootViewSurfaceTaker;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::View::Surface;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CInputChannel;
using Elastos::Droid::View::CInputQueue;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::DragEvent;
using Elastos::Droid::View::FocusFinder;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::CWindowInsets;
using Elastos::Droid::View::CViewRootImplW;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::ThreadedRenderer;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::CAccessibilityInteractionController;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnection;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::InputMethod::EIID_IInputMethodManagerFinishedInputEventCallback;
using Elastos::Droid::View::EIID_IInputQueueFinishedInputEventCallback;
using Elastos::Droid::View::Animation::IAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityManagerHighTextContrastChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::ViewTreeObserver;
using Elastos::Droid::Widget::CScroller;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::CString;
using Elastos::Core::Thread;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;

#ifndef TRACE_IN_TERMINAL
#define TRACE_IN_TERMINAL          0
#endif

namespace Elastos {
namespace Droid {
namespace View {

static void LocalTrace(IView* view, const char* info)
{
#if TRACE_IN_TERMINAL
    Int32 viewId;
    view->GetId(&viewId);
    if (viewId != 0x7f0700a2 /* content_parent */) {
        printf("%s\n", info);
        Logger::D("ViewRootImpl", "%s view:%08x", info, viewId);
    }
#endif
}

const Int32 ViewRootImpl::MSG_INVALIDATE = 1;
const Int32 ViewRootImpl::MSG_INVALIDATE_RECT = 2;
const Int32 ViewRootImpl::MSG_DIE = 3;
const Int32 ViewRootImpl::MSG_RESIZED = 4;
const Int32 ViewRootImpl::MSG_RESIZED_REPORT = 5;
const Int32 ViewRootImpl::MSG_WINDOW_FOCUS_CHANGED = 6;
const Int32 ViewRootImpl::MSG_DISPATCH_INPUT_EVENT = 7;
const Int32 ViewRootImpl::MSG_DISPATCH_APP_VISIBILITY = 8;
const Int32 ViewRootImpl::MSG_DISPATCH_GET_NEW_SURFACE = 9;
const Int32 ViewRootImpl::MSG_DISPATCH_KEY_FROM_IME = 11;
const Int32 ViewRootImpl::MSG_FINISH_INPUT_CONNECTION = 12;
const Int32 ViewRootImpl::MSG_CHECK_FOCUS = 13;
const Int32 ViewRootImpl::MSG_CLOSE_SYSTEM_DIALOGS = 14;
const Int32 ViewRootImpl::MSG_DISPATCH_DRAG_EVENT = 15;
const Int32 ViewRootImpl::MSG_DISPATCH_DRAG_LOCATION_EVENT = 16;
const Int32 ViewRootImpl::MSG_DISPATCH_SYSTEM_UI_VISIBILITY = 17;
const Int32 ViewRootImpl::MSG_UPDATE_CONFIGURATION = 18;
const Int32 ViewRootImpl::MSG_PROCESS_INPUT_EVENTS = 19;
const Int32 ViewRootImpl::MSG_CLEAR_ACCESSIBILITY_FOCUS_HOST = 21;
const Int32 ViewRootImpl::MSG_DISPATCH_DONE_ANIMATING = 22;
const Int32 ViewRootImpl::MSG_INVALIDATE_WORLD = 23;
const Int32 ViewRootImpl::MSG_WINDOW_MOVED = 24;
const Int32 ViewRootImpl::MSG_SYNTHESIZE_INPUT_EVENT = 25;

const Int32 ViewRootImpl::InputStage::FORWARD = 0;
const Int32 ViewRootImpl::InputStage::FINISH_HANDLED = 1;
const Int32 ViewRootImpl::InputStage::FINISH_NOT_HANDLED = 2;

const Int32 ViewRootImpl::AsyncInputStage::DEFER = 3;

//=======================================================================================
// ViewRootImpl::SystemUiVisibilityInfo
//=======================================================================================
CAR_INTERFACE_IMPL(ViewRootImpl::SystemUiVisibilityInfo, Object, ISystemUiVisibilityInfo)

ViewRootImpl::SystemUiVisibilityInfo::SystemUiVisibilityInfo()
    : mSeq(0)
    , mGlobalVisibility(0)
    , mLocalValue(0)
    , mLocalChanges(0)
{}

//=======================================================================================
// ViewRootImpl::ViewRootHandler
//=======================================================================================
ViewRootImpl::ViewRootHandler::ViewRootHandler(
    /* [in] */ ViewRootImpl* host)
{
    host->GetWeakReference((IWeakReference**)&mWeakHost);
}

String ViewRootImpl::ViewRootHandler::GetMessageNameImpl(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case ViewRootImpl::MSG_INVALIDATE:
            return String("MSG_INVALIDATE");
        case ViewRootImpl::MSG_INVALIDATE_RECT:
            return String("MSG_INVALIDATE_RECT");
        case ViewRootImpl::MSG_DIE:
            return String("MSG_DIE");
        case ViewRootImpl::MSG_RESIZED:
            return String("MSG_RESIZED");
        case ViewRootImpl::MSG_RESIZED_REPORT:
            return String("MSG_RESIZED_REPORT");
        case ViewRootImpl::MSG_WINDOW_FOCUS_CHANGED:
            return String("MSG_WINDOW_FOCUS_CHANGED");
        case ViewRootImpl::MSG_DISPATCH_INPUT_EVENT:
            return String("MSG_DISPATCH_INPUT_EVENT");
        case ViewRootImpl::MSG_DISPATCH_APP_VISIBILITY:
            return String("MSG_DISPATCH_APP_VISIBILITY");
        case ViewRootImpl::MSG_DISPATCH_GET_NEW_SURFACE:
            return String("MSG_DISPATCH_GET_NEW_SURFACE");
        case ViewRootImpl::MSG_DISPATCH_KEY_FROM_IME:
            return String("MSG_DISPATCH_KEY_FROM_IME");
        case ViewRootImpl::MSG_FINISH_INPUT_CONNECTION:
            return String("MSG_FINISH_INPUT_CONNECTION");
        case ViewRootImpl::MSG_CHECK_FOCUS:
            return String("MSG_CHECK_FOCUS");
        case ViewRootImpl::MSG_CLOSE_SYSTEM_DIALOGS:
            return String("MSG_CLOSE_SYSTEM_DIALOGS");
        case ViewRootImpl::MSG_DISPATCH_DRAG_EVENT:
            return String("MSG_DISPATCH_DRAG_EVENT");
        case ViewRootImpl::MSG_DISPATCH_DRAG_LOCATION_EVENT:
            return String("MSG_DISPATCH_DRAG_LOCATION_EVENT");
        case ViewRootImpl::MSG_DISPATCH_SYSTEM_UI_VISIBILITY:
            return String("MSG_DISPATCH_SYSTEM_UI_VISIBILITY");
        case ViewRootImpl::MSG_UPDATE_CONFIGURATION:
            return String("MSG_UPDATE_CONFIGURATION");
        case ViewRootImpl::MSG_PROCESS_INPUT_EVENTS:
            return String("MSG_PROCESS_INPUT_EVENTS");
        case ViewRootImpl::MSG_CLEAR_ACCESSIBILITY_FOCUS_HOST:
            return String("MSG_CLEAR_ACCESSIBILITY_FOCUS_HOST");
        case ViewRootImpl::MSG_DISPATCH_DONE_ANIMATING:
            return String("MSG_DISPATCH_DONE_ANIMATING");
        case ViewRootImpl::MSG_WINDOW_MOVED:
            return String("MSG_WINDOW_MOVED");
        case MSG_SYNTHESIZE_INPUT_EVENT:
            return String("MSG_SYNTHESIZE_INPUT_EVENT");
    }
    return Handler::GetMessageNameImpl(msg);
}

ECode ViewRootImpl::ViewRootHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IViewParent> hostObj;
    mWeakHost->Resolve(EIID_IViewParent, (IInterface**)&hostObj);
    if (NULL == hostObj.Get()) {
        if (ViewRootImpl::LOCAL_LOGV) {
            String msgInfo = GetMessageNameImpl(msg);
            Slogger::D(ViewRootImpl::TAG, "ViewRootImpl has been destroyed, ignore message: %s.",
                msgInfo.string());
        }
        return NOERROR;
    }

    AutoPtr<ViewRootImpl> mHost = (ViewRootImpl*)hostObj.Get();

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case ViewRootImpl::MSG_INVALIDATE: {
            IView* view = IView::Probe(obj);
            view->Invalidate();
            break;
        }
        case ViewRootImpl::MSG_INVALIDATE_RECT: {
            View::AttachInfo::InvalidateInfo* info = (View::AttachInfo::InvalidateInfo*)IObject::Probe(obj);
            info->mTarget->Invalidate(info->mLeft, info->mTop, info->mRight, info->mBottom);
            info->Recycle();
            break;
        }
        case ViewRootImpl::MSG_PROCESS_INPUT_EVENTS: {
            mHost->mProcessInputEventsScheduled = FALSE;
            mHost->DoProcessInputEvents();
            break;
        }
        case ViewRootImpl::MSG_DISPATCH_APP_VISIBILITY: {
            mHost->HandleAppVisibility(arg1 != 0);
            break;
        }
        case ViewRootImpl::MSG_DISPATCH_GET_NEW_SURFACE: {
            mHost->HandleGetNewSurface();
            break;
        }
        case ViewRootImpl::MSG_RESIZED:  {
            // Recycled in the fall through...
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            Boolean bval;
            if ((mHost->mWinFrame->Equals(args->mArg1, &bval), bval)
                    && (mHost->mPendingOverscanInsets->Equals(args->mArg5, &bval), bval)
                    && (mHost->mPendingContentInsets->Equals(args->mArg2, &bval), bval)
                    && (mHost->mPendingStableInsets->Equals(args->mArg6, &bval), bval)
                    && (mHost->mPendingVisibleInsets->Equals(args->mArg3, &bval), bval)
                    && args->mArg4 == NULL) {
                args->Recycle();
                break;
            }
        } // fall through...
        case ViewRootImpl::MSG_RESIZED_REPORT: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            if (mHost->mAdded) {
                IConfiguration* config = IConfiguration::Probe(args->mArg4);
                if (config != NULL) {
                    mHost->UpdateConfiguration(config, FALSE);
                }

                mHost->mWinFrame->Set(IRect::Probe(args->mArg1));
                mHost->mPendingOverscanInsets->Set(IRect::Probe(args->mArg5));
                mHost->mPendingContentInsets->Set(IRect::Probe(args->mArg2));
                mHost->mPendingStableInsets->Set(IRect::Probe(args->mArg6));
                mHost->mPendingVisibleInsets->Set(IRect::Probe(args->mArg3));

                if (what == MSG_RESIZED_REPORT) {
                    mHost->mReportNextDraw = TRUE;
                }

                if (mHost->mView != NULL) {
                    mHost->ForceLayout(mHost->mView);
                }

                mHost->RequestLayout();
            }

            args->Recycle();
            break;
        }
        case ViewRootImpl::MSG_WINDOW_MOVED: {
            if (mHost->mAdded) {
                Int32 w, h;
                mHost->mWinFrame->GetWidth(&w);
                mHost->mWinFrame->GetHeight(&h);
                Int32 l = arg1;
                Int32 t = arg2;
                mHost->mWinFrame->Set(l, t, l + w, t + h);

                if (mHost->mView != NULL) {
                    mHost->ForceLayout(mHost->mView);
                }
                mHost->RequestLayout();
            }
            break;
        }
        case ViewRootImpl::MSG_WINDOW_FOCUS_CHANGED: {
            Boolean hasWindowFocus = arg1 != 0;
            Boolean inTouchMode = arg2 != 0;
            mHost->HandleWindowFocusChanged(hasWindowFocus, inTouchMode);
            break;
        }
        case ViewRootImpl::MSG_DIE: {
            mHost->DoDie();
            break;
        }
        case ViewRootImpl::MSG_DISPATCH_INPUT_EVENT: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            AutoPtr<IInputEvent> event = IInputEvent::Probe(args->mArg1);
            AutoPtr<IInputEventReceiver> receiver = IInputEventReceiver::Probe(args->mArg2);
            mHost->EnqueueInputEvent(event, receiver, 0, TRUE);
            args->Recycle();
            break;
        }
        case ViewRootImpl::MSG_SYNTHESIZE_INPUT_EVENT: {
            AutoPtr<IInputEvent> event = IInputEvent::Probe(obj);
            mHost->EnqueueInputEvent(event, NULL, QueuedInputEvent::FLAG_UNHANDLED, TRUE);
            break;
        }

        case ViewRootImpl::MSG_DISPATCH_KEY_FROM_IME: {
            IKeyEvent* event = IKeyEvent::Probe(obj);

            if (ViewRootImpl::LOCAL_LOGV) {
                Logger::V(ViewRootImpl::TAG, "Dispatching key 0x%08x  from IME to 0x%08x",
                    event, mHost->mView.Get());
            }

            Int32 flags;
            event->GetFlags(&flags);
            AutoPtr<IKeyEvent> newEvent;
            if ((flags & IKeyEvent::FLAG_FROM_SYSTEM) != 0) {
                // The IME is trying to say this event is from the
                // system!  Bad bad bad!
                //noinspection UnusedAssignment
                CKeyEvent::ChangeFlags(
                    event, flags & ~IKeyEvent::FLAG_FROM_SYSTEM, (IKeyEvent**)&newEvent);
            }
            else {
                newEvent = event;
            }

            mHost->EnqueueInputEvent(IInputEvent::Probe(newEvent), NULL, QueuedInputEvent::FLAG_DELIVER_POST_IME, TRUE);
            break;
        }
        case ViewRootImpl::MSG_FINISH_INPUT_CONNECTION: {
            AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
            if (imm != NULL) {
                imm->ReportFinishInputConnection(IInputConnection::Probe(obj));
            }
            break;
        }
        case ViewRootImpl::MSG_CHECK_FOCUS: {
            AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
            if (imm != NULL) {
                imm->CheckFocus();
            }
            break;
        }
        case ViewRootImpl::MSG_CLOSE_SYSTEM_DIALOGS: {
            if (mHost->mView != NULL) {
                ICharSequence* seq = ICharSequence::Probe(obj);
                String info;
                seq->ToString(&info);
                mHost->mView->OnCloseSystemDialogs(info);
            }
            break;
        }
        case ViewRootImpl::MSG_DISPATCH_DRAG_EVENT:
        case ViewRootImpl::MSG_DISPATCH_DRAG_LOCATION_EVENT: {
            IDragEvent* event = IDragEvent::Probe(obj);
            event->SetLocalState(mHost->mLocalDragState);    // only present when this app called startDrag()
            mHost->HandleDragEvent(event);
            break;
        }
        case ViewRootImpl::MSG_DISPATCH_SYSTEM_UI_VISIBILITY: {
            SystemUiVisibilityInfo* info = (SystemUiVisibilityInfo*)IObject::Probe(obj);
            mHost->HandleDispatchSystemUiVisibilityChanged(info);
            break;
        }
        case ViewRootImpl::MSG_UPDATE_CONFIGURATION: {
            AutoPtr<IConfiguration> config = IConfiguration::Probe(obj);
            Boolean result;
            if (config->IsOtherSeqNewer(mHost->mLastConfiguration, &result), result) {
                config = mHost->mLastConfiguration;
            }
            mHost->UpdateConfiguration(config, FALSE);
            break;
        }
        case ViewRootImpl::MSG_CLEAR_ACCESSIBILITY_FOCUS_HOST: {
            mHost->SetAccessibilityFocus(NULL, NULL);
            break;
        }
        case ViewRootImpl::MSG_DISPATCH_DONE_ANIMATING: {
            mHost->HandleDispatchDoneAnimating();
            break;
        }
        case ViewRootImpl::MSG_INVALIDATE_WORLD: {
            if (mHost->mView != NULL) {
                mHost->InvalidateWorld(mHost->mView);
            }
            break;
        }
    }
    return NOERROR;
}

//=======================================================================================
// ViewRootImpl::TraversalRunnable
//=======================================================================================
ViewRootImpl::TraversalRunnable::TraversalRunnable(
    /* [in] */ IWeakReference* host)
    : mHost(host)
{}

ECode ViewRootImpl::TraversalRunnable::Run()
{
    AutoPtr<IInterface> obj;
    mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IViewRootImpl> viewRoot = IViewRootImpl::Probe(obj);
        viewRoot->DoTraversal();
    }

    return NOERROR;
}

//=======================================================================================
// ViewRootImpl::WindowInputEventReceiver
//=======================================================================================
ECode ViewRootImpl::WindowInputEventReceiver::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ IWeakReference* viewRootImpl)
{
    mHost = viewRootImpl;
    return InputEventReceiver::constructor(inputChannel, looper);
}

ECode ViewRootImpl::WindowInputEventReceiver::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    if (mHost != NULL) {
        AutoPtr<IInterface> obj;
        mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (obj) {
            AutoPtr<IViewRootImpl> viewRoot = IViewRootImpl::Probe(obj);
            viewRoot->EnqueueInputEvent(event, this, 0, TRUE);
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::WindowInputEventReceiver::OnBatchedInputEventPending()
{
    if (mHost != NULL) {
        AutoPtr<IInterface> obj;
        mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (obj) {
            AutoPtr<ViewRootImpl> viewRoot = VIEWIMPL_PROBE(obj);
            if (viewRoot->mUnbufferedInputDispatch) {
                IInputEventReceiver::OnBatchedInputEventPending();
            } else {
                viewRoot->ScheduleConsumeBatchedInput();
            }
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::WindowInputEventReceiver::Dispose()
{
    if (mHost != NULL) {
        AutoPtr<IInterface> obj;
        mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (obj) {
            AutoPtr<ViewRootImpl> viewRoot = VIEWIMPL_PROBE(obj);
            viewRoot->UnscheduleConsumeBatchedInput();
            mHost = NULL;
        }
    }

    return InputEventReceiver::Dispose();
}

ECode ViewRootImpl::WindowInputEventReceiver::FinishInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Boolean handled)
{
    return NOERROR;
}

ECode ViewRootImpl::WindowInputEventReceiver::ConsumeBatchedInputEvents(
    /* [in] */ Int64 frameTimeNanos,
    /* [in] */ Boolean* result)
{
    return NOERROR;
}

//=======================================================================================
// ViewRootImpl::ConsumeBatchedInputRunnable
//=======================================================================================
ViewRootImpl::ConsumeBatchedInputRunnable::ConsumeBatchedInputRunnable(
    /* [in] */ IWeakReference* host)
    : mHost(host)
{}

ECode ViewRootImpl::ConsumeBatchedInputRunnable::Run()
{
    AutoPtr<IInterface> obj;
    mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj) {
        Int64 timeNanos;
        AutoPtr<ViewRootImpl> viewRoot = VIEWIMPL_PROBE(obj);
        viewRoot->mChoreographer->GetFrameTimeNanos(&timeNanos);
        viewRoot->DoConsumeBatchedInput(timeNanos);
    }

    return NOERROR;
}

//=======================================================================================
// ViewRootImpl::InvalidateOnAnimationRunnable
//=======================================================================================
ViewRootImpl::InvalidateOnAnimationRunnable::InvalidateOnAnimationRunnable(
    /* [in] */ IWeakReference* host)
    : mPosted(FALSE)
    , mHost(host)
{}

void ViewRootImpl::InvalidateOnAnimationRunnable::AddView(
    /* [in] */ IView* view)
{
    AutoLock lock(mSelfLock);
    mViews.PushBack(view);
    PostIfNeededLocked();
}

void ViewRootImpl::InvalidateOnAnimationRunnable::AddViewRect(
    /* [in] */ View::AttachInfo::InvalidateInfo* info)
{
    AutoLock lock(mSelfLock);
    mViewRects.PushBack(info);
    PostIfNeededLocked();
}

void ViewRootImpl::InvalidateOnAnimationRunnable::RemoveView(
    /* [in] */ IView* view)
{
    AutoLock lock(mSelfLock);
    mViews.Remove(view);

    typedef typename List<AutoPtr<View::AttachInfo::InvalidateInfo> >::ReverseIterator InfoReverseIterator;
    InfoReverseIterator iter;
    for (iter = mViewRects.RBegin(); iter != mViewRects.REnd();) {
        AutoPtr<View::AttachInfo::InvalidateInfo> info = *iter;
        if (info->mTarget.Get() == view) {
            info->Recycle();
            iter = InfoReverseIterator(mViewRects.Erase(--(iter.GetBase())));
        }
        else {
            ++iter;
        }
    }

    AutoPtr<IInterface> obj;
    mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (mPosted && mViews.IsEmpty() && mViewRects.IsEmpty() && obj != NULL) {
        AutoPtr<IViewRootImpl> viewRoot = IViewRootImpl::Probe(obj);
        VIEWIMPL_PROBE(viewRoot)->mChoreographer->RemoveCallbacks(
            IChoreographer::CALLBACK_ANIMATION, IRunnable::Probe(this), NULL);
        mPosted = FALSE;
    }
}

//@Override
ECode ViewRootImpl::InvalidateOnAnimationRunnable::Run()
{
    Int32 viewCount;
    Int32 viewRectCount;
    {
        AutoLock lock(mSelfLock);
        mPosted = FALSE;

        viewCount = mViews.GetSize();
        if (viewCount != 0) {
            if (mTempViews == NULL || mTempViews->GetLength() < viewCount) {
                mTempViews = ArrayOf<IView*>::Alloc(viewCount);
            }

            List<AutoPtr<IView> >::Iterator iter = mViews.Begin();
            for (Int32 i = 0; iter != mViews.End(); ++iter, ++i) {
                mTempViews->Set(i, iter->Get());
            }
            mViews.Clear();
        }

        viewRectCount = mViewRects.GetSize();
        if (viewRectCount != 0) {
            if (mTempViewRects == NULL || mTempViewRects->GetLength() < viewRectCount) {
                mTempViewRects = ArrayOf<View::AttachInfo::InvalidateInfo*>::Alloc(viewRectCount);
            }

            List<AutoPtr<View::AttachInfo::InvalidateInfo> >::Iterator iter = mViewRects.Begin();
            for (Int32 i = 0; iter != mViewRects.End(); ++iter, ++i) {
                mTempViewRects->Set(i, iter->Get());
            }
            mViewRects.Clear();
        }
    }

    for (Int32 i = 0; i < viewCount; i++) {
        (*mTempViews)[i]->Invalidate();
        mTempViews->Set(i, NULL);
    }

    for (Int32 i = 0; i < viewRectCount; i++) {
        View::AttachInfo::InvalidateInfo* info = (*mTempViewRects)[i];
        info->mTarget->Invalidate(info->mLeft, info->mTop, info->mRight, info->mBottom);
        info->Recycle();
    }

    return NOERROR;
}

void ViewRootImpl::InvalidateOnAnimationRunnable::PostIfNeededLocked()
{
    if (!mPosted) {
        AutoPtr<IInterface> obj;
        mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (mPosted == FALSE && obj != NULL) {
            AutoPtr<ViewRootImpl> viewRoot = VIEWIMPL_PROBE(obj);
            viewRoot->mChoreographer->PostCallback(
                IChoreographer::CALLBACK_ANIMATION, this, NULL);
            mPosted = TRUE;
        }
    }
}

//=======================================================================================
// ViewRootImpl::TakenSurfaceHolder
//=======================================================================================
ViewRootImpl::TakenSurfaceHolder::TakenSurfaceHolder(
    /* [in] */ ViewRootImpl* viewRoot) :
    mViewRoot(viewRoot)
{
}

Boolean ViewRootImpl::TakenSurfaceHolder::OnAllowLockCanvas()
{
    return mViewRoot->mDrawingAllowed;
}

ECode ViewRootImpl::TakenSurfaceHolder::OnRelayoutContainer()
{
    return NOERROR;
    // Not currently interesting -- from changing between fixed and layout size.
}

ECode ViewRootImpl::TakenSurfaceHolder::SetFormat(
    /* [in] */ Int32 format)
{
    assert(mViewRoot->mView.Get());
    IRootViewSurfaceTaker* rvst =
        (IRootViewSurfaceTaker*)(mViewRoot->mView)->Probe(EIID_IRootViewSurfaceTaker);
    assert(rvst);

    return rvst->SetSurfaceFormat(format);
}

ECode ViewRootImpl::TakenSurfaceHolder::SetType(
    /* [in] */ Int32 type)
{
    assert(mViewRoot->mView.Get());
    IRootViewSurfaceTaker* rvst =
        (IRootViewSurfaceTaker*)(mViewRoot->mView)->Probe(EIID_IRootViewSurfaceTaker);
    assert(rvst);

    return rvst->SetSurfaceType(type);
}

ECode ViewRootImpl::TakenSurfaceHolder::OnUpdateSurface()
{
    // We take care of format and type changes on our own.
    Logger::E(ViewRootImpl::TAG, "Shouldn't be here");

    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ViewRootImpl::TakenSurfaceHolder::IsCreating(
    /* [out] */ Boolean* result)
{
    if (result == NULL) {
        return E_INVALID_ARGUMENT;
    }
    *result = mViewRoot->mIsCreating;

    return NOERROR;
}

ECode ViewRootImpl::TakenSurfaceHolder::SetFixedSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Logger::E(ViewRootImpl::TAG, "Currently only support sizing from layout");

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ViewRootImpl::TakenSurfaceHolder::SetKeepScreenOn(
    /* [in] */ Boolean screenOn)
{
    assert(mViewRoot->mView.Get());
    IRootViewSurfaceTaker* rvst =
        (IRootViewSurfaceTaker*)(mViewRoot->mView)->Probe(EIID_IRootViewSurfaceTaker);
    assert(rvst);

    return rvst->SetSurfaceKeepScreenOn(screenOn);
}

//=======================================================================================
// ViewRootImpl::TrackballAxis
//=======================================================================================
const Float ViewRootImpl::TrackballAxis::MAX_ACCELERATION = 20;
const Int64 ViewRootImpl::TrackballAxis::FAST_MOVE_TIME = 150;
const Float ViewRootImpl::TrackballAxis::ACCEL_MOVE_SCALING_FACTOR = (1.0f/40);
const Float FIRST_MOVEMENT_THRESHOLD = 0.5f;
const Float SECOND_CUMULATIVE_MOVEMENT_THRESHOLD = 2.0f;
const Float SUBSEQUENT_INCREMENTAL_MOVEMENT_THRESHOLD = 1.0f;

ViewRootImpl::TrackballAxis::TrackballAxis()
    : mPosition(0)
    , mAcceleration(1)
    , mLastMoveTime(0)
    , mStep(0)
    , mDir(0)
    , mNonAccelMovement(0)
{
}

/**
 * Add trackball movement into the state.  If the direction of movement
 * has been reversed, the state is reset before adding the
 * movement (so that you don't have to compensate for any previously
 * collected movement before see the result of the movement in the
 * new direction).
 *
 * @return Returns the absolute value of the amount of movement
 * collected so far.
 */
void ViewRootImpl::TrackballAxis::Reset(
    /* [in] */ Int32 _step)
{
    mPosition = 0;
    mAcceleration = 1;
    mLastMoveTime = 0;
    mStep = _step;
    mDir = 0;
}

Float ViewRootImpl::TrackballAxis::Collect(
    /* [in] */ Float off,
    /* [in] */ Int64 time,
    /* [in] */ const char* axis)
{
    Int64 normTime;
    if (off > 0) {
        normTime = (Int64)(off * FAST_MOVE_TIME);
        if (mDir < 0) {
            if (ViewRootImpl::DEBUG_TRACKBALL)
                Logger::D(ViewRootImpl::TAG, "%s reversed to positive!", axis);
            mPosition = 0;
            mStep = 0;
            mAcceleration = 1;
            mLastMoveTime = 0;
        }
        mDir = 1;
    }
    else if (off < 0) {
        normTime = (Int64)((-off) * FAST_MOVE_TIME);
        if (mDir > 0) {
            if (ViewRootImpl::DEBUG_TRACKBALL)
                Logger::D(ViewRootImpl::TAG, "%s reversed to negative!", axis);
            mPosition = 0;
            mStep = 0;
            mAcceleration = 1;
            mLastMoveTime = 0;
        }
        mDir = -1;
    }
    else {
        normTime = 0;
    }

    // The number of milliseconds between each movement that is
    // considered "normal" and will not result in any acceleration
    // or deceleration, scaled by the offset we have here.
    if (normTime > 0) {
        Int64 delta = time - mLastMoveTime;
        mLastMoveTime = time;
        Float acc = mAcceleration;
        if (delta < normTime) {
            // The user is scrolling rapidly, so increase acceleration.
            Float scale = (normTime-delta) * ACCEL_MOVE_SCALING_FACTOR;
            if (scale > 1) acc *= scale;
            if (ViewRootImpl::DEBUG_TRACKBALL) {
                //Logger::D(ViewRootImpl::TAG, StringBuffer(axis) + " accelerate: off="
                //+ off + " normTime=" + normTime + " delta=" + delta
                //+ " scale=" + scale + " acc=" + acc);
            }
            mAcceleration = acc < MAX_ACCELERATION ? acc : MAX_ACCELERATION;
        }
        else {
            // The user is scrolling slowly, so decrease acceleration.
            Float scale = (delta-normTime) * ACCEL_MOVE_SCALING_FACTOR;
            if (scale > 1)
                acc /= scale;

            if (ViewRootImpl::DEBUG_TRACKBALL) {
                //Logger::D(ViewRootImpl::TAG, StringBuffer(axis) + " deccelerate: off="
                //    + off + " normTime=" + normTime + " delta=" + delta
                //    + " scale=" + scale + " acc=" + acc);
            }
            mAcceleration = acc > 1 ? acc : 1;
        }
    }

    mPosition += off;

    return Elastos::Core::Math::Abs(mPosition);
}

/**
 * Generate the number of discrete movement events appropriate for
 * the currently collected trackball movement.
 *
 * @param precision The minimum movement required to generate the
 * first discrete movement.
 *
 * @return Returns the number of discrete movements, either positive
 * or negative, or 0 if there is not enough trackball movement yet
 * for a discrete movement.
 */
Int32 ViewRootImpl::TrackballAxis::Generate()
{
    Int32 movement = 0;
    mNonAccelMovement = 0;
    do {
        const Int32 dir = mPosition >= 0 ? 1 : -1;
        switch (mStep) {
            // If we are going to execute the first step, then we want
            // to do this as soon as possible instead of waiting for
            // a full movement, in order to make things look responsive.
        case 0:
            {
                if (Elastos::Core::Math::Abs(mPosition) < FIRST_MOVEMENT_THRESHOLD) {
                    return movement;
                }
                movement += dir;
                mNonAccelMovement += dir;
                mStep = 1;
                break;
            }
            // If we have generated the first movement, then we need
            // to wait for the second complete trackball motion before
            // generating the second discrete movement.
        case 1:
            {
                if (Elastos::Core::Math::Abs(mPosition) < SECOND_CUMULATIVE_MOVEMENT_THRESHOLD) {
                    return movement;
                }
                movement += dir;
                mNonAccelMovement += dir;
                mPosition -= SECOND_CUMULATIVE_MOVEMENT_THRESHOLD * dir;
                mStep = 2;
            }
            break;
            // After the first two, we generate discrete movements
            // consistently with the trackball, applying an acceleration
            // if the trackball is moving quickly.  This is a simple
            // acceleration on top of what we already compute based
            // on how quickly the wheel is being turned, to apply
            // a longer increasing acceleration to continuous movement
            // in one direction.
        default:
            {
                if (Elastos::Core::Math::Abs(mPosition) < SUBSEQUENT_INCREMENTAL_MOVEMENT_THRESHOLD) {
                    return movement;
                }
                movement += dir;
                mPosition -= dir * SUBSEQUENT_INCREMENTAL_MOVEMENT_THRESHOLD;
                Float acc = mAcceleration;
                acc *= 1.1f;
                mAcceleration = acc < MAX_ACCELERATION ? acc : mAcceleration;
            }
            break;
        }
    } while (TRUE);
}

//=======================================================================================
// ViewRootImpl::RunQueue
//=======================================================================================
void ViewRootImpl::RunQueue::Post(
    /* [in] */ IRunnable* action)
{
    PostDelayed(action, 0);
}

void ViewRootImpl::RunQueue::PostDelayed(
    /* [in] */ IRunnable* action,
    /* [in] */ Int32 delayMillis)
{
    AutoPtr<HandlerAction> handlerAction = new HandlerAction();
    handlerAction->mAction = action;
    handlerAction->mDelay = delayMillis;

    AutoLock lock(mLock);

    mActions.PushBack(handlerAction);
}

void ViewRootImpl::RunQueue::RemoveCallbacks(
    /* [in] */ IRunnable* action)
{
    AutoLock lock(mLock);

    List<AutoPtr<HandlerAction> >::Iterator iter = mActions.Begin();
    while (iter != mActions.End()) {
        if ((*iter)->mAction.Get() == action) {
            iter = mActions.Erase(iter);
        }
        else ++iter;
    }
}

void ViewRootImpl::RunQueue::ExecuteActions(
    /* [in] */ IHandler* handler)
{
    AutoLock lock(mLock);

    Boolean result;

    List<AutoPtr<HandlerAction> >::Iterator iter;
    for (iter = mActions.Begin(); iter != mActions.End(); ++iter) {
        handler->PostDelayed((*iter)->mAction, (*iter)->mDelay, &result);
    }

    mActions.Clear();
}

//=======================================================================================
// ViewRootImpl::AccessibilityInteractionConnectionManager
//=======================================================================================
CAR_INTERFACE_IMPL(
    ViewRootImpl::AccessibilityInteractionConnectionManager, Object,
    IAccessibilityManagerAccessibilityStateChangeListener);

ViewRootImpl::AccessibilityInteractionConnectionManager::AccessibilityInteractionConnectionManager(
    /* [in] */ ViewRootImpl* viewRootImpl)
    : mViewRootImpl(viewRootImpl)
{
}

ECode ViewRootImpl::AccessibilityInteractionConnectionManager::OnAccessibilityStateChanged(
    /* [in] */ Boolean enabled)
{
    if (enabled) {
        EnsureConnection();
        if (mViewRootImpl->mAttachInfo != NULL) {
            IAccessibilityEventSource::Probe(mViewRootImpl->mView)->SendAccessibilityEvent(
                IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
            AutoPtr<IView> focusedView;
            mViewRootImpl->mView->FindFocus((IView**)&focusedView);
            if (focusedView != NULL && focusedView != mViewRootImpl->mView) {
                IAccessibilityEventSource::Probe(focusedView)->SendAccessibilityEvent(
                    IAccessibilityEvent::TYPE_VIEW_FOCUSED);
            }
        }
    }
    else {
        EnsureNoConnection();
        Boolean result;
        mViewRootImpl->mHandler->SendEmptyMessage(
            ViewRootImpl::MSG_CLEAR_ACCESSIBILITY_FOCUS_HOST, &result);
    }
    return NOERROR;
}

ECode ViewRootImpl::AccessibilityInteractionConnectionManager::EnsureConnection()
{
    Boolean registered =
        mViewRootImpl->mAttachInfo->mAccessibilityWindowId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID;
    if (!registered) {
        AutoPtr<IIAccessibilityInteractionConnection> connection;
        AutoPtr<IWeakReference> weakThis;
        mViewRootImpl->GetWeakReference((IWeakReference**)&weakThis);
        CAccessibilityInteractionConnection::New(
            weakThis, (IIAccessibilityInteractionConnection**)&connection);
        Int32 status;
        mViewRootImpl->mAttachInfo->mAccessibilityWindowId =
            mViewRootImpl->mAccessibilityManager->AddAccessibilityInteractionConnection(
                mViewRootImpl->mWindow, connection, &status);
    }
    return NOERROR;
}

ECode ViewRootImpl::AccessibilityInteractionConnectionManager::EnsureNoConnection()
{
    Boolean registered =
        mViewRootImpl->mAttachInfo->mAccessibilityWindowId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID;
    if (registered) {
        mViewRootImpl->mAttachInfo->mAccessibilityWindowId = IAccessibilityNodeInfo::UNDEFINED_ITEM_ID;
        mViewRootImpl->mAccessibilityManager->RemoveAccessibilityInteractionConnection(
            mViewRootImpl->mWindow);
    }
    return NOERROR;
}

ViewRootImpl::SendWindowContentChangedAccessibilityEvent::SendWindowContentChangedAccessibilityEvent(
    /* [in] */ ViewRootImpl* host)
    : mLastEventTimeMillis(0)
    , mChangeTypes(0)
    , mHost(host)
{}

ECode ViewRootImpl::SendWindowContentChangedAccessibilityEvent::Run()
{
    // The accessibility may be turned off while we were waiting so check again.
    AutoPtr<IAccessibilityManager> amg;
    CAccessibilityManager::GetInstance(sContext, (IAccessibilityManager**)&amg);
    Boolean isEnabled;
    amg->IsEnabled(&isEnabled);
    if (isEnabled) {
        mLastEventTimeMillis = SystemClock::GetUptimeMillis();
        AutoPtr<IAccessibilityEvent> event;
        CAccessibilityEvent::Obtain((IAccessibilityEvent**)&event);
        event->SetEventType(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);
        event->SetContentChangeTypes(mChangeTypes);
        mSource->SendAccessibilityEventUnchecked(event);
    } else {
        mLastEventTimeMillis = 0;
    }
    // In any case reset to initial state.
    VIEW_PROBE(mSource)->ResetSubtreeAccessibilityStateChanged();
    mSource = NULL;
    mChangeTypes = 0;

    return NOERROR;
}

ECode ViewRootImpl::SendWindowContentChangedAccessibilityEvent::RunOrPost(
    /* [in] */ IView* source,
    /* [in] */ Int32 changeType)
{
    Boolean result;
    if (mSource != NULL) {
        // If there is no common predecessor, then mSource points to
        // a removed view, hence in this case always prefer the source.
        AutoPtr<IView> predecessor = mHost->GetCommonPredecessor(mSource, source);
        if (predecessor != NULL) {
            mSource = predecessor;

        } else {
            mSource = source;
        }
        mChangeTypes |= changeType;
        return NOERROR;
    }
    mSource = source;
    mChangeTypes = changeType;
    Int64 timeSinceLastMillis = SystemClock::GetUptimeMillis() - mLastEventTimeMillis;
    Int64 minEventIntevalMillis = CViewConfiguration::GetSendRecurringAccessibilityEventsInterval();

    if (timeSinceLastMillis >= minEventIntevalMillis) {
        mSource->RemoveCallbacks(IRunnable::Probe(this), &result);
        Run();
    } else {
        mSource->PostDelayed(IRunnable::Probe(this), minEventIntevalMillis - timeSinceLastMillis, &result);
    }
    return NOERROR;
}

ViewRootImpl::RenderProfileRunnable::MyRunnable::MyRunnable(
    /* [in] */ IWeakReference* viewRootImpl)
    : mWeakHost(viewRootImpl)
{
}

ECode ViewRootImpl::RenderProfileRunnable::MyRunnable::Run()
{
    AutoPtr<IInterface> obj;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj) {
        ViewRootImpl* mViewRootImpl = VIEWIMPL_PROBE(obj);
        mViewRootImpl->mDirty->Set(0, 0, mViewRootImpl->mWidth, mViewRootImpl->mHeight);
        mViewRootImpl->ScheduleTraversals();
    }

    return NOERROR;
}

ViewRootImpl::RenderProfileRunnable::RenderProfileRunnable(
    /* [in] */ IWeakReference* viewRootImpl)
    : mWeakHost(viewRootImpl)
{
}

ECode ViewRootImpl::RenderProfileRunnable::Run()
{
    Logger::D(TAG, "Starting profiling thread");

    AutoPtr<IInterface> obj;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj) {
        ViewRootImpl* mViewRootImpl = VIEWIMPL_PROBE(obj);
        Boolean result;
        while (mViewRootImpl->mRenderProfilingEnabled) {
            AutoPtr<IRunnable> runnable = new MyRunnable(mWeakHost);
            mViewRootImpl->mAttachInfo->mHandler->Post(runnable, &result);

            // TODO: This should use vsync when we get an API
            if (FAILED(Thread::Sleep(15))) {
                Logger::D(TAG, "Exiting profiling thread");
            }
        }
    }
    return NOERROR;
}

const char* ViewRootImpl::TAG = "ViewRootImpl";
const Boolean ViewRootImpl::DBG = FALSE;
const Boolean ViewRootImpl::LOCAL_LOGV = FALSE;
const Boolean ViewRootImpl::DEBUG_DRAW = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_LAYOUT = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_DIALOG = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_INPUT_RESIZE = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_ORIENTATION = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_TRACKBALL = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_IMF = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_CONFIGURATION = FALSE || LOCAL_LOGV;
const Boolean ViewRootImpl::DEBUG_FPS = FALSE;
const Boolean ViewRootImpl::DEBUG_INPUT_STAGES = FALSE;
const String ViewRootImpl::PROPERTY_EMULATOR_CIRCULAR("ro.emulator.circular");
const String ViewRootImpl::PROPERTY_MEDIA_DISABLED("config.disable_media");
const String ViewRootImpl::PROPERTY_PROFILE_RENDERING("viewroot.profile_rendering");
const Int32 ViewRootImpl::MAX_TRACKBALL_DELAY = 250;
const Int32 ViewRootImpl::MAX_QUEUED_INPUT_EVENT_POOL_SIZE = 10;

pthread_key_t ViewRootImpl::sKeyRunQueues;
Boolean ViewRootImpl::sKeyRunQueuesInited = FALSE;

List<AutoPtr<IRunnable> > ViewRootImpl::sFirstDrawHandlers;
Object ViewRootImpl::sFirstDrawHandlersLock;

Boolean ViewRootImpl::sFirstDrawComplete = FALSE;

List<AutoPtr<IComponentCallbacks> > ViewRootImpl::sConfigCallbacks;
Object ViewRootImpl::sConfigCallbacksLock;

AutoPtr<IContext> ViewRootImpl::sContext;

static AutoPtr<IInterpolator> CreateInterpolator()
{
    AutoPtr<CAccelerateDecelerateInterpolator> interpolator;
    CAccelerateDecelerateInterpolator::NewByFriend((CAccelerateDecelerateInterpolator**)&interpolator);
    return IInterpolator::Probe(interpolator);
}

AutoPtr<Elastos::Droid::View::Animation::IInterpolator> ViewRootImpl::mResizeInterpolator = CreateInterpolator();

CAR_INTERFACE_IMPL_3(ViewRootImpl, Object, IViewRootImpl, IViewParent, IHardwareDrawCallbacks)

ViewRootImpl::ViewRootImpl()
    : mSeq(0)
    , mViewVisibility(IView::GONE)
    , mAppVisible(TRUE)
    , mOrigWindowType(-1)
    , mStopped(FALSE)
    , mLastInCompatMode(FALSE)
    , mIsCreating(FALSE)
    , mDrawingAllowed(FALSE)
    , mWidth(-1)
    , mHeight(-1)
    , mIsAnimating(FALSE)
    , mTraversalScheduled(FALSE)
    , mTraversalBarrier(0)
    , mWillDrawSoon(FALSE)
    , mIsInTraversal(FALSE)
    , mApplyInsetsRequested(FALSE)
    , mLayoutRequested(FALSE)
    , mFirst(TRUE) // TRUE for the first time the view is added
    , mReportNextDraw(FALSE)
    , mFullRedrawNeeded(FALSE)
    , mNewSurfaceNeeded(FALSE)
    , mHasHadWindowFocus(FALSE)
    , mLastWasImTarget(FALSE)
    , mWindowsAnimating(FALSE)
    , mDrawDuringWindowsAnimating(FALSE)
    , mIsDrawing(FALSE)
    , mLastSystemUiVisibility(0)
    , mClientWindowLayoutFlags(0)
    , mLastOverscanRequested(FALSE)
    , mPendingInputEventCount(0)
    , mProcessInputEventsScheduled(FALSE)
    , mUnbufferedInputDispatch(FALSE)
    , mPendingInputEventQueueLengthCounterName("pq")
    , mWindowAttributesChanged(FALSE)
    , mWindowAttributesChangesFlag(0)
    , mAdded(FALSE)
    , mAddedTouchMode(FALSE)
    , mScrollMayChange(FALSE)
    , mScrollY(0)
    , mCurScrollY(0)
    , mResizeBufferStartTime(0)
    , mResizeBufferDuration(0)
    , mBlockResizeBuffer(FALSE)
    , mHardwareYOffset(0)
    , mResizeAlpha(0)
    , mConsumeBatchedInputScheduled(FALSE)
    , mConsumeBatchedInputImmediatelyScheduled(FALSE)
    , mQueuedInputEventPoolSize(0)
    , mProfileRendering(FALSE)
    , mRenderProfilingEnabled(FALSE)
    , mMediaDisabled(FALSE)
    , mFpsStartTime(-1)
    , mFpsPrevTime(-1)
    , mFpsNumFrames(0)
    , mInLayout(FALSE)
    , mHandlingLayoutInLayoutRequest(FALSE)
    , mViewLayoutDirectionInitial(0)
    , mRemoved(FALSE)
    , mIsEmulator(FALSE)
    , mIsCircularEmulator(FALSE)
    , mWindowIsRound(FALSE)
    , mProfile(FALSE)
{
    mDisplayListener = new RootDisplayListener(this);
    mConsumeBatchedInputImmediatelyRunnable = new ConsumeBatchedInputImmediatelyRunnable(this);
}

ECode ViewRootImpl::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDisplay* display)
{
    sContext = context;
    mWindowSession = CWindowManagerGlobal::GetWindowSession();
    mDisplay = display;
    context->GetBasePackageName(&mBasePackageName);

    display->GetDisplayAdjustments((IDisplayAdjustments**)&mDisplayAdjustments);

    mThread = Thread::GetCurrentThread();
    /*mLocation = new WindowLeaked(null);
    mLocation.fillInStackTrace();*/
    mWidth = -1;
    mHeight = -1;
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mDirty));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mTempRect));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mVisRect));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mWinFrame));
    CViewRootImplW::New((Handle32)this, (IIWindow**)&mWindow);

    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    info->GetTargetSdkVersion(&mTargetSdkVersion);
    mViewVisibility = IView::GONE;

    ASSERT_SUCCEEDED(CRegion::NewByFriend((CRegion**)&mTransparentRegion));
    ASSERT_SUCCEEDED(CRegion::NewByFriend((CRegion**)&mPreviousTransparentRegion));
    mFirst = TRUE; // true for the first time the view is added
    mAdded = FALSE;
    mAttachInfo = new View::AttachInfo(mWindowSession, mWindow, display, this, mHandler, this);
    CAccessibilityManager::GetInstance(context, (IAccessibilityManager**)&mAccessibilityManager);

    mAccessibilityInteractionConnectionManager = new AccessibilityInteractionConnectionManager(this);
    Boolean temp;
    mAccessibilityManager->AddAccessibilityStateChangeListener(
            mAccessibilityInteractionConnectionManager, &temp);
    mHighContrastTextManager = new HighContrastTextManager(this);
    mAccessibilityManager->AddHighTextContrastStateChangeListener(mHighContrastTextManager, &temp);
    mViewConfiguration = CViewConfiguration::Get(context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetDensityDpi(&mDensity);
    metrics->GetNoncompatDensityDpi(&mNoncompatDensity);
    AutoPtr<IPolicyManager> helper;
    CPolicyManager::AcquireSingleton((IPolicyManager**)&helper);
    helper->MakeNewFallbackEventHandler(context, (IFallbackEventHandler**)&mFallbackEventHandler);
    mChoreographer = Choreographer::GetInstance();
    AutoPtr<IInterface> it;
    context->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&it);
    mDisplayManager = IDisplayManager::Probe(it);
    LoadSystemProperties();
    resources->GetBoolean(R::bool_::config_windowIsRound, &mWindowIsRound);
    return NOERROR;
}

ViewRootImpl::~ViewRootImpl()
{
}

static void RunQueueDestructor(void* st)
{
    ViewRootImpl::RunQueue* obj = static_cast<ViewRootImpl::RunQueue*>(st);
    if (obj) {
        obj->Release();
    }
}

AutoPtr<ViewRootImpl::RunQueue> ViewRootImpl::GetRunQueue()
{
    if (!sKeyRunQueuesInited) {
        ASSERT_TRUE(pthread_key_create(&sKeyRunQueues, RunQueueDestructor) == 0);
        sKeyRunQueuesInited = TRUE;
    }
    AutoPtr<RunQueue> runQueue = (RunQueue*)pthread_getspecific(sKeyRunQueues);
    if (runQueue == NULL) {
        runQueue = new RunQueue();
        ASSERT_TRUE(pthread_setspecific(sKeyRunQueues, runQueue.Get()) == 0);
        runQueue->AddRef();
    }
    assert(runQueue);

    return runQueue;
}

void ViewRootImpl::AddFirstDrawHandler(
    /* [in] */ IRunnable* callback)
{
    AutoLock lock(sFirstDrawHandlersLock);

    if (!sFirstDrawComplete) {
        sFirstDrawHandlers.PushBack(callback);
    }
}

void ViewRootImpl::AddConfigCallback(
    /* [in] */ IComponentCallbacks* callback)
{
    AutoLock lock(sConfigCallbacksLock);

    sConfigCallbacks.PushBack(callback);
}

/**
* Call this to profile the next traversal call.
* FIXME for perf testing only. Remove eventually
*/
ECode ViewRootImpl::Profile()
{
    mProfile = TRUE;
    return NOERROR;
}

/**
 * Indicates whether we are in touch mode. Calling this method triggers an IPC
 * call and should be avoided whenever possible.
 *
 * @return True, if the device is in touch mode, FALSE otherwise.
 *
 * @hide
 */
Boolean ViewRootImpl::IsInTouchMode()
{
    AutoPtr<IWindowSession> windowSession = CWindowManagerGlobal::PeekWindowSession();
    Boolean isInTouchMode = FALSE;
    if (windowSession != NULL) {
        windowSession->GetInTouchMode(&isInTouchMode);
    }

    return isInTouchMode;
}

ECode ViewRootImpl::SetView(
    /* [in] */ IView* view,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ IView* panelParentView)
{
    AutoLock lock(mSyncLock);

    if (mView == NULL) {
        mView = view;

        mDisplay->GetState(&mAttachInfo->mDisplayState);
        mDisplayManager->RegisterDisplayListener(mDisplayListener, mHandler);

        mView->GetRawLayoutDirection(&mViewLayoutDirectionInitial);
        mFallbackEventHandler->SetView(view);
        Int32 changes;
        mWindowAttributes->CopyFrom(attrs, &changes);
        CWindowManagerLayoutParams* temp = WINLAY_PROBE(mWindowAttributes);
        if (temp->mPackageName == NULL) {
            temp->mPackageName = mBasePackageName;
        }
        attrs = mWindowAttributes.Get();

        // Keep track of the actual window flags supplied by the client.
        mClientWindowLayoutFlags = temp->mFlags;

        SetAccessibilityFocus(NULL, NULL);

        IRootViewSurfaceTaker* rootViewST = (IRootViewSurfaceTaker*)view->Probe(EIID_IRootViewSurfaceTaker);
        if (rootViewST) {
            mSurfaceHolderCallback = NULL;
            rootViewST->WillYouTakeTheSurface(
                (ISurfaceHolderCallback2**)&mSurfaceHolderCallback);
            if (mSurfaceHolderCallback != NULL) {
                mSurfaceHolder = new TakenSurfaceHolder(this);
                mSurfaceHolder->SetFormat(IPixelFormat::UNKNOWN);
            }
        }

        // Compute surface insets required to draw at specified Z value.
        // TODO: Use real shadow insets for a constant max Z.
        Float z;
        view->GetZ(&z);
        Int32 surfaceInset = (Int32) Elastos::Core::Math::Ceil(z * 2);
        temp->mSurfaceInsets->Set(surfaceInset, surfaceInset, surfaceInset, surfaceInset);

        AutoPtr<ICompatibilityInfo> compatibilityInfo;
        mDisplayAdjustments->GetCompatibilityInfo((ICompatibilityInfo**)&compatibilityInfo);
        compatibilityInfo->GetTranslator((ICompatibilityInfoTranslator**)&mTranslator);
        mDisplayAdjustments->SetActivityToken(WINLAY_PROBE(attrs)->mToken);

        // If the application owns the surface, don't enable hardware acceleration
        if (mSurfaceHolder == NULL) {
            EnableHardwareAcceleration(attrs);
        }

        Boolean restore = FALSE;
        if (mTranslator != NULL) {
            ((Surface*)mSurface.Get())->SetCompatibilityTranslator(mTranslator);
            restore = TRUE;
            attrs->Backup();
            mTranslator->TranslateWindowLayout(attrs);
        }

        if (DEBUG_LAYOUT) {
            String str;
            IObject::Probe(mWindowAttributes)->ToString(&str);
            Logger::D(TAG, "WindowLayout in setView:%s", str.string());
        }

        Boolean supportsScreen = TRUE;
        compatibilityInfo->SupportsScreen(&supportsScreen);
        if (!supportsScreen) {
            temp->mPrivateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW;
            mLastInCompatMode = TRUE;
        }
        mSoftInputMode = temp->mSoftInputMode;
        mWindowAttributesChanged = TRUE;
        mAttachInfo->mRootView = view;
        mAttachInfo->mScalingRequired = mTranslator != NULL;
        mAttachInfo->mApplicationScale = (mTranslator == NULL)
            ? 1.0f : ((CCompatibilityInfo::Translator*)mTranslator.Get())->mApplicationScale;
        if (panelParentView != NULL) {
            mAttachInfo->mPanelParentWindowToken = NULL;
            panelParentView->GetApplicationWindowToken(
                (IBinder**)&mAttachInfo->mPanelParentWindowToken);
        }
        mAdded = TRUE;

        // Schedule the first layout -before- adding to the window
        // manager, to make sure we do the relayout before receiving
        // any other events from the system.
        //
        RequestLayout();
        if ((temp->mInputFeatures
            & IWindowManagerLayoutParams::INPUT_FEATURE_NO_INPUT_CHANNEL) == 0) {
            mInputChannel = NULL;
            ASSERT_SUCCEEDED(CInputChannel::New((IInputChannel**)&mInputChannel));
        }

        mOrigWindowType = temp->mType;
        mAttachInfo->mRecomputeGlobalAttributes = TRUE;
        CollectViewAttributes();

        Int32 displayId;
        mDisplay->GetDisplayId(&displayId);
        AutoPtr<IRect> tempRect;
        AutoPtr<IInputChannel> tempInputChannel;
        Int32 res, hostVisibility;
        GetHostVisibility(&hostVisibility);
        ECode ec = mWindowSession->AddToDisplay(
            mWindow.Get(), mSeq, mWindowAttributes.Get(),
            hostVisibility, displayId, mAttachInfo->mContentInsets.Get(),
             mInputChannel.Get(), (IRect**)&tempRect,
            (IInputChannel**)&tempInputChannel, &res);

        if (tempRect != NULL) {
            mAttachInfo->mContentInsets->Set(tempRect);
        }
        if (tempInputChannel != NULL) {
            tempInputChannel->TransferTo(mInputChannel);
        }

        if (FAILED(ec)) {
            mAdded = FALSE;
            mView = NULL;
            mAttachInfo->mRootView = NULL;
            mInputChannel = NULL;
            mFallbackEventHandler->SetView(NULL);
            UnscheduleTraversals();
            SetAccessibilityFocus(NULL, NULL);

            if (restore) {
                attrs->Restore();
            }

            Logger::E(TAG, "Adding window failed");
            return E_RUNTIME_EXCEPTION;
        }

        if (restore) {
            mWindowAttributes->Restore();
        }

        if (mTranslator != NULL) {
            mTranslator->TranslateRectInScreenToAppWindow(mAttachInfo->mContentInsets);
        }

        mPendingOverscanInsets->Set(0, 0, 0, 0);
        mPendingContentInsets->Set((IRect*)mAttachInfo->mContentInsets);
        mPendingStableInsets->Set((IRect*)mAttachInfo->mStableInsets);
        mPendingVisibleInsets->Set(0, 0, 0, 0);

        if (DEBUG_LAYOUT)
            Logger::V(TAG, "Added window %p", mWindow.Get());

        if (res < IWindowManagerGlobal::ADD_OKAY) {
            mAttachInfo->mRootView = NULL;
            mAdded = FALSE;
            mFallbackEventHandler->SetView(NULL);
            UnscheduleTraversals();
            SetAccessibilityFocus(NULL, NULL);

            switch (res) {
                case IWindowManagerGlobal::ADD_BAD_APP_TOKEN:
                case IWindowManagerGlobal::ADD_BAD_SUBWINDOW_TOKEN:
                    /*Logger::E(TAG, "Unable to add window -- token 0x%08x"
                        "  is not valid; is your activity running?",
                        mWindowAttributes->mToken.Get());*/
                    return E_BAD_TOKEN_EXCEPTION;

                case IWindowManagerGlobal::ADD_NOT_APP_TOKEN:
                    /*Logger::E(TAG, "Unable to add window -- token 0x%08x"
                        "  is not for an application",
                        mWindowAttributes->mToken.Get());*/
                    return E_BAD_TOKEN_EXCEPTION;

                case IWindowManagerGlobal::ADD_APP_EXITING:
                    /*Logger::E(TAG, "Unable to add window -- app for token 0x%08x"
                        "   is exiting?", mWindowAttributes->mToken.Get());*/
                    return E_BAD_TOKEN_EXCEPTION;

                case IWindowManagerGlobal::ADD_DUPLICATE_ADD:
                    Logger::E(TAG, "Unable to add window -- window 0x%08x"
                        "  has already been added", mWindow.Get());
                    return E_BAD_TOKEN_EXCEPTION;

                case IWindowManagerGlobal::ADD_STARTING_NOT_NEEDED:
                    // Silently ignore -- we would have just removed it
                    // right away, anyway.
                    return NOERROR;

                case IWindowManagerGlobal::ADD_MULTIPLE_SINGLETON:
                    Logger::E(TAG, "Unable to add window 0x%08x -- another "
                        "window of this type already exists", mWindow.Get());
                    return E_BAD_TOKEN_EXCEPTION;

                case IWindowManagerGlobal::ADD_PERMISSION_DENIED:
                    Logger::E(TAG, "Unable to add window 0x%08x -- permission "
                        "denied for this window type", mWindow.Get());
                    return E_BAD_TOKEN_EXCEPTION;

                case IWindowManagerGlobal::ADD_INVALID_DISPLAY:
                    Logger::E(TAG, "Unable to add window 0x%08x -- the specified "
                        " display can not be found", mWindow.Get());
                    return E_INVALID_DISPLAY_EXCEPTION;

                default:
                    Logger::E(TAG, "Unable to add window -- unknown error code %d", res);
                    return E_RUNTIME_EXCEPTION;
            }
        }

        if (rootViewST) {
            rootViewST->WillYouTakeTheInputQueue((IInputQueueCallback**)&mInputQueueCallback);
        }

        if (mInputChannel != NULL) {
            if (mInputQueueCallback != NULL) {
                CInputQueue::New((IInputQueue**)&mInputQueue);
                mInputQueueCallback->OnInputQueueCreated(mInputQueue);
            }
            AutoPtr<IWeakReference> weakThis;
            GetWeakReference((IWeakReference**)&weakThis);
            mInputEventReceiver = new WindowInputEventReceiver();
            mInputEventReceiver->constructor(mInputChannel, mHandler->mLooper, weakThis);
            assert(mInputEventReceiver != NULL);
        }

        VIEW_PROBE(view)->AssignParent(THIS_PROBE(IViewParent));
        mAddedTouchMode = (res & IWindowManagerGlobal::ADD_FLAG_IN_TOUCH_MODE) != 0;
        mAppVisible = (res & IWindowManagerGlobal::ADD_FLAG_APP_VISIBLE) != 0;

        Boolean isEnabled = FALSE;
        mAccessibilityManager->IsEnabled(&isEnabled);
        if (isEnabled) {
            mAccessibilityInteractionConnectionManager->EnsureConnection();
        }

        Int32 ia;
        view->GetImportantForAccessibility(&ia);
        if (ia == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
            view->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
        }

        // Set up the input pipeline.
        AutoPtr<ICharSequence> counterSuffix;
        attrs->GetTitle((ICharSequence**)&counterSuffix);
        String titleStr;
        counterSuffix->ToString(&titleStr);
        mSyntheticInputStage = new SyntheticInputStage(this);
        AutoPtr<InputStage> viewPostImeStage = new ViewPostImeInputStage(this, mSyntheticInputStage);

        AutoPtr<InputStage> nativePostImeStage = new NativePostImeInputStage(this, viewPostImeStage,
                String("aq:native-post-ime:") + titleStr);
        AutoPtr<InputStage> earlyPostImeStage = new EarlyPostImeInputStage(this, nativePostImeStage);
        AutoPtr<InputStage> imeStage = new ImeInputStage(this, earlyPostImeStage,
                String("aq:ime:") + titleStr);
        AutoPtr<InputStage> viewPreImeStage = new ViewPreImeInputStage(this, imeStage);
        AutoPtr<InputStage> nativePreImeStage = new NativePreImeInputStage(this, viewPreImeStage,
                String("aq:native-pre-ime:") + titleStr);

        mFirstInputStage = nativePreImeStage;
        mFirstPostImeInputStage = earlyPostImeStage;
        String lastTepm;
        counterSuffix->ToString(&lastTepm);
        mPendingInputEventQueueLengthCounterName = String("aq:pending:") + lastTepm;
    }

    return NOERROR;
}

/** Whether the window is in local focus mode or not */
Boolean ViewRootImpl::IsInLocalFocusMode()
{
    CWindowManagerLayoutParams* obj = WINLAY_PROBE(mWindowAttributes);
    return (obj->mFlags & IWindowManagerLayoutParams::FLAG_LOCAL_FOCUS_MODE) != 0;
}

ECode ViewRootImpl::DestroyHardwareResources()
{
    if (mAttachInfo->mHardwareRenderer != NULL) {
        mAttachInfo->mHardwareRenderer->DestroyHardwareResources(mView);
        mAttachInfo->mHardwareRenderer->Destroy();
    }
    return NOERROR;
}

ECode ViewRootImpl::DetachFunctor(
    /* [in] */ Int64 functor)
{
    mBlockResizeBuffer = TRUE;
    if (mAttachInfo->mHardwareRenderer != NULL) {
        mAttachInfo->mHardwareRenderer->StopDrawing();
    }
    return NOERROR;
}

ECode ViewRootImpl::InvokeFunctor(
    /* [in] */ Int64 functor,
    /* [in] */ Boolean waitForCompletion)
{
    ThreadedRenderer::InvokeFunctor(functor, waitForCompletion);
    return NOERROR;
}

ECode ViewRootImpl::RegisterAnimatingRenderNode(
    /* [in] */ IRenderNode* animator)
{
    if (mAttachInfo->mHardwareRenderer != NULL) {
        mAttachInfo->mHardwareRenderer->RegisterAnimatingRenderNode(animator);
    } else {
        if (mAttachInfo->mPendingAnimatingRenderNodes == NULL) {
            CArrayList::New((IArrayList**)&mAttachInfo->mPendingAnimatingRenderNodes);
        }
        mAttachInfo->mPendingAnimatingRenderNodes->Add(animator);
    }
    return NOERROR;
}

void ViewRootImpl::EnableHardwareAcceleration(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    Logger::D("ViewRootImpl", "TODO no impl, EnableHardwareAcceleration,line:%d ,", __LINE__);
    mAttachInfo->mHardwareAccelerated = FALSE;
    mAttachInfo->mHardwareAccelerationRequested = FALSE;

    // Don't enable hardware acceleration when the application is in compatibility mode
    if (mTranslator != NULL)
        return;

    // Try to enable hardware acceleration if requested
    Int32 flags;
    attrs->GetFlags(&flags);
    const Boolean hardwareAccelerated =
            (flags & IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED) != 0;

    if (hardwareAccelerated) {
        Boolean available;
        if (HardwareRenderer::IsAvailable(&available), !available) {
            return;
        }

        // Persistent processes (including the system) should not do
        // accelerated rendering on low-end devices.  In that case,
        // sRendererDisabled will be set.  In addition, the system process
        // itself should never do accelerated rendering.  In that case, both
        // sRendererDisabled and sSystemRendererDisabled are set.  When
        // sSystemRendererDisabled is set, PRIVATE_FLAG_FORCE_HARDWARE_ACCELERATED
        // can be used by code on the system process to escape that and enable
        // HW accelerated drawing.  (This is basically for the lock screen.)

        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        const Boolean fakeHwAccelerated = (privateFlags &
                IWindowManagerLayoutParams::PRIVATE_FLAG_FAKE_HARDWARE_ACCELERATED) != 0;
        const Boolean forceHwAccelerated = (privateFlags &
                IWindowManagerLayoutParams::PRIVATE_FLAG_FORCE_HARDWARE_ACCELERATED) != 0;

        if (fakeHwAccelerated) {
            mAttachInfo->mHardwareAccelerationRequested = TRUE;
        }
        else if (!HardwareRenderer::sRendererDisabled ||
           (HardwareRenderer::sSystemRendererDisabled && forceHwAccelerated)) {
            if (mAttachInfo->mHardwareRenderer != NULL) {
                mAttachInfo->mHardwareRenderer->Destroy();
            }

            Boolean translucent = WINLAY_PROBE(attrs)->mFormat != IPixelFormat::OPAQUE;
            mAttachInfo->mHardwareRenderer = HardwareRenderer::Create(sContext, translucent);
            if (mAttachInfo->mHardwareRenderer != NULL) {
                AutoPtr<ICharSequence> temp;
                attrs->GetTitle((ICharSequence**)&temp);
                String name;
                temp->ToString(&name);
                mAttachInfo->mHardwareRenderer->SetName(name);
                mAttachInfo->mHardwareAccelerated =
                        mAttachInfo->mHardwareAccelerationRequested = TRUE;
            }
        }
    }
}

ViewRootImpl::GetView(
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mView;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

// final WindowLeaked getLocation() {
//     return mLocation;
// }

ECode ViewRootImpl::SetLayoutParams(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Boolean newView)
{
    AutoLock lock(mSyncLock);
    CWindowManagerLayoutParams* obj = WINLAY_PROBE(mWindowAttributes);
    CRect* objRect = (CRect*)IRect::Probe(obj->mSurfaceInsets);
    Int32 oldInsetLeft = objRect->mLeft;
    Int32 oldInsetTop = objRect->mTop;
    Int32 oldInsetRight = objRect->mRight;
    Int32 oldInsetBottom = objRect->mBottom;

    Int32 oldSoftInputMode = obj->mSoftInputMode;

    // Keep track of the actual window flags supplied by the client.
    attrs->GetFlags(&mClientWindowLayoutFlags);
    // preserve compatible window flag if exists.
    //
    Int32 compatibleWindowFlag =
        obj->mPrivateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW;

    // transfer over system UI visibility values as they carry current state.
    attrs->SetSystemUiVisibility(obj->mSystemUiVisibility);
    attrs->SetSubtreeSystemUiVisibility(obj->mSubtreeSystemUiVisibility);
    obj->CopyFrom(attrs, &mWindowAttributesChangesFlag);

    if ((mWindowAttributesChangesFlag & IWindowManagerLayoutParams::TRANSLUCENT_FLAGS_CHANGED) != 0) {
        // Recompute system ui visibility.
        mAttachInfo->mRecomputeGlobalAttributes = TRUE;
    }
    if (obj->mPackageName == NULL) {
        obj->mPackageName = mBasePackageName;
    }

    obj->mPrivateFlags |= compatibleWindowFlag;

    // Restore old surface insets.
    obj->mSurfaceInsets->Set(oldInsetLeft, oldInsetTop, oldInsetRight, oldInsetBottom);

    ApplyKeepScreenOnFlag(obj);

    if (newView) {
        mSoftInputMode = obj->mSoftInputMode;
        RequestLayout();
    }

    // Don't lose the mode we last auto-computed.
    //
    if ((obj->mSoftInputMode & IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST)
        == IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_UNSPECIFIED) {
        obj->mSoftInputMode = (obj->mSoftInputMode
            & ~IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST)
            | (oldSoftInputMode
            & IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST);
    }

    mWindowAttributesChanged = TRUE;
    ScheduleTraversals();
    return NOERROR;
}

ECode ViewRootImpl::HandleAppVisibility(
    /* [in] */ Boolean visible)
{
    if (mAppVisible != visible) {
        mAppVisible = visible;
        ScheduleTraversals();
        if (!mAppVisible) {
            CWindowManagerGlobal::TrimForeground();
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::HandleGetNewSurface()
{
    mNewSurfaceNeeded = TRUE;
    mFullRedrawNeeded = TRUE;
    ScheduleTraversals();

    return NOERROR;
}

ECode ViewRootImpl::RequestFitSystemWindows()
{
    FAIL_RETURN(CheckThread());
    mApplyInsetsRequested = TRUE;
    ScheduleTraversals();

    return NOERROR;
}

ECode ViewRootImpl::RequestLayout()
{
    if (!mHandlingLayoutInLayoutRequest) {
        FAIL_RETURN(CheckThread());

        mLayoutRequested = TRUE;
        ScheduleTraversals();
    }

    return NOERROR;
}

ECode ViewRootImpl::IsLayoutRequested(
    /* [out] */ Boolean* result)
{
    assert(result);
    *result = mLayoutRequested;

    return NOERROR;
}

ECode ViewRootImpl::Invalidate()
{
    mDirty->Set(0, 0, mWidth, mHeight);
    if (!mWillDrawSoon) {
        ScheduleTraversals();
    }

    return NOERROR;
}

ECode ViewRootImpl::InvalidateWorld(
    /* [in] */ IView* view)
{
    view->Invalidate();
    ViewGroup* parent = VIEWGROUP_PROBE(view);
    if (parent) {
        Int32 count = 0;
        parent->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            parent->GetChildAt(i, (IView**)&child);
            InvalidateWorld(child);
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::InvalidateChild(
    /* [in] */ IView* child,
    /* [in] */ IRect* dirty)
{
    AutoPtr<IViewParent> parent;
    return InvalidateChildInParent(NULL, dirty, (IViewParent**)&parent);
}

ECode ViewRootImpl::InvalidateChildInParent(
    /* [in] */ ArrayOf<Int32>* location,
    /* [in] */ IRect* dirty,
    /* [out] */ IViewParent** parent)
{
    FAIL_RETURN(CheckThread());

    if (DEBUG_DRAW)
        Logger::V(TAG, "Invalidate child: "/* + dirty*/);

    Boolean isEmpty;
    if (dirty == NULL) {
        Invalidate();
        *parent = NULL;
        return NOERROR;
    }
    else if ((dirty->IsEmpty(&isEmpty), isEmpty) && !mIsAnimating) {
        *parent = NULL;
        return NOERROR;
    }

    if (mCurScrollY != 0 || mTranslator != NULL) {
        mTempRect->Set(dirty);
        dirty = mTempRect;
        if (mCurScrollY != 0) {
            dirty->Offset(0, -mCurScrollY);
        }
        if (mTranslator != NULL) {
            mTranslator->TranslateRectInAppWindowToScreen(dirty);
        }
        if (mAttachInfo->mScalingRequired) {
            dirty->Inset(-1, -1);
        }
    }

    CRect* localDirty = mDirty;
    localDirty->IsEmpty(&isEmpty);
    Boolean contains;
    localDirty->Contains(dirty, &contains);
    if (!isEmpty && !contains) {
        mAttachInfo->mSetIgnoreDirtyState = TRUE;
        mAttachInfo->mIgnoreDirtyState = TRUE;
    }

    // Add the new dirty rect to the current one
    localDirty->Union(dirty);
    // Intersect with the bounds of the window to skip
    // updates that lie outside of the visible region
    const Float appScale = mAttachInfo->mApplicationScale;
    Boolean intersected;
    localDirty->Intersect(
        0, 0, (Int32)(mWidth * appScale + 0.5f),
        (Int32)(mHeight * appScale + 0.5f), &intersected);
    if (!intersected) {
        localDirty->SetEmpty();
    }
    if (!mWillDrawSoon && (intersected || mIsAnimating)) {
        ScheduleTraversals();
    }

    *parent = NULL;
    return NOERROR;
}

ECode ViewRootImpl::SetStopped(
    /* [in] */ Boolean stopped)
{
    if (mStopped != stopped) {
        mStopped = stopped;
        if (!stopped) {
            ScheduleTraversals();
        }
    }
    return NOERROR;
}

ECode ViewRootImpl::GetParent(
    /* [out] */ IViewParent** parent)
{
    assert(parent);
    *parent = NULL;

    return NOERROR;
}

ECode ViewRootImpl::GetChildVisibleRect(
    /* [in] */ IView* child,
    /* [in] */ IRect* r,
    /* [in] */ IPoint* offset,
    /* [out] */ Boolean* result)
{
    if (child != mView) {
        Logger::W(TAG, "child is not mine, honest!");

        return E_RUNTIME_EXCEPTION;
    }

    // Note: don't apply scroll offset, because we want to know its
    // visibility in the virtual canvas being given to the view hierarchy.
    //
    return r->Intersect(0, 0, mWidth, mHeight, result);
}

ECode ViewRootImpl::BringChildToFront(
    /* [in] */ IView* child)
{
    return NOERROR;
}

ECode ViewRootImpl::GetHostVisibility(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    assert(mView.Get());

    Int32 visibility = IView::GONE;
    if (mAppVisible) {
        mView->GetVisibility(&visibility);
    }
    *res = visibility;
    return NOERROR;
}

ECode ViewRootImpl::DisposeResizeBuffer()
{
    if (mResizeBuffer != NULL) {
        mResizeBuffer->Destory();
        mResizeBuffer = NULL;
    }

    return NOERROR;
}

ECode ViewRootImpl::RequestTransitionStart(
    /* [in] */ ILayoutTransition* transition)
{
    AutoPtr<ILayoutTransition> temp = transition;
    List<AutoPtr<ILayoutTransition> >::Iterator iter =
        Find(mPendingTransitions.Begin(), mPendingTransitions.End(), temp);
    if (iter == mPendingTransitions.End()) {
        mPendingTransitions.PushBack(transition);
    }

    return NOERROR;
}

/**
 * Notifies the HardwareRenderer that a new frame will be coming soon.
 * Currently only {@link ThreadedRenderer} cares about this, and uses
 * this knowledge to adjust the scheduling of off-thread animations
 */
ECode ViewRootImpl::NotifyRendererOfFramePending()
{
    if (mAttachInfo->mHardwareRenderer != NULL) {
        mAttachInfo->mHardwareRenderer->NotifyFramePending();
    }
    return NOERROR;
}

ECode ViewRootImpl::ScheduleTraversals()
{
    if (!mTraversalScheduled) {
        mTraversalScheduled = TRUE;
        mHandler->mLooper->PostSyncBarrier(&mTraversalBarrier);
        mChoreographer->PostCallback(
            IChoreographer::CALLBACK_TRAVERSAL, mTraversalRunnable, NULL);
        if (!mUnbufferedInputDispatch) {
            ScheduleConsumeBatchedInput();
        }
        NotifyRendererOfFramePending();
    }
    return NOERROR;
}

ECode ViewRootImpl::UnscheduleTraversals()
{
    if (mTraversalScheduled) {
        mTraversalScheduled = FALSE;
        mHandler->mLooper->RemoveSyncBarrier(mTraversalBarrier);
        mChoreographer->RemoveCallbacks(
            IChoreographer::CALLBACK_TRAVERSAL, mTraversalRunnable, NULL);
    }

    return NOERROR;
}

ECode ViewRootImpl::DoTraversal()
{
    if (mTraversalScheduled) {
        mTraversalScheduled = FALSE;
        mHandler->mLooper->RemoveSyncBarrier(mTraversalBarrier);

        if (mProfile) {
            //Debug.startMethodTracing("ViewAncestor");
        }

        //Trace::TraceBegin(Trace::TRACE_TAG_VIEW, "performTraversals");
        PerformTraversals();
        //Trace::TraceEnd(Trace::TRACE_TAG_VIEW);

        if (mProfile) {
            //Debug.stopMethodTracing();
            mProfile = FALSE;
        }
    }

    return NOERROR;
}


void ViewRootImpl::ApplyKeepScreenOnFlag(
    /* [in] */ IWindowManagerLayoutParams* params)
{
    // Update window's global keep screen on flag: if a view has requested
    // that the screen be kept on, then it is always set; otherwise, it is
    // set to whatever the client last requested for the global state.
    CWindowManagerLayoutParams* obj = (CWindowManagerLayoutParams*)params;
    if (mAttachInfo->mKeepScreenOn) {
        obj->mFlags |= IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON;
    }
    else {
        obj->mFlags =
            (obj->mFlags & ~IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON)
            | (mClientWindowLayoutFlags&IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON);
    }
}

Boolean ViewRootImpl::CollectViewAttributes()
{
    if (mAttachInfo->mRecomputeGlobalAttributes) {
        //ger::oG.i(TAG, "Computing view hierarchy attributes!");
        mAttachInfo->mRecomputeGlobalAttributes = FALSE;
        Boolean oldScreenOn = mAttachInfo->mKeepScreenOn;
        mAttachInfo->mKeepScreenOn = FALSE;
        mAttachInfo->mSystemUiVisibility = 0;
        mAttachInfo->mHasSystemUiListeners = FALSE;
        VIEW_PROBE(mView)->DispatchCollectViewAttributes(mAttachInfo, 0);
        mAttachInfo->mSystemUiVisibility &= ~mAttachInfo->mDisabledSystemUiVisibility;
        CWindowManagerLayoutParams* params = WINLAY_PROBE(mWindowAttributes);
        if (mAttachInfo->mKeepScreenOn != oldScreenOn
            || mAttachInfo->mSystemUiVisibility != params->mSubtreeSystemUiVisibility
            || mAttachInfo->mHasSystemUiListeners != params->mHasSystemUiListeners) {
            ApplyKeepScreenOnFlag(params);
            params->mSubtreeSystemUiVisibility = mAttachInfo->mSystemUiVisibility;
            params->mHasSystemUiListeners = mAttachInfo->mHasSystemUiListeners;
            mView->DispatchWindowSystemUiVisiblityChanged(mAttachInfo->mSystemUiVisibility);
            return TRUE;
        }
    }
    return FALSE;
}

Int32 ViewRootImpl::GetImpliedSystemUiVisibility(
    /* [in] */ IWindowManagerLayoutParams* params)
{
    Int32 vis = 0;
    CWindowManagerLayoutParams* temp = WINLAY_PROBE(params);
    // Translucent decor window flags imply stable system ui visibility.
    if ((temp->mFlags & IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS) != 0) {
        vis |= IView::SYSTEM_UI_FLAG_LAYOUT_STABLE | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
    }
    if ((temp->mFlags & IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION) != 0) {
        vis |= IView::SYSTEM_UI_FLAG_LAYOUT_STABLE | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
    }
    return vis;
}

Boolean ViewRootImpl::MeasureHierarchy(
    /* [in] */ View* host,
    /* [in] */ IWindowManagerLayoutParams* lp,
    /* [in] */ IResources* res,
    /* [in] */ Int32 desiredWindowWidth,
    /* [in] */ Int32 desiredWindowHeight)
{
    Int32 childWidthMeasureSpec;
    Int32 childHeightMeasureSpec;
    Boolean windowSizeMayChange = FALSE;

    if (DEBUG_ORIENTATION || DEBUG_LAYOUT)
        Logger::V(TAG, "Measuring 0x%08x in display %d x %d ...",
            host, desiredWindowWidth, desiredWindowHeight);

    Boolean goodMeasure = FALSE;
    CWindowManagerLayoutParams* obj = WINLAY_PROBE(lp);
    if (obj->mWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
        // On large screens, we don't want to allow dialogs to just
        // stretch to fill the entire width of the screen to display
        // one line of text.  First try doing the layout at a smaller
        // size to see if it will fit.
        AutoPtr<IDisplayMetrics> packageMetrics;
        res->GetDisplayMetrics((IDisplayMetrics**)&packageMetrics);
        res->GetValue(R::dimen::config_prefDialogWidth, mTmpValue, TRUE);
        Int32 baseSize = 0;
        Int32 type;
        mTmpValue->GetType(&type);
        if (type == ITypedValue::TYPE_DIMENSION) {
            Float dimension;
            mTmpValue->GetDimension(packageMetrics, &dimension);
            baseSize = (Int32)dimension;
        }

        if (DEBUG_DIALOG)
            Logger::V(TAG, "Window 0x%08x : baseSize=%d", mView.Get(), baseSize);

        if (baseSize != 0 && desiredWindowWidth > baseSize) {
            childWidthMeasureSpec = GetRootMeasureSpec(baseSize, obj->mWidth);
            childHeightMeasureSpec = GetRootMeasureSpec(desiredWindowHeight, obj->mHeight);
            PerformMeasure(childWidthMeasureSpec, childHeightMeasureSpec);

            Int32 tempWidth, tempHeight, tempState;
            host->GetMeasuredWidth(&tempWidth);
            host->GetMeasuredHeight(&tempHeight);
            if (DEBUG_DIALOG)
                Logger::V(TAG, "Window 0x%08x : measured (%d, %d)",
                    mView.Get(), tempWidth, tempHeight);
            if (((host->GetMeasuredWidthAndState(&tempState),tempState)
                & IView::MEASURED_STATE_TOO_SMALL) == 0) {
                goodMeasure = TRUE;
            }
            else {
                // Didn't fit in that size... try expanding a bit.
                baseSize = (baseSize + desiredWindowWidth) / 2;
                if (DEBUG_DIALOG)
                    Logger::V(TAG, "Window 0x%08x : next baseSize=%d",
                    mView.Get(), baseSize);
                childWidthMeasureSpec = GetRootMeasureSpec(baseSize, obj->mWidth);
                PerformMeasure(childWidthMeasureSpec, childHeightMeasureSpec);

                if (DEBUG_DIALOG)
                    Logger::V(TAG, "Window 0x%08x : measured (%d, %d)",
                        mView.Get(), tempWidth, tempHeight);
                if (((host->GetMeasuredWidthAndState(&tempState), tempState)
                    & IView::MEASURED_STATE_TOO_SMALL) == 0) {
                    if (DEBUG_DIALOG)
                        Logger::V(TAG, "Good!");
                    goodMeasure = TRUE;
                }
            }
        }
    }

    if (!goodMeasure) {
        childWidthMeasureSpec = GetRootMeasureSpec(desiredWindowWidth, obj->mWidth);
        childHeightMeasureSpec = GetRootMeasureSpec(desiredWindowHeight, obj->mHeight);
        PerformMeasure(childWidthMeasureSpec, childHeightMeasureSpec);
        Int32 tempWidth, tempHeight;
        if (mWidth != (host->GetMeasuredWidth(&tempWidth), tempWidth)
            || mHeight != (host->GetMeasuredHeight(&tempHeight), tempHeight)) {
            windowSizeMayChange = TRUE;
        }
    }

    if (DBG) {
        Logger::D(TAG, "======================================");
        Logger::D(TAG, "performTraversals -- after measure");
        //host->Debug();
    }

    return windowSizeMayChange;
}

/**
 * Modifies the input matrix such that it maps view-local coordinates to
 * on-screen coordinates.
 *
 * @param m input matrix to modify
 */
ECode ViewRootImpl::TransformMatrixToGlobal(
    /* [in] */ IMatrix* m)
{
    Boolean res;
    m->PreTranslate(mAttachInfo->mWindowLeft, mAttachInfo->mWindowTop, &res);
    return NOERROR;
}

/**
 * Modifies the input matrix such that it maps on-screen coordinates to
 * view-local coordinates.
 *
 * @param m input matrix to modify
 */
ECode ViewRootImpl::TransformMatrixToLocal(
    /* [in] */ IMatrix* m)
{
    Boolean res;
    m->PostTranslate(-mAttachInfo->mWindowLeft, -mAttachInfo->mWindowTop, &res);
    return NOERROR;
}

ECode ViewRootImpl::DispatchApplyInsets(
    /* [in] */ IView* host)
{
    mDispatchContentInsets->Set(mAttachInfo->mContentInsets);
    mDispatchStableInsets->Set(mAttachInfo->mStableInsets);
    Boolean isRound = (mIsEmulator && mIsCircularEmulator) || mWindowIsRound;
    AutoPtr<IWindowInsets> temp;
    CWindowInsets::New(mDispatchContentInsets, NULL, mDispatchStableInsets, isRound, (IWindowInsets**)&temp);
    host->DispatchApplyWindowInsets(temp, (IWindowInsets**)&temp);
    return NOERROR;
}

void ViewRootImpl::PerformTraversals()
{
    LocalTrace(mView, "::ENTER >> ViewRootImpl::PerformTraversals()");
    //LogView(String("PerformTraversals"));

    // cache mView since it is used so much below...
    //
    View* host = mView != NULL ? VIEW_PROBE(mView) : NULL;

    if (host == NULL || !mAdded) {
        LocalTrace(mView, "::LEAVE >> ViewRootImpl::PerformTraversals() : host == NULL || !mAdded.");
        return;
    }

    mIsInTraversal = TRUE;
    mWillDrawSoon = TRUE;

    Boolean windowSizeMayChange = FALSE;
    Boolean newSurface = FALSE;
    Boolean surfaceChanged = FALSE;
    CWindowManagerLayoutParams* lp = WINLAY_PROBE(mWindowAttributes);

    Int32 desiredWindowWidth = 0;
    Int32 desiredWindowHeight = 0;

    Int32 viewVisibility = 0;
    GetHostVisibility(&viewVisibility);
    Boolean viewVisibilityChanged = mViewVisibility != viewVisibility
        || mNewSurfaceNeeded;
    CWindowManagerLayoutParams* params = NULL;
    if (mWindowAttributesChanged) {
        mWindowAttributesChanged = FALSE;
        surfaceChanged = TRUE;
        params = lp;
    }
    AutoPtr<ICompatibilityInfo> compatibilityInfo;
    mDisplayAdjustments->GetCompatibilityInfo((ICompatibilityInfo**)&compatibilityInfo);
    Boolean supportsScreen;
    compatibilityInfo->SupportsScreen(&supportsScreen);
    if (supportsScreen == mLastInCompatMode) {
        params = lp;
        mFullRedrawNeeded = TRUE;
        mLayoutRequested = TRUE;
        if (mLastInCompatMode) {
            params->mPrivateFlags &= ~IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW;
            mLastInCompatMode = FALSE;
        }
        else {
            params->mPrivateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW;
            mLastInCompatMode = TRUE;
        }
    }

    mWindowAttributesChangesFlag = 0;

    CRect* frame = mWinFrame;
    if (mFirst) {
        mFullRedrawNeeded = TRUE;
        mLayoutRequested = TRUE;
        if (lp->mType == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL
                || lp->mType == IWindowManagerLayoutParams::TYPE_INPUT_METHOD) {
            // NOTE -- system code, won't try to do compat mode.
            AutoPtr<IPoint> size;
            CPoint::New((IPoint**)&size);
            mDisplay->GetRealSize(size);
            size->GetX(&desiredWindowWidth);
            size->GetY(&desiredWindowHeight);
        }
        else {
            AutoPtr<IContext> ctx;
            mView->GetContext((IContext**)&ctx);
            assert(ctx);

            AutoPtr<IResources> resources;
            ctx->GetResources((IResources**)&resources);
            assert(resources);

            AutoPtr<IDisplayMetrics> displayMetrics;
            resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
            displayMetrics->GetWidthPixels(&desiredWindowWidth);
            displayMetrics->GetHeightPixels(&desiredWindowHeight);
        }
        // We used to use the following condition to choose 32 bits drawing caches:
        // PixelFormat.hasAlpha(lp->mFormat) || lp.format == PixelFormat.RGBX_8888
        // However, windows are now always 32 bits by default, so choose 32 bits
        mAttachInfo->mUse32BitDrawingCache = TRUE;
        mAttachInfo->mHasWindowFocus = FALSE;
        mAttachInfo->mWindowVisibility = viewVisibility;
        mAttachInfo->mRecomputeGlobalAttributes = FALSE;
        viewVisibilityChanged = FALSE;

        AutoPtr<IResources> resources;
        host->GetResources((IResources**)&resources);
        assert(resources);
        AutoPtr<IConfiguration> config;
        resources->GetConfiguration((IConfiguration**)&config);
        mLastConfiguration->SetTo(config);
        mLastSystemUiVisibility = mAttachInfo->mSystemUiVisibility;
        // Set the layout direction if it has not been set before (inherit is the default)
        if (mViewLayoutDirectionInitial == IView::LAYOUT_DIRECTION_INHERIT) {
            Int32 direction;
            mLastConfiguration->GetLayoutDirection(&direction);
            host->SetLayoutDirection(direction);
        }
        host->DispatchAttachedToWindow(mAttachInfo, 0);
        ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->DispatchOnWindowAttachedChange(TRUE);
        DispatchApplyInsets(host);
        Logger::I(TAG, "Screen on initialized: %d", mAttachInfo->mKeepScreenOn);
    }
    else {
        frame->GetWidth(&desiredWindowWidth);
        frame->GetHeight(&desiredWindowHeight);
        if (desiredWindowWidth != mWidth || desiredWindowHeight != mHeight) {
            if (DEBUG_ORIENTATION) {
                Logger::V(TAG, "View 0x%08x resized to l = %d, r = %d, t = %d, b = %d"
                    , (Int32)host, frame->mLeft, frame->mRight, frame->mTop, frame->mBottom);
            }

            mFullRedrawNeeded = TRUE;
            mLayoutRequested = TRUE;
            windowSizeMayChange = TRUE;
        }
    }
    if (viewVisibilityChanged) {
        mAttachInfo->mWindowVisibility = viewVisibility;
        host->DispatchWindowVisibilityChanged(viewVisibility);

        if (viewVisibility != IView::VISIBLE || mNewSurfaceNeeded) {
            DestroyHardwareResources();
        }

        if (viewVisibility == IView::GONE) {
            // After making a window gone, we will count it as being
            // shown for the first time the next time it gets focus.
            //
            mHasHadWindowFocus = FALSE;
        }
    }

    // Execute enqueued actions on every traversal in case a detached view enqueued an action
    GetRunQueue()->ExecuteActions(mAttachInfo->mHandler);

    Boolean insetsChanged = FALSE;

    Boolean layoutRequested = mLayoutRequested;
    if (layoutRequested) {
        AutoPtr<IContext> ctx;
        mView->GetContext((IContext**)&ctx);
        assert(ctx);

        AutoPtr<IResources> res;
        ctx->GetResources((IResources**)&res);
        assert(res);
        if (mFirst) {
            // make sure touch mode code executes by setting cached value
            // to opposite of the added touch mode.
            //
            mAttachInfo->mInTouchMode = !mAddedTouchMode;
            EnsureTouchModeLocally(mAddedTouchMode);
        }
        else {
            Boolean equals;
            mPendingOverscanInsets->Equals(mAttachInfo->mOverscanInsets, &equals);
            if (!equals) {
                insetsChanged = TRUE;
            }

            mPendingContentInsets->Equals(mAttachInfo->mContentInsets, &equals);
            if (!equals) {
                insetsChanged = TRUE;
            }

            mPendingStableInsets->Equals(mAttachInfo->mStableInsets, &equals);
            if (!equals) {
                insetsChanged = TRUE;
            }

            mPendingVisibleInsets->Equals(mAttachInfo->mVisibleInsets, &equals);
            if (!equals) {
                mAttachInfo->mVisibleInsets->Set(mPendingVisibleInsets);
                if (DEBUG_LAYOUT)
                    Logger::V(TAG, "Visible insets changing to: "
                        /*+ mAttachInfo->mVisibleInsets*/);
            }
            if (lp->mWidth == IViewGroupLayoutParams::WRAP_CONTENT
                || lp->mHeight == IViewGroupLayoutParams::WRAP_CONTENT) {
                windowSizeMayChange = TRUE;

                if (lp->mType == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL
                        || lp->mType == IWindowManagerLayoutParams::TYPE_INPUT_METHOD) {
                    // NOTE -- system code, won't try to do compat mode.
                    AutoPtr<IPoint> size;
                    CPoint::New((IPoint**)&size);
                    mDisplay->GetRealSize(size);
                    size->GetX(&desiredWindowWidth);
                    size->GetY(&desiredWindowHeight);
                }
                else {
                    AutoPtr<IDisplayMetrics> packageMetrics;
                    res->GetDisplayMetrics((IDisplayMetrics**)&packageMetrics);
                    assert(packageMetrics);

                    packageMetrics->GetWidthPixels(&desiredWindowWidth);
                    packageMetrics->GetHeightPixels(&desiredWindowHeight);
                }
            }
        }

        // Ask host how big it wants to be
        windowSizeMayChange |= MeasureHierarchy(host, lp, res,
                desiredWindowWidth, desiredWindowHeight);
    }
    if (CollectViewAttributes()) {
        params = lp;
    }

    if (mAttachInfo->mForceReportNewAttributes) {
        mAttachInfo->mForceReportNewAttributes = FALSE;
        params = lp;
    }

    if (mFirst || mAttachInfo->mViewVisibilityChanged) {
        mAttachInfo->mViewVisibilityChanged = FALSE;
        Int32 resizeMode = mSoftInputMode &
            IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST;

        // If we are in auto resize mode, then we need to determine
        // what mode to use now.
        //
        if (resizeMode == IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_UNSPECIFIED) {
            List<View* >::Iterator iter = mAttachInfo->mScrollContainers.Begin();
            for (; iter!=mAttachInfo->mScrollContainers.End(); ++iter) {
                Boolean isShown;
                (*iter)->IsShown(&isShown);
                if (isShown) {
                    resizeMode = IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE;
                }
            }

            if (resizeMode == 0) {
                resizeMode = IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_PAN;
            }
            if ((lp->mSoftInputMode &
                IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST) != resizeMode) {
                    lp->mSoftInputMode = (lp->mSoftInputMode &
                        ~IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST) |
                        resizeMode;
                params = lp;
            }
        }
    }
    if (params != NULL) {
        if ((host->mPrivateFlags & View::PFLAG_REQUEST_TRANSPARENT_REGIONS) != 0) {
            if (!PixelFormat::FormatHasAlpha(params->mFormat)) {
                params->mFormat = IPixelFormat::TRANSLUCENT;
            }
        }

        mAttachInfo->mOverscanRequested = (params->mFlags
                    & IWindowManagerLayoutParams::FLAG_LAYOUT_IN_OVERSCAN) != 0;
    }

    if (mApplyInsetsRequested) {
        mApplyInsetsRequested = FALSE;
        mLastOverscanRequested  = mAttachInfo->mOverscanRequested;
        DispatchApplyInsets(host);
        if (mLayoutRequested) {
            // Short-circuit catching a new layout request here, so
            // we don't need to go through two layout passes when things
            // change due to fitting system windows, which can happen a lot.
            AutoPtr<IContext> ctx;
            mView->GetContext((IContext**)&ctx);
            assert(ctx);

            AutoPtr<IResources> res;
            ctx->GetResources((IResources**)&res);
            windowSizeMayChange |= MeasureHierarchy(
                host, lp, res, desiredWindowWidth, desiredWindowHeight);
        }
    }

    if (layoutRequested) {
        // Clear this now, so that if anything requests a layout in the
        // rest of this function we will catch it and re-run a full
        // layout pass.
        mLayoutRequested = FALSE;
    }

    Int32 frameWidth, framewHeight;
    frame->GetWidth(&frameWidth);
    frame->GetHeight(&framewHeight);
    Int32 hostWidth, hostHeight;
    Boolean windowShouldResize = layoutRequested && windowSizeMayChange
        && ((mWidth != (host->GetMeasuredWidth(&hostWidth), hostWidth)
        || mHeight != (host->GetMeasuredHeight(&hostHeight), hostHeight))
        || (lp->mWidth == IViewGroupLayoutParams::WRAP_CONTENT &&
        frameWidth < desiredWindowWidth && frameWidth != mWidth)
        || (lp->mHeight == IViewGroupLayoutParams::WRAP_CONTENT &&
        framewHeight < desiredWindowHeight && framewHeight != mHeight));

    Boolean computesInternalInsetsTemp;
    ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->HasComputeInternalInsetsListeners(&computesInternalInsetsTemp);
    Boolean computesInternalInsets = computesInternalInsetsTemp || mAttachInfo->mHasNonEmptyGivenInternalInsets;

    Boolean insetsPending = FALSE;
    Int32 relayoutResult = 0;
    if (mFirst || windowShouldResize || insetsChanged
        || viewVisibilityChanged || params != NULL) {
        if (viewVisibility == IView::VISIBLE) {
            // If this window is giving internal insets to the window
            // manager, and it is being added or changing its visibility,
            // then we want to first give the window manager "fake"
            // insets to cause it to effectively ignore the content of
            // the window during layout.  This avoids it briefly causing
            // other windows to resize/move based on the raw frame of the
            // window, waiting until we can finish laying out this window
            // and get back to the window manager with the ultimately
            // computed insets.
            //
            insetsPending = computesInternalInsets
                && (mFirst || viewVisibilityChanged);
        }

        if (mSurfaceHolder != NULL) {
            mSurfaceHolder->mSurfaceLock->Lock();
            mDrawingAllowed = TRUE;
        }

        Boolean hwInitialized = FALSE;
        Boolean contentInsetsChanged = FALSE;
        Boolean hadSurface = FALSE;
        mSurface->IsValid(&hadSurface);

        if (DEBUG_LAYOUT) {
           /*Logger::I(TAG, "host=w:%d, h:%d, params=%p", host->GetMeasuredWidth(),
                host->GetMeasuredHeight(), params);*/
        }

        if (mAttachInfo->mHardwareRenderer != NULL) {
            // relayoutWindow may decide to destroy mSurface. As that decision
            // happens in WindowManager service, we need to be defensive here
            // and stop using the surface in case it gets destroyed.
            mAttachInfo->mHardwareRenderer->PauseSurface(mSurface);
        }

        Int32 surfaceGenerationId;
        mSurface->GetGenerationId(&surfaceGenerationId);
        relayoutResult = RelayoutWindow(params, viewVisibility, insetsPending);

        if (!mDrawDuringWindowsAnimating &&
                (relayoutResult & IWindowManagerGlobal::RELAYOUT_RES_ANIMATING) != 0) {
            mWindowsAnimating = TRUE;
        }

        if (DEBUG_LAYOUT) {
            String os, fs, cs, vs, vvs;
            frame->ToShortString(&fs);
            mPendingOverscanInsets->ToShortString(&os);
            mPendingContentInsets->ToShortString(&cs);
            mPendingVisibleInsets->ToShortString(&vs);
            mPendingStableInsets->ToShortString(&vvs);
            Logger::V(TAG, "relayout: frame=%s, overscan=%s, content=%s, visible=%s, visible=%s, surface=%p",
                    fs.string(), os.string(), cs.string(), vs.string(), vvs.string(), mSurface.Get());
        }

        Int32 seq;
        mPendingConfiguration->GetSeq(&seq);
        if (seq != 0) {
            if (DEBUG_CONFIGURATION) {
                Logger::D(TAG, "Visible with new config: "
                    /*+ mPendingConfiguration*/);
            }
            UpdateConfiguration(mPendingConfiguration, !mFirst);
            mPendingConfiguration->SetSeq(0);
        }

        Boolean overscanInsetsChangedTemp;
        mPendingOverscanInsets->Equals(mAttachInfo->mOverscanInsets, &overscanInsetsChangedTemp);
        Boolean overscanInsetsChanged = !overscanInsetsChangedTemp;
        mPendingContentInsets->Equals(mAttachInfo->mContentInsets, &contentInsetsChanged);
        contentInsetsChanged = !contentInsetsChanged;
        Boolean visibleInsetsChangedTemp;
        mPendingVisibleInsets->Equals(mAttachInfo->mVisibleInsets, &visibleInsetsChangedTemp);
        Boolean visibleInsetsChanged = !visibleInsetsChangedTemp;
        Boolean stableInsetsChangedTemp;
        mPendingStableInsets->Equals(mAttachInfo->mStableInsets, &stableInsetsChangedTemp);
        Boolean stableInsetsChanged = !stableInsetsChangedTemp;

        if (contentInsetsChanged) {
            Boolean isValid, enabled;
            if (mWidth > 0 && mHeight > 0 && lp != NULL &&
                ((lp->mSystemUiVisibility|lp->mSubtreeSystemUiVisibility)
                    & IView::SYSTEM_UI_LAYOUT_FLAGS) == 0 &&
                mSurface != NULL && (mSurface->IsValid(&isValid), isValid) &&
                !mAttachInfo->mTurnOffWindowResizeAnim &&
                mAttachInfo->mHardwareRenderer != NULL &&
                (mAttachInfo->mHardwareRenderer->IsEnabled(&enabled), enabled) &&
                lp != NULL && !PixelFormat::FormatHasAlpha(lp->mFormat)
                && !mBlockResizeBuffer) {

                DisposeResizeBuffer();

// TODO: Again....
               /*if (mResizeBuffer == null) {
                   mResizeBuffer = mAttachInfo.mHardwareRenderer.createDisplayListLayer(
                           mWidth, mHeight);
               }
               mResizeBuffer.prepare(mWidth, mHeight, false);
               RenderNode layerRenderNode = mResizeBuffer.startRecording();
               HardwareCanvas layerCanvas = layerRenderNode.start(mWidth, mHeight);
               try {
                   final int restoreCount = layerCanvas.save();

                   int yoff;
                   final boolean scrolling = mScroller != null
                           && mScroller.computeScrollOffset();
                   if (scrolling) {
                       yoff = mScroller.getCurrY();
                       mScroller.abortAnimation();
                   } else {
                       yoff = mScrollY;
                   }

                   layerCanvas.translate(0, -yoff);
                   if (mTranslator != null) {
                       mTranslator.translateCanvas(layerCanvas);
                   }

                   RenderNode renderNode = mView.mRenderNode;
                   if (renderNode != null && renderNode.isValid()) {
                       layerCanvas.drawDisplayList(renderNode, null,
                               RenderNode.FLAG_CLIP_CHILDREN);
                   } else {
                       mView.draw(layerCanvas);
                   }

                   drawAccessibilityFocusedDrawableIfNeeded(layerCanvas);

                   mResizeBufferStartTime = SystemClock::GetUptimeMillis();
                   mResizeBufferDuration = mView.getResources().getInteger(
                           com.android.internal.R.integer.config_mediumAnimTime);

                   layerCanvas.restoreToCount(restoreCount);
                   layerRenderNode.end(layerCanvas);
                   layerRenderNode.setCaching(true);
                   layerRenderNode.setLeftTopRightBottom(0, 0, mWidth, mHeight);
                   mTempRect.set(0, 0, mWidth, mHeight);
               } finally {
                   mResizeBuffer.endRecording(mTempRect);
               }
               mAttachInfo.mHardwareRenderer.flushLayerUpdates();*/
            }
            mAttachInfo->mContentInsets->Set(mPendingContentInsets);

            if (DEBUG_LAYOUT) {
                Logger::D(TAG, "Content insets changing to: "
                    /*+ mAttachInfo->mContentInsets*/);
            }
        }

        if (overscanInsetsChanged) {
            mAttachInfo->mOverscanInsets->Set(mPendingOverscanInsets);
            if (DEBUG_LAYOUT){
                Logger::D(TAG, "Overscan insets changing to: "
                    /*+ mAttachInfo.mOverscanInsets*/);
            }
            // Need to relayout with content insets.
            contentInsetsChanged = TRUE;
        }
        if (stableInsetsChanged) {
            mAttachInfo->mStableInsets->Set(mPendingStableInsets);
            if (DEBUG_LAYOUT) {
                Logger::D(TAG, "Decor insets changing to: "
                    /*+ mAttachInfo.mStableInsets*/);
            }
            // Need to relayout with content insets.
            contentInsetsChanged = TRUE;
        }

        if (contentInsetsChanged || mLastSystemUiVisibility !=
                mAttachInfo->mSystemUiVisibility || mApplyInsetsRequested
                || mLastOverscanRequested != mAttachInfo->mOverscanRequested) {
            mLastSystemUiVisibility = mAttachInfo->mSystemUiVisibility;
            mLastOverscanRequested = mAttachInfo->mOverscanRequested;
            mApplyInsetsRequested = FALSE;
            DispatchApplyInsets(host);
        }

        if (visibleInsetsChanged) {
            mAttachInfo->mVisibleInsets->Set(mPendingVisibleInsets);

            if (DEBUG_LAYOUT) {
                Logger::D(TAG, "Visible insets changing to: "
                    /*+ mAttachInfo->mVisibleInsets*/);
            }
        }

        Boolean surfaceValid = FALSE;
        mSurface->IsValid(&surfaceValid);
        Int32 gId;
        if (!hadSurface) {
            if (surfaceValid) {
                // If we are creating a new surface, then we need to
                // completely redraw it.  Also, when we get to the
                // point of drawing it we will hold off and schedule
                // a new traversal instead.  This is so we can tell the
                // window manager about all of the windows being displayed
                // before actually drawing them, so it can display then
                // all at once.
                //
                newSurface = TRUE;
                mFullRedrawNeeded = TRUE;
                mPreviousTransparentRegion->SetEmpty();

                if (mAttachInfo->mHardwareRenderer != NULL) {
                    //try {
                        ECode ec = mAttachInfo->mHardwareRenderer->Initialize(mSurface, &hwInitialized);
                    //} catch (OutOfResourcesException e) {
                        if (ec == (ECode) E_OUT_OF_RESOURCES_EXCEPTION) {
                            //HandleOutOfResourcesException(e);
                        }

                        return;
                    //}
                }
            }
        }
        else if (!surfaceValid) {
            // If the surface has been removed, then reset the scroll
            // positions.
            mLastScrolledFocus = NULL;
            mScrollY = mCurScrollY = 0;
            if (mScroller != NULL) {
                mScroller->AbortAnimation();
            }
            DisposeResizeBuffer();
            // Our surface is gone
            // if (mAttachInfo->mHardwareRenderer != NULL &&
            //     mAttachInfo->mHardwareRenderer->IsEnabled()) {
            //     mAttachInfo->mHardwareRenderer->Destroy(TRUE);
            // }
        }
        // else if (surfaceGenerationId != (mSurface->GetGenerationId(&gId), gId) &&
        //     mSurfaceHolder == NULL && mAttachInfo->mHardwareRenderer != NULL) {
        //     mFullRedrawNeeded = TRUE;
        //     try {
        //     ECode ec = mAttachInfo->mHardwareRenderer->UpdateSurface(mSurface);
        //     } catch (OutOfResourcesException e) {
        //     if (ec == (ECode) E_OUT_OF_RESOURCES_EXCEPTION) {
        //         handleOutOfResourcesException(e);
        //     }
        //         return;
        //     }
        // }

        if (DEBUG_ORIENTATION) {
            String fs;
            frame->ToShortString(&fs);
            Logger::V(TAG, "Relayout returned: frame=%s", fs.string());
        }

        mAttachInfo->mWindowLeft = frame->mLeft;
        mAttachInfo->mWindowTop = frame->mTop;

        // !!FIXME!! This next section handles the case where we did not get the
        // window size we asked for. We should avoid this by getting a maximum size from
        // the window session beforehand.
        //
        frame->GetWidth(&mWidth);
        frame->GetHeight(&mHeight);
        if (mSurfaceHolder != NULL) {
            // The app owns the surface; tell it about what is going on.
            Boolean isValid = FALSE;
            mSurface->IsValid(&isValid);
            if (isValid) {
                // XXX .copyFrom() doesn't work!
                //mSurfaceHolder.mSurface.copyFrom(mSurface);
                mSurfaceHolder->mSurface = mSurface;
            }
            mSurfaceHolder->SetSurfaceFrameSize(mWidth, mHeight);
            mSurfaceHolder->mSurfaceLock->UnLock();
            mSurface->IsValid(&isValid);
            if (isValid) {
                if (!hadSurface) {
                    mSurfaceHolder->UngetCallbacks();

                    mIsCreating = TRUE;
                    (ISurfaceHolderCallback::Probe(mSurfaceHolderCallback))->SurfaceCreated(mSurfaceHolder);
                    AutoPtr< ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
                    for (Int32 i = 0; i < callbacks->GetLength(); ++i) {
                        (*callbacks)[i]->SurfaceCreated(mSurfaceHolder);
                    }
                    surfaceChanged = TRUE;
                }

                if (surfaceChanged) {
                    (ISurfaceHolderCallback::Probe(mSurfaceHolderCallback))->SurfaceChanged(mSurfaceHolder,
                        lp->mFormat, mWidth, mHeight);

                    AutoPtr< ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
                    for (Int32 i = 0; i < callbacks->GetLength(); ++i) {
                        (*callbacks)[i]->SurfaceChanged(mSurfaceHolder, lp->mFormat, mWidth, mHeight);
                    }
                }
                mIsCreating = FALSE;
            }
            else if (hadSurface) {
                mSurfaceHolder->UngetCallbacks();

                (ISurfaceHolderCallback::Probe(mSurfaceHolderCallback))->SurfaceDestroyed(mSurfaceHolder);

                AutoPtr< ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
                for (Int32 i = 0; i < callbacks->GetLength(); ++i) {
                    (*callbacks)[i]->SurfaceDestroyed(mSurfaceHolder);
                }

                mSurfaceHolder->mSurfaceLock->Lock();
                mSurfaceHolder->mSurface = NULL;
                CSurface::New((ISurface**)&(mSurfaceHolder->mSurface));
                mSurfaceHolder->mSurfaceLock->UnLock();
             }
        }
        Boolean hardwareIsEnabled;
        if (mAttachInfo->mHardwareRenderer != NULL &&
            (mAttachInfo->mHardwareRenderer->IsEnabled(&hardwareIsEnabled), hardwareIsEnabled)) {
            Int32 hardwareWidth, hardwareHeight;
            if (hwInitialized ||
                mWidth != (mAttachInfo->mHardwareRenderer->GetWidth(&hardwareWidth), hardwareWidth) ||
                mHeight != (mAttachInfo->mHardwareRenderer->GetHeight(&hardwareHeight), hardwareHeight)) {
                AutoPtr<IRect> surfaceInsets = params != NULL ? params->mSurfaceInsets : NULL;
                mAttachInfo->mHardwareRenderer->Setup(mWidth, mHeight, surfaceInsets);
                if (!hwInitialized) {
                    mAttachInfo->mHardwareRenderer->Invalidate(mSurface);
                    mFullRedrawNeeded = TRUE;
                }
            }
        }

        if (!mStopped) {
            Boolean focusChangedDueToTouchMode = EnsureTouchModeLocally(
                (relayoutResult & IWindowManagerGlobal::RELAYOUT_RES_IN_TOUCH_MODE) != 0);
            Int32 tempWidth, tempHeight;
            host->GetMeasuredWidth(&tempWidth);
            host->GetMeasuredHeight(&tempHeight);
            contentInsetsChanged = TRUE;
            if (focusChangedDueToTouchMode || mWidth != tempWidth
                || mHeight != tempHeight || contentInsetsChanged) {
                Int32 childWidthMeasureSpec = GetRootMeasureSpec(mWidth, lp->mWidth);
                Int32 childHeightMeasureSpec = GetRootMeasureSpec(mHeight, lp->mHeight);

                if (DEBUG_LAYOUT) {
                    Int32 tempWidth, tempHeight;
                    host->GetMeasuredWidth(&tempWidth);
                    host->GetMeasuredHeight(&tempHeight);
                    Logger::D(TAG, "Ooops, something changed!  mWidth=%d measuredWidth=%d"
                        " measuredWidth%d mHeight=%d measuredHeight=%d coveredInsetsChanged=%d"
                        , mWidth, tempWidth, mHeight, tempHeight
                        , contentInsetsChanged);
                }
                // Ask host how big it wants to be
                PerformMeasure(childWidthMeasureSpec, childHeightMeasureSpec);

                // Implementation of weights from IWindowManagerLayoutParams*
                // We just grow the dimensions as needed and re-measure if
                // needs be
                //
                Int32 width = 0;
                host->GetMeasuredWidth(&width);
                Int32 height = 0;
                host->GetMeasuredHeight(&height);
                Boolean measureAgain = FALSE;
                if (lp->mHorizontalWeight > 0.0f) {
                    width += (Int32) ((mWidth - width) * lp->mHorizontalWeight);
                    childWidthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(
                        width, View::MeasureSpec::EXACTLY);
                    measureAgain = TRUE;
                }
                if (lp->mVerticalWeight > 0.0f) {
                    height += (Int32) ((mHeight - height) * lp->mVerticalWeight);
                    childHeightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(
                        height, View::MeasureSpec::EXACTLY);
                    measureAgain = TRUE;
                }

                if (measureAgain) {
                    if (DEBUG_LAYOUT) {
                        Logger::D(TAG, "And hey let's measure once more: width=%d"
                            " height=%d", width, height);
                    }
                    PerformMeasure(childWidthMeasureSpec, childHeightMeasureSpec);
                }

                mLayoutRequested = TRUE;
            }
        }
    }
    else {
        // Not the first pass and no window/insets/visibility change but the window
        // may have moved and we need check that and if so to update the left and right
        // in the attach info. We translate only the window frame since on window move
        // the window manager tells us only for the new frame but the insets are the
        // same and we do not want to translate them more than once.

        // TODO: Well, we are checking whether the frame has changed similarly
        // to how this is done for the insets. This is however incorrect since
        // the insets and the frame are translated. For example, the old frame
        // was (1, 1 - 1, 1) and was translated to say (2, 2 - 2, 2), now the new
        // reported frame is (2, 2 - 2, 2) which implies no change but this is not
        // TRUE since we are comparing a not translated value to a translated one.
        // This scenario is rare but we may want to fix that.

        Boolean windowMoved = (mAttachInfo->mWindowLeft != frame->mLeft
                || mAttachInfo->mWindowTop != frame->mTop);
        if (windowMoved) {
            if (mTranslator != NULL) {
                mTranslator->TranslateRectInScreenToAppWinFrame(frame);
            }
            mAttachInfo->mWindowLeft = frame->mLeft;
            mAttachInfo->mWindowTop = frame->mTop;
        }
    }
    const Boolean didLayout = layoutRequested;
    Boolean triggerGlobalLayoutListener = didLayout
        || mAttachInfo->mRecomputeGlobalAttributes;
    if (didLayout) {
        PerformLayout(lp, desiredWindowWidth, desiredWindowHeight);

        if ((host->mPrivateFlags & View::PFLAG_REQUEST_TRANSPARENT_REGIONS) != 0) {
            // start out transparent
            // TODO: AVOID THAT CALL BY CACHING THE RESULT?
            AutoPtr<ArrayOf<Int32> > _mTmpLocation = ArrayOf<Int32>::Alloc(2);
            (*_mTmpLocation)[0] = mTmpLocation[0];
            (*_mTmpLocation)[1] = mTmpLocation[1];
            host->GetLocationInWindow(_mTmpLocation);
            Boolean result;
            mTransparentRegion->Set(mTmpLocation[0], mTmpLocation[1],
                mTmpLocation[0] + host->mRight - host->mLeft,
                mTmpLocation[1] + host->mBottom - host->mTop, &result);
            Boolean isEqual;
            host->GatherTransparentRegion(mTransparentRegion, &isEqual);
            if (mTranslator != NULL) {
                mTranslator->TranslateRegionInWindowToScreen(mTransparentRegion);
            }


            mTransparentRegion->Equals(mPreviousTransparentRegion, &isEqual);
            if (!isEqual) {
                mPreviousTransparentRegion->Set(mTransparentRegion, &result);
                mFullRedrawNeeded = TRUE;
                // reconfigure window manager
                mWindowSession->SetTransparentRegion(mWindow, mTransparentRegion);
            }
        }

        if (DBG) {
            Logger::D(TAG, "======================================");
            Logger::D(TAG, "performTraversals -- after setFrame");
            host->Debug();
        }
    }
    if (triggerGlobalLayoutListener) {
        mAttachInfo->mRecomputeGlobalAttributes = FALSE;
        mAttachInfo->mTreeObserver->DispatchOnGlobalLayout();
    }

    if (computesInternalInsets) {
        // Clear the original insets.
        ViewTreeObserver::InternalInsetsInfo* insets =
            (ViewTreeObserver::InternalInsetsInfo*)mAttachInfo->mGivenInternalInsets.Get();
        insets->Reset();

        // Compute new insets in place.
        ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->DispatchOnComputeInternalInsets(insets);
        Boolean insetsIsEmpty;
        insets->IsEmpty(&insetsIsEmpty);
        mAttachInfo->mHasNonEmptyGivenInternalInsets = !insetsIsEmpty;

        // Tell the window manager.
        Boolean equal = FALSE;
        IObject::Probe(mLastGivenInsets)->Equals((IInterface*)insets->Probe(EIID_IInterface), &equal);
        if (insetsPending || !equal) {
            ((ViewTreeObserver::InternalInsetsInfo*)mLastGivenInsets.Get())->Set(insets);

            // Translate insets to screen coordinates if needed.
            AutoPtr<IRect> contentInsets;
            AutoPtr<IRect> visibleInsets;
            AutoPtr<IRegion> touchableRegion;
            if (mTranslator != NULL) {
                mTranslator->GetTranslatedContentInsets(
                    insets->mContentInsets, (IRect**)&contentInsets);
                mTranslator->GetTranslatedVisibleInsets(
                    insets->mVisibleInsets, (IRect**)&visibleInsets);
                mTranslator->GetTranslatedTouchableArea(
                    insets->mTouchableRegion, (IRegion**)&touchableRegion);
            }
            else {
                contentInsets = insets->mContentInsets;
                visibleInsets = insets->mVisibleInsets;
                touchableRegion = insets->mTouchableRegion;
            }

            mWindowSession->SetInsets(
                mWindow, insets->mTouchableInsets,
                contentInsets, visibleInsets, touchableRegion);
        }
    }
    Boolean skipDraw = FALSE;

    if (mFirst) {
        // handle first focus request
        //if (DEBUG_INPUT_RESIZE) {
        //    Logger::D(TAG, StringBuffer("First: mView.hasFocus()=")
        //    + mView->HasFocus());
        //}

        if (mView != NULL) {
            Boolean hasFocus;
            mView->HasFocus(&hasFocus);
            if (!hasFocus) {
                Boolean result;
                mView->RequestFocus(IView::FOCUS_FORWARD, &result);
                if (DEBUG_INPUT_RESIZE) {
                    Logger::D(TAG, "First: requested focused view=0x"
                        /*mView.findFocus()*/);
                }
            }
            else {
                if (DEBUG_INPUT_RESIZE) {
                    Logger::D(TAG, "First: existing focused view=0x"
                        /*mView.findFocus()*/);
                }
            }
        }
        if ((relayoutResult & IWindowManagerGlobal::RELAYOUT_RES_ANIMATING) != 0) {
            // The first time we relayout the window, if the system is
            // doing window animations, we want to hold of on any future
            // draws until the animation is done.
            mWindowsAnimating = TRUE;
        }
    }
    else if (mWindowsAnimating) {
        skipDraw = TRUE;
    }
    mFirst = FALSE;
    mWillDrawSoon = FALSE;
    mNewSurfaceNeeded = FALSE;
    mViewVisibility = viewVisibility;

    if (mAttachInfo->mHasWindowFocus && !IsInLocalFocusMode()) {
        const Boolean imTarget =
            CWindowManagerLayoutParams::MayUseInputMethod(WINLAY_PROBE(mWindowAttributes)->mFlags);
        if (imTarget != mLastWasImTarget) {
            mLastWasImTarget = imTarget;
            AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
            if (imm != NULL && imTarget) {
               imm->StartGettingWindowFocus(mView);
               AutoPtr<IView> focView;
               mView->FindFocus((IView**)&focView);
               imm->OnWindowFocus(mView, focView,
                   WINLAY_PROBE(mWindowAttributes)->mSoftInputMode,
                   !mHasHadWindowFocus, WINLAY_PROBE(mWindowAttributes)->mFlags);
            }
        }
    }

    // Remember if we must report the next draw.
    if ((relayoutResult & IWindowManagerGlobal::RELAYOUT_RES_FIRST_TIME) != 0) {
        mReportNextDraw = TRUE;
    }
    Boolean cancelDraw = FALSE;
    mAttachInfo->mTreeObserver->DispatchOnPreDraw(&cancelDraw);

    cancelDraw = cancelDraw || viewVisibility != IView::VISIBLE;
    if (!cancelDraw && !newSurface) {
        if (!skipDraw || mReportNextDraw) {
            if (mPendingTransitions.IsEmpty() == FALSE) {
                List<AutoPtr<ILayoutTransition> >::Iterator iter;
                for (iter = mPendingTransitions.Begin();
                    iter != mPendingTransitions.End(); ++iter) {
                    (*iter)->StartChangingAnimations();
                }
                mPendingTransitions.Clear();
            }

            PerformDraw();
        }
    }
    else {
        if (viewVisibility == IView::VISIBLE) {
            // Try again
            ScheduleTraversals();
        }
        else if (mPendingTransitions.IsEmpty() == FALSE) {
            List<AutoPtr<ILayoutTransition> >::Iterator iter;
            for (iter = mPendingTransitions.Begin();
                iter != mPendingTransitions.End(); ++iter) {
                (*iter)->EndChangingAnimations();
            }
            mPendingTransitions.Clear();
        }
    }

    mIsInTraversal = FALSE;

    LocalTrace(mView, "::LEAVE << ViewRootImpl::PerformTraversals()");
}

CARAPI_(void) ViewRootImpl::HandleOutOfResourcesException(
    /* [in] */ ECode ec)
{
    Logger::D(TAG, "OutOfResourcesException initializing HW surface", ec);
    /*try {
        if (!mWindowSession.outOfMemory(mWindow) &&
                Process.myUid() != Process.SYSTEM_UID) {*/
            Logger::D(TAG, "No processes killed for memory; killing self");
    /*        Process.killProcess(Process.myPid());
        }
    } catch (RemoteException ex) {
    }*/
    mLayoutRequested = TRUE;    // ask wm for a new surface next time.
}

void ViewRootImpl::PerformMeasure(
    /* [in] */ Int32 childWidthMeasureSpec,
    /* [in] */ Int32 childHeightMeasureSpec)
{
    LocalTrace(mView, "::ENTER >> ViewRootImpl::PerformMeasure()");

    //Trace::TraceBegin(Trace::TRACE_TAG_VIEW, "measure");
    mView->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
    //Trace::TraceEnd(Trace::TRACE_TAG_VIEW);

    LocalTrace(mView, "::LEAVE << ViewRootImpl::PerformMeasure()");
}

/**
 * Called by {@link android.view.View#isInLayout()} to determine whether the view hierarchy
 * is currently undergoing a layout pass.
 *
 * @return whether the view hierarchy is currently undergoing a layout pass
 */
ECode ViewRootImpl::IsInLayout(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mInLayout;
    return NOERROR;
}

/**
 * Called by {@link android.view.View#requestLayout()} if the view hierarchy is currently
 * undergoing a layout pass. requestLayout() should not generally be called during layout,
 * unless the container hierarchy knows what it is doing (i.e., it is fine as long as
 * all children in that container hierarchy are measured and laid out at the end of the layout
 * pass for that container). If requestLayout() is called anyway, we handle it correctly
 * by registering all requesters during a frame as it proceeds. At the end of the frame,
 * we check all of those views to see if any still have pending layout requests, which
 * indicates that they were not correctly handled by their container hierarchy. If that is
 * the case, we clear all such flags in the tree, to remove the buggy flag state that leads
 * to blank containers, and force a second request/measure/layout pass in this frame. If
 * more requestLayout() calls are received during that second layout pass, we post those
 * requests to the next frame to avoid possible infinite loops.
 *
 * <p>The return value from this method indicates whether the request should proceed
 * (if it is a request during the first layout pass) or should be skipped and posted to the
 * next frame (if it is a request during the second layout pass).</p>
 *
 * @param view the view that requested the layout.
 *
 * @return true if request should proceed, false otherwise.
 */
ECode ViewRootImpl::RequestLayoutDuringLayout(
    /* [in] */ IView* view,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (VIEW_PROBE(view)->mParent == NULL || VIEW_PROBE(view)->mAttachInfo == NULL) {
        // Would not normally trigger another layout, so just let it pass through as usual
        *res = TRUE;
        return NOERROR;
    }
    Boolean contains;
    mLayoutRequesters->Contains(view, &contains);
    if (!contains) {
        mLayoutRequesters->Add(view);
    }
    if (!mHandlingLayoutInLayoutRequest) {
        // Let the request proceed normally; it will be processed in a second layout pass
        // if necessary
        *res = TRUE;
        return NOERROR;
    }
    else {
        // Don't let the request proceed during the second layout pass.
        // It will post to the next frame instead.
        *res = FALSE;
        return NOERROR;
    }
}

void ViewRootImpl::PerformLayout(
    /* [in] */ IWindowManagerLayoutParams* lp,
    /* [in] */ Int32 desiredWindowWidth,
    /* [in] */ Int32 desiredWindowHeight)
{
    LocalTrace(mView, "::ENTER >> ViewRootImpl::PerformLayout()");

    mLayoutRequested = FALSE;
    mScrollMayChange = TRUE;
    mInLayout = TRUE;

    View* host = VIEW_PROBE(mView);
    Int32 tempWidth, tempHeight;
    host->GetMeasuredWidth(&tempWidth);
    host->GetMeasuredHeight(&tempHeight);
    if (DEBUG_ORIENTATION || DEBUG_LAYOUT) {
        /*Logger::V(TAG, "Laying out 0x%08x to (%d, %d)",
            host, tempWidth, tempHeight);*/
    }

    //Trace::TraceBegin(Trace::TRACE_TAG_VIEW, "layout");
    host->Layout(0, 0, tempWidth, tempHeight);
    //Trace::TraceEnd(Trace::TRACE_TAG_VIEW);

    mInLayout = FALSE;
    Int32 numViewsRequestingLayout = 0;
    mLayoutRequesters->GetSize(&numViewsRequestingLayout);
    if (numViewsRequestingLayout > 0) {
        // requestLayout() was called during layout.
        // If no layout-request flags are set on the requesting views, there is no problem.
        // If some requests are still pending, then we need to clear those flags and do
        // a full request/measure/layout pass to handle this situation.
        AutoPtr<IArrayList> validLayoutRequesters = GetValidLayoutRequesters(mLayoutRequesters, FALSE);
        if (validLayoutRequesters != NULL) {
            // Set this flag to indicate that any further requests are happening during
            // the second pass, which may result in posting those requests to the next
            // frame instead
            mHandlingLayoutInLayoutRequest = TRUE;

            // Process fresh layout requests, then measure and layout
            Int32 numValidRequests = 0;
            validLayoutRequesters->GetSize(&numValidRequests);
            for (Int32 i = 0; i < numValidRequests; ++i) {
                AutoPtr<IInterface> temp;
                validLayoutRequesters->Get(i, (IInterface**)&temp);
                AutoPtr<IView> view = IView::Probe(temp);
                Logger::V("View", "requestLayout() improperly called by %d during layout: running second layout pass", view.Get());
                view->RequestLayout();
            }
            AutoPtr<IContext> mViewContext;
            mView->GetContext((IContext**)&mViewContext);
            AutoPtr<IResources> mViewResources;
            mViewContext->GetResources((IResources**)&mViewResources);
            MeasureHierarchy(host, lp, mViewResources,desiredWindowWidth, desiredWindowHeight);
            mInLayout = TRUE;
            Int32 measuredWidth, measuredHeight;
            host->GetMeasuredWidth(&measuredWidth);
            host->GetMeasuredHeight(&measuredHeight);
            host->Layout(0, 0, measuredWidth, measuredHeight);

            mHandlingLayoutInLayoutRequest = FALSE;

            // Check the valid requests again, this time without checking/clearing the
            // layout flags, since requests happening during the second pass get noop'd
            validLayoutRequesters = GetValidLayoutRequesters(mLayoutRequesters, TRUE);
            if (validLayoutRequesters != NULL) {
                AutoPtr<LayoutRunnable> runnable = new LayoutRunnable(validLayoutRequesters);
                // Post second-pass requests to the next frame
                GetRunQueue()->Post(runnable);
            }
        }

    }
    LocalTrace(mView, "::LEAVE << ViewRootImpl::PerformLayout()");

    mInLayout = FALSE;
}

/**
 * This method is called during layout when there have been calls to requestLayout() during
 * layout. It walks through the list of views that requested layout to determine which ones
 * still need it, based on visibility in the hierarchy and whether they have already been
 * handled (as is usually the case with ListView children).
 *
 * @param layoutRequesters The list of views that requested layout during layout
 * @param secondLayoutRequests Whether the requests were issued during the second layout pass.
 * If so, the FORCE_LAYOUT flag was not set on requesters.
 * @return A list of the actual views that still need to be laid out.
 */
AutoPtr<IArrayList> ViewRootImpl::GetValidLayoutRequesters(
    /* [in] */ IArrayList* layoutRequesters,
    /* [in] */ Boolean secondLayoutRequests)
{

    Int32 numViewsRequestingLayout = 0;
    layoutRequesters->GetSize(&numViewsRequestingLayout);
    AutoPtr<IArrayList> validLayoutRequesters;
    for (Int32 i = 0; i < numViewsRequestingLayout; ++i) {
        AutoPtr<IInterface> temp;
        layoutRequesters->Get(i, (IInterface**)&temp);
        AutoPtr<IView> view = IView::Probe(temp);
        if (view != NULL && VIEW_PROBE(view)->mAttachInfo != NULL && VIEW_PROBE(view)->mParent != NULL &&
                (secondLayoutRequests || (VIEW_PROBE(view)->mPrivateFlags & View::PFLAG_FORCE_LAYOUT) ==
                        View::PFLAG_FORCE_LAYOUT)) {
            Boolean gone = FALSE;
            AutoPtr<IView> parent = view;
            // Only trigger new requests for views in a non-GONE hierarchy
            while (parent != NULL) {
                if ((VIEW_PROBE(view)->mViewFlags & View::VISIBILITY_MASK) == IView::GONE) {
                    gone = TRUE;
                    break;
                }
                if (IView::Probe(VIEW_PROBE(parent)->mParent)) {
                    parent = IView::Probe(VIEW_PROBE(parent)->mParent);
                }
                else {
                    parent = NULL;
                }
            }
            if (!gone) {
                if (validLayoutRequesters == NULL) {
                    CArrayList::New((IArrayList**)&validLayoutRequesters);
                }
                validLayoutRequesters->Add(view);
            }
        }
    }
    if (!secondLayoutRequests) {
        // If we're checking the layout flags, then we need to clean them up also
        for (Int32 i = 0; i < numViewsRequestingLayout; ++i) {
            AutoPtr<IInterface> temp;
            layoutRequesters->Get(i, (IInterface**)&temp);
            AutoPtr<IView> view = IView::Probe(temp);
            while (view != NULL &&
                    (VIEW_PROBE(view)->mPrivateFlags & View::PFLAG_FORCE_LAYOUT) != 0) {
                VIEW_PROBE(view)->mPrivateFlags &= ~View::PFLAG_FORCE_LAYOUT;
                if (IView::Probe(VIEW_PROBE(view)->mParent)) {
                    view = IView::Probe(VIEW_PROBE(view)->mParent);
                }
                else {
                    view = NULL;
                }
            }
        }
    }
    layoutRequesters->Clear();
    return validLayoutRequesters;
}

ECode ViewRootImpl::RequestTransparentRegion(
    /* [in] */ IView* child)
{
    // the test below should not fail unless someone is messing with us
    //
    FAIL_RETURN(CheckThread());

    if (mView.Get() == child) {
        VIEW_PROBE(mView)->mPrivateFlags
            |= View::PFLAG_REQUEST_TRANSPARENT_REGIONS;

        // Need to make sure we re-evaluate the window attributes next
        // time around, to ensure the window has the correct format.
        //
        mWindowAttributesChanged = TRUE;
        mWindowAttributesChangesFlag = 0;
        RequestLayout();
    }

    return NOERROR;
}

Int32 ViewRootImpl::GetRootMeasureSpec(
    /* [in] */ Int32 windowSize,
    /* [in] */ Int32 rootDimension)
{
    Int32 measureSpec;
    switch (rootDimension) {
        case IViewGroupLayoutParams::MATCH_PARENT:
            // Window can't resize. Force root view to be windowSize.
            measureSpec = View::MeasureSpec::MakeMeasureSpec(
                windowSize, View::MeasureSpec::EXACTLY);
            break;
        case IViewGroupLayoutParams::WRAP_CONTENT:
            // Window can resize. Set max size for root view.
            measureSpec = View::MeasureSpec::MakeMeasureSpec(
                windowSize, View::MeasureSpec::AT_MOST);
            break;
        default:
            // Window wants to be an exact size. Force root view to be that size.
            measureSpec = View::MeasureSpec::MakeMeasureSpec(
                rootDimension, View::MeasureSpec::EXACTLY);
            break;
    }

    return measureSpec;
}

ECode ViewRootImpl::OnHardwarePreDraw(
    /* [in] */ IHardwareCanvas* canvas)
{
    ICanvas::Probe(canvas)->Translate(-mHardwareXOffset, -mHardwareYOffset);
    return NOERROR;
}

ECode ViewRootImpl::OnHardwarePostDraw(
    /* [in] */ IHardwareCanvas* canvas)
{
    if (mResizeBuffer != NULL) {
        mResizePaint->SetAlpha(mResizeAlpha);
        canvas->DrawHardwareLayer(mResizeBuffer, mHardwareXOffset, mHardwareYOffset, mResizePaint);
    }
    return NOERROR;
}

/**
 * @hide
 */
ECode ViewRootImpl::OutputDisplayList(
    /* [in] */ IView* view)
{
    AutoPtr<IRenderNode> renderNode;
    view->GetDisplayList((IRenderNode**)&renderNode);
    if (renderNode != NULL) {
        renderNode->Output();
    }

    return NOERROR;
}

/**
 * @see #PROPERTY_PROFILE_RENDERING
 */
void ViewRootImpl::ProfileRendering(
    /* [in] */ Boolean enabled)
{
    if (mProfileRendering) {
        mRenderProfilingEnabled = enabled;
        if (mRenderProfiler != NULL) {
            mChoreographer->RemoveFrameCallback(mRenderProfiler);
        }
        if (mRenderProfilingEnabled) {
            if (mRenderProfiler == NULL) {
                mRenderProfiler = new ProfileFrameCallback(this);
            }
            mChoreographer->PostFrameCallback(mRenderProfiler);
        }
        else {
            mRenderProfiler = NULL;
        }
    }
}

/**
 * Called from draw() when DEBUG_FPS is enabled
 */
void ViewRootImpl::TrackFPS()
{
    // Tracks frames per second drawn. First value in a series of draws may be bogus
    // because it down not account for the intervening idle time
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nowTime;
    system->GetCurrentTimeMillis(&nowTime);
    if (mFpsStartTime < 0) {
        mFpsStartTime = mFpsPrevTime = nowTime;
        mFpsNumFrames = 0;
    }
    else {
        ++mFpsNumFrames;
        Int64 frameTime = nowTime - mFpsPrevTime;
        Int64 totalTime = nowTime - mFpsStartTime;
        Logger::V(TAG, "0x%08x\tFrame time:\t%lld", this, frameTime);
        mFpsPrevTime = nowTime;
        if (totalTime > 1000) {
            Float fps = (Float)mFpsNumFrames * 1000 / totalTime;
            Logger::V(TAG, "0x%08x\tFPS time:\t%f", this, fps);
            mFpsStartTime = nowTime;
            mFpsNumFrames = 0;
        }
    }
}

void ViewRootImpl::PerformDraw()
{
    LocalTrace(mView, "::ENTER >> ViewRootImpl::PerformDraw()");

    if (!mAttachInfo->mDisplayState == IDisplay::STATE_OFF && !mReportNextDraw) {
        return;
    }

    Boolean fullRedrawNeeded = mFullRedrawNeeded;
    mFullRedrawNeeded = FALSE;

    mIsDrawing = TRUE;
    //Trace::TraceBegin(Trace::TRACE_TAG_VIEW, "draw");

    Draw(fullRedrawNeeded);

    mIsDrawing = FALSE;
    //Trace::TraceEnd(Trace::TRACE_TAG_VIEW);

    // For whatever reason we didn't create a HardwareRenderer, end any
    // hardware animations that are now dangling
    if (mAttachInfo->mPendingAnimatingRenderNodes != NULL) {
        Int32 count = 0;
        mAttachInfo->mPendingAnimatingRenderNodes->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> temp;
            mAttachInfo->mPendingAnimatingRenderNodes->Get(i, (IInterface**)&temp);
            AutoPtr<IRenderNode> node = IRenderNode::Probe(temp);
            node->EndAllAnimators();
        }
        mAttachInfo->mPendingAnimatingRenderNodes->Clear();
    }

    if (mReportNextDraw) {
        mReportNextDraw = FALSE;
        if (mAttachInfo->mHardwareRenderer != NULL) {
            mAttachInfo->mHardwareRenderer->Fence();
        }

        if (LOCAL_LOGV) {
            AutoPtr<ICharSequence> title;
            mWindowAttributes->GetTitle((ICharSequence**)&title);
            String str;
            title->ToString(&str);
            Logger::V(TAG, "FINISHED DRAWING: %s", str.string());
        }
        Boolean isValid;
        if (mSurfaceHolder != NULL && (mSurface->IsValid(&isValid), isValid)) {
            mSurfaceHolderCallback->SurfaceRedrawNeeded(mSurfaceHolder);

            AutoPtr< ArrayOf<ISurfaceHolderCallback*> > callbacks = mSurfaceHolder->GetCallbacks();
            for (Int32 i = 0; i < callbacks->GetLength(); i++) {
                ISurfaceHolderCallback2* callback2 = ISurfaceHolderCallback2::Probe((*callbacks)[i]);
                if (callback2) {
                    callback2->SurfaceRedrawNeeded(mSurfaceHolder);
                }
            }
        }

        mWindowSession->FinishDrawing(mWindow);
    }

    LocalTrace(mView, "::LEAVE << ViewRootImpl::PerformDraw()");
}

void ViewRootImpl::Draw(
    /* [in] */ Boolean fullRedrawNeeded)
{
    ISurface* surface = mSurface;
    Boolean surfaceValid = FALSE;
    if (surface == NULL ||
        !(surface->IsValid(&surfaceValid), surfaceValid)) {
        return;
    }

    if (DEBUG_FPS) {
        TrackFPS();
    }

    if (!sFirstDrawComplete) {
        AutoLock lock(sFirstDrawHandlersLock);

        sFirstDrawComplete = TRUE;
        Boolean bval;
        List<AutoPtr<IRunnable> >::Iterator iter = sFirstDrawHandlers.Begin();
        for (; iter != sFirstDrawHandlers.End(); ++iter) {
            mHandler->Post(iter->Get(), &bval);
        }
    }

    Boolean isScrollToRectOrFocus;
    ScrollToRectOrFocus(NULL, FALSE, &isScrollToRectOrFocus);

    if (mAttachInfo->mViewScrollChanged) {
        mAttachInfo->mViewScrollChanged = FALSE;
        ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->DispatchOnScrollChanged();
    }

    Boolean tempAnimating;
    Boolean animating = mScroller != NULL
        && (mScroller->ComputeScrollOffset(&tempAnimating), tempAnimating);
    Int32 curScrollY;
    if (animating) {
        mScroller->GetCurrY(&curScrollY);
    }
    else {
        curScrollY = mScrollY;
    }

    if (mCurScrollY != curScrollY) {
        mCurScrollY = curScrollY;
        fullRedrawNeeded = TRUE;
    }

    Float appScale = mAttachInfo->mApplicationScale;
    Boolean scalingRequired = mAttachInfo->mScalingRequired;

    Int32 resizeAlpha = 0;
    if (mResizeBuffer != NULL) {
        Int64 deltaTime = SystemClock::GetUptimeMillis() - mResizeBufferStartTime;
        if (deltaTime < mResizeBufferDuration) {
            Float amt = deltaTime/(Float)mResizeBufferDuration;
            Float newAmt;
            ITimeInterpolator::Probe(mResizeInterpolator)->GetInterpolation(amt, &newAmt);
            animating = TRUE;
            resizeAlpha = 255 - (Int32)(newAmt*255);
        }
        else {
            DisposeResizeBuffer();
        }
    }

    CRect* dirty = mDirty;
    if (mSurfaceHolder != NULL) {
        // The app owns the surface, we won't draw.
        //
        dirty->SetEmpty();
        if (animating) {
            if (mScroller != NULL) {
                mScroller->AbortAnimation();
            }
            DisposeResizeBuffer();
        }
        return;
    }

    if (fullRedrawNeeded) {
        mAttachInfo->mIgnoreDirtyState = TRUE;
        dirty->Set(0, 0, (Int32)(mWidth * appScale + 0.5f), (Int32)(mHeight * appScale + 0.5f));
    }

    //if (DEBUG_ORIENTATION || DEBUG_DRAW) {
    //    Logger::V(TAG, "Draw " + mView + "/"
    //        + mWindowAttributes.getTitle()
    //        + ": dirty={" + dirty->mLeft + "," + dirty->top
    //        + "," + dirty->right + "," + dirty->bottom + "} surface="
    //        + surface + " surface.isValid()=" + surface.isValid() + ", appScale:" +
    //        appScale + ", width=" + mWidth + ", height=" + mHeight);
    //}

    mAttachInfo->mTreeObserver->DispatchOnDraw();

    Int32 xOffset = 0;
    Int32 yOffset = curScrollY;
    CWindowManagerLayoutParams* params = WINLAY_PROBE(mWindowAttributes);
    AutoPtr<IRect> obj = params != NULL ? params->mSurfaceInsets : NULL;
    CRect* surfaceInsets = (CRect*)obj.Get();
    if (surfaceInsets != NULL) {
        xOffset -= surfaceInsets->mLeft;
        yOffset -= surfaceInsets->mTop;

        // Offset dirty rect for surface insets.
        dirty->Offset(surfaceInsets->mLeft, surfaceInsets->mRight);
    }

    Boolean isEmpty;
    if (!(dirty->IsEmpty(&isEmpty), isEmpty) || mIsAnimating) {
        Boolean isEnabled;
        if (mAttachInfo->mHardwareRenderer != NULL
            && (mAttachInfo->mHardwareRenderer->IsEnabled(&isEnabled), isEnabled)) {
            // Draw with hardware renderer.
            mIsAnimating = FALSE;
            if (mHardwareYOffset != yOffset || mHardwareXOffset != xOffset) {
                mHardwareYOffset = yOffset;
                mHardwareXOffset = xOffset;
                mAttachInfo->mHardwareRenderer->InvalidateRoot();
            }
            mResizeAlpha = resizeAlpha;

            // sometimes we get the dirty rect as null
            // and also its better to check the surface
            // validity to avoid any crash.
            Boolean valid;
            if((mSurface->IsValid(&valid), valid) && dirty != NULL) {
                mSurface->SetDirtyRect(dirty);
            }

            dirty->SetEmpty();

            mBlockResizeBuffer = false;
            ((HardwareRenderer*)mAttachInfo->mHardwareRenderer.Get())->Draw(mView, mAttachInfo, this);
        } else {
            // If we get here with a disabled & requested hardware renderer, something went
            // wrong (an invalidate posted right before we destroyed the hardware surface
            // for instance) so we should just bail out. Locking the surface with software
            // rendering at this point would lock it forever and prevent hardware renderer
            // from doing its job when it comes back.
            // Before we request a new frame we must however attempt to reinitiliaze the
            // hardware renderer if it's in requested state. This would happen after an
            // eglTerminate() for instance.
            Boolean isEnabled, isRequested;
            if (mAttachInfo->mHardwareRenderer != NULL &&
                    (mAttachInfo->mHardwareRenderer->IsEnabled(&isEnabled), !isEnabled) &&
                    (mAttachInfo->mHardwareRenderer->IsRequested(&isRequested), isRequested)) {

                //try {
                ECode ec = mAttachInfo->mHardwareRenderer->InitializeIfNeeded(
                    mWidth, mHeight, mSurface, surfaceInsets, &isEnabled);

                //} catch (OutOfResourcesException e) {
                if (ec == (ECode) E_OUT_OF_RESOURCES_EXCEPTION) {
                    HandleOutOfResourcesException(ec);
                    return;
                }
                //}

                mFullRedrawNeeded = TRUE;
                ScheduleTraversals();
                return;
            }

            if (!DrawSoftware(surface, mAttachInfo, xOffset, yOffset, scalingRequired, dirty)) {
                return;
            }
        }
    }

    if (animating) {
        mFullRedrawNeeded = TRUE;
        ScheduleTraversals();
    }
}

/**
 * @return TRUE if drawing was succesfull, FALSE if an error occurred
 */
Boolean ViewRootImpl::DrawSoftware(
    /* [in] */ ISurface* surface,
    /* [in] */ View::AttachInfo* attachInfo,
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Boolean scalingRequired,
    /* [in] */ CRect* dirty)
{
    AutoPtr<ICanvas> canvas;

    Int32 left = dirty->mLeft;
    Int32 top = dirty->mTop;
    Int32 right = dirty->mRight;
    Int32 bottom = dirty->mBottom;

    ECode ec;
    do {
        ec = mSurface->LockCanvas(dirty, (ICanvas**)&canvas);
        if (FAILED(ec)) {
            break;
        }
        // The dirty rectangle can be modified by Surface.lockCanvas()
        //noinspection ConstantConditions
        if (left != dirty->mLeft || top != dirty->mTop || right != dirty->mRight ||
            bottom != dirty->mBottom) {
            mAttachInfo->mIgnoreDirtyState = TRUE;
        }

        // TODO: Do this in native
        ec = canvas->SetDensity(mDensity);
    } while(0);

    if (ec == (ECode)E_OUT_OF_RESOURCES_EXCEPTION) {
        HandleOutOfResourcesException(ec);
        return FALSE;
    }
    else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::E(TAG, "Could not lock surface");
        // Don't assume this is due to out of memory, it could be
        // something else, and if it is something else then we could
        // kill stuff (or ourself) for no reason.
        mLayoutRequested = TRUE;    // ask wm for a new surface next time.
        return FALSE;
    }

    if (DEBUG_ORIENTATION || DEBUG_DRAW) {
        Int32 w, h;
        canvas->GetWidth(&w);
        canvas->GetHeight(&h);
        Logger::V(TAG, "Surface 0x%08x drawing to bitmap w=%d, h=%d",
            surface, w, h);
        canvas->DrawARGB(255, 255, 0, 0);
    }

    // If this bitmap's format includes an alpha channel, we
    // need to clear it before drawing so that the child will
    // properly re-composite its drawing on a transparent
    // background. This automatically respects the clip/dirty region
    // or
    // If we are applying an offset, we need to clear the area
    // where the offset doesn't appear to avoid having garbage
    // left in the blank areas.
    Boolean isOpaque = FALSE;
    canvas->IsOpaque(&isOpaque);
    if (!isOpaque || yoff != 0 || xoff != 0) {
        if (FAILED(canvas->DrawColor(0, Elastos::Droid::Graphics::PorterDuffMode_CLEAR))) {
            goto _DrawSoftware_;
        }
    }

    dirty->SetEmpty();
    mIsAnimating = FALSE;
    mAttachInfo->mDrawingTime = SystemClock::GetUptimeMillis();
    VIEW_PROBE(mView)->mPrivateFlags |= View::PFLAG_DRAWN;

    //if (DEBUG_DRAW) {
    //    AutoPtr<IContext> cxt;
    //    mView->GetContext((IContext**)&cxt);

    //    Logger::I(TAG, "Drawing: package:" + cxt.getPackageName() +
    //        ", metrics=" + cxt.getResources().getDisplayMetrics() +
    //        ", compatibilityInfo=" + cxt.getResources().getCompatibilityInfo());
    //}

    canvas->Translate((Float)-xoff, (Float)-yoff);
    if (mTranslator != NULL) {
        mTranslator->TranslateCanvas(canvas);
    }

    canvas->SetScreenDensity(scalingRequired ? mNoncompatDensity : 0);
    mAttachInfo->mIgnoreDirtyState = FALSE;

    mView->Draw(canvas);

    if (!attachInfo->mSetIgnoreDirtyState) {
        // Only clear the flag if it was not set during the mView.draw() call
        attachInfo->mIgnoreDirtyState = FALSE;
    }

_DrawSoftware_:
    if (FAILED(surface->UnlockCanvasAndPost(canvas))) {
        Logger::E(TAG, "Could not unlock surface");
        mLayoutRequested = TRUE;    // ask wm for a new surface next time.
        //noinspection ReturnInsideFinallyBlock
        return FALSE;
    }

    if (LOCAL_LOGV) {
        Logger::V(TAG, "Surface 0x%08x unlockCanvasAndPost", surface);
    }

    return TRUE;
}

ECode ViewRootImpl::GetAccessibilityFocusedDrawable(
    /* [out] */ IDrawable** res)
{
    VALIDATE_NOT_NULL(res)
    // Lazily load the accessibility focus drawable.
    if (mAttachInfo->mAccessibilityFocusDrawable == NULL) {
        AutoPtr<ITypedValue> value;
        CTypedValue::New((ITypedValue**)&value);
        AutoPtr<IContext> context;
        mView->GetContext((IContext**)&context);
        AutoPtr<IResourcesTheme> theme;
        context->GetTheme((IResourcesTheme**)&theme);
        Boolean resolved;
        theme->ResolveAttribute(
            R::attr::accessibilityFocusedDrawable, value, TRUE, &resolved);
        if (resolved) {
            Int32 resourceId;
            value->GetResourceId(&resourceId);
            VIEW_PROBE(mView)->mContext->GetDrawable(resourceId,
                (IDrawable**)&mAttachInfo->mAccessibilityFocusDrawable);
        }
    }
    *res = mAttachInfo->mAccessibilityFocusDrawable;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode ViewRootImpl::SetDrawDuringWindowsAnimating(
    /* [in] */ Boolean value)
{
    mDrawDuringWindowsAnimating = value;
    if (value) {
        HandleDispatchDoneAnimating();
    }
    return NOERROR;
}

ECode ViewRootImpl::ScrollToRectOrFocus(
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    CRect* ci = (CRect*)mAttachInfo->mContentInsets.Get();
    CRect* vi = (CRect*)mAttachInfo->mVisibleInsets.Get();
    Int32 scrollY = 0;
    Boolean handled = FALSE;

    if (vi->mLeft > ci->mLeft || vi->mTop > ci->mTop
        || vi->mRight > ci->mRight || vi->mBottom > ci->mBottom) {
        // We'll assume that we aren't going to change the scroll
        // offset, since we want to avoid that unless it is actually
        // going to make the focus visible...  otherwise we scroll
        // all over the place.
        //
        scrollY = mScrollY;

        // We can be called for two different situations: during a draw,
        // to update the scroll position if the focus has changed (in which
        // case 'rectangle' is NULL), or in response to a
        // requestChildRectangleOnScreen() call (in which case 'rectangle'
        // is non-NULL and we just want to scroll to whatever that
        // rectangle is).
        //
        AutoPtr<IView> focus;
        mView->FindFocus((IView**)&focus);

        if (focus == NULL) {
            *res = FALSE;
            return NOERROR;
        }

        AutoPtr<IView> lastScrolledFocus;
        if (mLastScrolledFocus != NULL) {
            AutoPtr<IInterface> obj;
            mLastScrolledFocus->Resolve(EIID_IInterface, (IInterface**)&obj);
            lastScrolledFocus = IView::Probe(obj);
        } else {
            lastScrolledFocus = NULL;
        }

        if ((IView*)focus->Probe(EIID_IView) != lastScrolledFocus) {
            // If the focus has changed, then ignore any requests to scroll
            // to a rectangle; first we want to make sure the entire focus
            // view is visible.
            //
            rectangle = NULL;
        }

        //if (DEBUG_INPUT_RESIZE) Logger::V(TAG, "Eval scroll: focus=" + focus
        //    + " rectangle=" + rectangle + " ci=" + ci
        //    + " vi=" + vi);

        if ((IView*)focus->Probe(EIID_IView) == lastScrolledFocus
            && !mScrollMayChange && rectangle == NULL) {
                //// Optimization: if the focus hasn't changed since last
                //// time, and no layout has happened, then just leave things
                //// as they are.
                //if (DEBUG_INPUT_RESIZE) Logger::V(TAG, "Keeping scroll y="
                //    + mScrollY + " vi=" + vi->toShortString());
        }
        else {
            // We need to determine if the currently focused view is
            // within the visible part of the window and, if not, apply
            // a pan so it can be seen.
            //
            IWeakReferenceSource* source = IWeakReferenceSource::Probe(focus);
            source->GetWeakReference((IWeakReference**)&mLastScrolledFocus);
            mScrollMayChange = FALSE;

            if (DEBUG_INPUT_RESIZE) {
                Logger::D(TAG, "Need to scroll?");
            }

            // Try to find the rectangle from the focus view.
            //
            if (focus->GetGlobalVisibleRect(mVisRect, NULL)) {
                //if (DEBUG_INPUT_RESIZE) Logger::V(TAG, "Root w="
                //    + mView.getWidth() + " h=" + mView.getHeight()
                //    + " ci=" + ci->toShortString()
                //    + " vi=" + vi->toShortString());

                if (rectangle == NULL) {
                    focus->GetFocusedRect(mTempRect);

                    //if (DEBUG_INPUT_RESIZE) Logger::V(TAG, "Focus " + focus
                    //    + ": focusRect=" + mTempRect.toShortString());

                    IViewGroup* viewGroup =
                        (IViewGroup*)mView->Probe(EIID_IViewGroup);
                    if (viewGroup) {
                        viewGroup->OffsetDescendantRectToMyCoords(
                            (IView*)focus->Probe(EIID_IView), (IRect*)mTempRect);
                    }

                    //if (DEBUG_INPUT_RESIZE) Logger::V(TAG,
                    //    "Focus in window: focusRect="
                    //    + mTempRect.toShortString()
                    //    + " visRect=" + mVisRect.toShortString());
                }
                else {
                    mTempRect->Set(rectangle);
                    //if (DEBUG_INPUT_RESIZE) Logger::V(TAG,
                    //    "Request scroll to rect: "
                    //    + mTempRect.toShortString()
                    //    + " visRect=" + mVisRect.toShortString());
                }

                Boolean isIntersect;
                mTempRect->Intersect(mVisRect, &isIntersect);
                if (isIntersect) {
                    //if (DEBUG_INPUT_RESIZE) Logger::V(TAG,
                    //    "Focus window visible rect: "
                    //    + mTempRect.toShortString());

                    Int32 height, tempHeight;
                    mView->GetHeight(&height);
                    mTempRect->GetHeight(&tempHeight);
                    if (tempHeight > (height - vi->mTop - vi->mBottom)) {
                        // If the focus simply is not going to fit, then
                        // best is probably just to leave things as-is.
                        //
                        if (DEBUG_INPUT_RESIZE) {
                            Logger::D(TAG, "Too tall; leaving scrollY=%d", scrollY);
                        }
                    }
                    else if ((mTempRect->mTop-scrollY) < vi->mTop) {
                        scrollY -= vi->mTop - (mTempRect->mTop-scrollY);
                        if (DEBUG_INPUT_RESIZE) {
                            Logger::D(TAG, "Top covered; scrollY=%d", scrollY);
                        }
                    }
                    else if ((mTempRect->mBottom - scrollY) >
                        (height - vi->mBottom)) {
                        scrollY += (mTempRect->mBottom - scrollY)
                            - (height - vi->mBottom);

                        if (DEBUG_INPUT_RESIZE) {
                            Logger::D(TAG, "Bottom covered; scrollY=%d", scrollY);
                        }
                    }
                    handled = TRUE;
                }
            }
        }
    }

    if (scrollY != mScrollY) {
        if (DEBUG_INPUT_RESIZE) {
            Logger::D(TAG, "Pan scroll changed: old=%d, new=%d", mScrollY, scrollY);
        }
        if (!immediate && mResizeBuffer == NULL) {
            if (mScroller == NULL) {
                AutoPtr<IContext> ctx;
                mView->GetContext((IContext**)&ctx);
                CScroller::NewByFriend(ctx, (CScroller**)&mScroller);
            }
            mScroller->StartScroll(0, mScrollY, 0, scrollY - mScrollY);
        }
        else if (mScroller != NULL) {
            mScroller->AbortAnimation();
        }
        mScrollY = scrollY;
    }

    *res = handled;
    return NOERROR;
}

ECode ViewRootImpl::GetAccessibilityFocusedHost(
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mAccessibilityFocusedHost;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode ViewRootImpl::GetAccessibilityFocusedVirtualView(
    /* [out] */ IAccessibilityNodeInfo** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mAccessibilityFocusedVirtualView;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode ViewRootImpl::SetAccessibilityFocus(
    /* [in] */ IView* view,
    /* [in] */ IAccessibilityNodeInfo* node)
{
    // If we have a virtual view with accessibility focus we need
    // to clear the focus and Invalidate the virtual view bounds.
    if (mAccessibilityFocusedVirtualView != NULL) {

        IAccessibilityNodeInfo* focusNode = mAccessibilityFocusedVirtualView;
        View* focusHost = VIEW_PROBE(mAccessibilityFocusedHost);

        // Wipe the state of the current accessibility focus since
        // the call into the provider to clear accessibility focus
        // will fire an accessibility event which will end up calling
        // this method and we want to have clean state when this
        // invocation happens.
        mAccessibilityFocusedHost = NULL;
        mAccessibilityFocusedVirtualView = NULL;

        // Clear accessibility focus on the host after clearing state since
        // this method may be reentrant.
        focusHost->ClearAccessibilityFocusNoCallbacks();

        AutoPtr<IAccessibilityNodeProvider> provider;
        focusHost->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        if (provider != NULL) {
            // Invalidate the area of the cleared accessibility focus.
            focusNode->GetBoundsInParent(mTempRect);
            focusHost->Invalidate(mTempRect);
            // Clear accessibility focus in the virtual node.
            Int64 sourceNodeId;
            focusNode->GetSourceNodeId(&sourceNodeId);
            Int32 virtualNodeId = CAccessibilityNodeInfo::GetVirtualDescendantId(sourceNodeId);
            Boolean result;
            provider->PerformAction(virtualNodeId,
                IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS, NULL, &result);
        }
        focusNode->Recycle();
    }
    if (mAccessibilityFocusedHost != NULL) {
        // Clear accessibility focus in the view.
        VIEW_PROBE(mAccessibilityFocusedHost)->ClearAccessibilityFocusNoCallbacks();
    }

    // Set the new focus host and node.
    mAccessibilityFocusedHost = view;
    mAccessibilityFocusedVirtualView = node;

    if (mAttachInfo->mHardwareRenderer != NULL) {
        mAttachInfo->mHardwareRenderer->InvalidateRoot();
    }

    return NOERROR;
}

ECode ViewRootImpl::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    if (DEBUG_INPUT_RESIZE) {
        Logger::V(TAG, "Request child focus: focus now 0x%08x", focused);
    }

    FAIL_RETURN(CheckThread());
    ScheduleTraversals();

    return NOERROR;
}

ECode ViewRootImpl::ClearChildFocus(
    /* [in] */ IView* child)
{

    if (DEBUG_INPUT_RESIZE) {
        Logger::D(TAG, "Clearing child focus");
    }

    FAIL_RETURN(CheckThread());
    ScheduleTraversals();

    return NOERROR;
}

//@Override
ECode ViewRootImpl::GetParentForAccessibility(
    /* [out] */ IViewParent** parent)
{
    VALIDATE_NOT_NULL(parent);
    *parent = NULL;

    return NOERROR;
}

ECode ViewRootImpl::FocusableViewAvailable(
    /* [in] */ IView* v)
{
    assert(v);
    FAIL_RETURN(CheckThread());

    if (mView != NULL) {
        Boolean hasFocus;
        mView->HasFocus(&hasFocus);
        if (!hasFocus) {
            Boolean result;
            v->RequestFocus(&result);
        }
        else {
            // the one case where will transfer focus away from the current one
            // is if the current view is a view group that prefers to give focus
            // to its children first AND the view is a descendant of it.
            //
            AutoPtr<IView> focused;
            mView->FindFocus((IView**)&focused);

            if (IViewGroup::Probe(focused)) {
                AutoPtr<IViewGroup> group = IViewGroup::Probe(focused);
                Int32 focusability = 0;
                group->GetDescendantFocusability(&focusability);
                if (focusability == IViewGroup::FOCUS_AFTER_DESCENDANTS
                        && IsViewDescendantOf(v, focused)) {
                    Boolean temp;
                    v->RequestFocus(&temp);
                }
            }
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::RecomputeViewAttributes(
    /* [in] */ IView* child)
{
    FAIL_RETURN(CheckThread());

    if (mView.Get() == child) {
        mAttachInfo->mRecomputeGlobalAttributes = TRUE;
        if (!mWillDrawSoon) {
            ScheduleTraversals();
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::DispatchDetachedFromWindow()
{
    if (mView != NULL && VIEW_PROBE(mView)->mAttachInfo != NULL) {
        ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->DispatchOnWindowAttachedChange(FALSE);
        VIEW_PROBE(mView)->DispatchDetachedFromWindow();
    }

    mAccessibilityInteractionConnectionManager->EnsureNoConnection();
    Boolean result;
    mAccessibilityManager->RemoveAccessibilityStateChangeListener(
        mAccessibilityInteractionConnectionManager, &result);
    mAccessibilityManager->RemoveHighTextContrastStateChangeListener(mHighContrastTextManager, &result);
    RemoveSendWindowContentChangedCallback();

    DestroyHardwareRenderer();

    SetAccessibilityFocus(NULL, NULL);

    VIEW_PROBE(mView)->AssignParent(NULL);
    mView = NULL;
    mAttachInfo->mRootView = NULL;

    mSurface->ReleaseSurface();

    if (mInputQueueCallback != NULL && mInputQueue != NULL) {
        mInputQueueCallback->OnInputQueueDestroyed(mInputQueue);
        mInputQueue->Dispose();
        mInputQueueCallback = NULL;
        mInputQueue = NULL;
    }
    if (mInputEventReceiver != NULL) {
        mInputEventReceiver->Dispose();
        mInputEventReceiver = NULL;
    }

    mWindowSession->Remove(mWindow);

    // Dispose the input channel after removing the window so the Window Manager
    // doesn't interpret the input channel being closed as an abnormal termination.
    if (mInputChannel != NULL) {
        mInputChannel->Dispose();
        mInputChannel = NULL;
    }

    mDisplayManager->UnregisterDisplayListener(mDisplayListener);

    UnscheduleTraversals();

    return NOERROR;
}

ECode ViewRootImpl::UpdateConfiguration(
    /* [in] */ IConfiguration* config,
    /* [in] */ Boolean force)
{
    //if (DEBUG_CONFIGURATION) Logger::V(TAG,
    //    "Applying new config to window "
    //    + mWindowAttributes.getTitle()
    //    + ": " + config);

    AutoPtr<ICompatibilityInfo> ci;
    mDisplayAdjustments->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
    Boolean equal;
    IObject::Probe(ci)->Equals(CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO, &equal);
    if (!equal) {
        AutoPtr<IConfiguration> _config;
        CConfiguration::New(config, (IConfiguration**)&_config);
        config = _config;
        ci->ApplyToConfiguration(mNoncompatDensity, config);
    }

    {
        AutoLock lock(sConfigCallbacksLock);

        List<AutoPtr<IComponentCallbacks> >::Iterator iter;
        for (iter = sConfigCallbacks.Begin(); iter != sConfigCallbacks.End(); ++iter) {
            iter->Get()->OnConfigurationChanged(config);
        }
    }

    if (mView != NULL) {
        // At this point the resources have been updated to
        // have the most recent config, whatever that is.  Use
        // the on in them which may be newer.
        //
        AutoPtr<IResources> resources;
        mView->GetResources((IResources**)&resources);
        assert(resources);

        config = NULL;
        resources->GetConfiguration((IConfiguration**)&config);
        assert(NULL != config);

        Int32 result = 0;
        mLastConfiguration->Diff((CConfiguration*)config, &result);

        if (force || (result != 0)) {
            Int32 lastLayoutDirection;
            mLastConfiguration->GetLayoutDirection(&lastLayoutDirection);
            Int32 currentLayoutDirection;
            config->GetLayoutDirection(&currentLayoutDirection);
            mLastConfiguration->SetTo(config);
            if (lastLayoutDirection != currentLayoutDirection &&
                mViewLayoutDirectionInitial == IView::LAYOUT_DIRECTION_INHERIT) {
                mView->SetLayoutDirection(currentLayoutDirection);
            }
            mView->DispatchConfigurationChanged(config);
        }
    }

    return NOERROR;
}

/**
 * Return TRUE if child is an ancestor of parent, (or equal to the parent).
 */
Boolean ViewRootImpl::IsViewDescendantOf(
    /* [in] */ IView* child,
    /* [in] */ IView* parent)
{
    assert(child != NULL && parent != NULL);
    if (child == parent) {
        return TRUE;
    }

    AutoPtr<IViewParent> theParent;
    child->GetParent((IViewParent**)&theParent);

    return IViewGroup::Probe(theParent) && IsViewDescendantOf(IView::Probe(theParent), parent);
}

void ViewRootImpl::ForceLayout(
    /* [in] */ IView* view)
{
    assert(view != NULL);
    view->ForceLayout();
    IViewGroup* group = IViewGroup::Probe(view);
    if (group) {
        Int32 count = 0;
        group->GetChildCount(&count);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            group->GetChildAt(i, (IView**)&child);
            ForceLayout(child);
        }
    }
}

/**
* Something in the current window tells us we need to change the touch mode.  For
* example, we are not in touch mode, and the user touches the screen.
*
* If the touch mode has changed, tell the window manager, and handle it locally.
*
* @param inTouchMode Whether we want to be in touch mode.
* @return True if the touch mode changed and focus changed was changed as a result
*/
ECode ViewRootImpl::EnsureTouchMode(
    /* [in] */ Boolean inTouchMode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (DBG) {
        Logger::D("touchmode", "ensureTouchMode(%d), current touch mode is%d",
            inTouchMode, mAttachInfo->mInTouchMode);
    }

    if (mAttachInfo->mInTouchMode == inTouchMode) {
        *res = FALSE;
        return NOERROR;
    }

    // tell the window manager
    //
    if (!IsInLocalFocusMode()) {
        mWindowSession->SetInTouchMode(inTouchMode);
    }

    //try {
    //    mWindowSession.setInTouchMode(inTouchMode);
    //} catch (RemoteException e) {
    //    throw new RuntimeException(e);
    //}

    // handle the change
    return EnsureTouchModeLocally(inTouchMode);
}

/**
* Ensure that the touch mode for this window is set, and if it is changing,
* take the appropriate action.
* @param inTouchMode Whether we want to be in touch mode.
* @return True if the touch mode changed and focus changed was changed as a result
*/
Boolean ViewRootImpl::EnsureTouchModeLocally(
    /* [in] */ Boolean inTouchMode)
{
    if (DBG) {
        Logger::D("touchmode", "ensureTouchModeLocally(%d), current touch mode is %d",
            inTouchMode, mAttachInfo->mInTouchMode);
    }

    if (mAttachInfo->mInTouchMode == inTouchMode) {
        return FALSE;
    }

    mAttachInfo->mInTouchMode = inTouchMode;
    ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->DispatchOnTouchModeChanged(inTouchMode);

    return (inTouchMode) ? EnterTouchMode() : LeaveTouchMode();
}

Boolean ViewRootImpl::EnterTouchMode()
{
    if (mView != NULL) {
        Boolean hasFocus = FALSE;
        mView->HasFocus(&hasFocus);
        if (hasFocus) {
            // note: not relying on mFocusedView here because this could
            // be when the window is first being added, and mFocused isn't
            // set yet.
            //
            AutoPtr<IView> focused;
            mView->FindFocus((IView**)&focused);
            if (focused != NULL) {
                Boolean isFocusable;
                focused->IsFocusableInTouchMode(&isFocusable);
                if (!isFocusable) {
                    AutoPtr<IViewGroup> ancestorToTakeFocus =
                        FindAncestorToTakeFocusInTouchMode(focused.Get());

                    if (ancestorToTakeFocus != NULL) {
                        // there is an ancestor that wants focus after its descendants that
                        // is focusable in touch mode.. give it focus
                        //
                        Boolean result;
                        IVIEW_PROBE(ancestorToTakeFocus)->RequestFocus(&result);

                        return result;
                    }
                    else {
                        // nothing appropriate to have focus in touch mode, clear it out
                        //
                        VIEW_PROBE(focused)->ClearFocusInternal(NULL, TRUE, FALSE);

                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}

/**
* Find an ancestor of focused that wants focus after its descendants and is
* focusable in touch mode.
* @param focused The currently focused view.
* @return An appropriate view, or NULL if no such view exists.
*/
AutoPtr<IViewGroup> ViewRootImpl::FindAncestorToTakeFocusInTouchMode(
    /* [in] */ IView* focused)
{
    AutoPtr<IViewParent> parent;
    focused->GetParent((IViewParent**)&parent);

    AutoPtr<IViewGroup> vgParent = IViewGroup::Probe(parent);
    while (vgParent) {
        Int32 focusability;
        vgParent->GetDescendantFocusability(&focusability);
        Boolean isFocusable;
        IVIEW_PROBE(vgParent)->IsFocusableInTouchMode(&isFocusable);
        if (focusability == ViewGroup::FOCUS_AFTER_DESCENDANTS
            && isFocusable) {
            return vgParent;
        }

        Boolean isRootNamespace;
        IVIEW_PROBE(vgParent)->IsRootNamespace(&isRootNamespace);
        if (isRootNamespace) {
            return NULL;
        }
        else {
            parent = NULL;
            IVIEW_PROBE(vgParent)->GetParent((IViewParent**)&parent);
            vgParent = IViewGroup::Probe(parent);
        }
    }

    return NULL;
}

Boolean ViewRootImpl::LeaveTouchMode()
{
    if (mView != NULL) {
        Boolean hasFocus = FALSE;
        mView->HasFocus(&hasFocus);
        if (hasFocus) {
            // i learned the hard way to not trust mFocusedView :)
            //
            AutoPtr<IView> focusedView;
            mView->FindFocus((IView**)&focusedView);
            IViewGroup* vg = IViewGroup::Probe(focusedView);
            if (vg == NULL) {
                // some view has focus, let it keep it
                return FALSE;
            }
            else {
                Int32 focusability;
                vg->GetDescendantFocusability(&focusability);
                if (focusability != ViewGroup::FOCUS_AFTER_DESCENDANTS) {
                    // some view group has focus, and doesn't prefer its children
                    // over itself for focus, so let them keep it.
                    return FALSE;
                }
            }
        }

        // find the best view to give focus to in this brave new non-touch-mode
        // world
        //
        AutoPtr<IView> focused;
        FocusSearch(NULL, IView::FOCUS_DOWN, (IView**)&focused);
        if (focused != NULL) {
            Boolean result;
            focused->RequestFocus(IView::FOCUS_DOWN, &result);
            return result;
        }
    }

    return FALSE;
}

void ViewRootImpl::DeliverInputEvent(
    /* [in] */ QueuedInputEvent* q)
{
    /*Trace::AsyncTraceBegin(Trace::TRACE_TAG_VIEW, "deliverInputEvent",
            q->mEvent->GetSequenceNumber());*/
    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnInputEvent(q->mEvent, 0);
    }

    InputStage* stage;
    if (q->ShouldSendToSynthesizer()) {
        stage = mSyntheticInputStage;
    } else {
        stage = q->ShouldSkipIme() ? mFirstPostImeInputStage : mFirstInputStage;
    }

    if (stage != NULL) {
        stage->Deliver(q);
    } else {
        FinishInputEvent(q);
    }
}

Int32 ViewRootImpl::JoystickAxisValueToDirection(
    /* [in] */ Float value)
{
    if (value >= 0.5f) {
        return 1;
    }
    else if (value <= -0.5f) {
        return -1;
    }
    else {
        return 0;
    }
}

Boolean ViewRootImpl::IsNavigationKey(
    /* [in] */ IKeyEvent* keyEvent)
{
    Int32 keyCode;
    keyEvent->GetKeyCode(&keyCode);
    switch (keyCode) {
    case IKeyEvent::KEYCODE_DPAD_LEFT:
    case IKeyEvent::KEYCODE_DPAD_RIGHT:
    case IKeyEvent::KEYCODE_DPAD_UP:
    case IKeyEvent::KEYCODE_DPAD_DOWN:
    case IKeyEvent::KEYCODE_DPAD_CENTER:
    case IKeyEvent::KEYCODE_PAGE_UP:
    case IKeyEvent::KEYCODE_PAGE_DOWN:
    case IKeyEvent::KEYCODE_MOVE_HOME:
    case IKeyEvent::KEYCODE_MOVE_END:
    case IKeyEvent::KEYCODE_TAB:
    case IKeyEvent::KEYCODE_SPACE:
    case IKeyEvent::KEYCODE_ENTER:
        return TRUE;
    }
    return FALSE;
}

Boolean ViewRootImpl::IsTypingKey(
    /* [in] */ IKeyEvent* keyEvent)
{
    Int32 unicodeChar;
    keyEvent->GetUnicodeChar(&unicodeChar);
    return unicodeChar > 0;
}

Boolean ViewRootImpl::CheckForLeavingTouchModeAndConsume(
    /* [in] */ IKeyEvent* event)
{
    // Only relevant in touch mode.
    if (!mAttachInfo->mInTouchMode) {
        return FALSE;
    }

    // Only consider leaving touch mode on DOWN or MULTIPLE actions, never on UP.
    Int32 action;
    event->GetAction(&action);
    if (action != IKeyEvent::ACTION_DOWN
        && action != IKeyEvent::ACTION_MULTIPLE) {
        return FALSE;
    }

    Int32 flags;
    event->GetFlags(&flags);
    if ((flags & IKeyEvent::FLAG_KEEP_TOUCH_MODE) != 0) {
        return FALSE;
    }

    // If the key can be used for keyboard navigation then leave touch mode
    // and select a focused view if needed (in ensureTouchMode).
    // When a new focused view is selected, we consume the navigation key because
    // navigation doesn't make much sense unless a view already has focus so
    // the key's purpose is to set focus.
    if (IsNavigationKey(event)) {
        Boolean temp;
        EnsureTouchMode(FALSE, &temp);
        return temp;
    }

    // If the key can be used for typing then leave touch mode
    // and select a focused view if needed (in ensureTouchMode).
    // Always allow the view to process the typing key.
    if (IsTypingKey(event)) {
        Boolean temp;
        EnsureTouchMode(FALSE, &temp);
        return FALSE;
    }

    return FALSE;
}

/* drag/drop */
ECode ViewRootImpl::SetLocalDragState(
    /* [in] */ IInterface* obj)
{
    mLocalDragState = obj;
    return NOERROR;
}

void ViewRootImpl::HandleDragEvent(
    /* [in] */ IDragEvent* event)
{
    // From the root, only drag start/end/location are dispatched.  entered/exited
    // are determined and dispatched by the viewgroup hierarchy, who then report
    // that back here for ultimate reporting back to the framework.
    if (mView != NULL && mAdded) {
        Int32 what;
        event->GetAction(&what);

        if (what == IDragEvent::ACTION_DRAG_EXITED) {
            // A direct EXITED event means that the window manager knows we've just crossed
            // a window boundary, so the current drag target within this one must have
            // just been exited.  Send it the usual notifications and then we're done
            // for now.
            Boolean res;
            mView->DispatchDragEvent(event, &res);
        }
        else {
            // Cache the drag description when the operation starts, then fill it in
            // on subsequent calls as a convenience
            if (what == IDragEvent::ACTION_DRAG_STARTED) {
                mCurrentDragView = NULL;    // Start the current-recipient tracking
                 event->GetClipDescription((IClipDescription**)&mDragDescription);
            }
            else {

                 ((DragEvent*)event)->mClipDescription = mDragDescription;
            }

            // For events with a [screen] location, translate into window coordinates
            if ((what == IDragEvent::ACTION_DRAG_LOCATION) || (what == IDragEvent::ACTION_DROP)) {
                Float x, y;
                event->GetX(&x);
                event->GetY(&y);
                mDragPoint->Set(x, y);
                if (mTranslator != NULL) {
                    mTranslator->TranslatePointInScreenToAppWindow(mDragPoint);
                }

                if (mCurScrollY != 0) {
                    mDragPoint->Offset(0, mCurScrollY);
                }

                mDragPoint->GetX(&x);
                mDragPoint->GetY(&y);
                event->SetX(x);
                event->SetY(y);
            }

            // Remember who the current drag target is pre-dispatch
            IView* prevDragView = mCurrentDragView;

            // Now dispatch the drag/drop event
            Boolean result;
            mView->DispatchDragEvent(event, &result);

            // If we changed apparent drag target, tell the OS about it
            if (prevDragView != mCurrentDragView.Get()) {
                //try {
                    if (prevDragView != NULL) {
                        mWindowSession->DragRecipientExited(mWindow);
                    }
                    if (mCurrentDragView != NULL) {
                        mWindowSession->DragRecipientEntered(mWindow);
                    }
                //} catch (RemoteException e) {
                //    Slog.e(TAG, "Unable to note drag target change");
                //}
            }

            // Report the drop result when we're done
            if (what == IDragEvent::ACTION_DROP) {
                mDragDescription = NULL;
                Logger::I(TAG, "Reporting drop result: %d" , result);
                if (FAILED(mWindowSession->ReportDropResult(mWindow, result)))
                    Logger::E(TAG, "Unable to report drop result");
            }

            // When the drag operation ends, release any local state object
            // that may have been in use
            if (what == IDragEvent::ACTION_DRAG_ENDED) {
                SetLocalDragState(NULL);
            }
        }
    }
    event->Recycle();
}

ECode ViewRootImpl::HandleDispatchSystemUiVisibilityChanged(
    /* [in] */ SystemUiVisibilityInfo* args)
{
    if (mSeq != args->mSeq) {
        // The sequence has changed, so we need to update our value and make
        // sure to do a traversal afterward so the window manager is given our
        // most recent data.
        mSeq = args->mSeq;
        mAttachInfo->mForceReportNewAttributes = TRUE;
        ScheduleTraversals();
    }

    if (mView == NULL) {
        delete args;
        return NOERROR;
    }

    if (args->mLocalChanges != 0) {
        VIEW_PROBE(mView)->UpdateLocalSystemUiVisibility(args->mLocalValue, args->mLocalChanges);
    }

    Int32 visibility = args->mGlobalVisibility & IView::SYSTEM_UI_CLEARABLE_FLAGS;
    if (visibility != mAttachInfo->mGlobalSystemUiVisibility) {
        mAttachInfo->mGlobalSystemUiVisibility = visibility;
        mView->DispatchSystemUiVisibilityChanged(visibility);
    }

    delete args;
    return NOERROR;
}

ECode ViewRootImpl::HandleDispatchDoneAnimating()
{
    if (mWindowsAnimating) {
        mWindowsAnimating = FALSE;
        Boolean isEmpty;
        mDirty->IsEmpty(&isEmpty);
        if (!isEmpty || mIsAnimating || mFullRedrawNeeded)  {
            ScheduleTraversals();
        }
    }

    return NOERROR;
}

ECode ViewRootImpl::GetLastTouchPoint(
    /* [in] */ IPoint* outLocation)
{
    Float x, y;
    mLastTouchPoint->GetX(&x);
    mLastTouchPoint->GetY(&y);
    outLocation->Set((Int32)x, (Int32)y);

    return NOERROR;
}

ECode ViewRootImpl::SetDragFocus(
    /* [in] */ IView* newDragTarget)
{
    if (mCurrentDragView.Get() != newDragTarget) {
        mCurrentDragView = newDragTarget;
    }

    return NOERROR;
}

AutoPtr<IAudioManager> ViewRootImpl::GetAudioManager()
{
    if (mView == NULL) {
        Logger::E(TAG, "getAudioManager called when there is no mView");
        return NULL;
    }

    if (mAudioManager == NULL) {
        AutoPtr<IContext> context;
        mView->GetContext((IContext**)&context);

        AutoPtr<IInterface> audioService;
        context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
        mAudioManager = IAudioManager::Probe(audioService);
    }

    return mAudioManager;
}

ECode ViewRootImpl::GetAccessibilityInteractionController(
    /* [out] */ IAccessibilityInteractionController** res)
{
    VALIDATE_NOT_NULL(res)
    if (mView == NULL) {
        Logger::E(TAG, "getAccessibilityInteractionController"
            " called when there is no mView");
        return E_ILLEGAL_STATE_EXCEPTION;
/*        throw new IllegalStateException("getAccessibilityInteractionController"
                + " called when there is no mView");*/
    }
    if (mAccessibilityInteractionController == NULL) {
        CAccessibilityInteractionController::New(
            this, (IAccessibilityInteractionController**)&mAccessibilityInteractionController);
    }
    *res = mAccessibilityInteractionController;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

Int32 ViewRootImpl::RelayoutWindow(
    /* [in] */ IWindowManagerLayoutParams* params,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ Boolean insetsPending)
{
    Float appScale = mAttachInfo->mApplicationScale;
    Boolean restore = FALSE;

    if (params != NULL && mTranslator != NULL) {
        restore = TRUE;
        params->Backup();
        mTranslator->TranslateWindowLayout(params);
    }

    if (params != NULL) {
        if (DBG) {
            Logger::D(TAG, "WindowLayout in layoutWindow:" /*+ params*/);
        }
    }

    mPendingConfiguration->SetSeq(0);

    //ger::oG.d(TAG, ">>>>>> CALLING relayout");
    if (params != NULL && mOrigWindowType != WINLAY_PROBE(params)->mType) {
        // For compatibility with old apps, don't crash here.
        if (mTargetSdkVersion < Build::VERSION_CODES::ICE_CREAM_SANDWICH) {
           /* Slogger::W(TAG, "Window type can not be changed after "
                "the window is added; ignoring change of 0x%08x", mView.Get());*/
            WINLAY_PROBE(params)->mType = mOrigWindowType;
        }
    }

    Int32 measuredWidth, measuredHeight;
    AutoPtr<IRect> winFrame, pendingOverscanInsets, pendingContentInsets, pendingVisibleInsets, pendingStableInsets;
    AutoPtr<IConfiguration> pendingConfiguration;
    AutoPtr<ISurface> surface;
    Int32 relayoutResult = 0;

    mView->GetMeasuredWidth(&measuredWidth);
    mView->GetMeasuredHeight(&measuredHeight);

    mWindowSession->Relayout(
        (IIWindow*)mWindow, mSeq, params,
        (Int32)(measuredWidth * appScale + 0.5f),
        (Int32)(measuredHeight * appScale + 0.5f),
        viewVisibility, insetsPending ? IWindowManagerGlobal::RELAYOUT_INSETS_PENDING : 0,
        mWinFrame, mPendingOverscanInsets, mPendingContentInsets, mPendingVisibleInsets,
        mPendingStableInsets, mPendingConfiguration, mSurface,
        (IRect**)&winFrame, (IRect**)&pendingOverscanInsets, (IRect**)&pendingContentInsets,
        (IRect**)&pendingVisibleInsets, (IRect**)&pendingStableInsets,
        (IConfiguration**)&pendingConfiguration, (ISurface**)&surface, &relayoutResult);

    if (mWinFrame != NULL && winFrame != NULL)
        mWinFrame->Set(winFrame);
    if (mPendingOverscanInsets != NULL && pendingOverscanInsets != NULL)
        mPendingOverscanInsets->Set(pendingOverscanInsets);
    if (mPendingContentInsets != NULL && pendingContentInsets != NULL)
        mPendingContentInsets->Set(pendingContentInsets);
    if (mPendingVisibleInsets != NULL && pendingVisibleInsets != NULL)
        mPendingVisibleInsets->Set(pendingVisibleInsets);
    if (mPendingStableInsets != NULL && pendingStableInsets != NULL)
        mPendingStableInsets->Set(pendingStableInsets);
    if (mPendingConfiguration != NULL && pendingConfiguration != NULL)
        mPendingConfiguration->SetTo(pendingConfiguration);

    if (mSurface != NULL && surface != NULL) {
        mSurface->TransferFrom(surface);
    }

    if (restore) {
        params->Restore();
    }

    if (mTranslator != NULL) {
        mTranslator->TranslateRectInScreenToAppWinFrame(mWinFrame);
        mTranslator->TranslateRectInScreenToAppWindow(mPendingOverscanInsets);
        mTranslator->TranslateRectInScreenToAppWindow(mPendingContentInsets);
        mTranslator->TranslateRectInScreenToAppWindow(mPendingVisibleInsets);
        mTranslator->TranslateRectInScreenToAppWindow(mPendingStableInsets);
    }

    return relayoutResult;
}

ECode ViewRootImpl::PlaySoundEffect(
    /* [in] */ Int32 effectId)
{
    FAIL_RETURN(CheckThread());

    if (mMediaDisabled) {
        return NOERROR;
    }

    AutoPtr<IAudioManager> audioManager = GetAudioManager();
    if (audioManager == NULL) {
        Logger::E(TAG, "Audio service is not ready when play sound effect.");
        return E_RUNTIME_EXCEPTION;
    }

    ECode ec = E_ILLEGAL_ARGUMENT_EXCEPTION;

    switch (effectId) {
        case SoundEffectConstants::CLICK:
            ec = audioManager->PlaySoundEffect(IAudioManager::FX_KEY_CLICK);
            if (FAILED(ec))
                goto _Exit_;
            return NOERROR;
        case SoundEffectConstants::NAVIGATION_DOWN:
            ec = audioManager->PlaySoundEffect(IAudioManager::FX_FOCUS_NAVIGATION_DOWN);
            if (FAILED(ec))
                goto _Exit_;
            return NOERROR;
        case SoundEffectConstants::NAVIGATION_LEFT:
            ec = audioManager->PlaySoundEffect(IAudioManager::FX_FOCUS_NAVIGATION_LEFT);
            if (FAILED(ec))
                goto _Exit_;
            return NOERROR;
        case SoundEffectConstants::NAVIGATION_RIGHT:
            ec = audioManager->PlaySoundEffect(IAudioManager::FX_FOCUS_NAVIGATION_RIGHT);
            if (FAILED(ec))
                goto _Exit_;
            return NOERROR;
        case SoundEffectConstants::NAVIGATION_UP:
            ec = audioManager->PlaySoundEffect(IAudioManager::FX_FOCUS_NAVIGATION_UP);
            if (FAILED(ec))
                goto _Exit_;
            return NOERROR;
        default:
            Logger::E(TAG, "unknown effect id %d  not defined in SoundEffectConstants.class.getCanonicalName().", effectId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

_Exit_:
    // Exception thrown by getAudioManager() when mView is NULL
    Logger::E(TAG, "FATAL EXCEPTION when attempting to play sound effect: ");
    return ec;
}

ECode ViewRootImpl::PerformHapticFeedback(
    /* [in] */ Int32 effectId,
    /* [in] */ Boolean always,
    /* [out] */ Boolean* result)
{
    return mWindowSession->PerformHapticFeedback(mWindow, effectId, always, result);
}

ECode ViewRootImpl::FocusSearch(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out */ IView** result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckThread());

    IViewGroup* viewGroup = (IViewGroup*)mView->Probe(EIID_IViewGroup);
    if (viewGroup == NULL) {
        *result  = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FocusFinder::GetInstance()->FindNextFocus(
        IViewGroup::Probe(mView), focused, direction, result);

    return NOERROR;
}

ECode ViewRootImpl::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String> * args)
{
    String innerPrefix = prefix + String("  ");
    writer->Print(prefix);
    writer->Println(String("ViewRoot:"));
    writer->Print(innerPrefix);
    writer->Print(String("mAdded="));
    writer->Print(mAdded);
    writer->Print(String(" mRemoved="));
    writer->Println(mRemoved);
    writer->Print(innerPrefix);
    writer->Print(String("mConsumeBatchedInputScheduled="));
    writer->Println(mConsumeBatchedInputScheduled);
    writer->Print(innerPrefix);
    writer->Print(String("mConsumeBatchedInputImmediatelyScheduled="));
    writer->Println(mConsumeBatchedInputImmediatelyScheduled);
    writer->Print(innerPrefix);
    writer->Print(String("mPendingInputEventCount="));
    writer->Println(mPendingInputEventCount);
    writer->Print(innerPrefix);
    writer->Print(String("mProcessInputEventsScheduled="));
    writer->Println(mProcessInputEventsScheduled);
    writer->Print(innerPrefix);
    writer->Print(String("mTraversalScheduled="));
    writer->Print(mTraversalScheduled);
    if (mTraversalScheduled) {
        writer->Print(String(" (barrier="));
        writer->Print(mTraversalBarrier);
        writer->Println(String(")"));
    } else {
        writer->Println();
    }
    mFirstInputStage->Dump(innerPrefix, writer);
    assert(0 && "TODO");
    //mChoreographer->Dump(prefix, writer);

    writer->Print(prefix);
    writer->Println(String("View Hierarchy:"));
    DumpViewHierarchy(innerPrefix, writer, mView);
    return NOERROR;
}

void ViewRootImpl::DumpViewHierarchy(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ IView* view)
{
    writer->Print(prefix);
    if (view == NULL) {
        writer->Println(String("NULL"));
        return;
    }
    String viewStr;
    assert(0 && "TODO");
    //view->ToString(&viewStr);
    writer->Println(viewStr);
    if (!IViewGroup::Probe(view)) {
        return;
    }
    IViewGroup* grp = IViewGroup::Probe(view);
    Int32 N = 0;
    grp->GetChildCount(&N);
    if (N <= 0) {
        return;
    }
    String newPrefix(prefix);
    newPrefix += "  ";
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> temp;
        grp->GetChildAt(i, (IView**)&temp);
        DumpViewHierarchy(newPrefix, writer, temp);
    }
}

ECode ViewRootImpl::Debug()
{
    //mView->Debug();
    return NOERROR;
}

ECode ViewRootImpl::DumpGfxInfo(
    /* [in] */ ArrayOf<Int32>* info)
{
    (*info)[0] = (*info)[1] = 0;
    if (mView != NULL) {
        GetGfxInfo(mView, info);
    }
    return NOERROR;
}

void ViewRootImpl::GetGfxInfo(
    /* [in] */ IView* view,
    /* [in] */ ArrayOf<Int32>* info)
{
    AutoPtr<IRenderNode> renderNode = VIEW_PROBE(view)->mRenderNode;
    (*info)[0]++;
    if (renderNode != NULL) {
        Int32 size;
        renderNode->GetDebugSize(&size);
        (*info)[1] += size;
    }

    IViewGroup* group = IViewGroup::Probe(view);
    if (group) {
        Int32 count;
        group->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            group->GetChildAt(i, (IView**)&child);
            GetGfxInfo(child, info);
        }
    }
}

ECode ViewRootImpl::Die(
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    // Make sure we do execute immediately if we are in the middle of a traversal or the damage
    // done by dispatchDetachedFromWindow will cause havoc on return.

    // For DoDie to release
    AddRef();

    if (immediate && !mIsInTraversal) {
        DoDie();
        *res = FALSE;
        return NOERROR;
    }
    else {
        if (!mIsDrawing) {
            DestroyHardwareRenderer();
        }
        else {
            Logger::E(TAG, "Attempting to destroy the window while drawing!\n" /*+
                    "  window=" + this + ", title=" + mWindowAttributes.getTitle()*/);
        }

        Boolean result;
        mHandler->SendEmptyMessage(MSG_DIE, &result);
        *res = TRUE;
        return NOERROR;
    }

    return NOERROR;
}

ECode ViewRootImpl::DoDie()
{
    FAIL_RETURN(CheckThread());

    if (LOCAL_LOGV) {
        Logger::D(TAG, "DIE in 0x%08x of 0x%08x", this, mSurface.Get());
    }

    AutoLock lock(mSyncLock);

    if (mRemoved) {
        return NOERROR;
    }
    mRemoved = TRUE;

    if (mAdded) {
        DispatchDetachedFromWindow();
    }

    if (mAdded && !mFirst) {
        DestroyHardwareRenderer();

        if (mView != NULL) {
            Int32 viewVisibility;
            mView->GetVisibility(&viewVisibility);

            Boolean viewVisibilityChanged = mViewVisibility != viewVisibility;
            if (mWindowAttributesChanged || viewVisibilityChanged) {
                // If layout params have been changed, first give them
                // to the window manager to make sure it has the correct
                // animation info.
                //
                if ((RelayoutWindow(mWindowAttributes, viewVisibility, FALSE)
                    & IWindowManagerGlobal::RELAYOUT_RES_FIRST_TIME) != 0) {
                    mWindowSession->FinishDrawing(mWindow);
                }
            }
        }

        mSurface->ReleaseSurface();
        mAdded = FALSE;
    }
    AutoPtr<IWindowManagerGlobal> instance = CWindowManagerGlobal::GetInstance();
    CWindowManagerGlobal* obj = (CWindowManagerGlobal*)instance.Get();
    obj->DoRemoveView(this);

    Release();
    // TODO: stalling tactics for wrong reference count

    return NOERROR;
}

ECode ViewRootImpl::RequestUpdateConfiguration(
    /* [in] */ IConfiguration* config)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_UPDATE_CONFIGURATION, config, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode ViewRootImpl::LoadSystemProperties()
{
    AutoPtr<IRunnable> run = new PropertiesRunnable(this);
    Boolean temp;
    mHandler->Post(run, &temp);
    return NOERROR;
}

void ViewRootImpl::DestroyHardwareRenderer()
{
    assert(0 && "TODO");
    // HardwareRenderer* hardwareRenderer = mAttachInfo->mHardwareRenderer;

    // if (hardwareRenderer != NULL) {
    //     if (mView != NULL) {
    //         hardwareRenderer->DestroyHardwareResources(mView);
    //     }
    //     hardwareRenderer->Destroy(TRUE);
    //     hardwareRenderer->SetRequested(FALSE);

    //     mAttachInfo->mHardwareRenderer = NULL;
    //     mAttachInfo->mHardwareAccelerated = FALSE;
    // }
}

ECode ViewRootImpl::DispatchFinishInputConnection(
    /* [in] */ IInputConnection* connection)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_FINISH_INPUT_CONNECTION, connection, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode ViewRootImpl::DispatchResized(
    /* [in] */ IRect* frame,
    /* [in] */ IRect* overscanInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRect* stableInsets,
    /* [in] */ Boolean reportDraw,
    /* [in] */ IConfiguration* newConfig)
{
    // if (DEBUG_LAYOUT) Logger::V(TAG, "Resizing " + this + ": frame=" + frame.toShortString()
    //         + " contentInsets=" + contentInsets.toShortString()
    //         + " visibleInsets=" + visibleInsets.toShortString()
    //         + " reportDraw=" + reportDraw);

    if (mTranslator != NULL) {
        mTranslator->TranslateRectInScreenToAppWindow(frame);
        mTranslator->TranslateRectInScreenToAppWindow(overscanInsets);
        mTranslator->TranslateRectInScreenToAppWindow(contentInsets);
        mTranslator->TranslateRectInScreenToAppWindow(visibleInsets);
    }

    Boolean sameProcessCall = (Binder::GetCallingPid() == Process::MyPid());
    AutoPtr<IRect> argFrame, argoverscanInsets, argContentInsets, argVisibleInsets, argstableInsets;
    AutoPtr<IConfiguration> argConfig;
    if (sameProcessCall) {
        CRect::New(frame, (IRect**)&argFrame);
        CRect::New(overscanInsets, (IRect**)&argoverscanInsets);
        CRect::New(contentInsets, (IRect**)&argContentInsets);
        CRect::New(visibleInsets, (IRect**)&argVisibleInsets);
        CRect::New(stableInsets, (IRect**)&argstableInsets);
        if (newConfig != NULL) {
            CConfiguration::New(newConfig, (IConfiguration**)&argConfig);
        }
    }
    else {
        argFrame = frame;
        argoverscanInsets = overscanInsets;
        argContentInsets = contentInsets;
        argVisibleInsets = visibleInsets;
        argstableInsets = stableInsets;
        argConfig = newConfig;
    }

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = argFrame;
    args->mArg2 = argContentInsets;
    args->mArg3 = argVisibleInsets;
    args->mArg4 = argConfig;
    args->mArg5 = argoverscanInsets;
    args->mArg6 = argstableInsets;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(reportDraw ? MSG_RESIZED_REPORT : MSG_RESIZED,
        (IInterface*)(IObject*)args, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    return NOERROR;
}

ECode ViewRootImpl::DispatchMoved(
    /* [in] */ Int32 newX,
    /* [in] */ Int32 newY)
{
    if (DEBUG_LAYOUT)
        Logger::V(TAG, "Window moved 0x%08x: newX=%d, newY=%d", this, newX, newY);

    if (mTranslator != NULL) {
        AutoPtr<IPointF> point;
        CPointF::New(newX, newY, (IPointF**)&point);
        mTranslator->TranslatePointInScreenToAppWindow(point);
        Float x, y;
        point->GetX(&x);
        point->GetY(&y);
        newX = (Int32)(x + 0.5);
        newY = (Int32)(y + 0.5);
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_WINDOW_MOVED, newX, newY, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    return NOERROR;
}

AutoPtr<ViewRootImpl::QueuedInputEvent> ViewRootImpl::ObtainQueuedInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInputEventReceiver* receiver,
    /* [in] */ Int32 flags)
{
    AutoPtr<QueuedInputEvent> q = mQueuedInputEventPool;
    if (q != NULL) {
        mQueuedInputEventPoolSize -= 1;
        mQueuedInputEventPool = q->mNext;
        q->mNext = NULL;
    }
    else {
        q = new QueuedInputEvent();
    }

    q->mEvent = event;
    q->mReceiver = receiver;
    q->mFlags = flags;
    return q;
}

void ViewRootImpl::RecycleQueuedInputEvent(
    /* [in] */ QueuedInputEvent* q)
{
    q->mEvent = NULL;
    q->mReceiver = NULL;

    if (mQueuedInputEventPoolSize < MAX_QUEUED_INPUT_EVENT_POOL_SIZE) {
        mQueuedInputEventPoolSize += 1;
        q->mNext = mQueuedInputEventPool;
        mQueuedInputEventPool = q;
    }
}

ECode ViewRootImpl::EnqueueInputEvent(
    /* [in] */ IInputEvent* event)
{
    return EnqueueInputEvent(event, NULL, 0, FALSE);
}

ECode ViewRootImpl::EnqueueInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInputEventReceiver* receiver,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean processImmediately)
{
    AutoPtr<QueuedInputEvent> q = ObtainQueuedInputEvent(event, receiver, flags);

    // Always enqueue the input event in order, regardless of its time stamp.
    // We do this because the application or the IME may inject key events
    // in response to touch events and we want to ensure that the injected keys
    // are processed in the order they were received and we cannot trust that
    // the time stamp of injected events are monotonic.
    QueuedInputEvent* last = mPendingInputEventTail;
    if (last == NULL) {
        mPendingInputEventHead = q;
        mPendingInputEventTail = q;
    }
    else {
        last->mNext = q;
        mPendingInputEventTail = q;
    }

    mPendingInputEventCount += 1;
    /*Trace::TraceCounter(Trace::TRACE_TAG_INPUT, mPendingInputEventQueueLengthCounterName,
            mPendingInputEventCount);*/

    if (processImmediately) {
        DoProcessInputEvents();
    }
    else {
        ScheduleProcessInputEvents();
    }

    return NOERROR;
}

void ViewRootImpl::ScheduleProcessInputEvents()
{
    if (!mProcessInputEventsScheduled) {
        mProcessInputEventsScheduled = TRUE;

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_PROCESS_INPUT_EVENTS, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }
}

ECode ViewRootImpl::DoProcessInputEvents()
{
    while (mPendingInputEventHead == NULL) {
        AutoPtr<QueuedInputEvent> q = mPendingInputEventHead;
        mPendingInputEventHead = q->mNext;
        if (mPendingInputEventHead == NULL) {
            mPendingInputEventTail = NULL;
        }
        q->mNext = NULL;
        mPendingInputEventCount -= 1;
        /*Trace::TraceCounter(Trace::TRACE_TAG_INPUT, mPendingInputEventQueueLengthCounterName,
                mPendingInputEventCount);*/
        DeliverInputEvent(q);
    }

    // We are done processing all input events that we can process right now
    // so we can clear the pending flag immediately.
    if (mProcessInputEventsScheduled) {
        mProcessInputEventsScheduled = FALSE;
        mHandler->RemoveMessages(MSG_PROCESS_INPUT_EVENTS);
    }
    return NOERROR;
}

ECode ViewRootImpl::FinishInputEvent(
    /* [in] */ QueuedInputEvent* q)
{
    /*Trace::AsyncTraceEnd(Trace.TRACE_TAG_VIEW, "deliverInputEvent",
            q->mEvent->GetSequenceNumber());*/

    if (q->mReceiver != NULL) {
        Boolean handled = (q->mFlags & QueuedInputEvent::FLAG_FINISHED_HANDLED) != 0;
        q->mReceiver->FinishInputEvent(q->mEvent, handled);
    } else {
        q->mEvent->RecycleIfNeededAfterDispatch();
    }

    RecycleQueuedInputEvent(q);

    return NOERROR;
}

Boolean ViewRootImpl::IsTerminalInputEvent(
    /* [in] */ IInputEvent* event)
{
    if (IKeyEvent::Probe(event)) {
        AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(event);
        Int32 action = 0;
        keyEvent->GetAction(&action);
        return action == IKeyEvent::ACTION_UP;
    } else {
        AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        Int32 action = 0;
        motionEvent->GetAction(&action);
        return action == IMotionEvent::ACTION_UP
                || action == IMotionEvent::ACTION_CANCEL
                || action == IMotionEvent::ACTION_HOVER_EXIT;
    }
}

ECode ViewRootImpl::ScheduleConsumeBatchedInput()
{
    if (!mConsumeBatchedInputScheduled) {
        mConsumeBatchedInputScheduled = TRUE;
        mChoreographer->PostCallback(IChoreographer::CALLBACK_INPUT,
                mConsumedBatchedInputRunnable, NULL);
    }
    return NOERROR;
}

ECode ViewRootImpl::UnscheduleConsumeBatchedInput()
{
    if (mConsumeBatchedInputScheduled) {
        mConsumeBatchedInputScheduled = FALSE;
        mChoreographer->RemoveCallbacks(IChoreographer::CALLBACK_INPUT,
                mConsumedBatchedInputRunnable, NULL);
    }

    return NOERROR;
}

ECode ViewRootImpl::ScheduleConsumeBatchedInputImmediately()
{
    if (!mConsumeBatchedInputImmediatelyScheduled) {
        UnscheduleConsumeBatchedInput();
        mConsumeBatchedInputImmediatelyScheduled = TRUE;
        Boolean temp;
        mHandler->Post(mConsumeBatchedInputImmediatelyRunnable, &temp);
    }
    return NOERROR;
}

ECode ViewRootImpl::DoConsumeBatchedInput(
    /* [in] */ Int64 frameTimeNanos)
{
    if (mConsumeBatchedInputScheduled) {
        mConsumeBatchedInputScheduled = FALSE;
        if (mInputEventReceiver != NULL) {
            Boolean res;
            mInputEventReceiver->ConsumeBatchedInputEvents(frameTimeNanos, &res);
            if (res && frameTimeNanos != -1) {
                // If we consumed a batch here, we want to go ahead and schedule the
                // consumption of batched input events on the next frame. Otherwise, we would
                // wait until we have more input events pending and might get starved by other
                // things occurring in the process. If the frame time is -1, however, then
                // we're in a non-batching mode, so there's no need to schedule this.
                ScheduleConsumeBatchedInput();
            }

        }
        DoProcessInputEvents();
    }

    return NOERROR;
}

ECode ViewRootImpl::DispatchInvalidateDelayed(
    /* [in] */ IView* view,
    /* [in] */ Int64 delayMilliseconds)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_INVALIDATE, view, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessageDelayed(msg, delayMilliseconds, &result);

    return NOERROR;
}

void ViewRootImpl::DispatchInvalidateRectDelayed(
    /* [in] */ View::AttachInfo::InvalidateInfo* info,
    /* [in] */ Int64 delayMilliseconds)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_INVALIDATE_RECT, (IInterface*)(IObject*)info, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessageDelayed(msg, delayMilliseconds, &result);
}

ECode ViewRootImpl::DispatchInvalidateOnAnimation(
    /* [in] */ IView* view)
{
    mInvalidateOnAnimationRunnable->AddView(view);
    return NOERROR;
}

void ViewRootImpl::DispatchInvalidateRectOnAnimation(
    /* [in] */ View::AttachInfo::InvalidateInfo* info)
{
    mInvalidateOnAnimationRunnable->AddViewRect(info);
}

ECode ViewRootImpl::CancelInvalidate(
    /* [in] */ IView* view)
{
    mHandler->RemoveMessages(MSG_INVALIDATE, view);
    // fixme: might leak the AttachInfo.InvalidateInfo objects instead of returning
    // them to the pool
    mHandler->RemoveMessages(MSG_INVALIDATE_RECT, view);
    mInvalidateOnAnimationRunnable->RemoveView(view);

    return NOERROR;
}

ECode ViewRootImpl::DispatchInputEvent(
    /* [in] */ IInputEvent* event)
{
    return DispatchInputEvent(event, NULL);
}

ECode ViewRootImpl::DispatchInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInputEventReceiver* receiver)
{
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = event;
    args->mArg2 = receiver;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_DISPATCH_INPUT_EVENT, (IInterface*)(IObject*)args, (IMessage**)&msg);
    msg->SetAsynchronous(TRUE);
    Boolean temp;
    mHandler->SendMessage(msg, &temp);
    return NOERROR;
}

ECode ViewRootImpl::SynthesizeInputEvent(
    /* [in] */ IInputEvent* event)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SYNTHESIZE_INPUT_EVENT, event, (IMessage**)&msg);
    msg->SetAsynchronous(TRUE);
    Boolean temp;
    mHandler->SendMessage(msg, &temp);
    return NOERROR;
}

ECode ViewRootImpl::DispatchKeyFromIme(
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_DISPATCH_KEY_FROM_IME, event, (IMessage**)&msg);
    msg->SetAsynchronous(TRUE);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

/**
 * Reinject unhandled {@link InputEvent}s in order to synthesize fallbacks events.
 *
 * Note that it is the responsibility of the caller of this API to recycle the InputEvent it
 * passes in.
 */
ECode ViewRootImpl::DispatchUnhandledInputEvent(
    /* [in] */ IInputEvent* event)
{
    if (IMotionEvent::Probe(event)) {
        AutoPtr<IMotionEvent> temp;
        CMotionEvent::Obtain(IMotionEvent::Probe(event), (IMotionEvent**)&temp);
        event = IInputEvent::Probe(temp);
    }
    SynthesizeInputEvent(event);
    return NOERROR;
}

ECode ViewRootImpl::DispatchAppVisibility(
    /* [in] */ Boolean visible)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_DISPATCH_APP_VISIBILITY,
        visible ? 1 : 0, 0, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode ViewRootImpl::DispatchGetNewSurface()
{
    Boolean result;
    mHandler->SendEmptyMessage(MSG_DISPATCH_GET_NEW_SURFACE, &result);
    return NOERROR;
}

ECode ViewRootImpl::WindowFocusChanged(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean inTouchMode)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_WINDOW_FOCUS_CHANGED,
        hasFocus ? 1 : 0, inTouchMode ? 1 : 0, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode ViewRootImpl::DispatchCloseSystemDialogs(
    /* [in] */ const String& reason)
{
    AutoPtr<ICharSequence> seq;
    CString::New(reason, (ICharSequence**)&seq);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_CLOSE_SYSTEM_DIALOGS,
        seq, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    return NOERROR;
}

ECode ViewRootImpl::DispatchDragEvent(
    /* [in] */ IDragEvent* event)
{
    Int32 what = MSG_DISPATCH_DRAG_EVENT;
    Int32 action;
    event->GetAction(&action);
    if (action == IDragEvent::ACTION_DRAG_LOCATION) {
        what = MSG_DISPATCH_DRAG_LOCATION_EVENT;
        mHandler->RemoveMessages(what);
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(what, event, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    return NOERROR;
}

ECode ViewRootImpl::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 seq,
    /* [in] */ Int32 globalVisibility,
    /* [in] */ Int32 localValue,
    /* [in] */ Int32 localChanges)
{
    AutoPtr<SystemUiVisibilityInfo> args = new SystemUiVisibilityInfo();
    args->mSeq = seq;
    args->mGlobalVisibility = globalVisibility;
    args->mLocalValue = localValue;
    args->mLocalChanges = localChanges;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_DISPATCH_SYSTEM_UI_VISIBILITY, (IInterface*)(IObject*)args, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    return NOERROR;
}

ECode ViewRootImpl::DispatchDoneAnimating()
{
    Boolean result;
    mHandler->SendEmptyMessage(MSG_DISPATCH_DONE_ANIMATING, &result);

    return NOERROR;
}

ECode ViewRootImpl::DispatchCheckFocus()
{
    Boolean result;
    if (mHandler->HasMessages(MSG_CHECK_FOCUS, &result), !result) {
        // This will result in a call to checkFocus() below.
        mHandler->SendEmptyMessage(MSG_CHECK_FOCUS, &result);
    }

    return NOERROR;
}

/**
 * Post a callback to send a
 * {@link AccessibilityEvent#TYPE_WINDOW_CONTENT_CHANGED} event->
 * This event is send at most once every
 * {@link ViewConfiguration#getSendRecurringAccessibilityEventsInterval()}.
 */
void ViewRootImpl::PostSendWindowContentChangedCallback(
    /* [in] */ IView* source,
    /* [in] */ Int32 changeType)
{
    if (mSendWindowContentChangedAccessibilityEvent == NULL) {
        mSendWindowContentChangedAccessibilityEvent =
            new SendWindowContentChangedAccessibilityEvent(this);
    }
    mSendWindowContentChangedAccessibilityEvent->RunOrPost(source, changeType);
}

/**
 * Remove a posted callback to send a
 * {@link AccessibilityEvent#TYPE_WINDOW_CONTENT_CHANGED} event->
 */
void ViewRootImpl::RemoveSendWindowContentChangedCallback()
{
    if (mSendWindowContentChangedAccessibilityEvent != NULL) {
        mHandler->RemoveCallbacks(mSendWindowContentChangedAccessibilityEvent);
    }
}

ECode ViewRootImpl::ShowContextMenuForChild(
    /* [in] */ IView* originalView,
    /* [out] */ Boolean* result)
{
    assert(result);
    *result = FALSE;

    return NOERROR;
}

ECode ViewRootImpl::StartActionModeForChild(
    /* [in] */ IView* originalView,
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** actionMode)
{
    VALIDATE_NOT_NULL(actionMode);
    *actionMode = NULL;
    return NOERROR;
}

ECode ViewRootImpl::CreateContextMenu(
    /* [in] */ IContextMenu* menu)
{
    return NOERROR;
}

ECode ViewRootImpl::ChildDrawableStateChanged(
    /* [in] */ IView* child)
{
    return NOERROR;
}

ECode ViewRootImpl::RequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (mView == NULL) {
        return NOERROR;
    }

    // Intercept accessibility focus events fired by virtual nodes to keep
    // track of accessibility focus position in such nodes.
    Int32 eventType;
    event->GetEventType(&eventType);
    switch (eventType) {
        case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED: {
            Int64 sourceNodeId;
            IAccessibilityRecord::Probe(event)->GetSourceNodeId(&sourceNodeId);
            Int32 accessibilityViewId = 0;
            CAccessibilityNodeInfo::GetAccessibilityViewId(sourceNodeId);
            AutoPtr<IView> source = VIEW_PROBE(mView)->FindViewByAccessibilityId(accessibilityViewId);
            if (source != NULL) {
                AutoPtr<IAccessibilityNodeProvider> provider;
                source->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                if (provider != NULL) {
                    assert(0);
                    Int32 virtualNodeId = CAccessibilityNodeInfo::GetVirtualDescendantId(sourceNodeId);
                    AutoPtr<IAccessibilityNodeInfo> node;
                    if (virtualNodeId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                        provider->CreateAccessibilityNodeInfo(
                                IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&node);
                    } else {
                        provider->CreateAccessibilityNodeInfo(virtualNodeId, (IAccessibilityNodeInfo**)&node);
                    }
                    SetAccessibilityFocus(source, node);
                }
            }
            break;
        }
        case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED: {
            Int64 sourceNodeId;
            IAccessibilityRecord::Probe(event)->GetSourceNodeId(&sourceNodeId);
            Int32 accessibilityViewId = 0;
            CAccessibilityNodeInfo::GetAccessibilityViewId(sourceNodeId);
            AutoPtr<IView> source = VIEW_PROBE(mView.Get())->FindViewByAccessibilityId(accessibilityViewId);
            if (source != NULL) {
                AutoPtr<IAccessibilityNodeProvider> provider;
                source->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                if (provider != NULL) {
                    SetAccessibilityFocus(NULL, NULL);
                }
            }
            break;
        }
        case IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED: {
            if (mAccessibilityFocusedHost != NULL && mAccessibilityFocusedVirtualView != NULL) {
                // We care only for changes rooted in the focused host.
                Int64 eventSourceId;
                IAccessibilityRecord::Probe(event)->GetSourceNodeId(&eventSourceId);
                Int32 hostViewId = CAccessibilityNodeInfo::GetAccessibilityViewId(eventSourceId);
                Int32 id;
                mAccessibilityFocusedHost->GetAccessibilityViewId(&id);
                if (hostViewId != id) {
                    break;
                }

                // We only care about changes that may change the virtual focused view bounds.
                Int32 changes;
                event->GetContentChangeTypes(&changes);
                if ((changes & IAccessibilityEvent::CONTENT_CHANGE_TYPE_SUBTREE) != 0
                        || changes == IAccessibilityEvent::CONTENT_CHANGE_TYPE_UNDEFINED) {
                    AutoPtr<IAccessibilityNodeProvider> provider;
                    mAccessibilityFocusedHost->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                    if (provider != NULL) {
                        Int64 tempId;
                        mAccessibilityFocusedVirtualView->GetSourceNodeId(&tempId);
                        assert(0);
                        Int32 virtualChildId; //= CAccessibilityNodeInfo::GetVirtualDescendantId(tempId);
                        if (virtualChildId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                            provider->CreateAccessibilityNodeInfo(
                                IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&mAccessibilityFocusedVirtualView);
                        } else {
                            provider->CreateAccessibilityNodeInfo(
                                virtualChildId, (IAccessibilityNodeInfo**)&mAccessibilityFocusedVirtualView);
                        }
                    }
                }
            }
            break;
        }
    }
    mAccessibilityManager->SendAccessibilityEvent(event);
    *res = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::NotifySubtreeAccessibilityStateChanged(
    /* [in] */ IView* child,
    /* [in] */ IView* source,
    /* [in] */ Int32 changeType)
{
    PostSendWindowContentChangedCallback(source, changeType);
    return NOERROR;
}

//@Override
ECode ViewRootImpl::CanResolveLayoutDirection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::IsLayoutDirectionResolved(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::GetLayoutDirection(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = View::LAYOUT_DIRECTION_RESOLVED_DEFAULT;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::CanResolveTextDirection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::IsTextDirectionResolved(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::GetTextDirection(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = View::TEXT_DIRECTION_RESOLVED_DEFAULT;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::CanResolveTextAlignment(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::IsTextAlignmentResolved(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::GetTextAlignment(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = View::TEXT_ALIGNMENT_RESOLVED_DEFAULT;
    return NOERROR;
}

AutoPtr<IView> ViewRootImpl::GetCommonPredecessor(
    /* [in] */ IView* first,
    /* [in] */ IView* second)
{
    mTempHashSet->Clear();
    AutoPtr<IView>firstCurrent = first;
    while (firstCurrent != NULL) {
        mTempHashSet->Add(firstCurrent);
        AutoPtr<IViewParent> firstCurrentParent;
        firstCurrent->GetParent((IViewParent**)&firstCurrentParent);
        if (IView::Probe(firstCurrentParent)) {
            firstCurrent = IView::Probe(firstCurrentParent);
        } else {
            firstCurrent = NULL;
        }
    }

    AutoPtr<IView> secondCurrent = second;
    while (secondCurrent != NULL) {
        Boolean isContains;
        if (mTempHashSet->Contains(secondCurrent, &isContains), isContains) {
            mTempHashSet->Clear();
            return secondCurrent;
        }
        AutoPtr<IViewParent> secondCurrentParent;
        secondCurrent->GetParent((IViewParent**)&secondCurrentParent);
        if (IView::Probe(secondCurrentParent)) {
            secondCurrent = IView::Probe(secondCurrentParent);
        } else {
            secondCurrent = NULL;
        }
    }
    mTempHashSet->Clear();
    return NULL;
}

ECode ViewRootImpl::CheckThread()
{
    if (mThread != Thread::GetCurrentThread()) {
//         throw new CalledFromWrongThreadException(
//                    "Only the original thread that created a view hierarchy can touch its views.");
        /*Logger::E(TAG,
            "Only the original thread that created a view hierarchy can touch its views.");*/

        return E_CALLED_FROM_WRONG_THREAD_EXCEPTION;
    }

    return NOERROR;
}

ECode ViewRootImpl::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    // ViewRootImpl never intercepts touch event, so this can be a no-op

    return NOERROR;
}

ECode ViewRootImpl::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* result)
{
    assert(result);
    ScrollToRectOrFocus(rectangle, immediate, result);
    if (rectangle != NULL) {
        mTempRect->Set(rectangle);
        mTempRect->Offset(0, -mCurScrollY);
        mTempRect->Offset(mAttachInfo->mWindowLeft, mAttachInfo->mWindowTop);
        assert(0 && "TODO");
        /*mWindowSession->OnRectangleOnScreenRequested(
            IBinder::Probe(mWindow.Get()), mTempRect);*/
    }

    return NOERROR;
}

ECode ViewRootImpl::ChildHasTransientStateChanged(
    /* [in] */ IView* child,
    /* [in] */ Boolean hasTransientState)
{
    // Do nothing.
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    result = FALSE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnStopNestedScroll(
    /* [in] */ IView* target)
{
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnNestedScrollAccepted(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes)
{
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnNestedPreScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ ArrayOf<Int32> * consumed)
{
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [in] */ Boolean consumed,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode ViewRootImpl::OnNestedPreFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ViewRootImpl::ChangeCanvasOpacity(
    /* [in] */ Boolean opaque)
{
    //Log.d(TAG, "changeCanvasOpacity: opaque=" + opaque);
    if (mAttachInfo->mHardwareRenderer != NULL) {
        mAttachInfo->mHardwareRenderer->SetOpaque(opaque);
    }
    return NOERROR;
}

// void ViewRootImpl::HandleInvalidateRect(
//     /* [in] */ View::AttachInfo::InvalidateInfo* info)
// {
//     info->mTarget->Invalidate(info->mLeft, info->mTop, info->mRight, info->mBottom);
//     info->ReleaseInfo();
// }

// void ViewRootImpl::HandleProcessInputEvents()
// {
//     mProcessInputEventsScheduled = FALSE;
//     DoProcessInputEvents();
// }

// void ViewRootImpl::HandleResized(
//     /* [in] */ IRect* frame,
//     /* [in] */ IRect* contentInsets,
//     /* [in] */ IRect* visibleInsets,
//     /* [in] */ Boolean reportDraw,
//     /* [in] */ IConfiguration* newConfig)
// {
//     if (!reportDraw) {
//         Boolean isEqual1, isEqual2, isEqual3;
//         mWinFrame->Equals(frame, &isEqual1);
//         if (isEqual1
//             && (mPendingContentInsets->Equals(contentInsets, &isEqual2), isEqual2)
//             && (mPendingVisibleInsets->Equals(visibleInsets, &isEqual3), isEqual3)
//             && newConfig == NULL) {
//             return;
//         }
//     }

//     // MSG: MSG_RESIZED_REPORT
//     if (mAdded) {
//         if (newConfig != NULL) {
//             UpdateConfiguration(newConfig, FALSE);
//         }

//         mWinFrame->Set(frame);
//         mPendingContentInsets->Set(contentInsets);
//         mPendingVisibleInsets->Set(visibleInsets);

//         if (reportDraw) {
//             mReportNextDraw = TRUE;
//         }

//         if (mView != NULL) {
//             ForceLayout(mView);
//         }

//         RequestLayout();
//     }
// }

// void ViewRootImpl::HandleWindowMoved(
//     /* [in] */ Int32 l,
//     /* [in] */ Int32 t)
// {
//     if (mAdded) {
//         Int32 w, h;
//         mWinFrame->GetWidth(&w);
//         mWinFrame->GetHeight(&h);

//         mWinFrame->mLeft = l;
//         mWinFrame->mRight = l + w;
//         mWinFrame->mTop = t;
//         mWinFrame->mBottom = t + h;

//         if (mView != NULL) {
//             ForceLayout(mView);
//         }
//         RequestLayout();
//     }
// }

void ViewRootImpl::HandleWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus,
    /* [in] */ Boolean inTouchMode)
{
    if (mAdded) {
        mAttachInfo->mHasWindowFocus = hasWindowFocus;

        ProfileRendering(hasWindowFocus);

        if (hasWindowFocus) {
            EnsureTouchModeLocally(inTouchMode);
            Boolean isValid;
            if (mAttachInfo->mHardwareRenderer != NULL && (mSurface->IsValid(&isValid), isValid)){
                mFullRedrawNeeded = TRUE;
                // try {
                    AutoPtr<IWindowManagerLayoutParams> lp = mWindowAttributes;
                    AutoPtr<IRect> surfaceInsets;
                    if (lp != NULL) {
                        lp->GetSurfaceInsets((IRect**)&surfaceInsets);
                    }
                    Boolean result;
                    mAttachInfo->mHardwareRenderer->InitializeIfNeeded(
                            mWidth, mHeight, mSurface, surfaceInsets, &result);
                // } catch (OutOfResourcesException e) {
                //     Log.e(TAG, "OutOfResourcesException locking surface", e);
                //     try {
                //         if (!mWindowSession.outOfMemory(mWindow)) {
                //             Slog.w(TAG, "No processes killed for memory; killing self");
                //             Process.killProcess(Process.myPid());
                //         }
                //     } catch (RemoteException ex) {
                //     }
                //     // Retry in a bit.
                //     sendMessageDelayed(obtainMessage(msg.what, msg.arg1, msg.arg2), 500);
                //     return;
                // }
            }
        }
        mLastWasImTarget = CWindowManagerLayoutParams::MayUseInputMethod(
                WINLAY_PROBE(mWindowAttributes)->mFlags);
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::PeekInstance();
        if (mView != NULL) {
            if (hasWindowFocus && imm != NULL && mLastWasImTarget && !IsInLocalFocusMode()) {
                imm->StartGettingWindowFocus(mView);
            }

            mAttachInfo->mKeyDispatchState->Reset();
            mView->DispatchWindowFocusChanged(hasWindowFocus);
            ((ViewTreeObserver*)mAttachInfo->mTreeObserver.Get())->DispatchOnWindowFocusChange(hasWindowFocus);
        }

        // Note: must be done after the focus change callbacks,
        // so all of the view state is set up correctly.
        if (hasWindowFocus) {
            if (imm != NULL && mLastWasImTarget && !IsInLocalFocusMode()) {
                AutoPtr<IView> focus;
                mView->FindFocus((IView**)&focus);
                imm->OnWindowFocus(
                    mView, focus, WINLAY_PROBE(mWindowAttributes)->mSoftInputMode,
                    !mHasHadWindowFocus, WINLAY_PROBE(mWindowAttributes)->mFlags);
            }
            // Clear the forward bit.  We can just do this directly, since
            // the window manager doesn't care about it.
            //
            WINLAY_PROBE(mWindowAttributes)->mSoftInputMode &=
                ~IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION;

            AutoPtr<IWindowManagerLayoutParams> params;
            mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
            WINLAY_PROBE(params)->mSoftInputMode &=
                ~IWindowManagerLayoutParams::SOFT_INPUT_IS_FORWARD_NAVIGATION;
            mHasHadWindowFocus = TRUE;
        }

        Boolean isEnabled;
        if (mView != NULL && (mAccessibilityManager->IsEnabled(&isEnabled), isEnabled)) {
            if (hasWindowFocus) {
                IAccessibilityEventSource::Probe(mView)->SendAccessibilityEvent(
                    IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
            }
        }
    }
}

void ViewRootImpl::LogView(
    /* [in] */ const String& info)
{
#ifdef _DEBUG
    if (mView != NULL) {
        Int32 id;
        mView->GetId(&id);
        if (id == 0x7f0700a2 /* SystemUIR::content_parent */) return;

        if (id != IView::NO_ID) {
            AutoPtr<IResources> resources;
            mView->GetResources((IResources**)&resources);
            String name;
            resources->GetResourceEntryName(id, &name);
            printf("    %s: viewId = 0x%08x, IView* = %p, type = %s\n", info.string(), id, mView.Get(), name.string());
            Logger::D("ViewRootImpl", "    %s: viewId = 0x%08x, IView* = %p, type = %s\n", info.string(), id, mView.Get(), name.string());
        }
        else {
            printf("    %s: viewId = 0x%08x, IView* = %p\n", info.string(), id, mView.Get());
            Logger::D("ViewRootImpl", "    %s: viewId = 0x%08x, IView* = %p\n", info.string(), id, mView.Get());
        }
    }
#endif
}

ECode ViewRootImpl::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::RootDisplayListener
////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(ViewRootImpl::RootDisplayListener, Object, IDisplayListener)

ViewRootImpl::RootDisplayListener::RootDisplayListener(
    /* [in] */ ViewRootImpl* host)
    : mHost(host)
{}

CARAPI ViewRootImpl::RootDisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

CARAPI ViewRootImpl::RootDisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

CARAPI ViewRootImpl::RootDisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    Int32 id;
    mHost->mDisplay->GetDisplayId(&id);
    if (mHost->mView != NULL && id == displayId) {
        Int32 oldDisplayState = mHost->mAttachInfo->mDisplayState;
        Int32 newDisplayState;
        mHost->mDisplay->GetState(&newDisplayState);
        if (oldDisplayState != newDisplayState) {
            mHost->mAttachInfo->mDisplayState = newDisplayState;
            if (oldDisplayState != IDisplay::STATE_UNKNOWN) {
                Int32 oldScreenState = ToViewScreenState(oldDisplayState);
                Int32 newScreenState = ToViewScreenState(newDisplayState);
                if (oldScreenState != newScreenState) {
                    VIEW_PROBE(mHost->mView)->DispatchScreenStateChanged(newScreenState);
                }
                if (oldDisplayState == IDisplay::STATE_OFF) {
                    // Draw was suppressed so we need to for it to happen here.
                    mHost->mFullRedrawNeeded = TRUE;
                    mHost->ScheduleTraversals();
                }
            }
        }
    }
    return NOERROR;
}

Int32 ViewRootImpl::RootDisplayListener::ToViewScreenState(
    /* [in] */ Int32 displayState)
{
     return displayState == IDisplay::STATE_OFF ?
        IView::SCREEN_STATE_OFF : IView::SCREEN_STATE_ON;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::LayoutRunnable
////////////////////////////////////////////////////////////
ViewRootImpl::LayoutRunnable::LayoutRunnable(
    /* [in] */ IArrayList* list)
    : mList(list)

{}

ECode ViewRootImpl::LayoutRunnable::Run()
{
    Int32 numValidRequests = 0;
    mList->GetSize(&numValidRequests);
    for (Int32 i = 0; i < numValidRequests; ++i) {
        AutoPtr<IInterface> temp;
        mList->Get(i, (IInterface**)&temp);
        AutoPtr<IView> view = IView::Probe(temp);
        Logger::D("View", "requestLayout() improperly called by %d during second layout pass: posting in next frame", view.Get());
        view->RequestLayout();
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::ProfileFrameCallback
////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(ViewRootImpl::ProfileFrameCallback, Object, IFrameCallback)

ViewRootImpl::ProfileFrameCallback::ProfileFrameCallback(
    /* [in] */ ViewRootImpl* host)
    : mHost(host)
{}

ECode ViewRootImpl::ProfileFrameCallback::DoFrame(
    /* [in] */ Int64 frameTimeNanos)
{
    mHost->mDirty->Set(0, 0, mHost->mWidth, mHost->mHeight);
    mHost->ScheduleTraversals();
    if (mHost->mRenderProfilingEnabled) {
        mHost->mChoreographer->PostFrameCallback(mHost->mRenderProfiler);
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::InputStage
////////////////////////////////////////////////////////////

ViewRootImpl::InputStage::InputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next)
    : mHost(host)
    , mNext(next)
{}

ECode ViewRootImpl::InputStage::Deliver(
    /* [in] */ QueuedInputEvent* q)
{
    if ((q->mFlags & QueuedInputEvent::FLAG_FINISHED) != 0) {
        Forward(q);
    }
    else if (ShouldDropInputEvent(q)) {
        Finish(q, FALSE);
    }
    else {
        Apply(q, OnProcess(q));
    }
    return NOERROR;
}

ECode ViewRootImpl::InputStage::Finish(
    /* [in] */ QueuedInputEvent* q,
    /* [in] */ Boolean handled)
{
    q->mFlags |= QueuedInputEvent::FLAG_FINISHED;
    if (handled) {
        q->mFlags |= QueuedInputEvent::FLAG_FINISHED_HANDLED;
    }
    Forward(q);
    return NOERROR;
}

ECode ViewRootImpl::InputStage::Forward(
    /* [in] */ QueuedInputEvent* q)
{
    OnDeliverToNext(q);
    return NOERROR;
}

ECode ViewRootImpl::InputStage::Apply(
    /* [in] */ QueuedInputEvent* q,
    /* [in] */ Int32 result)
{
    if (result == FORWARD) {
        Forward(q);
    }
    else if (result == FINISH_HANDLED) {
        Finish(q, TRUE);
    }
    else if (result == FINISH_NOT_HANDLED) {
        Finish(q, FALSE);
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Int32 ViewRootImpl::InputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    return FORWARD;
}

ECode ViewRootImpl::InputStage::OnDeliverToNext(
    /* [in] */ QueuedInputEvent* q)
{
    if (mNext != NULL) {
        mNext->Deliver(q);
    } else {
        mHost->FinishInputEvent(q);
    }
    return NOERROR;
}

Boolean ViewRootImpl::InputStage::ShouldDropInputEvent(
    /* [in] */ QueuedInputEvent* q)
{
    if (mHost->mView == NULL || !mHost->mAdded) {
        return TRUE;
    } else if (!mHost->mAttachInfo->mHasWindowFocus) {
        Boolean isFromSource;
        q->mEvent->IsFromSource(IInputDevice::SOURCE_CLASS_POINTER, &isFromSource);
        if (!isFromSource && !IsTerminalInputEvent(q->mEvent)) {
            // If this is a focused event and the window doesn't currently have input focus,
            // then drop this event.  This could be an event that came back from the previous
            // stage but the window has lost focus in the meantime.
            return TRUE;
        }
    }
    return FALSE;
}

ECode ViewRootImpl::InputStage::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* writer)
{
    if (mNext != NULL) {
        mNext->Dump(prefix, writer);
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::AsyncInputStage
////////////////////////////////////////////////////////////

ViewRootImpl::AsyncInputStage::AsyncInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next,
    /* [in] */ const String& traceCounter)
    : InputStage(host, next)
    , mTraceCounter(traceCounter)
{}

ECode ViewRootImpl::AsyncInputStage::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* writer)
{
    writer->Print(prefix);
    writer->Print(String("ViewRootImpl"));
    writer->Print(String(": mQueueLength="));
    writer->Print(mQueueLength);

    InputStage::Dump(prefix, writer);
    return NOERROR;
}

ECode ViewRootImpl::AsyncInputStage::Defer(
    /* [in] */ QueuedInputEvent* q)
{
    q->mFlags |= QueuedInputEvent::FLAG_DEFERRED;
    Enqueue(q);
    return NOERROR;
}

ECode ViewRootImpl::AsyncInputStage::Forward(
    /* [in] */ QueuedInputEvent* q)
{
    // Clear the deferred flag.
    q->mFlags &= ~QueuedInputEvent::FLAG_DEFERRED;

    // Fast path if the queue is empty.
    AutoPtr<QueuedInputEvent> curr = mQueueHead;
    if (curr == NULL) {
        InputStage::Forward(q);
        return NOERROR;
    }

    // Determine whether the event must be serialized behind any others
    // before it can be delivered to the next stage.  This is done because
    // deferred events might be handled out of order by the stage.
    Int32 deviceId = 0;
    q->mEvent->GetDeviceId(&deviceId);
    AutoPtr<QueuedInputEvent> prev;
    Boolean blocked = FALSE;
    while (curr != NULL && q != curr) {
        Int32 id;
        curr->mEvent->GetDeviceId(&id);
        if (!blocked && deviceId == id) {
            blocked = TRUE;
        }
        prev = curr;
        curr = curr->mNext;
    }

    // If the event is blocked, then leave it in the queue to be delivered later.
    // Note that the event might not yet be in the queue if it was not previously
    // deferred so we will enqueue it if needed.
    if (blocked) {
        if (curr == NULL) {
            Enqueue(q);
        }
        return NOERROR;
    }

    // The event is not blocked.  Deliver it immediately.
    if (curr != NULL) {
        curr = curr->mNext;
        Dequeue(q, prev);
    }
    InputStage::Forward(q);

    // Dequeuing this event may have unblocked successors.  Deliver them.
    while (curr != NULL) {
        Int32 id;
        curr->mEvent->GetDeviceId(&id);
        if (deviceId == id) {
            if ((curr->mFlags & QueuedInputEvent::FLAG_DEFERRED) != 0) {
                break;
            }
            AutoPtr<QueuedInputEvent> next = curr->mNext;
            Dequeue(curr, prev);
            InputStage::Forward(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->mNext;
        }
    }
    return NOERROR;
}

ECode ViewRootImpl::AsyncInputStage::Apply(
    /* [in] */ QueuedInputEvent* q,
    /* [in] */ Int32 result)
{
    if (result == DEFER) {
        Defer(q);
    } else {
        InputStage::Apply(q, result);
    }
    return NOERROR;
}

void ViewRootImpl::AsyncInputStage::Enqueue(
    /* [in] */ QueuedInputEvent* q)
{
    if (mQueueTail == NULL) {
        mQueueHead = q;
        mQueueTail = q;
    } else {
        mQueueTail->mNext = q;
        mQueueTail = q;
    }

    mQueueLength += 1;
    //Trace.traceCounter(Trace.TRACE_TAG_INPUT, mTraceCounter, mQueueLength);
}

void ViewRootImpl::AsyncInputStage::Dequeue(
    /* [in] */ QueuedInputEvent* q,
    /* [in] */ QueuedInputEvent* prev)
{
    if (prev == NULL) {
        mQueueHead = q->mNext;
    } else {
        prev->mNext = q->mNext;
    }
    if (q == mQueueTail) {
        mQueueTail = prev;
    }
    q->mNext = NULL;

    mQueueLength -= 1;
    //Trace.traceCounter(Trace.TRACE_TAG_INPUT, mTraceCounter, mQueueLength);
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::NativePreImeInputStage
////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ViewRootImpl::NativePreImeInputStage, AsyncInputStage, IInputQueueFinishedInputEventCallback)

ViewRootImpl::NativePreImeInputStage::NativePreImeInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next,
    /* [in] */ const String& traceCounter)
    : AsyncInputStage(host, next, traceCounter)
{}

ECode ViewRootImpl::NativePreImeInputStage::OnFinishedInputEvent(
    /* [in] */ IInterface* token,
    /* [in] */ Boolean handled)
{
    AutoPtr<QueuedInputEvent> q = (QueuedInputEvent*)IObject::Probe(token);
    if (handled) {
        Finish(q, TRUE);
        return NOERROR;
    }
    Forward(q);
    return NOERROR;
}

Int32 ViewRootImpl::NativePreImeInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    if (mHost->mInputQueue != NULL && IKeyEvent::Probe(q->mEvent)) {
        //mHost->mInputQueue->SendInputEvent(q->mEvent, q, TRUE, IInputQueueFinishedInputEventCallback::Probe(this));
        return DEFER;
    }
    return FORWARD;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::ViewPreImeInputStage
////////////////////////////////////////////////////////////
ViewRootImpl::ViewPreImeInputStage::ViewPreImeInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next)
    : InputStage(host, next)
{}

Int32 ViewRootImpl::ViewPreImeInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    if (IKeyEvent::Probe(q->mEvent)) {
        return ProcessKeyEvent(q);
    }
    return FORWARD;
}

Int32 ViewRootImpl::ViewPreImeInputStage::ProcessKeyEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IKeyEvent> event = IKeyEvent::Probe(q->mEvent);
    Boolean res;
    if (mHost->mView->DispatchKeyEventPreIme(event, &res), res) {
        return FINISH_HANDLED;
    }
    return FORWARD;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::ImeInputStage
////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(ViewRootImpl::ImeInputStage, AsyncInputStage, IInputMethodManagerFinishedInputEventCallback)

ViewRootImpl::ImeInputStage::ImeInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next,
    /* [in] */ const String& traceCounter)
    : AsyncInputStage(host, next, traceCounter)
{}

ECode ViewRootImpl::ImeInputStage::OnFinishedInputEvent(
    /* [in] */ IInterface* token,
    /* [in] */ Boolean handled)
{
    AutoPtr<QueuedInputEvent> q = (QueuedInputEvent*)IObject::Probe(token);
    if (handled) {
        Finish(q, TRUE);
        return NOERROR;
    }
    Forward(q);
    return NOERROR;
}

Int32 ViewRootImpl::ImeInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    if (mHost->mLastWasImTarget && !mHost->IsInLocalFocusMode()) {
        assert(0);
        AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            AutoPtr<IInputEvent> event = q->mEvent;
            Int32 result = 0;
            assert(0);
            //imm->DispatchInputEvent(event, q, IInputMethodManagerFinishedInputEventCallback::Probe(this), mHandler, &result);
            if (result == IInputMethodManager::DISPATCH_HANDLED) {
                return FINISH_HANDLED;
            } else if (result == IInputMethodManager::DISPATCH_NOT_HANDLED) {
                // The IME could not handle it, so skip along to the next InputStage
                return FORWARD;
            } else {
                return DEFER; // callback will be invoked later
            }
        }
    }
    return FORWARD;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::EarlyPostImeInputStage
////////////////////////////////////////////////////////////
ViewRootImpl::EarlyPostImeInputStage::EarlyPostImeInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next)
    : InputStage(host, next)
{}

Int32 ViewRootImpl::EarlyPostImeInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    if (IKeyEvent::Probe(q->mEvent)) {
        return ProcessKeyEvent(q);
    } else {
        Int32 source = 0;
        q->mEvent->GetSource(&source);
        if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
            return ProcessPointerEvent(q);
        }
    }
    return FORWARD;
}

Int32 ViewRootImpl::EarlyPostImeInputStage::ProcessKeyEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IKeyEvent> event = IKeyEvent::Probe(q->mEvent);

    // If the key's purpose is to exit touch mode then we consume it
    // and consider it handled.
    if (mHost->CheckForLeavingTouchModeAndConsume(event)) {
        return FINISH_HANDLED;
    }

    // Make sure the fallback event policy sees all keys that will be
    // delivered to the view hierarchy.
    mHost->mFallbackEventHandler->PreDispatchKeyEvent(event);
    return FORWARD;
}

Int32 ViewRootImpl::EarlyPostImeInputStage::ProcessPointerEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IMotionEvent> event = IMotionEvent::Probe(q->mEvent);

    // Translate the pointer event for compatibility, if needed.
    if (mHost->mTranslator != NULL) {
        mHost->mTranslator->TranslateEventInScreenToAppWindow(event);
    }

    // Enter touch mode on down or scroll.
    Int32 action = 0;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN || action == IMotionEvent::ACTION_SCROLL) {
        Boolean temp;
        mHost->EnsureTouchMode(TRUE, &temp);
    }

    // Offset the scroll position.
    if (mHost->mCurScrollY != 0) {
        event->OffsetLocation(0, mHost->mCurScrollY);
    }

    // Remember the touch position for possible drag-initiation.
    Boolean isTouchEvent;
    if (event->IsTouchEvent(&isTouchEvent), isTouchEvent) {
        AutoPtr<CPointF> temp = (CPointF*)mHost->mLastTouchPoint.Get();
        event->GetRawX(&temp->mX);
        event->GetRawY(&temp->mY);
    }
    return FORWARD;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::NativePostImeInputStage
////////////////////////////////////////////////////////////
ViewRootImpl::NativePostImeInputStage::NativePostImeInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next,
    /* [in] */ const String& traceCounter)
    : AsyncInputStage(host, next, traceCounter)
{}

CAR_INTERFACE_IMPL(ViewRootImpl::NativePostImeInputStage, AsyncInputStage, IInputMethodManagerFinishedInputEventCallback)

ECode ViewRootImpl::NativePostImeInputStage::OnFinishedInputEvent(
    /* [in] */ IInterface* token,
    /* [in] */ Boolean handled)
{
    QueuedInputEvent* q = (QueuedInputEvent*)IObject::Probe(token);
    if (handled) {
        Finish(q, TRUE);
        return NOERROR;
    }
    Forward(q);
    return NOERROR;
}

Int32 ViewRootImpl::NativePostImeInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    if (mHost->mInputQueue != NULL) {
        assert(0);
        //mHost->mInputQueue->SendInputEvent(q->mEvent, q, FALSE, IInputMethodManagerFinishedInputEventCallback::Probe(this));
        return DEFER;
    }
    return FORWARD;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::ViewPostImeInputStage
////////////////////////////////////////////////////////////

ViewRootImpl::ViewPostImeInputStage::ViewPostImeInputStage(
    /* [in] */ ViewRootImpl* host,
    /* [in] */ InputStage* next)
    : InputStage(host, next)
{}

Int32 ViewRootImpl::ViewPostImeInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    if (IKeyEvent::Probe(q->mEvent)) {
        return ProcessKeyEvent(q);
    } else {
        // If delivering a new non-key event, make sure the window is
        // now allowed to start updating.
        mHost->HandleDispatchDoneAnimating();
        Int32 source = 0;
        q->mEvent->GetSource(&source);
        if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
            return ProcessPointerEvent(q);
        } else if ((source & IInputDevice::SOURCE_CLASS_TRACKBALL) != 0) {
            return ProcessTrackballEvent(q);
        } else {
            return ProcessGenericMotionEvent(q);
        }
    }
}

ECode ViewRootImpl::ViewPostImeInputStage::OnDeliverToNext(
    /* [in] */ QueuedInputEvent* q)
{
    Boolean isTouchEvent;
    IMotionEvent::Probe(q->mEvent)->IsTouchEvent(&isTouchEvent);
    if (mHost->mUnbufferedInputDispatch
            && IKeyEvent::Probe(q->mEvent)
            && isTouchEvent
            && mHost->IsTerminalInputEvent(q->mEvent)) {
        mHost->mUnbufferedInputDispatch = FALSE;
        mHost->ScheduleConsumeBatchedInput();
    }
    InputStage::OnDeliverToNext(q);
    return NOERROR;
}

Int32 ViewRootImpl::ViewPostImeInputStage::ProcessKeyEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IKeyEvent> event = IKeyEvent::Probe(q->mEvent);

    Int32 action = 0;
    event->GetAction(&action);
    if (action != IKeyEvent::ACTION_UP) {
        // If delivering a new key event, make sure the window is
        // now allowed to start updating.
        mHost->HandleDispatchDoneAnimating();
    }

    // Deliver the key to the view hierarchy.
    Boolean dispatchKeyEvent;
    if (mHost->mView->DispatchKeyEvent(event, &dispatchKeyEvent), dispatchKeyEvent) {
        return FINISH_HANDLED;
    }

    if (ShouldDropInputEvent(q)) {
        return FINISH_NOT_HANDLED;
    }

    // If the Control modifier is held, try to interpret the key as a shortcut.
    Boolean isCtrlPressed;
    event->IsCtrlPressed(&isCtrlPressed);
    Int32 repeatCount;
    event->GetRepeatCount(&repeatCount);
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    Boolean isModifierKey;
    assert(0);
    //CKeyEvent::IsModifierKey(keyCode, &isModifierKey);
    if (action == IKeyEvent::ACTION_DOWN
            && isCtrlPressed
            && repeatCount == 0
            && !isModifierKey) {
        Boolean tempEvent;
        mHost->mView->DispatchKeyShortcutEvent(event, &tempEvent);
        if (tempEvent) {
            return FINISH_HANDLED;
        }
        if (ShouldDropInputEvent(q)) {
            return FINISH_NOT_HANDLED;
        }
    }

    // Apply the fallback event policy.
    Boolean fallbackEventHandler;
    mHost->mFallbackEventHandler->DispatchKeyEvent(event, &fallbackEventHandler);
    if (fallbackEventHandler) {
        return FINISH_HANDLED;
    }
    if (ShouldDropInputEvent(q)) {
        return FINISH_NOT_HANDLED;
    }

    // Handle automatic focus changes.
    if (action == IKeyEvent::ACTION_DOWN) {
        int direction = 0, eventCode;
        switch (event->GetKeyCode(&eventCode), eventCode) {
            case IKeyEvent::KEYCODE_DPAD_LEFT: {
                Boolean hasNoModifiers;
                event->HasNoModifiers(&hasNoModifiers);
                if (hasNoModifiers) {
                    direction = IView::FOCUS_LEFT;
                }
                break;
            }
            case IKeyEvent::KEYCODE_DPAD_RIGHT: {
                Boolean hasNoModifiers;
                event->HasNoModifiers(&hasNoModifiers);
                if (hasNoModifiers) {
                    direction = IView::FOCUS_RIGHT;
                }
                break;
            }
            case IKeyEvent::KEYCODE_DPAD_UP: {
                Boolean hasNoModifiers;
                event->HasNoModifiers(&hasNoModifiers);
                if (hasNoModifiers) {
                    direction = IView::FOCUS_UP;
                }
                break;
            }
            case IKeyEvent::KEYCODE_DPAD_DOWN: {
                Boolean hasNoModifiers;
                event->HasNoModifiers(&hasNoModifiers);
                if (hasNoModifiers) {
                    direction = IView::FOCUS_DOWN;
                }
                break;
            }
            case IKeyEvent::KEYCODE_TAB: {
                Boolean hasNoModifiers, hasModifiers;
                event->HasNoModifiers(&hasNoModifiers);
                event->HasModifiers(IKeyEvent::META_SHIFT_ON, &hasModifiers);
                if (hasNoModifiers) {
                    direction = IView::FOCUS_FORWARD;
                } else if (hasModifiers) {
                    direction = IView::FOCUS_BACKWARD;
                }
                break;
            }
        }
        if (direction != 0) {
            AutoPtr<IView> focused;
            mHost->mView->FindFocus((IView**)&focused);
            Boolean temp;
            if (focused != NULL) {
                AutoPtr<IView> v;
                focused->FocusSearch(direction, (IView**)&v);
                if (v != NULL && v != focused) {
                    // do the math the get the interesting rect
                    // of previous focused into the coord system of
                    // newly focused view
                    focused->GetFocusedRect(mHost->mTempRect);
                    if (IViewGroup::Probe(mHost->mView)) {
                        (IViewGroup::Probe(mHost->mView))->OffsetDescendantRectToMyCoords(focused, mHost->mTempRect);
                        (IViewGroup::Probe(mHost->mView))->OffsetRectIntoDescendantCoords(v, mHost->mTempRect);
                    }

                    if (v->RequestFocus(direction, mHost->mTempRect, &temp), temp) {
                        assert(0);
                        //PlaySoundEffect(SoundEffectConstants::GetContantForFocusDirection(direction));
                        return FINISH_HANDLED;
                    }
                }
                // Give the focused view a last chance to handle the dpad key.
                if (mHost->mView->DispatchUnhandledMove(focused, direction, &temp), temp) {
                    return FINISH_HANDLED;
                }
            } else {
                // find the best view to give focus to in this non-touch-mode with no-focus
                AutoPtr<IView> v;
                mHost->FocusSearch(NULL, direction, (IView**)&v);
                if (v != NULL && (v->RequestFocus(direction, &temp), temp)) {
                    return FINISH_HANDLED;
                }
            }
        }
    }
    return FORWARD;
}

Int32 ViewRootImpl::ViewPostImeInputStage::ProcessPointerEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IMotionEvent> event = IMotionEvent::Probe(q->mEvent);

    mHost->mAttachInfo->mUnbufferedDispatchRequested = FALSE;
    Boolean handled;
    mHost->mView->DispatchPointerEvent(event, &handled);
    if (mHost->mAttachInfo->mUnbufferedDispatchRequested && !mHost->mUnbufferedInputDispatch) {
        mHost->mUnbufferedInputDispatch = TRUE;
        if (mHost->mConsumeBatchedInputScheduled) {
            mHost->ScheduleConsumeBatchedInputImmediately();
        }
    }
    return handled ? FINISH_HANDLED : FORWARD;
}

Int32 ViewRootImpl::ViewPostImeInputStage::ProcessTrackballEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IMotionEvent> event = IMotionEvent::Probe(q->mEvent);
    Boolean res;
    mHost->mView->DispatchTrackballEvent(event, &res);
    if (res) {
        return FINISH_HANDLED;
    }
    return FORWARD;
}

Int32 ViewRootImpl::ViewPostImeInputStage::ProcessGenericMotionEvent(
    /* [in] */ QueuedInputEvent* q)
{
    AutoPtr<IMotionEvent> event = IMotionEvent::Probe(q->mEvent);

    // Deliver the event to the view.
    Boolean res;
    mHost->mView->DispatchGenericMotionEvent(event, &res);
    if (res) {
        return FINISH_HANDLED;
    }
    return FORWARD;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::SyntheticInputStage
////////////////////////////////////////////////////////////
ViewRootImpl::SyntheticInputStage::SyntheticInputStage(
            /* [in] */ ViewRootImpl* host)
    : InputStage(host, NULL)
{
    mTrackball = new SyntheticTrackballHandler(host);
    mJoystick = new SyntheticJoystickHandler(host);
    mTouchNavigation = new SyntheticTouchNavigationHandler(host);
    mKeyboard = new SyntheticKeyboardHandler();
}

Int32 ViewRootImpl::SyntheticInputStage::OnProcess(
    /* [in] */ QueuedInputEvent* q)
{
    q->mFlags |= QueuedInputEvent::FLAG_RESYNTHESIZED;
    if (IMotionEvent::Probe(q->mEvent)) {
        AutoPtr<IMotionEvent> event = IMotionEvent::Probe(q->mEvent);
        Int32 source = 0;
        IInputEvent::Probe(event)->GetSource(&source);
        if ((source & IInputDevice::SOURCE_CLASS_TRACKBALL) != 0) {
            mTrackball->Process(event);
            return FINISH_HANDLED;
        } else if ((source & IInputDevice::SOURCE_CLASS_JOYSTICK) != 0) {
            mJoystick->Process(event);
            return FINISH_HANDLED;
        } else if ((source & IInputDevice::SOURCE_TOUCH_NAVIGATION)
                == IInputDevice::SOURCE_TOUCH_NAVIGATION) {
            mTouchNavigation->Process(event);
            return FINISH_HANDLED;
        }
    } else if ((q->mFlags & QueuedInputEvent::FLAG_UNHANDLED) != 0) {
        mKeyboard->Process(IKeyEvent::Probe(q->mEvent));
        return FINISH_HANDLED;
    }

    return FORWARD;
}

ECode ViewRootImpl::SyntheticInputStage::OnDeliverToNext(
    /* [in] */ QueuedInputEvent* q)
{
    if ((q->mFlags & QueuedInputEvent::FLAG_RESYNTHESIZED) == 0) {
        // Cancel related synthetic events if any prior stage has handled the event.
        if (IMotionEvent::Probe(q->mEvent)) {
            AutoPtr<IMotionEvent> event = IMotionEvent::Probe(q->mEvent);
            Int32 source = 0;
            IInputEvent::Probe(event)->GetSource(&source);
            if ((source & IInputDevice::SOURCE_CLASS_TRACKBALL) != 0) {
                mTrackball->Cancel(event);
            } else if ((source & IInputDevice::SOURCE_CLASS_JOYSTICK) != 0) {
                mJoystick->Cancel(event);
            } else if ((source & IInputDevice::SOURCE_TOUCH_NAVIGATION)
                    == IInputDevice::SOURCE_TOUCH_NAVIGATION) {
                mTouchNavigation->Cancel(event);
            }
        }
    }
    InputStage::OnDeliverToNext(q);
    return NOERROR;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::SyntheticTrackballHandler
////////////////////////////////////////////////////////////

ViewRootImpl::SyntheticTrackballHandler::SyntheticTrackballHandler(
    /* [in] */ ViewRootImpl* host)
    : mLastTime(0)
    , mHost(host)
{
    mX = new TrackballAxis();
    mY = new TrackballAxis();
}

ECode ViewRootImpl::SyntheticTrackballHandler::Process(
    /* [in] */ IMotionEvent* event)
{
    // Translate the trackball event into DPAD keys and try to deliver those.
    Int64 curTime = SystemClock::GetUptimeMillis();
    if ((mLastTime + ViewRootImpl::MAX_TRACKBALL_DELAY) < curTime) {
        // It has been too long since the last movement,
        // so restart at the beginning.
        mX->Reset(0);
        mY->Reset(0);
        mLastTime = curTime;
    }

    Int32 action = 0;
    event->GetAction(&action);
    Int32 metaState = 0;
    event->GetMetaState(&metaState);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            mX->Reset(2);
            mY->Reset(2);
            AutoPtr<IKeyEvent> tempEvent;
            assert(0);
            /*CKeyEvent::New(curTime, curTime,
                    IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_DPAD_CENTER, 0, metaState,
                    IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FALLBACK,
                    IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&tempEvent);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(tempEvent));
            break;
        }

        case IMotionEvent::ACTION_UP: {
            mX->Reset(2);
            mY->Reset(2);
            AutoPtr<IKeyEvent> tempEvent;
            assert(0);
            /*CKeyEvent::New(curTime, curTime,
                    IKeyEvent::ACTION_UP, IKeyEvent::KEYCODE_DPAD_CENTER, 0, metaState,
                    IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FALLBACK,
                    IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&tempEvent);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(tempEvent));
            break;
        }

    }

    /*if (DEBUG_TRACKBALL) Log.v(TAG, "TB X=" + mX.position + " step="
            + mX.step + " dir=" + mX.dir + " acc=" + mX.acceleration
            + " move=" + event.getX()
            + " / Y=" + mY.position + " step="
            + mY.step + " dir=" + mY.dir + " acc=" + mY.acceleration
            + " move=" + event.getY());*/
    Float eventPosition;
    Int64 eventTime;
    event->GetX(&eventPosition);
    IInputEvent::Probe(event)->GetEventTime(&eventTime);
    Float xOff = mX->Collect(eventPosition, eventTime, String("X"));
    event->GetY(&eventPosition);
    Float yOff = mY->Collect(eventPosition, eventTime, String("Y"));

    // Generate DPAD events based on the trackball movement.
    // We pick the axis that has moved the most as the direction of
    // the DPAD.  When we generate DPAD events for one axis, then the
    // other axis is reset -- we don't want to perform DPAD jumps due
    // to slight movements in the trackball when making major movements
    // along the other axis.
    Int32 keycode = 0;
    Int32 movement = 0;
    Float accel = 1;
    if (xOff > yOff) {
        movement = mX->Generate();
        if (movement != 0) {
            keycode = movement > 0 ? IKeyEvent::KEYCODE_DPAD_RIGHT
                    : IKeyEvent::KEYCODE_DPAD_LEFT;
            accel = mX->mAcceleration;
            mY->Reset(2);
        }
    } else if (yOff > 0) {
        movement = mY->Generate();
        if (movement != 0) {
            keycode = movement > 0 ? IKeyEvent::KEYCODE_DPAD_DOWN
                    : IKeyEvent::KEYCODE_DPAD_UP;
            accel = mY->mAcceleration;
            mX->Reset(2);
        }
    }

    if (keycode != 0) {
        if (movement < 0) movement = -movement;
        Int32 accelMovement = (Int32)(movement * accel);
        /*if (DEBUG_TRACKBALL) Log.v(TAG, "Move: movement=" + movement
                + " accelMovement=" + accelMovement
                + " accel=" + accel);*/
        if (accelMovement > movement) {
            /*if (DEBUG_TRACKBALL) Log.v(TAG, "Delivering fake DPAD: "
                    + keycode);*/
            movement--;
            Int32 repeatCount = accelMovement - movement;
            AutoPtr<IKeyEvent> tempEvent;
            assert(0);
            /*CKeyEvent::New(curTime, curTime,
                    IKeyEvent::ACTION_MULTIPLE, keycode, repeatCount, metaState,
                    IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FALLBACK,
                    IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&tempEvent);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(tempEvent));
        }
        while (movement > 0) {
            /*if (DEBUG_TRACKBALL) Log.v(TAG, "Delivering fake DPAD: "
                    + keycode);*/
            movement--;
            curTime = SystemClock::GetUptimeMillis();
            AutoPtr<IKeyEvent> tempEvent;
            assert(0);
            /*CKeyEvent::New(curTime, curTime,
                    IKeyEvent::ACTION_DOWN, keycode, 0, metaState,
                    IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FALLBACK,
                    IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&tempEvent);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(tempEvent));
            AutoPtr<IKeyEvent> otherEvent;
            assert(0);
            /*CKeyEvent::New(curTime, curTime,
                    IKeyEvent::ACTION_UP, keycode, 0, metaState,
                    IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FALLBACK,
                    IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&tempEvent);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(otherEvent));
        }
        mLastTime = curTime;
    }
    return NOERROR;
}

ECode ViewRootImpl::SyntheticTrackballHandler::Cancel(
    /* [in] */ IMotionEvent* event)
{
    mLastTime = Elastos::Core::Math::INT32_MAX_VALUE;

    // If we reach this, we consumed a trackball event.
    // Because we will not translate the trackball event into a key event,
    // touch mode will not exit, so we exit touch mode here.
    if (mHost->mView != NULL && mHost->mAdded) {
        Boolean temp;
        mHost->EnsureTouchMode(FALSE, &temp);
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////
//          ViewRootImpl::SyntheticJoystickHandler
////////////////////////////////////////////////////////////
const String ViewRootImpl::SyntheticJoystickHandler::TAG = String("SyntheticJoystickHandler");
const Int32 ViewRootImpl::SyntheticJoystickHandler::MSG_ENQUEUE_X_AXIS_KEY_REPEAT = 1;
const Int32 ViewRootImpl::SyntheticJoystickHandler::MSG_ENQUEUE_Y_AXIS_KEY_REPEAT = 2;

ViewRootImpl::SyntheticJoystickHandler::SyntheticJoystickHandler(
    /* [in] */ ViewRootImpl* host)
    : Handler(TRUE)
    , mLastXDirection(0)
    , mLastYDirection(0)
    , mLastXKeyCode(0)
    , mLastYKeyCode(0)
    , mHost(host)
{}

CARAPI ViewRootImpl::SyntheticJoystickHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_ENQUEUE_X_AXIS_KEY_REPEAT:
        case MSG_ENQUEUE_Y_AXIS_KEY_REPEAT:
        {
            AutoPtr<IInterface> temp;
            msg->GetObj((IInterface**)&temp);
            AutoPtr<IKeyEvent> oldEvent = IKeyEvent::Probe(temp);
            Int32 repeatCount;
            oldEvent->GetRepeatCount(&repeatCount);
            AutoPtr<IKeyEvent> e;
            assert(0);
            //CKeyEvent::ChangeTimeRepeat(oldEvent, SystemClock::GetUptimeMillis(), repeatCount + 1, (IKeyEvent**)&e);
            if (mHost->mAttachInfo->mHasWindowFocus) {
                mHost->EnqueueInputEvent(IInputEvent::Probe(e));
                AutoPtr<IMessage> m;
                ObtainMessage(what, e, (IMessage**)&m);
                m->SetAsynchronous(TRUE);
                assert(0);
                //SendMessageDelayed(m, ViewConfiguration::GetKeyRepeatDelay());
            }
            break;
        }
    }
    return NOERROR;
}

CARAPI ViewRootImpl::SyntheticJoystickHandler::Process(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetActionMasked(&action);
    switch(action) {
        case IMotionEvent::ACTION_CANCEL:
            Cancel(event);
            break;
        case IMotionEvent::ACTION_MOVE:
            Update(event, TRUE);
            break;
        //Log.w(TAG, "Unexpected action: " + event.getActionMasked());
    }
    return NOERROR;
}

void ViewRootImpl::SyntheticJoystickHandler::Cancel(
    /* [in] */ IMotionEvent* event)
{
    RemoveMessages(MSG_ENQUEUE_X_AXIS_KEY_REPEAT);
    RemoveMessages(MSG_ENQUEUE_Y_AXIS_KEY_REPEAT);
    Update(event, FALSE);
}

void ViewRootImpl::SyntheticJoystickHandler::Update(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Boolean synthesizeNewKeys)
{
    Int64 time = 0;
    IInputEvent::Probe(event)->GetEventTime(&time);
    Int32 metaState, deviceId,  source;
    event->GetMetaState(&metaState);
    IInputEvent::Probe(event)->GetDeviceId(&deviceId);
    IInputEvent::Probe(event)->GetSource(&source);

    Float axisValue;
    event->GetAxisValue(IMotionEvent::AXIS_HAT_X, &axisValue);
    Int32 xDirection = JoystickAxisValueToDirection(axisValue);
    if (xDirection == 0) {
        Float x;
        event->GetX(&x);
        xDirection = JoystickAxisValueToDirection(x);
    }

    event->GetAxisValue(IMotionEvent::AXIS_HAT_Y, &axisValue);
    Int32 yDirection = JoystickAxisValueToDirection(axisValue);
    if (yDirection == 0) {
        Float y;
        event->GetY(&y);
        yDirection = JoystickAxisValueToDirection(y);
    }

    if (xDirection != mLastXDirection) {
        if (mLastXKeyCode != 0) {
            RemoveMessages(MSG_ENQUEUE_X_AXIS_KEY_REPEAT);
            AutoPtr<IKeyEvent> temp;
            assert(0);
            /*CKeyEvent::New(time, time,
                    IKeyEvent::ACTION_UP, mLastXKeyCode, 0, metaState,
                    deviceId, 0, IKeyEvent::FLAG_FALLBACK, source, (IKeyEvent**)&temp);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(temp));
            mLastXKeyCode = 0;
        }

        mLastXDirection = xDirection;

        if (xDirection != 0 && synthesizeNewKeys) {
            mLastXKeyCode = xDirection > 0
                    ? IKeyEvent::KEYCODE_DPAD_RIGHT : IKeyEvent::KEYCODE_DPAD_LEFT;
            AutoPtr<IKeyEvent> e;
            assert(0);
            /*CKeyEvent::New(time, time,
                    IKeyEvent::ACTION_DOWN, mLastXKeyCode, 0, metaState,
                    deviceId, 0, IKeyEvent::FLAG_FALLBACK, source, (IKeyEvent**)&temp);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(e));
            AutoPtr<IMessage> m;
            ObtainMessage(MSG_ENQUEUE_X_AXIS_KEY_REPEAT, e, (IMessage**)&m);
            m->SetAsynchronous(TRUE);
            assert(0);
            //SendMessageDelayed(m, ViewConfiguration::GetKeyRepeatTimeout());
        }
    }

    if (yDirection != mLastYDirection) {
        if (mLastYKeyCode != 0) {
            RemoveMessages(MSG_ENQUEUE_Y_AXIS_KEY_REPEAT);
            AutoPtr<IKeyEvent> temp;
            assert(0);
            /*CKeyEvent::New(time, time,
                    IKeyEvent::ACTION_UP, mLastXKeyCode, 0, metaState,
                    deviceId, 0, IKeyEvent::FLAG_FALLBACK, source, (IKeyEvent**)&temp);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(temp));
            mLastYKeyCode = 0;
        }

        mLastYDirection = yDirection;

        if (yDirection != 0 && synthesizeNewKeys) {
            mLastYKeyCode = yDirection > 0
                    ? IKeyEvent::KEYCODE_DPAD_DOWN : IKeyEvent::KEYCODE_DPAD_UP;
            AutoPtr<IKeyEvent> e;
            assert(0);
            /*CKeyEvent::New(time, time,
                    IKeyEvent::ACTION_DOWN, mLastXKeyCode, 0, metaState,
                    deviceId, 0, IKeyEvent::FLAG_FALLBACK, source, (IKeyEvent**)&e);*/
            mHost->EnqueueInputEvent(IInputEvent::Probe(e));
            AutoPtr<IMessage> m;
            ObtainMessage(MSG_ENQUEUE_Y_AXIS_KEY_REPEAT, e, (IMessage**)&m);
            m->SetAsynchronous(TRUE);
            assert(0);
            //SendMessageDelayed(m, ViewConfiguration::GetKeyRepeatTimeout());
        }
    }
}

Int32 ViewRootImpl::SyntheticJoystickHandler::JoystickAxisValueToDirection(
    /* [in] */ Float value)
{
    if (value >= 0.5f) {
        return 1;
    } else if (value <= -0.5f) {
        return -1;
    } else {
        return 0;
    }
}

////////////////////////////////////////////////////////////
//      ViewRootImpl::SyntheticTouchNavigationHandler
////////////////////////////////////////////////////////////
const String ViewRootImpl::SyntheticTouchNavigationHandler::LOCAL_TAG("SyntheticTouchNavigationHandler");
const Boolean ViewRootImpl::SyntheticTouchNavigationHandler::LOCAL_DEBUG = FALSE;
const Float ViewRootImpl::SyntheticTouchNavigationHandler::DEFAULT_WIDTH_MILLIMETERS = 48;
const Float ViewRootImpl::SyntheticTouchNavigationHandler::DEFAULT_HEIGHT_MILLIMETERS = 48;
const Int32 ViewRootImpl::SyntheticTouchNavigationHandler::TICK_DISTANCE_MILLIMETERS = 12;
const Float ViewRootImpl::SyntheticTouchNavigationHandler::MIN_FLING_VELOCITY_TICKS_PER_SECOND = 6.0f;
const Float ViewRootImpl::SyntheticTouchNavigationHandler::MAX_FLING_VELOCITY_TICKS_PER_SECOND = 20.0f;
const Float ViewRootImpl::SyntheticTouchNavigationHandler::FLING_TICK_DECAY = 0.8f;

ViewRootImpl::SyntheticTouchNavigationHandler::SyntheticTouchNavigationHandler(
    /* [in] */ ViewRootImpl* host)
    : Handler(TRUE)
    , mCurrentDeviceId(-1)
    , mCurrentSource(0)
    , mCurrentDeviceSupported(FALSE)
    , mConfigTickDistance(0)
    , mConfigMinFlingVelocity(0)
    , mConfigMaxFlingVelocity(0)
    , mActivePointerId(-1)
    , mStartX(0)
    , mStartY(0)
    , mLastX(0)
    , mLastY(0)
    , mAccumulatedX(0)
    , mAccumulatedY(0)
    , mConsumedMovement(FALSE)
    , mPendingKeyDownTime(0)
    , mPendingKeyCode(IKeyEvent::KEYCODE_UNKNOWN)
    , mPendingKeyRepeatCount(0)
    , mPendingKeyMetaState(0)
    , mFlinging(FALSE)
    , mFlingVelocity(0)
    , mHost(host)
{
    mFlingRunnable = new NavigationRun(this);
}

ECode ViewRootImpl::SyntheticTouchNavigationHandler::Process(
    /* [in] */ IMotionEvent* event)
{
    // Update the current device information.
    Int64 time = 0;
    IInputEvent::Probe(event)->GetEventTime(&time);
    Int32 deviceId, source;
    if (mCurrentDeviceId != (IInputEvent::Probe(event)->GetDeviceId(&deviceId), deviceId)
        || mCurrentSource != (IInputEvent::Probe(event)->GetSource(&source), source)) {
        FinishKeys(time);
        FinishTracking(time);
        mCurrentDeviceId = deviceId;
        mCurrentSource = source;
        mCurrentDeviceSupported = FALSE;
        AutoPtr<IInputDevice> device;
        IInputEvent::Probe(event)->GetDevice((IInputDevice**)&device);
        if (device != NULL) {
            // In order to support an input device, we must know certain
            // characteristics about it, such as its size and resolution.
            AutoPtr<IMotionRange> xRange, yRange;
            device->GetMotionRange(IMotionEvent::AXIS_X, (IMotionRange**)&xRange);
            device->GetMotionRange(IMotionEvent::AXIS_Y, (IMotionRange**)&yRange);
            if (xRange != NULL && yRange != NULL) {
                mCurrentDeviceSupported = TRUE;

                // Infer the resolution if it not actually known.
                Float xRes, tempF;
                xRange->GetResolution(&xRes);
                if (xRes <= 0) {
                    xRange->GetRange(&tempF);
                    xRes = tempF / DEFAULT_WIDTH_MILLIMETERS;
                }
                Float yRes;
                yRange->GetResolution(&yRes);
                if (yRes <= 0) {
                    yRange->GetRange(&tempF);
                    yRes = tempF / DEFAULT_HEIGHT_MILLIMETERS;
                }
                Float nominalRes = (xRes + yRes) * 0.5f;

                // Precompute all of the configuration thresholds we will need.
                mConfigTickDistance = TICK_DISTANCE_MILLIMETERS * nominalRes;
                mConfigMinFlingVelocity =
                        MIN_FLING_VELOCITY_TICKS_PER_SECOND * mConfigTickDistance;
                mConfigMaxFlingVelocity =
                        MAX_FLING_VELOCITY_TICKS_PER_SECOND * mConfigTickDistance;
            }
        }
    }
    if (!mCurrentDeviceSupported) {
        return NOERROR;
    }

    // Handle the event.
    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
        {
            Boolean caughtFling = mFlinging;
            FinishKeys(time);
            FinishTracking(time);
            event->GetPointerId(0, &mActivePointerId);
            assert(0);
            //mVelocityTracker = VelocityTracker::Obtain();
            mVelocityTracker->AddMovement(event);
            event->GetX(&mStartX);
            event->GetY(&mStartY);
            mLastX = mStartX;
            mLastY = mStartY;
            mAccumulatedX = 0;
            mAccumulatedY = 0;

            // If we caught a fling, then pretend that the tap slop has already
            // been exceeded to suppress taps whose only purpose is to stop the fling.
            mConsumedMovement = caughtFling;
            break;
        }

        case IMotionEvent::ACTION_MOVE:
        case IMotionEvent::ACTION_UP:
        {
            if (mActivePointerId < 0) {
                break;
            }
            Int32 index = 0;
            event->FindPointerIndex(mActivePointerId, &index);
            if (index < 0) {
                FinishKeys(time);
                FinishTracking(time);
                break;
            }

            mVelocityTracker->AddMovement(event);
            Float x , y;
            event->GetX(index, &x);
            event->GetY(index, &y);
            mAccumulatedX += x - mLastX;
            mAccumulatedY += y - mLastY;
            mLastX = x;
            mLastY = y;

            // Consume any accumulated movement so far.
            Int32 metaState = 0;
            event->GetMetaState(&metaState);
            ConsumeAccumulatedMovement(time, metaState);

            // Detect taps and flings.
            if (action == IMotionEvent::ACTION_UP) {
                if (mConsumedMovement && mPendingKeyCode != IKeyEvent::KEYCODE_UNKNOWN) {
                    // It might be a fling.
                    mVelocityTracker->ComputeCurrentVelocity(1000, mConfigMaxFlingVelocity);
                    float vx, vy;
                    mVelocityTracker->GetXVelocity(mActivePointerId, &vx);
                    mVelocityTracker->GetYVelocity(mActivePointerId, &vy);
                    if (!StartFling(time, vx, vy)) {
                        FinishKeys(time);
                    }
                }
                FinishTracking(time);
            }
            break;
        }

        case IMotionEvent::ACTION_CANCEL:
        {
            FinishKeys(time);
            FinishTracking(time);
            break;
        }
    }
    return NOERROR;
}

ECode ViewRootImpl::SyntheticTouchNavigationHandler::Cancel(
    /* [in] */ IMotionEvent* event)
{
    Int32 id, source;
    IInputEvent::Probe(event)->GetDeviceId(&id);
    IInputEvent::Probe(event)->GetSource(&source);
    if (mCurrentDeviceId == id && mCurrentSource == source) {
        Int64 time = 0;
        IInputEvent::Probe(event)->GetEventTime(&time);
        FinishKeys(time);
        FinishTracking(time);
    }
    return NOERROR;
}

void ViewRootImpl::SyntheticTouchNavigationHandler::FinishKeys(
    /* [in] */ Int64 time)
{
    CancelFling();
    SendKeyUp(time);
}

void ViewRootImpl::SyntheticTouchNavigationHandler::FinishTracking(
    /* [in] */ Int64 time)
{
    if (mActivePointerId >= 0) {
        mActivePointerId = -1;
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

void ViewRootImpl::SyntheticTouchNavigationHandler::ConsumeAccumulatedMovement(
    /* [in] */ Int64 time,
    /* [in] */ Int32 metaState)
{
    Float absX = Elastos::Core::Math::Abs(mAccumulatedX);
    Float absY = Elastos::Core::Math::Abs(mAccumulatedY);
    if (absX >= absY) {
        if (absX >= mConfigTickDistance) {
            mAccumulatedX = ConsumeAccumulatedMovement(time, metaState, mAccumulatedX,
                    IKeyEvent::KEYCODE_DPAD_LEFT, IKeyEvent::KEYCODE_DPAD_RIGHT);
            mAccumulatedY = 0;
            mConsumedMovement = true;
        }
    } else {
        if (absY >= mConfigTickDistance) {
            mAccumulatedY = ConsumeAccumulatedMovement(time, metaState, mAccumulatedY,
                    IKeyEvent::KEYCODE_DPAD_UP, IKeyEvent::KEYCODE_DPAD_DOWN);
            mAccumulatedX = 0;
            mConsumedMovement = true;
        }
    }
}

Float ViewRootImpl::SyntheticTouchNavigationHandler::ConsumeAccumulatedMovement(
    /* [in] */ Int64 time,
    /* [in] */ Int32 metaState,
    /* [in] */ Float accumulator,
    /* [in] */ Int32 negativeKeyCode,
    /* [in] */ Int32 positiveKeyCode)
{
    while (accumulator <= -mConfigTickDistance) {
        SendKeyDownOrRepeat(time, negativeKeyCode, metaState);
        accumulator += mConfigTickDistance;
    }
    while (accumulator >= mConfigTickDistance) {
        SendKeyDownOrRepeat(time, positiveKeyCode, metaState);
        accumulator -= mConfigTickDistance;
    }
    return accumulator;
}

void ViewRootImpl::SyntheticTouchNavigationHandler::SendKeyDownOrRepeat(
    /* [in] */ Int64 time,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 metaState)
{
    if (mPendingKeyCode != keyCode) {
        SendKeyUp(time);
        mPendingKeyDownTime = time;
        mPendingKeyCode = keyCode;
        mPendingKeyRepeatCount = 0;
    } else {
        mPendingKeyRepeatCount += 1;
    }
    mPendingKeyMetaState = metaState;

    // Note: Normally we would pass FLAG_LONG_PRESS when the repeat count is 1
    // but it doesn't quite make sense when simulating the events in this way.
    AutoPtr<IKeyEvent> temp;
    assert(0);
    /*CKeyEvent::New(mPendingKeyDownTime, time,
            IKeyEvent::ACTION_DOWN, mPendingKeyCode, mPendingKeyRepeatCount,
            mPendingKeyMetaState, mCurrentDeviceId,
            IKeyEvent::FLAG_FALLBACK, mCurrentSource, (IKeyEvent**)&temp);*/
    mHost->EnqueueInputEvent(IInputEvent::Probe(temp));
}

void ViewRootImpl::SyntheticTouchNavigationHandler::SendKeyUp(
    /* [in] */ Int64 time)
{
    if (mPendingKeyCode != IKeyEvent::KEYCODE_UNKNOWN) {

        AutoPtr<IKeyEvent> temp;
        assert(0);
        /*CKeyEvent::New(mPendingKeyDownTime, time,
                IKeyEvent::ACTION_UP, mPendingKeyCode, 0,
                mPendingKeyMetaState, mCurrentDeviceId,
                0, IKeyEvent::FLAG_FALLBACK, mCurrentSource, (IKeyEvent**)&temp);*/

        mHost->EnqueueInputEvent(IInputEvent::Probe(temp));
        mPendingKeyCode = IKeyEvent::KEYCODE_UNKNOWN;
    }
}

Boolean ViewRootImpl::SyntheticTouchNavigationHandler::StartFling(
    /* [in] */ Int64 time,
    /* [in] */ Float vx,
    /* [in] */ Float vy)
{
    /*if (LOCAL_DEBUG) {
        Log.d(LOCAL_TAG, "Considering fling: vx=" + vx + ", vy=" + vy
                + ", min=" + mConfigMinFlingVelocity);
    }*/

    // Flings must be oriented in the same direction as the preceding movements.
    switch (mPendingKeyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
            if (-vx >= mConfigMinFlingVelocity
                    && Elastos::Core::Math::Abs(vy) < mConfigMinFlingVelocity) {
                mFlingVelocity = -vx;
                break;
            }
            return FALSE;

        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            if (vx >= mConfigMinFlingVelocity
                    && Elastos::Core::Math::Abs(vy) < mConfigMinFlingVelocity) {
                mFlingVelocity = vx;
                break;
            }
            return FALSE;

        case IKeyEvent::KEYCODE_DPAD_UP:
            if (-vy >= mConfigMinFlingVelocity
                    && Elastos::Core::Math::Abs(vx) < mConfigMinFlingVelocity) {
                mFlingVelocity = -vy;
                break;
            }
            return FALSE;

        case IKeyEvent::KEYCODE_DPAD_DOWN:
            if (vy >= mConfigMinFlingVelocity
                    && Elastos::Core::Math::Abs(vx) < mConfigMinFlingVelocity) {
                mFlingVelocity = vy;
                break;
            }
            return FALSE;
    }

    // Post the first fling event.
    mFlinging = PostFling(time);
    return mFlinging;
}

Boolean ViewRootImpl::SyntheticTouchNavigationHandler::PostFling(
    /* [in] */ Int64 time)
{
    // The idea here is to estimate the time when the pointer would have
    // traveled one tick distance unit given the current fling velocity.
    // This effect creates continuity of motion.
    if (mFlingVelocity >= mConfigMinFlingVelocity) {
        Int64 delay = (Int64)(mConfigTickDistance / mFlingVelocity * 1000);
        Boolean res;
        PostAtTime(mFlingRunnable, time + delay, &res);
        return TRUE;
    }
    return FALSE;
}

void ViewRootImpl::SyntheticTouchNavigationHandler::CancelFling()
{
    if (mFlinging) {
        RemoveCallbacks(mFlingRunnable);
        mFlinging = FALSE;
    }
}

////////////////////////////////////////////////////////////////////
//  ViewRootImpl::SyntheticTouchNavigationHandler::NavigationRun
////////////////////////////////////////////////////////////////////

ViewRootImpl::SyntheticTouchNavigationHandler::NavigationRun::NavigationRun(
    /* [in] */ SyntheticTouchNavigationHandler* host)
    : mHost(host)
{}

ECode ViewRootImpl::SyntheticTouchNavigationHandler::NavigationRun::Run()
{
    Int64 time = SystemClock::GetUptimeMillis();
    mHost->SendKeyDownOrRepeat(time, mHost->mPendingKeyCode, mHost->mPendingKeyMetaState);
    mHost->mFlingVelocity *= FLING_TICK_DECAY;
    if (!mHost->PostFling(time)) {
        mHost->mFlinging = FALSE;
        mHost->FinishKeys(time);
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////////////
//          ViewRootImpl::SyntheticKeyboardHandler
////////////////////////////////////////////////////////////////////

ViewRootImpl::SyntheticKeyboardHandler::SyntheticKeyboardHandler()
{

}

ECode ViewRootImpl::SyntheticKeyboardHandler::Process(
    /* [in] */ IKeyEvent* event)
{
    Int32 flag;
    event->GetFlags(&flag);
    if ((flag & IKeyEvent::FLAG_FALLBACK) != 0) {
        return NOERROR;
    }

    AutoPtr<IKeyCharacterMap> kcm;
    event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
    Int32 keyCode, metaState;
    event->GetKeyCode(&keyCode);
    event->GetMetaState(&metaState);

    // Check for fallback actions specified by the key character map.
    AutoPtr<IFallbackAction> fallbackAction;
    kcm->GetFallbackAction(keyCode, metaState, (IFallbackAction**)&fallbackAction);
    if (fallbackAction != NULL) {
        Int32 flags = flag | IKeyEvent::FLAG_FALLBACK;
        Int64 downTime, eventTime;
        Int32 action, repeatCount, eviceId, scanCode, source;
        event->GetDownTime(&downTime);
        IInputEvent::Probe(event)->GetEventTime(&eventTime);
        event->GetAction(&action);
        event->GetRepeatCount(&repeatCount);
        IInputEvent::Probe(event)->GetDeviceId(&eviceId);
        event->GetScanCode(&scanCode);
        IInputEvent::Probe(event)->GetSource(&source);
        assert(0);
        /*AutoPtr<IKeyEvent> fallbackEvent = CKeyEvent::Obtain(
                downTime, eventTime,
                action, ((FallbackAction*)fallbackAction)->mKeyCode,
                repeatCount, ((FallbackAction*)fallbackAction)->mMetaState,
                eviceId, scanCode,
                flags, source, NULL);
        fallbackAction->Recycle();
        mHost->EnqueueInputEvent(fallbackEvent);*/
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////////////
//          ViewRootImpl::PropertiesRunnable
////////////////////////////////////////////////////////////////////
ViewRootImpl::PropertiesRunnable::PropertiesRunnable(
    /* [in] */ ViewRootImpl* host)
    : mHost(host)
{
}

ECode ViewRootImpl::PropertiesRunnable::Run()
{
    // Profiling
    SystemProperties::GetBoolean(mHost->PROPERTY_PROFILE_RENDERING, FALSE, &mHost->mProfileRendering);
    mHost->ProfileRendering(mHost->mAttachInfo->mHasWindowFocus);

    // Media (used by sound effects)
    SystemProperties::GetBoolean(PROPERTY_MEDIA_DISABLED, FALSE, &mHost->mMediaDisabled);

    // Hardware rendering
    if (mHost->mAttachInfo->mHardwareRenderer != NULL) {
        Boolean res;
        mHost->mAttachInfo->mHardwareRenderer->LoadSystemProperties(&res);
        if (res) {
            mHost->Invalidate();
        }
    }

    // Layout debugging
    Boolean layout;
    SystemProperties::GetBoolean(IView::DEBUG_LAYOUT_PROPERTY, FALSE, &layout);
    if (layout != mHost->mAttachInfo->mDebugLayout) {
        mHost->mAttachInfo->mDebugLayout = layout;
        Boolean hasMessages;
        mHost->mHandler->HasMessages(MSG_INVALIDATE_WORLD, &hasMessages);
        if (!hasMessages) {
            Boolean temp;
            mHost->mHandler->SendEmptyMessageDelayed(MSG_INVALIDATE_WORLD, 200, &temp);
        }
    }

    // detect emulator
    assert(0);
    //mHost->mIsEmulator = Build::HARDWARE::Contains(String("goldfish"));
    SystemProperties::GetBoolean(PROPERTY_EMULATOR_CIRCULAR, FALSE, &mHost->mIsCircularEmulator);
    return NOERROR;
}

////////////////////////////////////////////////////////////////////
//          ViewRootImpl::QueuedInputEvent
////////////////////////////////////////////////////////////////////
const Int32 ViewRootImpl::QueuedInputEvent::FLAG_DELIVER_POST_IME = 1;
const Int32 ViewRootImpl::QueuedInputEvent::FLAG_DEFERRED = 1 << 1;
const Int32 ViewRootImpl::QueuedInputEvent::FLAG_FINISHED = 1 << 2;
const Int32 ViewRootImpl::QueuedInputEvent::FLAG_FINISHED_HANDLED = 1 << 3;
const Int32 ViewRootImpl::QueuedInputEvent::FLAG_RESYNTHESIZED = 1 << 4;
const Int32 ViewRootImpl::QueuedInputEvent::FLAG_UNHANDLED = 1 << 5;

ViewRootImpl::QueuedInputEvent::QueuedInputEvent()
    : mFlags(0)
{}

Boolean ViewRootImpl::QueuedInputEvent::ShouldSkipIme()
{
    if ((mFlags & FLAG_DELIVER_POST_IME) != 0) {
        return TRUE;
    }
    Boolean res = IMotionEvent::Probe(mEvent) ? TRUE : FALSE;
    Boolean source;
    mEvent->IsFromSource(IInputDevice::SOURCE_CLASS_POINTER, &source);
    return res && source;
}

Boolean ViewRootImpl::QueuedInputEvent::ShouldSendToSynthesizer()
{
    if ((mFlags & FLAG_UNHANDLED) != 0) {
        return TRUE;
    }

    return FALSE;
}

String ViewRootImpl::QueuedInputEvent::ToString()
{
    AutoPtr<StringBuilder> sb = new StringBuilder(String("QueuedInputEvent{flags="));
    Boolean hasPrevious = FALSE;
    hasPrevious = FlagToString(String("DELIVER_POST_IME"), FLAG_DELIVER_POST_IME, hasPrevious, IStringBuilder::Probe(sb));
    hasPrevious = FlagToString(String("DEFERRED"), FLAG_DEFERRED, hasPrevious, IStringBuilder::Probe(sb));
    hasPrevious = FlagToString(String("FINISHED"), FLAG_FINISHED, hasPrevious, IStringBuilder::Probe(sb));
    hasPrevious = FlagToString(String("FINISHED_HANDLED"), FLAG_FINISHED_HANDLED, hasPrevious, IStringBuilder::Probe(sb));
    hasPrevious = FlagToString(String("RESYNTHESIZED"), FLAG_RESYNTHESIZED, hasPrevious, IStringBuilder::Probe(sb));
    hasPrevious = FlagToString(String("UNHANDLED"), FLAG_UNHANDLED, hasPrevious, IStringBuilder::Probe(sb));
    if (!hasPrevious) {
        sb->Append(String("0"));
    }
    String str1, str2, str3;
    if (mEvent != NULL) {
        str1 = String(", hasNextQueuedEvent=") + String("TRUE");
    } else {
        str1 = String(", hasNextQueuedEvent=") + String("FALSE");
    }

    if (mReceiver != NULL) {
        str2 = String(", hasInputEventReceiver=") + String("TRUE");
    } else {
        str2 = String(", hasInputEventReceiver=") + String("FALSE");
    }

    str3 = String(", mEvent=") + String("}");
    sb->Append(str1);
    sb->Append(str2);
    sb->Append(str3);
    return sb->ToString();
}

Boolean ViewRootImpl::QueuedInputEvent::FlagToString(
    /* [in] */ const String& name,
    /* [in] */ Int32 flag,
    /* [in] */ Boolean hasPrevious,
    /* [in] */ IStringBuilder* sb)
{
    if ((mFlags & flag) != 0) {
        if (hasPrevious) {
            sb->Append(String("|"));
        }
        sb->Append(name);
        return TRUE;
    }
    return hasPrevious;
}

////////////////////////////////////////////////////////////////////
//          ViewRootImpl::ConsumeBatchedInputImmediatelyRunnable
////////////////////////////////////////////////////////////////////

ViewRootImpl::ConsumeBatchedInputImmediatelyRunnable::ConsumeBatchedInputImmediatelyRunnable(
    /* [in] */ ViewRootImpl* host)
    : mHost(host)
{}

ECode ViewRootImpl::ConsumeBatchedInputImmediatelyRunnable::Run()
{
    mHost->DoConsumeBatchedInput(-1);
    return NOERROR;
}

////////////////////////////////////////////////////////////////////
//          ViewRootImpl::HighContrastTextManager
////////////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ViewRootImpl::HighContrastTextManager, Object, IAccessibilityManagerHighTextContrastChangeListener)

ViewRootImpl::HighContrastTextManager::HighContrastTextManager(
    /* [in] */ ViewRootImpl* host)
    : mHost(host)
{
    mHost->mAccessibilityManager->IsHighTextContrastEnabled(&mHost->mAttachInfo->mHighContrastText);
}


ECode ViewRootImpl::HighContrastTextManager::OnHighTextContrastStateChanged(
    /* [in] */ Boolean enabled)
{
    mHost->mAttachInfo->mHighContrastText = enabled;

    // Destroy Displaylists so they can be recreated with high contrast recordings
    mHost->DestroyHardwareResources();

    // Schedule redraw, which will rerecord + redraw all text
    mHost->Invalidate();
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
