//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CAllJoynService.h"
#include "CChatApplication.h"
#include "CChatService.h"
#include "R.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CBusListener;
using Org::Alljoyn::Bus::CMutableInteger16Value;
using Org::Alljoyn::Bus::CMutableInteger32Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::EIID_IBusObject;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger16Value;
using Org::Alljoyn::Bus::IMutableInteger32Value;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::ISessionPortListener;
using Org::Alljoyn::Bus::IMessageContext;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::GlobalBroadcast_Off;
using Org::Alljoyn::Bus::ISignalEmitter;
using Org::Alljoyn::Bus::CSignalEmitter;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Core::IThread;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("CAllJoynService");

//========================================================
// CAllJoynService::MyHandler
//========================================================

CAllJoynService::MyHandler::MyHandler(
    /* [in] */ CAllJoynService* host)
    : mHost(host)
{}

ECode CAllJoynService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
    case CAllJoynService::HANDLE_APPLICATION_QUIT_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_APPLICATION_QUIT_EVENT");
            mHost->mBackgroundHandler->LeaveSession();
            mHost->mBackgroundHandler->CancelAdvertise();
            mHost->mBackgroundHandler->UnbindSession();
            mHost->mBackgroundHandler->ReleaseName();
            mHost->mBackgroundHandler->Exit();
            mHost->StopSelf();
        }
        break;
    case CAllJoynService::HANDLE_USE_JOIN_CHANNEL_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): USE_JOIN_CHANNEL_EVENT");
            mHost->mBackgroundHandler->JoinSession();
        }
        break;
    case CAllJoynService::HANDLE_USE_LEAVE_CHANNEL_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): USE_LEAVE_CHANNEL_EVENT");
            mHost->mBackgroundHandler->LeaveSession();
        }
        break;
    case CAllJoynService::HANDLE_HOST_INIT_CHANNEL_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HOST_INIT_CHANNEL_EVENT");
        }
        break;
    case CAllJoynService::HANDLE_HOST_START_CHANNEL_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HOST_START_CHANNEL_EVENT");
            mHost->mBackgroundHandler->RequestName();
            mHost->mBackgroundHandler->BindSession();
            mHost->mBackgroundHandler->Advertise();
        }
        break;
    case CAllJoynService::HANDLE_HOST_STOP_CHANNEL_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HOST_STOP_CHANNEL_EVENT");
            mHost->mBackgroundHandler->CancelAdvertise();
            mHost->mBackgroundHandler->UnbindSession();
            mHost->mBackgroundHandler->ReleaseName();
        }
        break;
    case CAllJoynService::HANDLE_OUTBOUND_CHANGED_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): OUTBOUND_CHANGED_EVENT");
            mHost->mBackgroundHandler->SendMessages();
        }
        break;
    default:
        break;
    }

    return NOERROR;
}

//===================================================================
// CAllJoynService::BackgroundHandler
//===================================================================

CAllJoynService::BackgroundHandler::BackgroundHandler()
{}

ECode CAllJoynService::BackgroundHandler::constructor(
    /* [in] */ CAllJoynService* service,
    /* [in] */ ILooper* looper)
{
    mHost = service;
    return Handler::constructor(looper);
}

