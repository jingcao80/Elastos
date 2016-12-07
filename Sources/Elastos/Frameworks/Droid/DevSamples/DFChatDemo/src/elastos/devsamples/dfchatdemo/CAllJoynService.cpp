#include "CAllJoynService.h"
#include "CChatApplication.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::EIID_IBusObject;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("CAllJoynService");

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

void CAllJoynService::CAllJoynService::BackgroundHandler::Exit()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::Connect()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::Disconnect()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::StartDiscovery()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::CancelDiscovery()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::RequestName()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::ReleaseName()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::BindSession()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::UnbindSession()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::Advertise()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::CancelAdvertise()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::JoinSession()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::LeaveSession()
{

}

void CAllJoynService::CAllJoynService::BackgroundHandler::SendMessages()
{

}

ECode CAllJoynService::BackgroundHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
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
    return NOERROR;
}

ECode CAllJoynService::ChatBusListener::LostAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    return NOERROR;
}


//===================================================================
// CAllJoynService::ChatService
//===================================================================

CAR_INTERFACE_IMPL(CAllJoynService::ChatService, IChatInterface, IBusObject)

CAllJoynService::ChatService::ChatService(
    /* [in] */ CAllJoynService* host)
    : mHost(host)
{
}

ECode CAllJoynService::ChatService::Chat(
    /* [in] */ const String& str)
{
    return NOERROR;
}


//===================================================================
// CAllJoynService
//===================================================================

const Int32 CAllJoynService::NOTIFICATION_ID = 0xdefaced;
const Int32 CAllJoynService::HANDLE_APPLICATION_QUIT_EVENT = 0;
const Int32 CAllJoynService::HANDLE_USE_JOIN_CHANNEL_EVENT = 1;
const Int32 CAllJoynService::HANDLE_USE_LEAVE_CHANNEL_EVENT = 2;
const Int32 CAllJoynService::HANDLE_HOST_INIT_CHANNEL_EVENT = 3;
const Int32 CAllJoynService::HANDLE_HOST_START_CHANNEL_EVENT = 4;
const Int32 CAllJoynService::HANDLE_HOST_STOP_CHANNEL_EVENT = 5;
const Int32 CAllJoynService::HANDLE_OUTBOUND_CHANGED_EVENT = 6;

const Int32 CAllJoynService::EXIT = 1;
const Int32 CAllJoynService::CONNECT = 2;
const Int32 CAllJoynService::DISCONNECT = 3;
const Int32 CAllJoynService::START_DISCOVERY = 4;
const Int32 CAllJoynService::CANCEL_DISCOVERY = 5;
const Int32 CAllJoynService::REQUEST_NAME = 6;
const Int32 CAllJoynService::RELEASE_NAME = 7;
const Int32 CAllJoynService::BIND_SESSION = 8;
const Int32 CAllJoynService::UNBIND_SESSION = 9;
const Int32 CAllJoynService::ADVERTISE = 10;
const Int32 CAllJoynService::CANCEL_ADVERTISE = 11;
const Int32 CAllJoynService::JOIN_SESSION = 12;
const Int32 CAllJoynService::LEAVE_SESSION = 13;
const Int32 CAllJoynService::SEND_MESSAGES = 14;

const String CAllJoynService::NAME_PREFIX("org.alljoyn.bus.samples.chat");
const Int16 CAllJoynService::CONTACT_PORT = 27;
const String CAllJoynService::OBJECT_PATH("/chatService");

CAR_INTERFACE_IMPL_2(CAllJoynService, Service, IAllJoynService, IObserver)

CAllJoynService::CAllJoynService()
{
}

CAllJoynService::~CAllJoynService()
{
}

ECode CAllJoynService::constructor()
{
    return NOERROR;
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
    // StartBusThread();
    // AutoPtr<IApplication> app;
    // GetApplication((IApplication**)&app);
    // mChatApplication = (CChatApplication*)app.Get();
    // mChatApplication->AddObserver(this);

    // AutoPtr<ICharSequence> title = CoreUtils::Convert("AllJoyn");
    // AutoPtr<ICharSequence> message = CoreUtils::Convert("Chat Channel Hosting Service.");
    // AutoPtr<IIntent> intent;
    // CIntent::New(this, ECLSID_CTabWidget, (IIntent**)&intent);

    // PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, 0);
    // Notification notification = new Notification(R.drawable.icon, null, System.currentTimeMillis());
    // notification.setLatestEventInfo(this, title, message, pendingIntent);
    // notification.flags |= Notification.DEFAULT_SOUND | Notification.FLAG_ONGOING_EVENT | Notification.FLAG_NO_CLEAR;

    // Logger::I(TAG, "onCreate(): startForeground()");
    // startForeground(NOTIFICATION_ID, notification);

    // /*
    //  * We have an AllJoyn handler thread running at this time, so take
    //  * advantage of the fact to get connected to the bus and start finding
    //  * remote channel instances in the background while the rest of the app
    //  * is starting up.
    //  */
    // mBackgroundHandler.connect();
    // mBackgroundHandler.startDiscovery();
    return NOERROR;
}

ECode CAllJoynService::OnDestroy()
{
    return NOERROR;
}

ECode CAllJoynService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode CAllJoynService::Update(
    /* [in] */ IObservable* o,
    /* [in] */ IInterface* arg)
{
    return NOERROR;
}

ECode CAllJoynService::Chat(
    /* [in] */ const String& string)
{
    return NOERROR;
}


void CAllJoynService::StartBusThread()
{

}

void CAllJoynService::StopBusThread()
{

}

void CAllJoynService::DoConnect()
{

}

Boolean CAllJoynService::DoDisconnect()
{
    return FALSE;
}

void CAllJoynService::DoStartDiscovery()
{

}

void CAllJoynService::DoStopDiscovery()
{

}

void CAllJoynService::DoRequestName()
{

}

void CAllJoynService::DoReleaseName()
{

}

void CAllJoynService::DoBindSession()
{

}

void CAllJoynService::DoUnbindSession()
{

}

void CAllJoynService::DoAdvertise()
{

}

void CAllJoynService::DoCancelAdvertise()
{

}

void CAllJoynService::DoJoinSession()
{

}


void CAllJoynService::DoLeaveSession()
{

}


void CAllJoynService::DoSendMessages()
{

}



} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos