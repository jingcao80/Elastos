#include "CAllJoynService.h"
#include "CChatApplication.h"

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
    return NOERROR;
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
    /* [in] */ CAllJoynService* host);

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

const String CAllJoynService::NAME_PREFIX;// = "org.alljoyn.bus.samples.chat";
const Int16 CAllJoynService::CONTACT_PORT;// = 27;
const String CAllJoynService::OBJECT_PATH;// = "/chatService";

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
    return NOERROR;
}

ECode CAllJoynService::OnCreate()
{
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
    /* [in] */ const String string)
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