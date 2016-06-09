
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IIMessenger;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Utility::CStack;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

static AutoPtr<IStack> InitStack()
{
    AutoPtr<IStack> stack;
    CStack::New((IStack**)&stack);
    return stack;
}

AutoPtr<IStack> AsyncChannel::SyncMessenger::sStack = InitStack();
Int32 AsyncChannel::SyncMessenger::sCount = 0;

const String AsyncChannel::TAG("AsyncChannel");
const Boolean AsyncChannel::DBG = FALSE;

//=============================================
// AsyncChannel::SyncMessenger::SyncHandler
//=============================================
ECode AsyncChannel::SyncMessenger::SyncHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    mResultMsg = NULL;
    helper->Obtain((IMessage**)&mResultMsg);
    mResultMsg->CopyFrom(msg);
    {
        AutoLock lock(mLockObject);
        mLockObject.Notify();
    }
    return NOERROR;
}


//=============================================
// AsyncChannel::SyncMessenger
//=============================================
AutoPtr<AsyncChannel::SyncMessenger> AsyncChannel::SyncMessenger::Obtain()
{
    AutoPtr<SyncMessenger> sm;
    {
        AutoLock lock(sStack);
        Boolean isEmpty;
        sStack->IsEmpty(&isEmpty);
        if (isEmpty) {
            sm = new SyncMessenger();
            AutoPtr<IThread> thread;
            //for new CHandlerThread
            Thread::Attach((IThread**)&thread);
            sm->mHandlerThread = NULL;
            CHandlerThread::New(String("SyncHandler-") + StringUtils::ToString(sCount++),
                (IHandlerThread**)&sm->mHandlerThread);
            IThread::Probe(sm->mHandlerThread)->Start();
            AutoPtr<ILooper> l;
            sm->mHandlerThread->GetLooper((ILooper**)&l);
            sm->mHandler = new SyncHandler(l);
            sm->mMessenger = NULL;
            CMessenger::New(sm->mHandler, (IMessenger**)&sm->mMessenger);
        }
        else {
            AutoPtr<IInterface> item;
            sStack->Pop((IInterface**)&item);
            sm = (SyncMessenger*)IObject::Probe(item);
        }
    }
    return sm;
}

void AsyncChannel::SyncMessenger::Recycle()
{
    AutoLock lock(sStack);
    sStack->Push(TO_IINTERFACE(this));
}

AutoPtr<IMessage> AsyncChannel::SyncMessenger::SendMessageSynchronously(
    /* [in] */ IMessenger* dstMessenger,
    /* [in] */ IMessage* msg)
{
    AutoPtr<SyncMessenger> sm = SyncMessenger::Obtain();
    // try {
    if (dstMessenger != NULL && msg != NULL) {
        msg->SetReplyTo(sm->mMessenger);
        AutoPtr<IThread> thread;
        Thread::Attach((IThread**)&thread);
        {
            AutoLock lock(sm->mHandler->mLockObject);
            dstMessenger->Send(msg);
            sm->mHandler->mLockObject.Wait();
        }
    }
    else {
        sm->mHandler->mResultMsg = NULL;
    }
    // } catch (InterruptedException e) {
    //     sm.mHandler.mResultMsg = null;
    // } catch (RemoteException e) {
    //     sm.mHandler.mResultMsg = null;
    // }
    AutoPtr<IMessage> resultMsg = sm->mHandler->mResultMsg;
    sm->Recycle();
    return resultMsg;
}

//=============================================
// AsyncChannel::AsyncChannelConnection
//=============================================
CAR_INTERFACE_IMPL(AsyncChannel::AsyncChannelConnection, Object, IServiceConnection)

