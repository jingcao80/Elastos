#include "CChatApplication.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("DFChatDemo.CChatApplication");

String CChatApplication::PACKAGE_NAME;

const String CChatApplication::APPLICATION_QUIT_EVENT("APPLICATION_QUIT_EVENT");
const String CChatApplication::ALLJOYN_ERROR_EVENT("ALLJOYN_ERROR_EVENT");
const String CChatApplication::HOST_CHANNEL_STATE_CHANGED_EVENT("HOST_CHANNEL_STATE_CHANGED_EVENT");
const String CChatApplication::USE_CHANNEL_STATE_CHANGED_EVENT("USE_CHANNEL_STATE_CHANGED_EVENT");
const String CChatApplication::USE_JOIN_CHANNEL_EVENT("USE_JOIN_CHANNEL_EVENT");
const String CChatApplication::USE_LEAVE_CHANNEL_EVENT("USE_LEAVE_CHANNEL_EVENT");
const String CChatApplication::HOST_INIT_CHANNEL_EVENT("HOST_INIT_CHANNEL_EVENT");
const String CChatApplication::HOST_START_CHANNEL_EVENT("HOST_START_CHANNEL_EVENT");
const String CChatApplication::HOST_STOP_CHANNEL_EVENT("HOST_STOP_CHANNEL_EVENT");
const String CChatApplication::OUTBOUND_CHANGED_EVENT("OUTBOUND_CHANGED_EVENT");
const String CChatApplication::HISTORY_CHANGED_EVENT("HISTORY_CHANGED_EVENT");

const Int32 CChatApplication::OUTBOUND_MAX = 5;
const Int32 CChatApplication::HISTORY_MAX = 20;

CChatApplication::CChatApplication()
    : mModule(Module_NONE)
    , mErrorString("ER_OK")
    , mBusAttachmentState(BusAttachmentState_DISCONNECTED)
    , mHostChannelState(HostChannelState_IDLE)
    , mUseChannelState(UseChannelState_IDLE)
{
}

CChatApplication::~CChatApplication()
{
}

ECode CChatApplication::constructor()
{
    return Application::constructor();
}

ECode CChatApplication::OnCreate()
{
    Logger::I(TAG, "onCreate()");

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    ctx->GetPackageName(&PACKAGE_NAME);

    // AutoPtr<IIntent> intent;
    // CIntent::New(this, ECLSID_CAllJoynService, (IIntent**)&intent);
    // StartService(intent, (IComponentName**)&mRunningService);
    // if (mRunningService == NULL) {
    //     Logger::E(TAG, "onCreate(): failed to startService()");
    // }
    return NOERROR;
}

ECode CChatApplication::Quit()
{
    return NOERROR;
}

void CChatApplication::Checkin()
{
}

void CChatApplication::AlljoynError(
    /* [in] */ Module m,
    /* [in] */ const String& s)
{
}

Module CChatApplication::GetErrorModule()
{
    Module state;
    return state;
}

String CChatApplication::GetErrorString()
{
    return String(NULL);
}

void CChatApplication::AddFoundChannel(
    /* [in] */ const String& channel)
{
}

void CChatApplication::RemoveFoundChannel(
    /* [in] */ const String& channel)
{
}

AutoPtr< List<String> > CChatApplication::GetFoundChannels()
{
    AutoPtr< List<String> > list;
    return list;
}

void CChatApplication::HostSetChannelState(
    /* [in] */ HostChannelState state)
{
}

HostChannelState CChatApplication::HostGetChannelState()
{
    HostChannelState state;
    return state;
}

void CChatApplication::HostSetChannelName(
    /* [in] */ const String& name)
{
}

String CChatApplication::HostGetChannelName()
{
    return String(NULL);
}

void CChatApplication::UseSetChannelState(
    /* [in] */ UseChannelState state)
{
}

UseChannelState CChatApplication::UseGetChannelState()
{
    UseChannelState state;
    return state;
}

void CChatApplication::UseSetChannelName(
    /* [in] */ const String& name)
{
}

String CChatApplication::UseGetChannelName()
{
    return String(NULL);
}

void CChatApplication::UseJoinChannel()
{
}

void CChatApplication::UseLeaveChannel()
{
}

void CChatApplication::HostInitChannel()
{
}

void CChatApplication::HostStartChannel()
{
}

void CChatApplication::HostStopChannel()
{
}

void CChatApplication::NewLocalUserMessage(
    /* [in] */ const String& message)
{
}

void CChatApplication::NewRemoteUserMessage(
    /* [in] */ const String& nickname,
    /* [in] */ const String& message)
{
}

String CChatApplication::GetOutboundItem()
{
    return String(NULL);
}

AutoPtr< List<String> > CChatApplication::GetHistory()
{
    AutoPtr< List<String> > list;
    return list;
}

ECode CChatApplication::AddObserver(
    /* [in] */ IObserver* obs)
{
    return NOERROR;
}

ECode CChatApplication::DeleteObserver(
    /* [in] */ IObserver* obs)
{
    return NOERROR;
}

void CChatApplication::AddOutboundItem(
    /* [in] */ const String& message)
{
}

void CChatApplication::AddInboundItem(
    /* [in] */ const String& nickname,
    /* [in] */ const String& message)
{
}

void CChatApplication::AddHistoryItem(
    /* [in] */ const String& nickname,
    /* [in] */ const String& message)
{
}

void CChatApplication::ClearHistory()
{
}

void CChatApplication::NotifyObservers(
    /* [in] */ IInterface* arg)
{
}


} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos