#include "elastos/droid/wifidisplay/WifiDisplayManagerBase.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace WifiDisplay {

//==============================================================================
//                  WifiDisplayManagerBase::WifiDisplayThread
//==============================================================================
CAR_INTERFACE_IMPL_2(WifiDisplayManagerBase::WifiDisplayThread, IWifiDisplayThread, IBinder)

WifiDisplayManagerBase::WifiDisplayThread::WifiDisplayThread(
    /* [in] */ WifiDisplayManagerBase* host)
    : mHost(host)
{
}

ECode WifiDisplayManagerBase::WifiDisplayThread::ExitWifiDisplayReceive()
{
    mHost->QueueOrSendMessage(
        WifiDisplayManagerBase::EXIT_WIFIDISPLAY_RECEIVE, NULL);
    return NOERROR;
}

ECode WifiDisplayManagerBase::WifiDisplayThread::ExitWifiDisplaySend()
{
    mHost->QueueOrSendMessage(
        WifiDisplayManagerBase::EXIT_WIFIDISPLAY_SEND, NULL);
    return NOERROR;
}

ECode WifiDisplayManagerBase::WifiDisplayThread::StartWifiDisplaySend()
{
    mHost->QueueOrSendMessage(
        WifiDisplayManagerBase::START_WIFIDISPLAY_SEND, NULL);
    return NOERROR;
}

ECode WifiDisplayManagerBase::WifiDisplayThread::DispatchMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    mHost->QueueOrSendMessage(
        WifiDisplayManagerBase::DISPATCH_WIFIDISPLAY_EVEVT, (IInterface*)event);
    return NOERROR;
}

ECode WifiDisplayManagerBase::WifiDisplayThread::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)this;
    return NOERROR;
}

ECode WifiDisplayManagerBase::WifiDisplayThread::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("WifiDisplayManagerBase::WifiDisplayThread");
    return NOERROR;
}


//==============================================================================
//                  WifiDisplayManagerBase::H
//==============================================================================

const Int32 WifiDisplayManagerBase::H::EXIT_WIFIDISPLAY_RECEIVE         = 100;
const Int32 WifiDisplayManagerBase::H::EXIT_WIFIDISPLAY_SEND            = 101;
const Int32 WifiDisplayManagerBase::H::START_WIFIDISPLAY_SEND           = 102;
const Int32 WifiDisplayManagerBase::H::DISPATCH_WIFIDISPLAY_EVEVT       = 103;

WifiDisplayManagerBase::H::H(
    /* [in] */ WifiDisplayManagerBase* host)
    : mHost(host)
{
}

String WifiDisplayManagerBase::H::CodeToString(
    /* [in] */ Int32 code)
{
    if (WifiDisplayManagerBase::DEBUG_MESSAGES) {
        switch (code)
        {
            case EXIT_WIFIDISPLAY_RECEIVE: return String("EXIT_WIFIDISPLAY_RECEIVE");
            case EXIT_WIFIDISPLAY_SEND: return String("EXIT_WIFIDISPLAY_SEND");
            case START_WIFIDISPLAY_SEND: return String("START_WIFIDISPLAY_SEND");
            case DISPATCH_WIFIDISPLAY_EVEVT: return String("DISPATCH_WIFIDISPLAY_EVEVT");
        }
    }
    return String("(unknown message)");
}

ECode WifiDisplayManagerBase::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what;
    msg->GetWhat(&what);

    if (WifiDisplayManagerBase::DEBUG_MESSAGES) {
        Slogger::V(TAG, ">>> handling: %s", CodeToString(what).string());
    }

    switch (what)  {
        case EXIT_WIFIDISPLAY_RECEIVE: {
            mHost->OnExitWifiDisplayReceive();
        }
        break;

        case EXIT_WIFIDISPLAY_SEND: {
            mHost->OnExitWifiDisplaySend();
        }
        break;

        case START_WIFIDISPLAY_SEND: {
            mHost->OnStartWifiDisplaySend();
        }
        break; // TODO fallthrough ????

        case DISPATCH_WIFIDISPLAY_EVEVT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IMotionEvent> me = IMotionEvent::Probe(obj);
            mHost->OnDispatchMotionEvent(me);
        }
        break;
    }

    if (WifiDisplayManagerBase::DEBUG_MESSAGES) {
        Slogger::V(TAG, "<<< done: %s", CodeToString(what).string());
    }
    return NOERROR;
}

//==============================================================================
//                  WifiDisplayManagerBase
//==============================================================================
const String WifiDisplayManagerBase::TAG("WifiDisplayManagerBase");
const Boolean WifiDisplayManagerBase::DEBUG_MESSAGES = TRUE;
const Boolean WifiDisplayManagerBase::DEBUG_REMOTE = TRUE;