void CAllJoynService::BackgroundHandler::SendMessageInner(
    /* [in] */ Int32 id)
{
    AutoPtr<IMessage> message;
    mHost->mBackgroundHandler->ObtainMessage(id, (IMessage**)&message);
    Boolean bval;
    mHost->mBackgroundHandler->SendMessage(message, &bval);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::Exit()
{
    Logger::I(TAG, "mBackgroundHandler.exit()");
    SendMessageInner(CAllJoynService::EXIT);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::Connect()
{
    Logger::I(TAG, "mBackgroundHandler.Connect()");
    SendMessageInner(CAllJoynService::CONNECT);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::Disconnect()
{
    Logger::I(TAG, "mBackgroundHandler.Disconnect()");
    SendMessageInner(CAllJoynService::DISCONNECT);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::StartDiscovery()
{
    Logger::I(TAG, "mBackgroundHandler.StartDiscovery()");
    SendMessageInner(CAllJoynService::START_DISCOVERY);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::CancelDiscovery()
{
    Logger::I(TAG, "mBackgroundHandler.CancelDiscovery()");
    SendMessageInner(CAllJoynService::CANCEL_DISCOVERY);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::RequestName()
{
    Logger::I(TAG, "mBackgroundHandler.RequestName()");
    SendMessageInner(CAllJoynService::REQUEST_NAME);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::ReleaseName()
{
    Logger::I(TAG, "mBackgroundHandler.ReleaseName()");
    SendMessageInner(CAllJoynService::RELEASE_NAME);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::BindSession()
{
    Logger::I(TAG, "mBackgroundHandler.BindSession()");
    SendMessageInner(CAllJoynService::BIND_SESSION);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::UnbindSession()
{
    Logger::I(TAG, "mBackgroundHandler.UnbindSession()");
    SendMessageInner(CAllJoynService::UNBIND_SESSION);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::Advertise()
{
    Logger::I(TAG, "mBackgroundHandler.Advertise()");
    SendMessageInner(CAllJoynService::ADVERTISE);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::CancelAdvertise()
{
    Logger::I(TAG, "mBackgroundHandler.CancelAdvertise()");
    SendMessageInner(CAllJoynService::CANCEL_ADVERTISE);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::JoinSession()
{
    Logger::I(TAG, "mBackgroundHandler.JoinSession()");
    SendMessageInner(CAllJoynService::JOIN_SESSION);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::LeaveSession()
{
    Logger::I(TAG, "mBackgroundHandler.LeaveSession()");
    SendMessageInner(CAllJoynService::LEAVE_SESSION);
}

void CAllJoynService::CAllJoynService::BackgroundHandler::SendMessages()
{
    Logger::I(TAG, "mBackgroundHandler.SendMessages()");
    SendMessageInner(CAllJoynService::SEND_MESSAGES);
}

ECode CAllJoynService::BackgroundHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::I(TAG, "mBackgroundHandler.HandleMessage() : %d", what);

    switch (what) {
    case CAllJoynService::CONNECT:
        mHost->DoConnect();
        break;
    case CAllJoynService::DISCONNECT:
        mHost->DoDisconnect();
        break;
    case CAllJoynService::START_DISCOVERY:
        mHost->DoStartDiscovery();
        break;
    case CAllJoynService::CANCEL_DISCOVERY:
        mHost->DoStopDiscovery();
        break;
    case CAllJoynService::REQUEST_NAME:
        mHost->DoRequestName();
        break;
    case CAllJoynService::RELEASE_NAME:
        mHost->DoReleaseName();
        break;
    case CAllJoynService::BIND_SESSION:
        mHost->DoBindSession();
        break;
    case CAllJoynService::UNBIND_SESSION:
        mHost->DoUnbindSession();
        break;
    case CAllJoynService::ADVERTISE:
        mHost->DoAdvertise();
        break;
    case CAllJoynService::CANCEL_ADVERTISE:
        mHost->DoCancelAdvertise();
        break;
    case CAllJoynService::JOIN_SESSION:
        mHost->DoJoinSession();
        break;
    case CAllJoynService::LEAVE_SESSION:
        mHost->DoLeaveSession();
        break;
    case CAllJoynService::SEND_MESSAGES:
        mHost->DoSendMessages();
        break;
    case CAllJoynService::EXIT: {
        AutoPtr<ILooper> looper;
        GetLooper((ILooper**)&looper);
        looper->Quit();
        break;

    }
    default:
        break;
    }
    return NOERROR;
}

//===================================================================
// CAllJoynService::ChatBusListener
//===================================================================

CAllJoynService::ChatBusListener::ChatBusListener(
    /* [in] */ CAllJoynService* host)
    : mHost(host)
{}

ECode CAllJoynService::ChatBusListener::FoundAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    Logger::I(TAG, "mBusListener.foundAdvertisedName(%s)", name.string());
    AutoPtr<IApplication> app;
    mHost->GetApplication((IApplication**)&app);
    CChatApplication* chatApp = (CChatApplication*)app.Get();
    chatApp->AddFoundChannel(name);
    return NOERROR;
}

ECode CAllJoynService::ChatBusListener::LostAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    Logger::I(TAG, "mBusListener.lostAdvertisedName(%s)", name.string());
    AutoPtr<IApplication> app;
    mHost->GetApplication((IApplication**)&app);
    CChatApplication* chatApp = (CChatApplication*)app.Get();
    chatApp->RemoveFoundChannel(name);
    return NOERROR;
}


//===================================================================
// CAllJoynService::ChatService
//===================================================================

CAR_INTERFACE_IMPL_2(CAllJoynService::ChatService, Object, IChatInterface, IBusObject)

ECode CAllJoynService::ChatService::constructor()
{
    return NOERROR;
}

ECode CAllJoynService::ChatService::Chat(
    /* [in] */ const String& str)
{
    return NOERROR;
}


//===================================================================
// CAllJoynService
//===================================================================

const Int32 CAllJoynService::NOTIFICATION_ID;// = 0xdefaced;
const Int32 CAllJoynService::HANDLE_APPLICATION_QUIT_EVENT;// = 0;
const Int32 CAllJoynService::HANDLE_USE_JOIN_CHANNEL_EVENT;// = 1;
const Int32 CAllJoynService::HANDLE_USE_LEAVE_CHANNEL_EVENT;// = 2;
const Int32 CAllJoynService::HANDLE_HOST_INIT_CHANNEL_EVENT;// = 3;
const Int32 CAllJoynService::HANDLE_HOST_START_CHANNEL_EVENT;// = 4;
const Int32 CAllJoynService::HANDLE_HOST_STOP_CHANNEL_EVENT;// = 5;
const Int32 CAllJoynService::HANDLE_OUTBOUND_CHANGED_EVENT;// = 6;

const Int32 CAllJoynService::EXIT;// = 1;
const Int32 CAllJoynService::CONNECT;// = 2;
const Int32 CAllJoynService::DISCONNECT;// = 3;
const Int32 CAllJoynService::START_DISCOVERY;// = 4;
const Int32 CAllJoynService::CANCEL_DISCOVERY;// = 5;
const Int32 CAllJoynService::REQUEST_NAME;// = 6;
const Int32 CAllJoynService::RELEASE_NAME;// = 7;
const Int32 CAllJoynService::BIND_SESSION;// = 8;
const Int32 CAllJoynService::UNBIND_SESSION;// = 9;
const Int32 CAllJoynService::ADVERTISE;// = 10;
const Int32 CAllJoynService::CANCEL_ADVERTISE;// = 11;
const Int32 CAllJoynService::JOIN_SESSION;// = 12;
const Int32 CAllJoynService::LEAVE_SESSION;// = 13;
const Int32 CAllJoynService::SEND_MESSAGES;// = 14;

const String CAllJoynService::NAME_PREFIX("org.alljoyn.bus.samples.chat");
const Int16 CAllJoynService::CONTACT_PORT = 27;
const String CAllJoynService::OBJECT_PATH("/chatService");

CAR_INTERFACE_IMPL_2(CAllJoynService, Service, IAllJoynService, IObserver)

CAR_OBJECT_IMPL(CAllJoynService)

CAllJoynService::CAllJoynService()
    : mChatApplication(NULL)
    , mBusAttachmentState(BusAttachmentState_DISCONNECTED)
    , mHostChannelState(HostChannelState_IDLE)
    , mUseChannelState(UseChannelState_IDLE)
    , mHostSessionId(-1)
    , mJoinedToSelf(FALSE)
    , mUseSessionId(-1)
{
}

CAllJoynService::~CAllJoynService()
{
}

ECode CAllJoynService::constructor()
{
    return Service::constructor();
}

ECode CAllJoynService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = NULL;
    Logger::I(TAG, "onBind()");
    return NOERROR;
}

ECode CAllJoynService::OnCreate()
{
    Logger::I(TAG, "onCreate()");

    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    AutoPtr<IDaemonInit> di;
    CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
    Boolean result;
    di->PrepareDaemon(context, &result);

    mHandler = new MyHandler(this);
    mHandler->constructor();

    mBusListener = new ChatBusListener(this);
    CChatService::New((IChatInterface**)&mChatService);
    Logger::I(TAG, " >> mChatService: %s", TO_CSTR(mChatService));

    CBusAttachment::New(CChatApplication::PACKAGE_NAME, RemoteMessage_Receive,
        (IBusAttachment**)&mBus);

    StartBusThread();
    AutoPtr<IApplication> app;
    GetApplication((IApplication**)&app);
    mChatApplication = (CChatApplication*)app.Get();
    mChatApplication->AddObserver(this);

    AutoPtr<ICharSequence> title = CoreUtils::Convert("AllJoyn");
    AutoPtr<ICharSequence> message = CoreUtils::Convert("Chat Channel Hosting Service.");
    AutoPtr<IIntent> intent;
    CIntent::New(this, ECLSID_CTabWidget, (IIntent**)&intent);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 curms;
    sys->GetCurrentTimeMillis(&curms);
    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    AutoPtr<IPendingIntent> pendingIntent;
    piHelper->GetActivity(this, 0, intent, 0, (IPendingIntent**)&pendingIntent);

    AutoPtr<INotification> notification;
    CNotification::New(R::drawable::icon, NULL, curms, (INotification**)&notification);
    notification->SetLatestEventInfo(this, title, message, pendingIntent);
    Int32 flags;
    notification->GetFlags(&flags);
    notification->SetFlags(flags | INotification::DEFAULT_SOUND
        | INotification::FLAG_ONGOING_EVENT | INotification::FLAG_NO_CLEAR);

    Logger::I(TAG, "onCreate(): startForeground()");
    StartForeground(NOTIFICATION_ID, notification);

    /*
     * We have an AllJoyn handler thread running at this time, so take
     * advantage of the fact to get connected to the bus and start finding
     * remote channel instances in the background while the rest of the app
     * is starting up.
     */
    mBackgroundHandler->Connect();
    mBackgroundHandler->StartDiscovery();
    return NOERROR;
}

ECode CAllJoynService::OnDestroy()
{
    Logger::I(TAG, "onDestroy()");
    mBackgroundHandler->CancelDiscovery();
    mBackgroundHandler->Disconnect();
    StopBusThread();
    mChatApplication->DeleteObserver(this);
    return NOERROR;
}

ECode CAllJoynService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::I(TAG, "onStartCommand()");
    *result = START_STICKY;
    return NOERROR;
}

void CAllJoynService::SendMessage(
    /* [in] */ Int32 id)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(id, (IMessage**)&message);
    Boolean bval;
    mHandler->SendMessage(message, &bval);
}

ECode CAllJoynService::Update(
    /* [in] */ IObservable* o,
    /* [in] */ IInterface* arg)
{
    AutoLock lock(this);
    String qualifier = Object::ToString(arg);
    Logger::I(TAG, "update(%s)", qualifier.string());

    if (qualifier.Equals(CChatApplication::APPLICATION_QUIT_EVENT)) {
        SendMessage(HANDLE_APPLICATION_QUIT_EVENT);
    }

    if (qualifier.Equals(CChatApplication::USE_JOIN_CHANNEL_EVENT)) {
        SendMessage(HANDLE_USE_JOIN_CHANNEL_EVENT);
    }

    if (qualifier.Equals(CChatApplication::USE_LEAVE_CHANNEL_EVENT)) {
        SendMessage(HANDLE_USE_LEAVE_CHANNEL_EVENT);
    }

    if (qualifier.Equals(CChatApplication::HOST_INIT_CHANNEL_EVENT)) {
        SendMessage(HANDLE_HOST_INIT_CHANNEL_EVENT);
    }

    if (qualifier.Equals(CChatApplication::HOST_START_CHANNEL_EVENT)) {
        SendMessage(HANDLE_HOST_START_CHANNEL_EVENT);
    }

    if (qualifier.Equals(CChatApplication::HOST_STOP_CHANNEL_EVENT)) {
        SendMessage(HANDLE_HOST_STOP_CHANNEL_EVENT);
    }

    if (qualifier.Equals(CChatApplication::OUTBOUND_CHANGED_EVENT)) {
        SendMessage(HANDLE_OUTBOUND_CHANGED_EVENT);
    }
    return NOERROR;
}

ECode CAllJoynService::Chat(
    /* [in] */ const String& string)
{
    /*
     * See the long comment in doJoinSession() for more explanation of
     * why this is needed.
     *
     * The only time we allow a signal from the hosted session ID to pass
     * through is if we are in mJoinedToSelf state.  If the source of the
     * signal is us, we also filter out the signal since we are going to
     * locally echo the signal.

     */
    String uniqueName;
    mBus->GetUniqueName(&uniqueName);
    AutoPtr<IMessageContext> ctx;
    mBus->GetMessageContext((IMessageContext**)&ctx);
    Int32 sessionId;
    ctx->GetSessionId(&sessionId);
    String sender;
    ctx->GetSender(&sender);
    Logger::I(TAG, "Chat(): use sessionId is %d", mUseSessionId);
    Logger::I(TAG, "Chat(): message sessionId is %d", sessionId);

    /*
     * Always drop our own signals which may be echoed back from the system.
     */
    if (sender.Equals(uniqueName)) {
        Logger::I(TAG, "Chat(): dropped our own signal received on session %08x", sessionId);
        return NOERROR;
    }

    /*
     * Drop signals on the hosted session unless we are joined-to-self.
     */
    if (mJoinedToSelf == FALSE && sessionId == mHostSessionId) {
        Logger::I(TAG, "Chat(): dropped signal received on hosted session %08x when not joined-to-self", sessionId);
        return NOERROR;
    }

    /*
     * To keep the application simple, we didn't force users to choose a
     * nickname.  We want to identify the message source somehow, so we
     * just use the unique name of the sender's bus attachment.
     */
    String nickname = sender.Substring(sender.GetLength() - 10, sender.GetLength());

    Logger::I(TAG, "Chat(): signal [%s] received from nickname [%s]", string.string(), nickname.string());
    mChatApplication->NewRemoteUserMessage(nickname, string);
    return NOERROR;
}


void CAllJoynService::StartBusThread()
{
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BackgroundHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBackgroundHandler = new BackgroundHandler();
    mBackgroundHandler->constructor(this, looper);
}

void CAllJoynService::StopBusThread()
{
    mBackgroundHandler->Exit();
}

void CAllJoynService::DoConnect()
{
    Logger::I(TAG, "doConnect()");

    assert(mBusAttachmentState == BusAttachmentState_DISCONNECTED);
    mBus->UseOSLogging(TRUE);
    mBus->SetDebugLevel(String("ALLJOYN_JAVA"), 7);
    mBus->RegisterBusListener(mBusListener);

    /*
     * To make a service available to other AllJoyn peers, first
     * register a BusObject with the BusAttachment at a specific
     * object path.  Our service is implemented by the ChatService
     * BusObject found at the "/chatService" object path.
     */
    ECode ec = mBus->RegisterBusObject(IBusObject::Probe(mChatService), OBJECT_PATH);
    if (ec != (ECode)E_STATUS_OK) {
        StringBuilder sb("Unable to register the chat bus object: (");
        sb += ec; sb += ")";
        mChatApplication->AlljoynError(Module_HOST, sb.ToString());
        return;
    }

    ec = mBus->Connect();
    if (ec != (ECode)E_STATUS_OK) {
        StringBuilder sb("Unable to connect to the bus: (");
        sb += ec; sb += ")";
        mChatApplication->AlljoynError(Module_GENERAL, sb.ToString());
        return;
    }

    ec = mBus->RegisterSignalHandlers((IAllJoynService*)this);
    if (ec != (ECode)E_STATUS_OK) {
        StringBuilder sb("Unable to register signal handlers: (");
        sb += ec; sb += ")";
        Logger::E(TAG, sb.ToString());
        mChatApplication->AlljoynError(Module_GENERAL, sb.ToString());
        return;
    }

    mBusAttachmentState = BusAttachmentState_CONNECTED;
}

Boolean CAllJoynService::DoDisconnect()
{
    Logger::I(TAG, "doDisonnect()");
    assert(mBusAttachmentState == BusAttachmentState_CONNECTED);
    mBus->UnregisterBusListener(mBusListener);
    mBus->Disconnect();
    mBusAttachmentState = BusAttachmentState_DISCONNECTED;
    return TRUE;
}

void CAllJoynService::DoStartDiscovery()
{
    Logger::I(TAG, "doStartDiscovery()");
    assert(mBusAttachmentState == BusAttachmentState_CONNECTED);
    ECode status = mBus->FindAdvertisedName(NAME_PREFIX);
    if (status == (ECode)E_STATUS_OK) {
        mBusAttachmentState = BusAttachmentState_DISCOVERING;
        return;
    }
    else {
        StringBuilder sb("Unable to start finding advertised names: (");
        sb += status; sb += ")";
        mChatApplication->AlljoynError(Module_USE, sb.ToString());
        return;
    }
}

void CAllJoynService::DoStopDiscovery()
{
    Logger::I(TAG, "doStopDiscovery()");
    assert(mBusAttachmentState == BusAttachmentState_CONNECTED);
    mBus->CancelFindAdvertisedName(NAME_PREFIX);
    mBusAttachmentState = BusAttachmentState_CONNECTED;
}

void CAllJoynService::DoRequestName()
{
    Logger::I(TAG, "doRequestName()");

    /*
     * In order to request a name, the bus attachment must at least be
     * connected.
     */
    Int32 stateRelation = mBusAttachmentState - BusAttachmentState_DISCONNECTED;
    assert(stateRelation >= 0);

    /*
     * We depend on the user interface and model to work together to not
     * get this process started until a valid name is set in the channel name.
     */
    StringBuilder sb(NAME_PREFIX);
    sb += ".";
    sb += mChatApplication->HostGetChannelName();
    String wellKnownName = sb.ToString();
    ECode status = mBus->RequestName(wellKnownName, IBusAttachment::ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE);
    if (status == (ECode)E_STATUS_OK) {
        mHostChannelState = HostChannelState_NAMED;
        mChatApplication->HostSetChannelState(mHostChannelState);
    }
    else {
        StringBuilder sb("Unable to acquire well-known name: [");
        sb += wellKnownName; sb += "], (";
        sb += status; sb += ")";
        mChatApplication->AlljoynError(Module_USE, sb.ToString());
    }
}

void CAllJoynService::DoReleaseName()
{
    Logger::I(TAG, "doReleaseName()");

    /*
     * In order to release a name, the bus attachment must at least be
     * connected.
     */
    Int32 stateRelation = mBusAttachmentState - BusAttachmentState_DISCONNECTED;
    assert (stateRelation >= 0);
    assert(mBusAttachmentState == BusAttachmentState_CONNECTED || mBusAttachmentState == BusAttachmentState_DISCOVERING);

    /*
     * We need to progress monotonically down the hosted channel states
     * for sanity.
     */
    assert(mHostChannelState == HostChannelState_NAMED);

    /*
     * We depend on the user interface and model to work together to not
     * change the name out from under us while we are running.
     */
    StringBuilder sb(NAME_PREFIX);
    sb += ".";
    sb += mChatApplication->HostGetChannelName();
    String wellKnownName = sb.ToString();

    /*
     * There's not a lot we can do if the bus attachment refuses to release
     * the name.  It is not a fatal error, though, if it doesn't.  This is
     * because bus attachments can have multiple names.
     */
    mBus->ReleaseName(wellKnownName);
    mHostChannelState = HostChannelState_IDLE;
    mChatApplication->HostSetChannelState(mHostChannelState);
}

//======================================================================
//  CAllJoynService::InnerSessionPortListener
//======================================================================
/**
 * This method is called when a client tries to join the session
 * we have bound.  It asks us if we want to accept the client into
 * our session.
 *
 * In the class documentation for the SessionPortListener note that
 * it is a requirement for this method to be multithread safe.
 * Since we never access any shared state, this requirement is met.
 */
ECode CAllJoynService::InnerSessionPortListener::AcceptSessionJoiner(
    /* [in] */ Int16 sessionPort,
    /* [in] */ const String& joiner,
    /* [in] */ ISessionOpts* opts,
    /* [out] */ Boolean* accepted)
{
    Logger::I(TAG, "SessionPortListener.acceptSessionJoiner(%d, %s, %s)", sessionPort, joiner.string(), TO_CSTR(opts));
    VALIDATE_NOT_NULL(accepted);
    if (sessionPort == CONTACT_PORT) {
        *accepted = TRUE;
    }
    else {
        *accepted = FALSE;
    }
    return NOERROR;
}

/**
 * If we return TRUE in acceptSessionJoiner, we admit a new client
 * into our session.  The session does not really exist until a
 * client joins, at which time the session is created and a session
 * ID is assigned.  This method communicates to us that this event
 * has happened, and provides the new session ID for us to use.
 *
 * In the class documentation for the SessionPortListener note that
 * it is a requirement for this method to be multithread safe.
 * Since we never access any shared state, this requirement is met.
 *
 * See comments in joinSession for why the hosted chat interface is
 * created here.
 */
ECode CAllJoynService::InnerSessionPortListener::SessionJoined(
    /* [in] */ Int16 sessionPort,
    /* [in] */ Int32 id,
    /* [in] */ const String& joiner)
{
    Logger::I(TAG, "SessionPortListener.sessionJoined(%d, %08x, %s)", sessionPort, id, joiner.string());
    mHost->mHostSessionId = id;
    AutoPtr<ISignalEmitter> emitter;
    CSignalEmitter::New(IBusObject::Probe(mHost->mChatService), id, GlobalBroadcast_Off, (ISignalEmitter**)&emitter);
    AutoPtr<IInterface> obj;
    emitter->GetInterface(EIID_IChatInterface, (IInterface**)&obj);
    mHost->mHostChatInterface = IChatInterface::Probe(obj);
    return NOERROR;
}

void CAllJoynService::DoBindSession()
{
    Logger::I(TAG, "doBindSession()");

    AutoPtr<IMutableInteger16Value> contactPort;
    CMutableInteger16Value::New(CONTACT_PORT, (IMutableInteger16Value**)&contactPort);
    AutoPtr<ISessionOpts> sessionOpts;
    CSessionOpts::New(ISessionOpts::TRAFFIC_MESSAGES, TRUE,
        ISessionOpts::PROXIMITY_ANY, ISessionOpts::TRANSPORT_ANY, (ISessionOpts**)&sessionOpts);

    AutoPtr<InnerSessionPortListener> listener = new InnerSessionPortListener(this);
    ECode status = mBus->BindSessionPort(contactPort, sessionOpts, listener);

    if (status == (ECode)E_STATUS_OK) {
        mHostChannelState = HostChannelState_BOUND;
        mChatApplication->HostSetChannelState(mHostChannelState);
    }
    else {
        StringBuilder sb("Unable to bind session contact port: (");
        sb += status; sb += ")";
        mChatApplication->AlljoynError(Module_HOST, sb.ToString());
        return;
    }
}

void CAllJoynService::DoUnbindSession()
{
    Logger::I(TAG, "doUnbindSession()");

    /*
     * There's not a lot we can do if the bus attachment refuses to unbind
     * our port.
     */
    mBus->UnbindSessionPort(CONTACT_PORT);
    mHostChatInterface = NULL;
    mHostChannelState = HostChannelState_NAMED;
    mChatApplication->HostSetChannelState(mHostChannelState);
}

void CAllJoynService::DoAdvertise()
{
    Logger::I(TAG, "doAdvertise()");

    /*
     * We depend on the user interface and model to work together to not
     * change the name out from under us while we are running.
     */
    StringBuilder sb(NAME_PREFIX);
    sb += ".";
    sb += mChatApplication->HostGetChannelName();
    String wellKnownName = sb.ToString();
    ECode status = mBus->AdvertiseName(wellKnownName, ISessionOpts::TRANSPORT_ANY);

    if (status == (ECode)E_STATUS_OK) {
        mHostChannelState = HostChannelState_ADVERTISED;
        mChatApplication->HostSetChannelState(mHostChannelState);
    }
    else {
        StringBuilder sb("Unable to advertise well-known name: [");
        sb += wellKnownName; sb += "], status:(";
        sb += status; sb += ")";
        mChatApplication->AlljoynError(Module_HOST, sb.ToString());
        return;
    }
}

void CAllJoynService::DoCancelAdvertise()
{
    Logger::I(TAG, "doCancelAdvertise()");

    /*
     * We depend on the user interface and model to work together to not
     * change the name out from under us while we are running.
     */
    StringBuilder sb(NAME_PREFIX);
    sb += ".";
    sb += mChatApplication->HostGetChannelName();
    String wellKnownName = sb.ToString();
    ECode status = mBus->CancelAdvertiseName(wellKnownName, ISessionOpts::TRANSPORT_ANY);

    if (status != (ECode)E_STATUS_OK) {
        StringBuilder sb("Unable to cancel advertisement of well-known name: [");
        sb += wellKnownName; sb += "], status:(";
        sb += status; sb += ")";
        mChatApplication->AlljoynError(Module_HOST, sb.ToString());
        return;
    }

    /*
     * There's not a lot we can do if the bus attachment refuses to cancel
     * our advertisement, so we don't bother to even get the status.
     */
    mHostChannelState = HostChannelState_BOUND;
    mChatApplication->HostSetChannelState(mHostChannelState);
}

void CAllJoynService::DoJoinSession()
{
    Logger::I(TAG, "doJoinSession()");

    /*
     * There is a relatively non-intuitive behavior of multipoint sessions
     * that one needs to grok in order to understand the code below.  The
     * important thing to uderstand is that there can be only one endpoint
     * for a multipoint session in a particular bus attachment.  This
     * endpoint can be created explicitly by a call to joinSession() or
     * implicitly by a call to bindSessionPort().  An attempt to call
     * joinSession() on a session port we have created with bindSessionPort()
     * will result in an error.
     *
     * When we call bindSessionPort(), we do an implicit joinSession() and
     * thus signals (which correspond to our chat messages) will begin to
     * flow from the hosted chat channel as soon as we begin to host a
     * corresponding session.
     *
     * To achieve sane user interface behavior, we need to block those
     * signals from the implicit join done by the bind until our user joins
     * the bound chat channel.  If we do not do this, the chat messages
     * from the chat channel hosted by the application will appear in the
     * chat channel joined by the application.
     *
     * Since the messages flow automatically, we can accomplish this by
     * turning a filter on and off in the chat signal handler.  So if we
     * detect that we are hosting a channel, and we find that we want to
     * join the hosted channel we turn the filter off.
     *
     * We also need to be able to send chat messages to the hosted channel.
     * This means we need to point the mChatInterface at the session ID of
     * the hosted session.  There is another complexity here since the
     * hosted session doesn't exist until a remote session has joined.
     * This means that we don't have a session ID to use to create a
     * SignalEmitter until a remote device does a joinSession on our
     * hosted session.  This, in turn, means that we have to create the
     * SignalEmitter after we get a sessionJoined() callback in the
     * SessionPortListener passed into bindSessionPort().  We chose to
     * create the signal emitter for this case in the sessionJoined()
     * callback itself.  Note that this hosted channel signal emitter
     * must be distinct from one constructed for the usual joinSession
     * since a hosted channel may have a remote device do a join at any
     * time, even when we are joined to another session.  If they were
     * not separated, a remote join on the hosted session could redirect
     * messages from the joined session unexpectedly.
     *
     * So, to summarize, these next few lines handle a relatively complex
     * case.  When we host a chat channel, we do a bindSessionPort which
     * *enables* the creation of a session.  As soon as a remote device
     * joins the hosted chat channel, a session is actually created, and
     * the SessionPortListener sessionJoined() callback is fired.  At that
     * point, we create a separate SignalEmitter using the hosted session's
     * sessionId that we can use to send chat messages to the channel we
     * are hosting.  As soon as the session comes up, we begin receiving
     * chat messages from the session, so we need to filter them until the
     * user joins the hosted chat channel.  In a separate timeline, the
     * user can decide to join the chat channel she is hosting.  She can
     * do so either before or after the corresponding session has been
     * created as a result of a remote device joining the hosted session.
     * If she joins the hosted channel before the underlying session is
     * created, her chat messages will be discarded.  If she does so after
     * the underlying session is created, there will be a session emitter
     * waiting to use to send chat messages.  In either case, the signal
     * filter will be turned off in order to listen to remote chat
     * messages.
     */
    if (mHostChannelState != HostChannelState_IDLE) {
        if (mChatApplication->UseGetChannelName().Equals(mChatApplication->HostGetChannelName())) {
            mUseChannelState = UseChannelState_JOINED;
            mChatApplication->UseSetChannelState(mUseChannelState);
            mJoinedToSelf = TRUE;
            return;
        }
    }

    /*
     * We depend on the user interface and model to work together to provide
     * a reasonable name.
     */
    StringBuilder sb(NAME_PREFIX);
    sb += ".";
    sb += mChatApplication->UseGetChannelName();
    String wellKnownName = sb.ToString();

    /*
     * Since we can act as the host of a channel, we know what the other
     * side is expecting to see.
     */
    Int16 contactPort = CONTACT_PORT;
    AutoPtr<ISessionOpts> sessionOpts;
    CSessionOpts::New(ISessionOpts::TRAFFIC_MESSAGES, TRUE,
        ISessionOpts::PROXIMITY_ANY, ISessionOpts::TRANSPORT_ANY, (ISessionOpts**)&sessionOpts);
    AutoPtr<IMutableInteger32Value> sessionId;
    CMutableInteger32Value::New((IMutableInteger32Value**)&sessionId);

    AutoPtr<InnerSessionListener> sessionListener = new InnerSessionListener(this);
    ECode status = mBus->JoinSession(wellKnownName, contactPort, sessionId, sessionOpts, sessionListener);

    if (status == (ECode)E_STATUS_OK) {
        sessionId->GetValue(&mUseSessionId);
        Logger::I(TAG, "doJoinSession(): use sessionId is %d", mUseSessionId);
    }
    else {
        StringBuilder sb("Unable to join chat session: (");
        sb += status; sb += ")";
        mChatApplication->AlljoynError(Module_USE, sb.ToString());
        return;
    }

    AutoPtr<ISignalEmitter> emitter;
    CSignalEmitter::New(IBusObject::Probe(mChatService), mUseSessionId, GlobalBroadcast_Off, (ISignalEmitter**)&emitter);
    AutoPtr<IInterface> obj;
    emitter->GetInterface(EIID_IInterface, (IInterface**)&obj);
    mChatInterface = IChatInterface::Probe(obj);

    mUseChannelState = UseChannelState_JOINED;
    mChatApplication->UseSetChannelState(mUseChannelState);
}

//======================================================================
//  CAllJoynService::InnerSessionListener
//======================================================================
ECode CAllJoynService::InnerSessionListener::SessionLost(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 reason)
{
    Logger::I(TAG, "InnerSessionListener.sessionLost(sessionId = %d, reason = %d)", sessionId, reason);
    mHost->mChatApplication->AlljoynError(Module_USE, String("The chat session has been lost"));
    mHost->mUseChannelState = UseChannelState_IDLE;
    mHost->mChatApplication->UseSetChannelState(mHost->mUseChannelState);
    return NOERROR;
}

void CAllJoynService::DoLeaveSession()
{
    Logger::I(TAG, "doLeaveSession()");
    if (mJoinedToSelf == FALSE) {
        mBus->LeaveSession(mUseSessionId);
    }
    mUseSessionId = -1;
    mJoinedToSelf = FALSE;
    mUseChannelState = UseChannelState_IDLE;
    mChatApplication->UseSetChannelState(mUseChannelState);
}

void CAllJoynService::DoSendMessages()
{
    Logger::I(TAG, "doSendMessages()");

    ECode status = NOERROR;
    String message;
    while ((message = mChatApplication->GetOutboundItem()) != NULL) {
        Logger::I(TAG, "doSendMessages(): sending message \"%s\"", message.string());
        /*
         * If we are joined to a remote session, we send the message over
         * the mChatInterface.  If we are implicityly joined to a session
         * we are hosting, we send the message over the mHostChatInterface.
         * The mHostChatInterface may or may not exist since it is created
         * when the sessionJoined() callback is fired in the
         * SessionPortListener, so we have to check for it.
         */
        if (mJoinedToSelf) {
            if (mHostChatInterface != NULL) {
                Logger::I(TAG, " >> mHostChatInterface %s: chat: %s",
                    TO_CSTR(mHostChatInterface), message.string());
                status = mHostChatInterface->Chat(message);
            }
        }
        else {
            Logger::I(TAG, " >> mChatInterface %s: chat: %s",
                TO_CSTR(mChatInterface), message.string());
            status = mChatInterface->Chat(message);
        }

        if (FAILED(status)) {
            StringBuilder sb("Bus exception while sending message: (");
            sb += status; sb += ")";
            mChatApplication->AlljoynError(Module_USE, sb.ToString());
        }
    }
}

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos