
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewRootImpl.h"
//#include "elastos/droid/view/CInputBindResult.h"
#include "elastos/droid/view/inputmethod/CEditorInfo.h"
#include "elastos/droid/view/inputmethod/CBaseInputConnection.h"
#include "elastos/droid/view/inputmethod/CIInputMethodClient.h"
#include "elastos/droid/view/inputmethod/CControlledInputConnectionWrapper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::View;
using Elastos::Droid::View::ViewRootImpl;
//using Elastos::Droid::View::CInputBindResult;
using Elastos::Droid::View::InputMethod::CControlledInputConnectionWrapper;

using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

const Boolean CInputMethodManager::DEBUG = FALSE;
const String CInputMethodManager::TAG("CInputMethodManager");

const String CInputMethodManager::PENDING_EVENT_COUNTER("aq:imm");
AutoPtr<IInputMethodManager> CInputMethodManager::sInstance;
const Int64 CInputMethodManager::INPUT_METHOD_NOT_RESPONDING_TIMEOUT = 2500;

const Int32 CInputMethodManager::DISPATCH_IN_PROGRESS = -1;

const Int32 CInputMethodManager::DISPATCH_NOT_HANDLED = 0;

const Int32 CInputMethodManager::DISPATCH_HANDLED = 1;

const Int32 CInputMethodManager::MSG_DUMP = 1;
const Int32 CInputMethodManager::MSG_BIND = 2;
const Int32 CInputMethodManager::MSG_UNBIND = 3;
const Int32 CInputMethodManager::MSG_SET_ACTIVE = 4;
const Int32 CInputMethodManager::MSG_SEND_INPUT_EVENT = 5;
const Int32 CInputMethodManager::MSG_TIMEOUT_INPUT_EVENT = 6;
const Int32 CInputMethodManager::MSG_FLUSH_INPUT_EVENT = 7;
const Int32 CInputMethodManager::MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER = 9;

const Int32 CInputMethodManager::REQUEST_UPDATE_CURSOR_ANCHOR_INFO_NONE = 0x0;

const Int32 CInputMethodManager::NOT_AN_ACTION_NOTIFICATION_SEQUENCE_NUMBER = -1;

//========================================================================================
//              CInputMethodManager::MyHandler::
//========================================================================================
ECode CInputMethodManager::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CInputMethodManager::MSG_DUMP:
            // SomeArgs args = (SomeArgs)msg.obj;
            // try {
            //     doDump((FileDescriptor)args.arg1,
            //             (PrintWriter)args.arg2, (String[])args.arg3);
            // } catch (RuntimeException e) {
            //     ((PrintWriter)args.arg2).println("Exception: " + e);
            // }
            // {    AutoLock syncLock(args.arg4);
            //     ((CountDownLatch)args.arg4).countDown();
            // }
            // args.recycle();
            break;
        case CInputMethodManager::MSG_BIND: {
            IInputBindResult* result = IInputBindResult::Probe(obj);
            mHost->HandleBind(result);
            break;
        }
        case CInputMethodManager::MSG_UNBIND: {
            mHost->HandleUnBind(arg1);
            break;
        }
        case CInputMethodManager::MSG_SET_ACTIVE: {
            mHost->HandleSetActive(arg1 != 0);
            break;
        }
        case CInputMethodManager::MSG_SEND_INPUT_EVENT: {
            PendingEvent* pe = (PendingEvent*)IRunnable::Probe(obj);
            mHost->SendInputEventAndReportResultOnMainLooper(pe);
            break;
        }
        case CInputMethodManager::MSG_TIMEOUT_INPUT_EVENT: {
            mHost->FinishedInputEvent(arg1, FALSE, TRUE);
            break;
        }
        case CInputMethodManager::MSG_FLUSH_INPUT_EVENT: {
            mHost->FinishedInputEvent(arg1, FALSE, FALSE);
            break;
        }
        case CInputMethodManager::MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER: {
            {
                AutoLock lock(mHost->mHLock);
                mHost->mNextUserActionNotificationSequenceNumber = arg1;
            }
        }
    }

    return NOERROR;
}

//========================================================================================
//              CInputMethodManager::StartInputRunnable::
//========================================================================================
ECode CInputMethodManager::StartInputRunnable::Run()
{
    mHost->StartInputInner(NULL, 0, 0, 0);
    return NOERROR;
}

//========================================================================================
//              CInputMethodManager::ImeInputEventSender::
//========================================================================================
CInputMethodManager::ImeInputEventSender::ImeInputEventSender(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ CInputMethodManager* h) : mHost(h) // : InputEventSender(inputChannel, looper)
{
}

ECode CInputMethodManager::ImeInputEventSender::OnInputEventFinished(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    mHost->FinishedInputEvent(seq, handled, FALSE);
    return NOERROR;
}

//========================================================================================
//              CInputMethodManager::PendingEvent::
//========================================================================================
CInputMethodManager::PendingEvent::PendingEvent(
    /* [in] */ CInputMethodManager* host) : mHost(host)
{
}

void CInputMethodManager::PendingEvent::Recycle()
{
    mEvent = NULL;
    mToken = NULL;
    mInputMethodId = NULL;
    mCallback = NULL;
    mHandler = NULL;
    mHandled = FALSE;
}

ECode CInputMethodManager::PendingEvent::Run()
{
    mCallback->OnFinishedInputEvent(mToken, mHandled);

    {
        AutoLock lock(mHost->mHLock);
        mHost->RecyclePendingEventLocked(this);
    }
    return NOERROR;
}

//========================================================================================
//              CInputMethodManager::
//========================================================================================
CAR_INTERFACE_IMPL(CInputMethodManager, Object, IInputMethodManager)

CAR_OBJECT_IMPL(CInputMethodManager)

CInputMethodManager::CInputMethodManager()
    : mActive(FALSE)
    , mHasBeenInactive(TRUE)
    , mFullscreenMode(FALSE)
    , mServedConnecting(FALSE)
    , mCursorSelStart(0)
    , mCursorSelEnd(0)
    , mCursorCandStart(0)
    , mCursorCandEnd(0)
    , mBindSequence(-1)
    , mNextUserActionNotificationSequenceNumber(NOT_AN_ACTION_NOTIFICATION_SEQUENCE_NUMBER)
    , mLastSentUserActionNotificationSequenceNumber(NOT_AN_ACTION_NOTIFICATION_SEQUENCE_NUMBER)
    , mRequestUpdateCursorAnchorInfoMonitorMode(REQUEST_UPDATE_CURSOR_ANCHOR_INFO_NONE)
{
    CRect::New((IRect**)&mTmpCursorRect);
    CRect::New((IRect**)&mCursorRect);
    CIInputMethodClient::New(this, (IInputMethodClient**)&mClient);

//    mPendingEventPool = new Pools::SimplePool<PendingEvent>(20);
    CSparseArray::New(20, (ISparseArray**)&mPendingEvents);

    mViewTopLeft = ArrayOf<Int32>::Alloc(2);

    CMatrix::New((IMatrix**)&mViewToScreenMatrix);

    CBaseInputConnection::New(this, FALSE, (IInputConnection**)&mDummyInputConnection);
}