WifiDisplayManagerBase::WifiDisplayManagerBase()
{
    AutoPtr<IInterface> tmpObj;
    tmpObj = ServiceManager::GetService(IContext::WINDOW_SERVICE);
    mWindowManager = IIWindowManager::Probe(tmpObj.Get());

    tmpObj = ServiceManager::GetService(IContext::WIFIDISPLAY_SERVICE);
    mWifiDisplayManager = IWifiDisplayManager::Probe(tmpObj.Get());

    mH = new H(this);
}

WifiDisplayManagerBase::~WifiDisplayManagerBase()
{
}

void WifiDisplayManagerBase::QueueOrSendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    QueueOrSendMessage(what, obj, 0, 0);
}

void WifiDisplayManagerBase::QueueOrSendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 arg1)
{
    QueueOrSendMessage(what, obj, arg1, 0);;
}

void WifiDisplayManagerBase::QueueOrSendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoLock lock(mThisLock);

    if (DEBUG_MESSAGES) {
        Slogger::V(TAG, "SCHEDULE %d : %s, arg1: %d",
            what, H::CodeToString(what).string(), arg1);
    }

    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
    Boolean result;
    mH->SendMessage(msg, &result);
}

ECode WifiDisplayManagerBase::StartWifiDisplaySend(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 intVal;
    ECode ec = NOERROR;
    if (mWifiDisplayThread == NULL) {
        mWifiDisplayThread  = new WifiDisplayThread(this);

        ec = mWifiDisplayManager->AddWifiDisplayClient(mWifiDisplayThread, &intVal);
        if (FAILED(ec)) {
            *result = -1;
            return NOERROR;
        }
    }

    if (DEBUG_REMOTE) {
        Slogger::V(TAG, "startWifiDisplaySend  mWifiDisplayThread = %p", mWifiDisplayThread.Get());
    }

    ec = mWifiDisplayManager->StartWifiDisplaySend(mWifiDisplayThread, &intVal);
    if (FAILED(ec)) {
        *result = -1;
        return NOERROR;
    }

    return NOERROR;
}

ECode WifiDisplayManagerBase::EndWifiDisplaySend(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    ECode ec = NOERROR;
    Int32 intVal;
    if (mWifiDisplayThread == NULL) {
        mWifiDisplayThread  = new WifiDisplayThread(this);

        ec = mWifiDisplayManager->AddWifiDisplayClient(mWifiDisplayThread, &intVal);
        if (FAILED(ec)) {
            *result = -1;
            return NOERROR;
        }
    }

    if (DEBUG_REMOTE) {
        Slogger::V(TAG, "endWifiDisplaySend  mWifiDisplayThread = %p", mWifiDisplayThread.Get());
    }

    ec = mWifiDisplayManager->EndWifiDisplaySend(mWifiDisplayThread, &intVal);
    if (FAILED(ec)) {
        *result = -1;
        return NOERROR;
    }

    return NOERROR;
}

ECode WifiDisplayManagerBase::StartWifiDisplayReceive(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 intVal;
    ECode ec = NOERROR;
    if (mWifiDisplayThread == NULL) {
        mWifiDisplayThread  = new WifiDisplayThread(this);

        ec = mWifiDisplayManager->AddWifiDisplayClient(mWifiDisplayThread, &intVal);
        if (FAILED(ec)) {
            *result = -1;
            return NOERROR;
        }
    }

    if (DEBUG_REMOTE) {
        Slogger::V(TAG, "startWifiDisplayReceive  mWifiDisplayThread = %p", mWifiDisplayThread.Get());
    }

    ec = mWifiDisplayManager->StartWifiDisplayReceive(mWifiDisplayThread, &intVal);
    if (FAILED(ec)) {
        *result = -1;
        return NOERROR;
    }

    return NOERROR;
}

ECode WifiDisplayManagerBase::EndWifiDisplayReceive(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG_REMOTE)
        Slogger::V(TAG, "endWifiDisplayReceive ");

    ECode ec = NOERROR;
    Int32 intVal;
    if (mWifiDisplayThread == NULL) {
        mWifiDisplayThread  = new WifiDisplayThread(this);
        ec = mWifiDisplayManager->AddWifiDisplayClient(mWifiDisplayThread, &intVal);
        if (FAILED(ec)) {
            *result = -1;
            return NOERROR;
        }
    }

    if (DEBUG_REMOTE) {
        Slogger::V(TAG, "endWifiDisplayReceive  mWifiDisplayThread = %p", mWifiDisplayThread.Get());
    }

    ec = mWifiDisplayManager->EndWifiDisplayReceive(mWifiDisplayThread, &intVal);
    if (FAILED(ec)) {
        *result = -1;
        return NOERROR;
    }

    return NOERROR;
}


} // namespace WifiDisplay
} // namespace Droid
} // namespace Elastos