AsyncChannel::AsyncChannelConnection::AsyncChannelConnection(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

ECode AsyncChannel::AsyncChannelConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<IWeakReferenceSource> wrs;
    mWeakHost->Resolve(EIID_IWeakReferenceSource, (IInterface**)&wrs);
    if (wrs) {
        AsyncChannel* ac = (AsyncChannel*)wrs.Get();
        ac->mDstMessenger = NULL;
        AutoPtr<IIMessenger> msger = IIMessenger::Probe(service);
        assert(msger != NULL);
        CMessenger::New(msger, (IMessenger**)&ac->mDstMessenger);
        ac->ReplyHalfConnected(AsyncChannel::STATUS_SUCCESSFUL);
    }
    return NOERROR;
}

ECode AsyncChannel::AsyncChannelConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoPtr<IWeakReferenceSource> wrs;
    mWeakHost->Resolve(EIID_IWeakReferenceSource, (IInterface**)&wrs);
    if (wrs) {
        AsyncChannel* ac = (AsyncChannel*)wrs.Get();
        ac->ReplyDisconnected(AsyncChannel::STATUS_SUCCESSFUL);
    }
    return NOERROR;
}

//=============================================
// AsyncChannel::ConnectAsyncRunnable
//=============================================
AsyncChannel::ConnectAsyncRunnable::ConnectAsyncRunnable(
    /* [in] */ IWeakReference* wr,
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ const String& dstPackageName,
    /* [in] */ const String& dstClassName)
    : mWeakHost(wr)
    , mSrcCtx(srcContext)
    , mSrcHdlr(srcHandler)
    , mDstPackageName(dstPackageName)
    , mDstClassName(dstClassName)
{
}

ECode AsyncChannel::ConnectAsyncRunnable::Run()
{
    AutoPtr<IWeakReferenceSource> wrs;
    mWeakHost->Resolve(EIID_IWeakReferenceSource, (IInterface**)&wrs);
    if (wrs) {
        AsyncChannel* ac = (AsyncChannel*)wrs.Get();
        Int32 result;
        ac->ConnectSrcHandlerToPackageSync(mSrcCtx, mSrcHdlr, mDstPackageName,
                mDstClassName, &result);
        ac->ReplyHalfConnected(result);
    }
    return NOERROR;
}

//=============================================
// AsyncChannel::DeathMonitor
//=============================================
CAR_INTERFACE_IMPL(AsyncChannel::DeathMonitor, Object, IProxyDeathRecipient)

AsyncChannel::DeathMonitor::DeathMonitor(
    /* [in] */ AsyncChannel* owner)
    : mOwner(owner)
    , mIsLinked(FALSE)
{
}

AsyncChannel::DeathMonitor::~DeathMonitor()
{
    if (mIsLinked) {
        mOwner->Release();
        mOwner = NULL;
    }
}

ECode AsyncChannel::DeathMonitor::ProxyDied()
{
    mOwner->ReplyDisconnected(STATUS_REMOTE_DISCONNECTION);
    return NOERROR;
}

void AsyncChannel::DeathMonitor::LinkToDeath()
{
    mIsLinked = TRUE;
    mOwner->AddRef();
}

//=============================================
// AsyncChannel
//=============================================
CAR_INTERFACE_IMPL(AsyncChannel, Object, IAsyncChannel)

AsyncChannel::AsyncChannel()
{
}

AsyncChannel::~AsyncChannel()
{
}

ECode AsyncChannel::constructor()
{
    return NOERROR;
}

ECode AsyncChannel::ConnectSrcHandlerToPackageSync(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ const String& dstPackageName,
    /* [in] */ const String& dstClassName,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)
    if (DBG) {
        Slogger::D(TAG, " +++ ConnectSrcHandlerToPackageSync srcHandler to dst Package %s & class %s",
            dstPackageName.string(), dstClassName.string());
    }

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mConnection = new AsyncChannelConnection(wr);

    /* Initialize the source information */
    mSrcContext = srcContext;
    mSrcHandler = srcHandler;
    mSrcMessenger = NULL;
    CMessenger::New(mSrcHandler, (IMessenger**)&mSrcMessenger);

    /*
     * Initialize destination information to null they will
     * be initialized when the AsyncChannelConnection#onServiceConnected
     * is called
     */
    mDstMessenger = NULL;

    /* Send intent to create the connection */
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->SetClassName(dstPackageName, dstClassName);
    Boolean result;
    srcContext->BindService(intent, mConnection, IContext::BIND_AUTO_CREATE, &result);
    if (DBG) {
        Slogger::D(TAG, " --- ConnectSrcHandlerToPackageSync srcHandler to dst Package %s & class %s, result=%d",
            dstPackageName.string(), dstClassName.string(), result);
    }
    *status = result ? STATUS_SUCCESSFUL : STATUS_BINDING_UNSUCCESSFUL;
    return NOERROR;
}