ECode CInputMethodManager::constructor(
    /* [in] */ IIInputMethodManager* service,
    /* [in] */ ILooper* looper)
{
    mService = service;
    mMainLooper = looper;
    mH = new MyHandler(looper, this);
    CControlledInputConnectionWrapper::New(looper, mDummyInputConnection,
        this, (IIInputContext**)&mIInputContext);

    return NOERROR;
}

AutoPtr<IInputMethodManager> CInputMethodManager::GetInstance()
{
//     AutoLock lock(CInputMethodManager.class);

    if (sInstance == NULL) {
        AutoPtr<IIInputMethodManager> service =
            IIInputMethodManager::Probe(ServiceManager::GetService(IContext::INPUT_METHOD_SERVICE));
        ASSERT_SUCCEEDED(CInputMethodManager::New(service, Looper::GetMainLooper(), (IInputMethodManager**)&sInstance));
    }
    return sInstance;
}

AutoPtr<IInputMethodManager> CInputMethodManager::PeekInstance()
{
    return sInstance;
}

ECode CInputMethodManager::GetClient(
    /* [out] */ IInputMethodClient** client)
{
    VALIDATE_NOT_NULL(client);
    *client = mClient;
    REFCOUNT_ADD(*client);
    return NOERROR;
}

ECode CInputMethodManager::GetInputContext(
    /* [out] */ IIInputContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mIInputContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CInputMethodManager::GetInputMethodList(
    /* [in] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    // try {
    return mService->GetInputMethodList(infos);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::GetEnabledInputMethodList(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    // try {
    return mService->GetEnabledInputMethodList(list);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::GetEnabledInputMethodSubtypeList(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    // try {
    AutoPtr<IInterface> p;
    if (imi != NULL) {
        String id;
        imi->GetId(&id);
        AutoPtr<ICharSequence> cs;
        CString::New(id, (ICharSequence**)&cs);
        p = cs;
    }
    assert(0 && "TODO"); // internal IIInputMethodManager method define wrong.
//    return mService->GetEnabledInputMethodSubtypeList(p, allowsImplicitlySelectedSubtypes, infos);
    return NOERROR;
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::ShowStatusIcon(
    /* [in] */ IBinder* imeToken,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 iconId)
{
    // try {
    return mService->UpdateStatusIcon(imeToken, packageName, iconId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::HideStatusIcon(
    /* [in] */ IBinder* imeToken)
{
    VALIDATE_NOT_NULL(imeToken);
    // try {
    return mService->UpdateStatusIcon(imeToken, String(NULL), 0);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::SetImeWindowStatus(
    /* [in] */ IBinder* imeToken,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    //try {
    return mService->SetImeWindowStatus(imeToken, vis, backDisposition);
    //} catch (RemoteException e) {
    //    throw new RuntimeException(e);
    //}
}

ECode CInputMethodManager::SetFullscreenMode(
    /* [in] */ Boolean fullScreen)
{
    mFullscreenMode = fullScreen;
    return NOERROR;
}

ECode CInputMethodManager::RegisterSuggestionSpansForNotification(
    /* [in] */ ArrayOf<ISuggestionSpan*>* spans)
{
    //try {
    return mService->RegisterSuggestionSpansForNotification(spans);
    //} catch (RemoteException e) {
    //    throw new RuntimeException(e);
    //}
}

ECode CInputMethodManager::NotifySuggestionPicked(
    /* [in] */ ISuggestionSpan* span,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 index)
{
    //try {
    Boolean result = FALSE;
    return mService->NotifySuggestionPicked(span, originalString, index, &result);
    //} catch (RemoteException e) {
    //    throw new RuntimeException(e);
    //}
}

ECode CInputMethodManager::IsFullscreenMode(
    /* [out] */ Boolean* isfull)
{
    VALIDATE_NOT_NULL(isfull);
    *isfull = mFullscreenMode;
    return NOERROR;
}

ECode CInputMethodManager::IsActive(
    /* [in] */ IView* view,
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active);
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    Boolean check = FALSE;
    *active = (mServedView.Get() == view ||
            (mServedView != NULL && (mServedView->CheckInputConnectionProxy(view, &check), check))) &&
            mCurrentTextBoxAttribute != NULL;
    return NOERROR;
}

ECode CInputMethodManager::IsActive(
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active);
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    *active = mServedView != NULL && mCurrentTextBoxAttribute != NULL;
    return NOERROR;
}

ECode CInputMethodManager::IsAcceptingText(
    /* [out] */ Boolean* isAccepting)
{
    VALIDATE_NOT_NULL(isAccepting);
    FAIL_RETURN(CheckFocus());

    *isAccepting = mServedInputConnection != NULL;
    return NOERROR;
}

void CInputMethodManager::ClearBindingLocked()
{
    if (DEBUG) {
        Logger::V(TAG, "Clearing binding!");
    }
    ClearConnectionLocked();
    SetInputChannelLocked(NULL);
    mBindSequence = -1;
    mCurId = NULL;
    mCurMethod = NULL;
}

void CInputMethodManager::SetInputChannelLocked(
    /* [in] */ IInputChannel* channel)
{
    if (mCurChannel.Get() != channel) {
        if (mCurSender != NULL) {
            FlushPendingEventsLocked();
            mCurSender->Dispose();
            mCurSender = NULL;
        }
        if (mCurChannel != NULL) {
           mCurChannel->Dispose();
        }
        mCurChannel = channel;
    }
}

void CInputMethodManager::ClearConnectionLocked()
{
    mCurrentTextBoxAttribute = NULL;
    mServedInputConnection = NULL;
    if (mServedInputConnectionWrapper != NULL) {
        mServedInputConnectionWrapper->Deactivate();
        mServedInputConnectionWrapper = NULL;
    }
}

void CInputMethodManager::FinishInputLocked()
{
    mCurRootView = NULL;
    mNextServedView = NULL;
    if (mServedView != NULL) {
        if (DEBUG) Logger::V(TAG, "FINISH INPUT: %p", mServedView.Get());

        if (mCurrentTextBoxAttribute != NULL) {
            // try {
            mService->FinishInput(mClient);
            // } catch (RemoteException e) {
            // }
        }

        NotifyInputConnectionFinished();

        mServedView = NULL;
        mCompletions = NULL;
        mServedConnecting = FALSE;
        ClearConnectionLocked();
    }
}

void CInputMethodManager::NotifyInputConnectionFinished()
{
    if (mServedView != NULL && mServedInputConnection != NULL) {
        // We need to tell the previously served view that it is no
        // longer the input target, so it can reset its state.  Schedule
        // this call on its window's Handler so it will be on the correct
        // thread and outside of our lock.
        View* view = VIEW_PROBE(mServedView);
        AutoPtr<IViewRootImpl> viewRootImpl;
        view->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
        if (viewRootImpl != NULL) {
            // This will result in a call to reportFinishInputConnection() below.
            viewRootImpl->DispatchFinishInputConnection(mServedInputConnection);
        }
    }
}

ECode CInputMethodManager::ReportFinishInputConnection(
    /* [in] */ IInputConnection* ic)
{
    if (mServedInputConnection.Get() != ic) {
        Boolean ret = FALSE;
        ic->FinishComposingText(&ret);
        // To avoid modifying the public InputConnection interface
        if (IBaseInputConnection::Probe(ic) != NULL) {
            BaseInputConnection* baseConnection = (BaseInputConnection*)IBaseInputConnection::Probe(ic);
            baseConnection->ReportFinish();
        }
    }
    return NOERROR;
}

ECode CInputMethodManager::DisplayCompletions(
    /* [in] */ IView* view,
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    Boolean check = FALSE;
    if (mServedView.Get() != view && (mServedView == NULL ||
            (mServedView->CheckInputConnectionProxy(view, &check), !check))) {
        return NOERROR;
    }

    mCompletions = completions;
    if (mCurMethod != NULL) {
        // try {
        return mCurMethod->DisplayCompletions(mCompletions);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::UpdateExtractedText(
    /* [in] */ IView* view,
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    Boolean check = FALSE;
    if (mServedView.Get() != view && (mServedView == NULL ||
            (mServedView->CheckInputConnectionProxy(view, &check), !check))) {
        return NOERROR;
    }

    if (mCurMethod != NULL) {
        // try {
        return mCurMethod->UpdateExtractedText(token, text);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::ShowSoftInput(
    /* [in] */ IView* view,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* show)
{
    return ShowSoftInput(view, flags, NULL, show);
}

ECode CInputMethodManager::ShowSoftInput(
    /* [in] */ IView* view,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [out] */ Boolean* show)
{
    VALIDATE_NOT_NULL(show);
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    Boolean check = FALSE;
    //Logger::V(TAG, "CInputMethodManager::ShowSoftInputEx, view:%p, ServedView:%p", view, mServedView.Get());
    if (mServedView.Get() != view && (mServedView == NULL ||
            (mServedView->CheckInputConnectionProxy(view, &check), !check))) {
        *show = FALSE;
        return NOERROR;
    }
    // try {
    return mService->ShowSoftInput(mClient, flags, resultReceiver, show);
    // } catch (RemoteException e) {
    // }

    // return false;
}

ECode CInputMethodManager::ShowSoftInputUnchecked(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    // try {
    Boolean ret = FALSE;
    return mService->ShowSoftInput(mClient, flags, resultReceiver, &ret);
    // } catch (RemoteException e) {
    // }
}

ECode CInputMethodManager::HideSoftInputFromWindow(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* hide)
{
    return HideSoftInputFromWindow(windowToken, flags, NULL, hide);
}

ECode CInputMethodManager::HideSoftInputFromWindow(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [out] */ Boolean* hide)
{
    VALIDATE_NOT_NULL(hide);
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    AutoPtr<IBinder> token;
    if (mServedView == NULL ||
            (mServedView->GetWindowToken((IBinder**)&token), token.Get() != windowToken)) {
        *hide = FALSE;
        return NOERROR;
    }

    // try {
    return mService->HideSoftInput(mClient, flags, resultReceiver, hide);
    // } catch (RemoteException e) {
    // }
    // return false;
}

ECode CInputMethodManager::ToggleSoftInputFromWindow(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    AutoLock lock(mHLock);

    AutoPtr<IBinder> token;
    if (mServedView == NULL ||
            (mServedView->GetWindowToken((IBinder**)&token), token.Get() != windowToken)) {
        return NOERROR;
    }
    if (mCurMethod != NULL) {
        // try {
        return mCurMethod->ToggleSoftInput(showFlags, hideFlags);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    if (mCurMethod != NULL) {
        // try {
        return mCurMethod->ToggleSoftInput(showFlags, hideFlags);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::RestartInput(
    /* [in] */ IView* view)
{
    FAIL_RETURN(CheckFocus());
    {
        AutoLock lock(mHLock);

        Boolean check = FALSE;
        if (mServedView.Get() != view && (mServedView == NULL ||
                (mServedView->CheckInputConnectionProxy(view, &check), !check))) {
            return NOERROR;
        }

        mServedConnecting = TRUE;
    }

    StartInputInner(NULL, 0, 0, 0);
    return NOERROR;
}

Boolean CInputMethodManager::StartInputInner(
    /* [in] */ IBinder* windowGainingFocus,
    /* [in] */ Int32 controlFlags,
    /* [in] */ Int32 softInputMode,
    /* [in] */ Int32 windowFlags)
{
    AutoPtr<IView> view;
    {
        AutoLock lock(mHLock);

        view = mServedView;

        // Make sure we have a window token for the served view.
        if (DEBUG) Logger::V(TAG, "Starting input: view=%p", view.Get());
        if (view == NULL) {
            if (DEBUG) Logger::V(TAG, "ABORT input: no served view!");
            return FALSE;
        }
    }

    // Now we need to get an input connection from the served view.
    // This is complicated in a couple ways: we can't be holding our lock
    // when calling out to the view, and we need to make sure we call into
    // the view on the same thread that is driving its view hierarchy.
    AutoPtr<IHandler> vh;
    view->GetHandler((IHandler**)&vh);
    if (vh == NULL) {
        // If the view doesn't have a handler, something has changed out
        // from under us, so just close the current input.
        // If we don't close the current input, the current input method can remain on the
        // screen without a connection.
        if (DEBUG) Logger::V(TAG, "ABORT input: no handler for view! Close current input.");
        CloseCurrentInput();
        return FALSE;
    }

    AutoPtr<ILooper> looper;
    vh->GetLooper((ILooper**)&looper);
    if (looper != Looper::GetMyLooper()) {
        // The view is running on a different thread than our own, so
        // we need to reschedule our work for over there.
        if (DEBUG) Logger::V(TAG, "Starting input: reschedule to view thread");
        AutoPtr<IRunnable> runnable = new StartInputRunnable(this);
        Boolean result;
        vh->Post(runnable, &result);
        return FALSE;
    }

    // Okay we are now ready to call into the served view and have it
    // do its stuff.
    // Life is good: let's hook everything up!
    AutoPtr<IEditorInfo> tba;
    ASSERT_SUCCEEDED(CEditorInfo::New((IEditorInfo**)&tba));
    AutoPtr<IContext> ctx;
    view->GetContext((IContext**)&ctx);
    String pkgName;
    ctx->GetPackageName(&pkgName);
    tba->SetPackageName(pkgName);
    Int32 id;
    view->GetId(&id);
    tba->SetFieldId(id);
    AutoPtr<IInputConnection> ic;
    view->OnCreateInputConnection(tba, (IInputConnection**)&ic);
    if (DEBUG) {
        Logger::V(TAG, "Starting input: IEditorInfo=%p, IInputConnection=%p, old mServedInputConnection=%p",
            tba.Get(), ic.Get(), mServedInputConnection.Get());
    }

    {
        AutoLock lock(mHLock);

        // Now that we are locked again, validate that our state hasn't
        // changed.
        if (mServedView != view || !mServedConnecting) {
            // Something else happened, so abort.
            if (DEBUG) {
                Logger::V(TAG, "Starting input: finished by someone else (view=%p, conn=%d)",
                    mServedView.Get(), mServedConnecting);
            }
            return FALSE;
        }

        // If we already have a text box, then this view is already
        // connected so we want to restart it.
        if (mCurrentTextBoxAttribute == NULL) {
            controlFlags |= CONTROL_START_INITIAL;
        }

        // Hook 'em up and let 'er rip.
        mCurrentTextBoxAttribute = tba;
        mServedConnecting = FALSE;
        // Notify the served view that its previous input connection is finished
        NotifyInputConnectionFinished();
        mServedInputConnection = ic;
        AutoPtr<CControlledInputConnectionWrapper> servedContext;
        if (ic != NULL) {
            tba->GetInitialSelStart(&mCursorSelStart);
            tba->GetInitialSelEnd(&mCursorSelEnd);
            mCursorCandStart = -1;
            mCursorCandEnd = -1;
            mCursorRect->SetEmpty();
            mCursorAnchorInfo = NULL;
            CControlledInputConnectionWrapper::NewByFriend(looper, ic, this,
                (CControlledInputConnectionWrapper**)&servedContext);
        }
        else {
            servedContext = NULL;
        }

        if (mServedInputConnectionWrapper != NULL) {
            mServedInputConnectionWrapper->Deactivate();
        }
        mServedInputConnectionWrapper = servedContext;

        // try {
        if (DEBUG) {
            Logger::V(TAG, "START INPUT: %p, ic=%p, IEditorInfo=%p, controlFlags=%08x",
                view.Get(), ic.Get(), tba.Get(), controlFlags);
        }
        AutoPtr<IInputBindResult> res;
        if (windowGainingFocus != NULL) {
            mService->WindowGainedFocus(mClient, windowGainingFocus,
                controlFlags, softInputMode, windowFlags,
                tba, servedContext, (IInputBindResult**)&res);
        }
        else {
            mService->StartInput(mClient,
                servedContext, tba, controlFlags, (IInputBindResult**)&res);
        }
        if (DEBUG) Logger::V(TAG, "Starting input: Bind result=%p", res.Get());
        if (res != NULL) {
            String id;
            res->GetId(&id);
            if (!id.IsNull()) {
                AutoPtr<IInputChannel> channel;
                res->GetChannel((IInputChannel**)&channel);
                SetInputChannelLocked(channel);
                res->GetSequence(&mBindSequence);
                mCurMethod = NULL;
                res->GetMethod((IIInputMethodSession**)&mCurMethod);
                res->GetId(&mCurId);
                res->GetUserActionNotificationSequenceNumber(
                        &mNextUserActionNotificationSequenceNumber);
            }
            else {
                AutoPtr<IInputChannel> channel;
                res->GetChannel((IInputChannel**)&channel);
                if (channel != NULL && channel != mCurChannel) {
                    channel->Dispose();
                }
                if (mCurMethod == NULL) {
                    // This means there is no input method available.
                    if (DEBUG) Logger::V(TAG, "ABORT input: no input method!");
                    return TRUE;
                }
            }
        }
        if (mCurMethod != NULL && mCompletions != NULL) {
            // try {
            mCurMethod->DisplayCompletions(mCompletions);
            // } catch (RemoteException e) {
            // }
        }
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        // }
    }

    return TRUE;
}

ECode CInputMethodManager::WindowDismissed(
    /* [in] */ IBinder* appWindowToken)
{
    FAIL_RETURN(CheckFocus());
    AutoLock lock(mHLock);

    AutoPtr<IBinder> token;
    if (mServedView != NULL &&
            (mServedView->GetWindowToken((IBinder**)&token), token.Get() == appWindowToken)) {
        FinishInputLocked();
    }
    return NOERROR;
}

ECode CInputMethodManager::FocusIn(
    /* [in] */ IView* view)
{
    AutoLock lock(mHLock);
    FocusInLocked(view);
    return NOERROR;
}

void CInputMethodManager::FocusInLocked(
    /* [in] */ IView* view)
{
    if (DEBUG) Logger::V(TAG, "focusIn: %p", view);
    if (DEBUG) {
        Logger::V(TAG, "CInputMethodManager::FocusInLocked, line:%d, view:%p", __LINE__, view);
    }

    AutoPtr<IView> rootView;
    view->GetRootView((IView**)&rootView);
    if (mCurRootView != rootView) {
        // This is a request from a window that isn't in the window with
        // IME focus, so ignore it.
        if (DEBUG) Logger::V(TAG, "Not IME target window, ignoring");
        return;
    }

    mNextServedView = view;
    ScheduleCheckFocusLocked(view);
}

ECode CInputMethodManager::FocusOut(
    /* [in] */ IView* view)
{
    AutoLock lock(mHLock);


    if (DEBUG) {
        Boolean hasFs = FALSE;
        view->HasWindowFocus(&hasFs);
        Logger::V(TAG, "focusOut: %p mServedView=%p winFocus=%d", view,
                    mServedView.Get(), hasFs);
    }
    if (mNextServedView.Get() != view) {
        // The following code would auto-hide the IME if we end up
        // with no more views with focus.  This can happen, however,
        // whenever we go into touch mode, so it ends up hiding
        // at times when we don't really want it to.  For now it
        // seems better to just turn it all off.
        Boolean focus = FALSE;
        if (FALSE && (view->HasWindowFocus(&focus), focus)) {
            mNextServedView = NULL;
            ScheduleCheckFocusLocked(view);
        }
    }
    return NOERROR;
}

void CInputMethodManager::ScheduleCheckFocusLocked(
    /* [in] */ IView* view)
{
    View* _view = VIEW_PROBE(view);
    AutoPtr<IViewRootImpl> viewRootImpl;
    _view->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
    if (viewRootImpl != NULL) {
        viewRootImpl->DispatchCheckFocus();
    }
}

ECode CInputMethodManager::CheckFocus()
{
    if (CheckFocusNoStartInput(FALSE, TRUE)) {
        StartInputInner(NULL, 0, 0, 0);
    }
    return NOERROR;
}

Boolean CInputMethodManager::CheckFocusNoStartInput(
    /* [in] */ Boolean forceNewFocus,
    /* [in] */ Boolean finishComposingText)
{
    // This is called a lot, so short-circuit before locking.
    if (mServedView == mNextServedView && !forceNewFocus) {
        return FALSE;
    }

    AutoPtr<IInputConnection> ic;
    {
        AutoLock lock(mHLock);

        if (mServedView == mNextServedView && !forceNewFocus) {
            return FALSE;
        }
        if (DEBUG) {
            String s("<none>");
            if (mServedView != NULL) {
                AutoPtr<IContext> cxt;
                mServedView->GetContext((IContext**)&cxt);
                cxt->GetPackageName(&s);
            }
            Logger::V(TAG, "checkFocus: view=%p next=%p forceNewFocus=%d package=%s",
                mServedView.Get(), mNextServedView.Get(), forceNewFocus, (const char*)s);
        }

        if (mNextServedView == NULL) {
            FinishInputLocked();
            // In this case, we used to have a focused view on the window,
            // but no longer do.  We should make sure the input method is
            // no longer shown, since it serves no purpose.
            CloseCurrentInput();
            return FALSE;
        }

        ic = mServedInputConnection;

        mServedView = mNextServedView;
        mCurrentTextBoxAttribute = NULL;
        mCompletions = NULL;
        mServedConnecting = TRUE;
    }

    if (finishComposingText && ic != NULL) {
        Boolean ret = FALSE;
        ic->FinishComposingText(&ret);
    }

    return TRUE;
}

void CInputMethodManager::CloseCurrentInput()
{
    // try {
    Boolean ret = FALSE;
    mService->HideSoftInput(mClient, IInputMethodManager::HIDE_NOT_ALWAYS, NULL, &ret);
    // } catch (RemoteException e) {
    // }
}

ECode CInputMethodManager::OnWindowFocus(
    /* [in] */ IView* rootView,
    /* [in] */ IView* focusedView,
    /* [in] */ Int32 softInputMode,
    /* [in] */ Boolean first,
    /* [in] */ Int32 windowFlags)
{
    Boolean forceNewFocus = FALSE;

    {
        AutoLock lock(mHLock);

        if (DEBUG) Logger::V(TAG, "onWindowFocus: %p softInputMode=%d first=%d flags=#%s",
                         focusedView, softInputMode, first, (const char*)StringUtils::ToHexString(windowFlags));
        if (mHasBeenInactive) {
            if (DEBUG) Logger::V(TAG, "Has been inactive!  Starting fresh");
            mHasBeenInactive = FALSE;
            forceNewFocus = TRUE;
        }

        FocusInLocked(focusedView != NULL ? focusedView : rootView);
    }

    Int32 controlFlags = 0;
    if (focusedView != NULL) {
        controlFlags |= CONTROL_WINDOW_VIEW_HAS_FOCUS;
        Boolean result = FALSE;
        if (focusedView->OnCheckIsTextEditor(&result), result) {
            controlFlags |= CONTROL_WINDOW_IS_TEXT_EDITOR;
        }
    }
    if (first) {
        controlFlags |= CONTROL_WINDOW_FIRST;
    }

    AutoPtr<IBinder> binder;
    rootView->GetWindowToken((IBinder**)&binder);
    if (CheckFocusNoStartInput(forceNewFocus, TRUE)) {
        // We need to restart input on the current focus view.  This
        // should be done in conjunction with telling the system service
        // about the window gaining focus, to help make the transition
        // smooth.
        if (StartInputInner(binder, controlFlags, softInputMode, windowFlags)) {
            return NOERROR;
        }
    }

    // For some reason we didn't do a startInput + windowFocusGain, so
    // we'll just do a window focus gain and call it a day.
    {
        AutoLock lock(mHLock);

        // try {
        if (DEBUG) Logger::V(TAG, "Reporting focus gain, without startInput");
        AutoPtr<IInputBindResult> result;
        mService->WindowGainedFocus(mClient, binder,
                controlFlags, softInputMode, windowFlags, NULL, NULL, (IInputBindResult**)&result);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::StartGettingWindowFocus(
    /* [in] */ IView* rootView)
{
    AutoLock lock(mHLock);
    mCurRootView = rootView;
    return NOERROR;
}

ECode CInputMethodManager::UpdateSelection(
    /* [in] */ IView* view,
    /* [in] */ Int32 selStart,
    /* [in] */ Int32 selEnd,
    /* [in] */ Int32 candidatesStart,
    /* [in] */ Int32 candidatesEnd)
{
    FAIL_RETURN(CheckFocus());
    {
        AutoLock lock(mHLock);

        Boolean check = FALSE;
        if ((mServedView.Get() != view && (mServedView == NULL ||
            (mServedView->CheckInputConnectionProxy(view, &check), !check))) ||
            mCurrentTextBoxAttribute == NULL || mCurMethod == NULL) {
            return NOERROR;
        }

        if (mCursorSelStart != selStart || mCursorSelEnd != selEnd
                || mCursorCandStart != candidatesStart
                || mCursorCandEnd != candidatesEnd) {
            if (DEBUG) Logger::D(TAG, "updateSelection");

            // try {
            if (DEBUG) Logger::V(TAG, "SELECTION CHANGE: %p", mCurMethod.Get());
            Int32 oldSelStart = mCursorSelStart;
            Int32 oldSelEnd = mCursorSelEnd;
            // Update internal values before sending updateSelection to the IME, because
            // if it changes the text within its onUpdateSelection handler in a way that
            // does not move the cursor we don't want to call it again with the same values.
            mCursorSelStart = selStart;
            mCursorSelEnd = selEnd;
            mCursorCandStart = candidatesStart;
            mCursorCandEnd = candidatesEnd;
            mCurMethod->UpdateSelection(oldSelStart, oldSelEnd,
                            selStart, selEnd, candidatesStart, candidatesEnd);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "IME died: " + mCurId, e);
            // }
        }
    }
    return NOERROR;
}

ECode CInputMethodManager::ViewClicked(
    /* [in] */ IView* view)
{
    const Boolean focusChanged = mServedView != mNextServedView;
    FAIL_RETURN(CheckFocus());
    {
        AutoLock lock(mHLock);

        Boolean check = FALSE;
        if ((mServedView.Get() != view && (mServedView.Get() == NULL
                || !(mServedView->CheckInputConnectionProxy(view, &check), check)))
                || mCurrentTextBoxAttribute == NULL || mCurMethod == NULL) {
            return NOERROR;
        }
        // try {
        if (DEBUG) Logger::V(TAG, "onViewClicked: %d", focusChanged);
        return mCurMethod->ViewClicked(focusChanged);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::IsWatchingCursor(
    /* [in] */ IView* view,
    /* [out] */ Boolean* isWatching)
{
    VALIDATE_NOT_NULL(isWatching);
    *isWatching = FALSE;
    return NOERROR;
}

ECode CInputMethodManager::IsCursorAnchorInfoEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock lock(mHLock);

        Boolean isImmediate = (mRequestUpdateCursorAnchorInfoMonitorMode &
                IInputConnection::CURSOR_UPDATE_IMMEDIATE) != 0;
        Boolean isMonitoring = (mRequestUpdateCursorAnchorInfoMonitorMode &
                IInputConnection::CURSOR_UPDATE_MONITOR) != 0;
        *result = isImmediate || isMonitoring;
        return NOERROR;
    }
}

ECode CInputMethodManager::SetUpdateCursorAnchorInfoMode(
    /* [in] */ Int32 flags)
{
    AutoLock lock(mHLock);

    mRequestUpdateCursorAnchorInfoMonitorMode = flags;
    return NOERROR;
}

ECode CInputMethodManager::UpdateCursor(
    /* [in] */ IView* view,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FAIL_RETURN(CheckFocus());
    {
        AutoLock lock(mHLock);

        Boolean check = FALSE;
        if ((mServedView.Get() != view && (mServedView == NULL ||
            (mServedView->CheckInputConnectionProxy(view, &check), !check))) ||
            mCurrentTextBoxAttribute == NULL || mCurMethod == NULL) {
            return NOERROR;
        }

        mTmpCursorRect->Set(left, top, right, bottom);
        Boolean equal = FALSE;
        if (mCursorRect->Equals(mTmpCursorRect, &equal), !equal) {
            // if (DEBUG) Log.d(TAG, "updateCursor");

            // try {
            if (DEBUG) Logger::V(TAG, "CURSOR CHANGE: %p", mCurMethod.Get());
            mCurMethod->UpdateCursor(mTmpCursorRect);
            mCursorRect->Set(mTmpCursorRect);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "IME died: " + mCurId, e);
            // }
        }
    }
    return NOERROR;
}

ECode CInputMethodManager::UpdateCursorAnchorInfo(
    /* [in] */ IView* view,
    /* [in] */ ICursorAnchorInfo* cursorAnchorInfo)
{
    if (view == NULL || cursorAnchorInfo == NULL) {
        return NOERROR;
    }
    CheckFocus();
    {
        AutoLock lock(mHLock);

        Boolean bProxy = FALSE;
        if ((!Object::Equals(IInterface::Probe(mServedView), IInterface::Probe(view)) &&
                (mServedView == NULL || !(mServedView->CheckInputConnectionProxy(view, &bProxy), bProxy)))
                || mCurrentTextBoxAttribute == NULL || mCurMethod == NULL) {
            return NOERROR;
        }
        // If immediate bit is set, we will call updateCursorAnchorInfo() even when the data has
        // not been changed from the previous call.
        Boolean isImmediate = (mRequestUpdateCursorAnchorInfoMonitorMode &
                IInputConnection::CURSOR_UPDATE_IMMEDIATE) != 0;
        if (!isImmediate && Object::Equals(mCursorAnchorInfo, cursorAnchorInfo)) {
            // TODO: Consider always emitting this message once we have addressed redundant
            // calls of this method from android.widget.Editor.
            if (DEBUG) {
                Logger::W(TAG, "Ignoring redundant updateCursorAnchorInfo: info=%p",
                         cursorAnchorInfo);
            }
            return NOERROR;
        }
        if (DEBUG) Logger::V(TAG, "updateCursorAnchorInfo: %p", cursorAnchorInfo);
        // try {
        mCurMethod->UpdateCursorAnchorInfo(cursorAnchorInfo);
        mCursorAnchorInfo = cursorAnchorInfo;
        // Clear immediate bit (if any).
        mRequestUpdateCursorAnchorInfoMonitorMode &=
                ~IInputConnection::CURSOR_UPDATE_IMMEDIATE;
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        // }
        return NOERROR;
    }
}

ECode CInputMethodManager::SendAppPrivateCommand(
    /* [in] */ IView* view,
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    FAIL_RETURN(CheckFocus());
    {
        AutoLock lock(mHLock);

        Boolean check;
        if ((mServedView.Get() != view && (mServedView == NULL ||
                (mServedView->CheckInputConnectionProxy(view, &check), !check))) ||
                mCurrentTextBoxAttribute == NULL || mCurMethod == NULL) {
            return NOERROR;
        }
        // try {
        if (DEBUG) Logger::V(TAG, "APP PRIVATE COMMAND %s: %p", (const char*)action, data);
        mCurMethod->AppPrivateCommand(action, data);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        // }
    }
    return NOERROR;
}

ECode CInputMethodManager::SetInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id)
{
    // try {
    return mService->SetInputMethod(token, id);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::SetInputMethodAndSubtype(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id,
    /* [in] */ IInputMethodSubtype* subtype)
{
    // try {
    return mService->SetInputMethodAndSubtype(token, id, subtype);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::HideSoftInputFromInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 flags)
{
    // try {
    return mService->HideMySoftInput(token, flags);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::ShowSoftInputFromInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 flags)
{
    // try {
    return mService->ShowMySoftInput(token, flags);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CInputMethodManager::DispatchInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInterface* token,
    /* [in] */ IInputMethodManagerFinishedInputEventCallback* callback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* result)
{
    {
        AutoLock lock(mHLock);

        if (mCurMethod != NULL) {
            if (IKeyEvent::Probe(event) != NULL) {
                AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(event);
                Int32 act = 0, kc = 0, rc = 0;
                keyEvent->GetAction(&act);
                keyEvent->GetKeyCode(&kc);
                keyEvent->GetRepeatCount(&rc);
                if (act == IKeyEvent::ACTION_DOWN
                        && kc == IKeyEvent::KEYCODE_SYM
                        && rc == 0) {
                    ShowInputMethodPickerLocked();
                    *result = DISPATCH_HANDLED;
                    return NOERROR;
                }
            }

            if (DEBUG) Logger::V(TAG, "DISPATCH INPUT EVENT: %p", mCurMethod.Get());

            AutoPtr<PendingEvent> p = ObtainPendingEventLocked(
                    event, token, mCurId, callback, handler);
            Boolean bIsCurTh = FALSE;
            if ((mMainLooper->IsCurrentThread(&bIsCurTh), bIsCurTh)) {
                // Already running on the IMM thread so we can send the event immediately.
                *result = SendInputEventOnMainLooperLocked(p);
                return NOERROR;
            }

            // Post the event to the IMM thread.
            AutoPtr<IMessage> msg;
            mH->ObtainMessage(MSG_SEND_INPUT_EVENT, IInterface::Probe(IRunnable::Probe(p)), (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            Boolean bSend = FALSE;
            mH->SendMessage(msg, &bSend);
            *result = DISPATCH_IN_PROGRESS;
            return NOERROR;
        }
    }
    *result = DISPATCH_NOT_HANDLED;
    return NOERROR;
}

void CInputMethodManager::SendInputEventAndReportResultOnMainLooper(
    /* [in] */ PendingEvent* p)
{
    Boolean handled;
    {
        AutoLock lock(mHLock);

        Int32 result = SendInputEventOnMainLooperLocked(p);
        if (result == DISPATCH_IN_PROGRESS) {
            return;
        }

        handled = (result == DISPATCH_HANDLED);
    }

    InvokeFinishedInputEventCallback(p, handled);
}

Int32 CInputMethodManager::SendInputEventOnMainLooperLocked(
    /* [in] */ PendingEvent* p)
{
    if (mCurChannel != NULL) {
        if (mCurSender == NULL) {
            AutoPtr<ILooper> lp;
            mH->GetLooper((ILooper**)&lp);
            mCurSender = new ImeInputEventSender(mCurChannel, lp, this);
        }

        AutoPtr<IInputEvent> event = p->mEvent;
        Int32 seq = 0;
        event->GetSequenceNumber(&seq);
        assert(0 && "TODO");
        // if (mCurSender->SendInputEvent(seq, event)) {
        //     mPendingEvents.put(seq, p);
        //     Trace.traceCounter(Trace.TRACE_TAG_INPUT, PENDING_EVENT_COUNTER,
        //             mPendingEvents.size());

        //     Message msg = mH.obtainMessage(MSG_TIMEOUT_INPUT_EVENT, p);
        //     msg.setAsynchronous(true);
        //     mH.sendMessageDelayed(msg, INPUT_METHOD_NOT_RESPONDING_TIMEOUT);
        //     return DISPATCH_IN_PROGRESS;
        // }

        Logger::W(TAG, "Unable to send input event to IME: %s dropping: %p",
                (const char*)mCurId, event.Get());
    }
    return DISPATCH_NOT_HANDLED;
}

void CInputMethodManager::FinishedInputEvent(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled,
    /* [in] */ Boolean timeout)
{
    AutoPtr<PendingEvent> p;
    {
        AutoLock lock(mHLock);

        Int32 index = 0;
        mPendingEvents->IndexOfKey(seq, &index);
        if (index < 0) {
            return; // spurious, event already finished or timed out
        }

        AutoPtr<IInterface> o;
        mPendingEvents->ValueAt(index, (IInterface**)&o);
        p = (PendingEvent*)(Runnable*)IRunnable::Probe(o);
        mPendingEvents->RemoveAt(index);
//        Trace.traceCounter(Trace.TRACE_TAG_INPUT, PENDING_EVENT_COUNTER, mPendingEvents.size());

        if (timeout) {
            Logger::W(TAG, "Timeout waiting for IME to handle input event after %d ms: %s",
                     INPUT_METHOD_NOT_RESPONDING_TIMEOUT, (const char*)(p->mInputMethodId));
        }
        else {
            mH->RemoveMessages(MSG_TIMEOUT_INPUT_EVENT, IInterface::Probe(IRunnable::Probe(p)));
        }
    }

    InvokeFinishedInputEventCallback(p, handled);
}

void CInputMethodManager::InvokeFinishedInputEventCallback(
    /* [in] */ PendingEvent* p,
    /* [in] */ Boolean handled)
{
    p->mHandled = handled;
    AutoPtr<ILooper> lp;
    p->mHandler->GetLooper((ILooper**)&lp);
    Boolean bIsCurTh = FALSE;
    if ((lp->IsCurrentThread(&bIsCurTh), bIsCurTh)) {
        // Already running on the callback handler thread so we can send the
        // callback immediately.
        p->Run();
    }
    else {
        // Post the event to the callback handler thread.
        // In this case, the callback will be responsible for recycling the event.
        assert(0 && "TODO");
        AutoPtr<IMessage> msg;// = Message.obtain(p.mHandler, p);
        msg->SetAsynchronous(TRUE);
        msg->SendToTarget();
    }
}

void CInputMethodManager::FlushPendingEventsLocked()
{
    mH->RemoveMessages(MSG_FLUSH_INPUT_EVENT);

    Int32 count = 0;
    mPendingEvents->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        Int32 seq = 0;
        mPendingEvents->KeyAt(i, &seq);
        AutoPtr<IMessage> msg;
        mH->ObtainMessage(MSG_FLUSH_INPUT_EVENT, seq, 0, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        msg->SendToTarget();
    }
}

AutoPtr<CInputMethodManager::PendingEvent> CInputMethodManager::ObtainPendingEventLocked(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInterface* token,
    /* [in] */ const String& inputMethodId,
    /* [in] */ IInputMethodManagerFinishedInputEventCallback* callback,
    /* [in] */ IHandler* handler)
{
    AutoPtr<PendingEvent> p = mPendingEventPool->AcquireItem();
    if (p == NULL) {
        p = new PendingEvent(this);
    }
    p->mEvent = event;
    p->mToken = token;
    p->mInputMethodId = inputMethodId;
    p->mCallback = callback;
    p->mHandler = handler;
    return p;
}

void CInputMethodManager::RecyclePendingEventLocked(
    /* [in] */ PendingEvent* p)
{
    p->Recycle();
    mPendingEventPool->ReleaseItem(p);
}

ECode CInputMethodManager::ShowInputMethodPicker()
{
    AutoLock lock(mHLock);
    ShowInputMethodPickerLocked();
    return NOERROR;
}

void CInputMethodManager::ShowInputMethodPickerLocked()
{
    // try {
    mService->ShowInputMethodPickerFromClient(mClient);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "IME died: " + mCurId, e);
    // }
}

ECode CInputMethodManager::ShowInputMethodAndSubtypeEnabler(
    /* [in] */ const String& imiId)
{
    AutoLock lock(mHLock);
    // try {
    return mService->ShowInputMethodAndSubtypeEnablerFromClient(mClient, imiId);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "IME died: " + mCurId, e);
    // }
}

ECode CInputMethodManager::GetCurrentInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);
    {
        AutoLock lock(mHLock);
        // try {
        return mService->GetCurrentInputMethodSubtype(subtype);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return NULL;
        // }
    }
}

ECode CInputMethodManager::SetCurrentInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mHLock);
        // try {
        return mService->SetCurrentInputMethodSubtype(subtype, result);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return false;
        // }
    }
}

ECode CInputMethodManager::NotifyUserAction()
{
    AutoLock lock(mHLock);

    if (mLastSentUserActionNotificationSequenceNumber ==
            mNextUserActionNotificationSequenceNumber) {
        if (DEBUG) {
            Logger::W(TAG, "Ignoring notifyUserAction as it has already been sent."
                     " mLastSentUserActionNotificationSequenceNumber: %d"
                     " mNextUserActionNotificationSequenceNumber: %d",
                     mLastSentUserActionNotificationSequenceNumber,
                     mNextUserActionNotificationSequenceNumber);
        }
        return NOERROR;
    }
    // try {
    if (DEBUG) {
        Logger::W(TAG, "notifyUserAction: "
                 " mLastSentUserActionNotificationSequenceNumber: %d"
                 " mNextUserActionNotificationSequenceNumber: %d",
                 mLastSentUserActionNotificationSequenceNumber,
                 mNextUserActionNotificationSequenceNumber);
    }
    mService->NotifyUserAction(mNextUserActionNotificationSequenceNumber);
    mLastSentUserActionNotificationSequenceNumber =
            mNextUserActionNotificationSequenceNumber;
    // } catch (RemoteException e) {
    //     Log.w(TAG, "IME died: " + mCurId, e);
    // }
    return NOERROR;
}

ECode CInputMethodManager::GetShortcutInputMethodsAndSubtypes(
    /* [out] */ IMap** res)
{
    VALIDATE_NOT_NULL(res);
    {
        AutoLock lock(mHLock);

        AutoPtr<IHashMap> ret;
        CHashMap::New((IHashMap**)&ret);
        // try {
        // TODO: We should change the return type from List<Object> to List<Parcelable>
        AutoPtr<IList> info;
        mService->GetShortcutInputMethodsAndSubtypes((IList**)&info);
        // "info" has imi1, subtype1, subtype2, imi2, subtype2, imi3, subtype3..in the list
        AutoPtr<IArrayList> subtypes;
        Int32 N = 0;
        info->GetSize(&N);
        if (info != NULL && N > 0) {
            for (Int32 i = 0; i < N; ++i) {
                AutoPtr<IInterface> o;
                info->Get(i, (IInterface**)&o);
                if (IInputMethodInfo::Probe(o) != NULL) {
                    Boolean bContainK = FALSE;
                    if ((ret->ContainsKey(o, &bContainK), bContainK)) {
                        Logger::E(TAG, "IMI list already contains the same InputMethod.");
                        break;
                    }
                    CArrayList::New((IArrayList**)&subtypes);
                    ret->Put(o, subtypes);
                }
                else if (subtypes != NULL && IInputMethodSubtype::Probe(o) != NULL) {
                    subtypes->Add(IInputMethodSubtype::Probe(o));
                }
            }
        }
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        // }
        return NOERROR;
    }
}

ECode CInputMethodManager::GetInputMethodWindowVisibleHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    {
        AutoLock lock(mHLock);
//         try {
        return mService->GetInputMethodWindowVisibleHeight(height);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return 0;
        // }
    }
}

ECode CInputMethodManager::SwitchToLastInputMethod(
    /* [in] */ IBinder* imeToken,
    /* [out] */ Boolean* switched)
{
    VALIDATE_NOT_NULL(switched)
    {
        AutoLock lock(mHLock);
        // try {
        return mService->SwitchToLastInputMethod(imeToken, switched);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return false;
        // }
    }
}

ECode CInputMethodManager::SwitchToNextInputMethod(
    /* [in] */ IBinder* imeToken,
    /* [in] */ Boolean onlyCurrentIme,
    /* [out] */ Boolean* switched)
{
    VALIDATE_NOT_NULL(switched)
    {
        AutoLock lock(mHLock);
        // try {
        return mService->SwitchToNextInputMethod(imeToken, onlyCurrentIme, switched);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return false;
        // }
    }
}

ECode CInputMethodManager::ShouldOfferSwitchingToNextInputMethod(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock lock(mHLock);
        // try {
        return mService->ShouldOfferSwitchingToNextInputMethod(token, result);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return false;
        // }
    }
}

ECode CInputMethodManager::SetAdditionalInputMethodSubtypes(
    /* [in] */ const String& imiId,
    /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes)
{
    {
        AutoLock lock(mHLock);
        // try {
        return mService->SetAdditionalInputMethodSubtypes(imiId, subtypes);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        // }
    }
}

ECode CInputMethodManager::GetLastInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);
    {
        AutoLock lock(mHLock);
        // try {
        return mService->GetLastInputMethodSubtype(subtype);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "IME died: " + mCurId, e);
        //     return NULL;
        // }
    }
}

void CInputMethodManager::HandleBind(
    /* [in] */ IInputBindResult* res)
{
    assert(res != NULL);
    {
        AutoLock lock(mHLock);

        Int32 sequence;
        res->GetSequence(&sequence);
        assert(0 && "TODO");
//         AutoPtr<CInputBindResult> cres = (CInputBindResult*)res;
        if (mBindSequence < 0 || mBindSequence != sequence) {
            // Logger::W(TAG, "Ignoring onBind: cur seq=%d, given seq=%d",
            //     mBindSequence, cres->mSequence);
            // if (cres->mChannel != NULL && cres->mChannel != mCurChannel) {
            //     cres->mChannel->Dispose();
            // }
            return;
        }

        mRequestUpdateCursorAnchorInfoMonitorMode =
                                REQUEST_UPDATE_CURSOR_ANCHOR_INFO_NONE;

//         SetInputChannelLocked(cres->mChannel);
        mCurMethod = NULL;
        // res->GetIIMSession((IIInputMethodSession**)&mCurMethod);
        res->GetId(&mCurId);
        mBindSequence = sequence;
    }
    StartInputInner(NULL, 0, 0, 0);
}

void CInputMethodManager::HandleUnBind(
    /* [in] */ Int32 sequence)
{
    if (DEBUG) {
        Logger::I(TAG, "handleMessage: MSG_UNBIND %d", sequence);
    }
    Boolean startInput = FALSE;
    {
        AutoLock lock(mHLock);

        if (mBindSequence == sequence) {
            if (FALSE) {
                // XXX the server has already unbound!
                if (mCurMethod != NULL && mCurrentTextBoxAttribute != NULL) {
                    // try {
                        mCurMethod->FinishInput();
                    // } catch (RemoteException e) {
                    //     Log.w(TAG, "IME died: " + mCurId, e);
                    // }
                }
            }
            ClearBindingLocked();

            // If we were actively using the last input method, then
            // we would like to re-connect to the next input method.
            Boolean focused = FALSE;
            if (mServedView != NULL && (mServedView->IsFocused(&focused), focused)) {
                mServedConnecting = TRUE;
            }
            if (mActive) {
                startInput = TRUE;
            }
        }
        if (startInput) {
            StartInputInner(NULL, 0, 0, 0);
        }
    }
}