ECode AsyncChannel::ConnectSync(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ IMessenger* dstMessenger,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)
    if (DBG) Slogger::D(TAG, " +++ halfConnectSync srcHandler to the dstMessenger  E");

    // We are connected
    Connected(srcContext, srcHandler, dstMessenger);

    if (DBG) Slogger::D(TAG, " --- halfConnectSync srcHandler to the dstMessenger X");
    *status = STATUS_SUCCESSFUL;
    return NOERROR;
}

ECode AsyncChannel::ConnectSync(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ IHandler* dstHandler,
    /* [out] */ Int32* status)
{
    AutoPtr<IMessenger> msger;
    CMessenger::New(dstHandler, (IMessenger**)&msger);
    return ConnectSync(srcContext, srcHandler, msger, status);
}

ECode AsyncChannel::FullyConnectSync(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ IHandler* dstHandler,
    /* [out] */ Int32* status)
{
    ConnectSync(srcContext, srcHandler, dstHandler, status);
    if (*status == STATUS_SUCCESSFUL) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(CMD_CHANNEL_FULL_CONNECTION, (IMessage**)&response);
        response->GetArg1(status);
    }
    return NOERROR;
}

ECode AsyncChannel::Connect(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ const String& dstPackageName,
    /* [in] */ const String& dstClassName)
{
    if (DBG) {
        Slogger::D(TAG, " +++ Connect srcHandler to dst Package %s & class %s",
            dstPackageName.string(), dstClassName.string());
    }

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    AutoPtr<IRunnable> ca = new ConnectAsyncRunnable(wr, srcContext, srcHandler, dstPackageName, dstClassName);
    AutoPtr<IThread> thread;
    CThread::New(ca, (IThread**)&thread);
    thread->Start();

    if (DBG) {
        Slogger::D(TAG, " --- Connect srcHandler to dst Package %s & class %s",
            dstPackageName.string(), dstClassName.string());
    }
    return NOERROR;
}

ECode AsyncChannel::Connect(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ IMessenger* dstMessenger)
{
    if (DBG) Slogger::D(TAG, " +++ Connect srcHandler to the dstMessenger");

    // We are connected
    Connected(srcContext, srcHandler, dstMessenger);

    // Tell source we are half connected
    ReplyHalfConnected(STATUS_SUCCESSFUL);

    if (DBG) Slogger::D(TAG, " --- Connect srcHandler to the dstMessenger");
    return NOERROR;
}

ECode AsyncChannel::Connected(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ IMessenger* dstMessenger)
{
    if (DBG) Slogger::D(TAG, " +++ Connected srcHandler to the dstMessenger  E");

    // Initialize source fields
    mSrcContext = srcContext;
    mSrcHandler = srcHandler;
    mSrcMessenger = NULL;
    CMessenger::New(mSrcHandler, (IMessenger**)&mSrcMessenger);

    // Initialize destination fields
    mDstMessenger = dstMessenger;
    if (mDstMessenger == NULL) {
        Slogger::E(TAG, "connected srcHandler to null dstMessenger!");
    }

    if (DBG) Slogger::D(TAG, " --- Connected srcHandler to the dstMessenger X");
    return NOERROR;
}