void CInputMethodManager::HandleSetActive(
    /* [in] */ Boolean active)
{
    if (DEBUG) {
        Logger::I(TAG, "handleMessage: MSG_SET_ACTIVE %d, was %d", active, mActive);
    }
    {
        AutoLock lock(mHLock);

        mActive = active;
        mFullscreenMode = FALSE;
        if (!active) {
            // Some other client has starting using the IME, so note
            // that this happened and make sure our own editor's
            // state is reset.
            mHasBeenInactive = TRUE;
            // try {
            // Note that finishComposingText() is allowed to run
            // even when we are not active.
            mIInputContext->FinishComposingText();
            // } catch (RemoteException e) {
            // }
            // Check focus again in case that "onWindowFocus" is called before
            // handling this message.
            Boolean hasFocus = FALSE;
            if (mServedView != NULL && (mServedView->HasWindowFocus(&hasFocus), hasFocus)) {
                // "finishComposingText" has been already called above. So we
                // should not call mServedInputConnection.finishComposingText here.
                // Also, please note that this handler thread could be different
                // from a thread that created mServedView. That could happen
                // the current activity is running in the system process.
                // In that case, we really should not call
                // mServedInputConnection.finishComposingText.
                if (CheckFocusNoStartInput(mHasBeenInactive, FALSE)) {
                    StartInputInner(NULL, 0, 0, 0);
                }
            }
        }
    }
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