ECode AsyncChannel::Connect(
    /* [in] */ IContext* srcContext,
    /* [in] */ IHandler* srcHandler,
    /* [in] */ IHandler* dstHandler)
{
    AutoPtr<IMessenger> mgr;
    CMessenger::New(dstHandler, (IMessenger**)&mgr);
    return Connect(srcContext, srcHandler, mgr);
}

ECode AsyncChannel::Connect(
    /* [in] */ IAsyncService* srcAsyncService,
    /* [in] */ IMessenger* dstMessenger)
{
    AutoPtr<IHandler> handler;
    srcAsyncService->GetHandler((IHandler**)&handler);
    return Connect(IContext::Probe(srcAsyncService), handler, dstMessenger);
}

ECode AsyncChannel::Disconnected()
{
    mSrcContext = NULL;
    mSrcHandler = NULL;
    mSrcMessenger = NULL;
    mDstMessenger = NULL;
    mDeathMonitor = NULL;
    mConnection = NULL;
    return NOERROR;
}

ECode AsyncChannel::Disconnect()
{
    if (DBG) Slogger::D(TAG, " >> AsyncChannel::Disconnect()");
    if ((mConnection != NULL) && (mSrcContext != NULL)) {
        mSrcContext->UnbindService(mConnection);
        mConnection = NULL;
    }

    // try {
        // Send the DISCONNECTED, although it may not be received
        // but its the best we can do.
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(CMD_CHANNEL_DISCONNECTED);
    msg->SetReplyTo(mSrcMessenger);
    mDstMessenger->Send(msg);
    // } catch(Exception e) {
    // }

    // Tell source we're disconnected.
    if (mSrcHandler != NULL) {
        ReplyDisconnected(STATUS_SUCCESSFUL);
        mSrcHandler = NULL;
    }
    // Unlink only when bindService isn't used
    if (mConnection == NULL && mDstMessenger != NULL && mDeathMonitor!= NULL) {
        AutoPtr<IBinder> binder;
        mDstMessenger->GetBinder((IBinder**)&binder);
        AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean res;
            proxy->UnlinkToDeath(mDeathMonitor, 0, &res);
            mDeathMonitor = NULL;
        }
        else {
            if (DBG) Slogger::D(TAG, " AsyncChannel::Disconnect(), happen in server side");
        }
    }
    return NOERROR;
}

ECode AsyncChannel::SendMessage(
    /* [in] */ IMessage* msg)
{
    msg->SetReplyTo(mSrcMessenger);
    // try {
        if (mDstMessenger != NULL) {
            ECode ec = mDstMessenger->Send(msg);
            if (FAILED(ec)) {
                ReplyDisconnected(STATUS_SEND_UNSUCCESSFUL);
            }
        }
    // } catch (RemoteException e) {
    //     replyDisconnected(STATUS_SEND_UNSUCCESSFUL);
    // }
    return NOERROR;
}

ECode AsyncChannel::SendMessage(
    /* [in] */ Int32 what)
{
    return SendMessage(what, 0);
}

ECode AsyncChannel::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    return SendMessage(what, arg1, 0);
}

ECode AsyncChannel::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    return SendMessage(what, arg1, 0, NULL);
}

ECode AsyncChannel::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(what);
    msg->SetArg1(arg1);
    msg->SetArg2(arg2);
    msg->SetObj(obj);

    return SendMessage(msg);
}

ECode AsyncChannel::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return SendMessage(what, 0, 0, obj);
}

ECode AsyncChannel::ReplyToMessage(
    /* [in] */ IMessage* srcMsg,
    /* [in] */ IMessage* dstMsg)
{
    // try {
    dstMsg->SetReplyTo(mSrcMessenger);
    AutoPtr<IMessenger> replyTo;
    srcMsg->GetReplyTo((IMessenger**)&replyTo);
    replyTo->Send(dstMsg);
    // } catch (RemoteException e) {
    //     log("TODO: handle replyToMessage RemoteException" + e);
    //     e.printStackTrace();
    // }
    return NOERROR;
}

ECode AsyncChannel::ReplyToMessage(
    /* [in] */ IMessage* srcMsg,
    /* [in] */ Int32 what)
{
    return ReplyToMessage(srcMsg, what, 0);
}

ECode AsyncChannel::ReplyToMessage(
    /* [in] */ IMessage* srcMsg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    return ReplyToMessage(srcMsg, what, arg1, 0);
}

ECode AsyncChannel::ReplyToMessage(
    /* [in] */ IMessage* srcMsg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    return ReplyToMessage(srcMsg, what, arg1, arg2, NULL);
}

ECode AsyncChannel::ReplyToMessage(
    /* [in] */ IMessage* srcMsg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(what);
    msg->SetArg1(arg1);
    msg->SetArg2(arg2);
    msg->SetObj(obj);
    return ReplyToMessage(srcMsg, msg);
}

ECode AsyncChannel::ReplyToMessage(
    /* [in] */ IMessage* srcMsg,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return ReplyToMessage(srcMsg, what, 0, 0, obj);
}

ECode AsyncChannel::SendMessageSynchronously(
    /* [in] */ IMessage* msg,
    /* [out] */ IMessage** _resultMsg)
{
    VALIDATE_NOT_NULL(_resultMsg)
    AutoPtr<IMessage> resultMsg = SyncMessenger::SendMessageSynchronously(mDstMessenger, msg);
    *_resultMsg = resultMsg;
    REFCOUNT_ADD(*_resultMsg)
    return NOERROR;
}

ECode AsyncChannel::SendMessageSynchronously(
    /* [in] */ Int32 what,
    /* [out] */ IMessage** resultMsg)
{
    return SendMessageSynchronously(what, 0, resultMsg);
}

ECode AsyncChannel::SendMessageSynchronously(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [out] */ IMessage** resultMsg)
{
    return SendMessageSynchronously(what, arg1, 0, resultMsg);
}

ECode AsyncChannel::SendMessageSynchronously(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [out] */ IMessage** resultMsg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(what);
    msg->SetArg1(arg1);
    msg->SetArg2(arg2);
    return SendMessageSynchronously(msg, resultMsg);
}

ECode AsyncChannel::SendMessageSynchronously(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** resultMsg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(what);
    msg->SetObj(obj);
    return SendMessageSynchronously(msg, resultMsg);
}

void AsyncChannel::ReplyHalfConnected(
    /* [in] */ Int32 status)
{
    AutoPtr<IMessage> msg;
    mSrcHandler->ObtainMessage(CMD_CHANNEL_HALF_CONNECTED, (IMessage**)&msg);
    msg->SetArg1(status);
    msg->SetObj(TO_IINTERFACE(this));
    msg->SetReplyTo(mDstMessenger);

    /*
     * Link to death only when bindService isn't used.
     */
    if (mConnection == NULL) {
        mDeathMonitor = new DeathMonitor(this);
        AutoPtr<IBinder> binder;
        mDstMessenger->GetBinder((IBinder**)&binder);
        AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
        //only when the dst is not local
        if (proxy != NULL) {
            if (FAILED(proxy->LinkToDeath(mDeathMonitor, 0))) {
                mDeathMonitor = NULL;
                // Override status to indicate failure
                msg->SetArg1(STATUS_BINDING_UNSUCCESSFUL);
            }
            else mDeathMonitor->LinkToDeath();
        }
    }

    Boolean result;
    mSrcHandler->SendMessage(msg, &result);
}

void AsyncChannel::ReplyDisconnected(
    /* [in] */ Int32 status)
{
    AutoPtr<IMessage> msg;
    mSrcHandler->ObtainMessage(CMD_CHANNEL_DISCONNECTED, (IMessage**)&msg);
    msg->SetArg1(status);
    msg->SetObj(TO_IINTERFACE(this));
    msg->SetReplyTo(mDstMessenger);
    Boolean result;
    mSrcHandler->SendMessage(msg, &result);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
