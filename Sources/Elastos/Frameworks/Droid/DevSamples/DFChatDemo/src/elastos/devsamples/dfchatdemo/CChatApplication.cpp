#include "CChatApplication.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Text::IDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
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

CAR_INTERFACE_IMPL(CChatApplication, Application, IObservable)

CAR_OBJECT_IMPL(CChatApplication)

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

    AutoPtr<IIntent> intent;
    CIntent::New(this, ECLSID_CAllJoynService, (IIntent**)&intent);
    StartService(intent, (IComponentName**)&mRunningService);
    if (mRunningService == NULL) {
        Logger::E(TAG, "onCreate(): failed to startService()");
    }
    return NOERROR;
}

ECode CChatApplication::Quit()
{
    NotifyObservers(APPLICATION_QUIT_EVENT);
    mRunningService = NULL;
    return NOERROR;
}

void CChatApplication::Checkin()
{
    Logger::I(TAG, "checkin()");
    if (mRunningService == NULL) {
        Logger::I(TAG, "checkin():  Starting the AllJoynService");
        AutoPtr<IIntent> intent;
        CIntent::New(this, ECLSID_CAllJoynService, (IIntent**)&intent);
        StartService(intent, (IComponentName**)&mRunningService);
        if (mRunningService == NULL) {
            Logger::I(TAG, "checkin(): failed to startService()");
        }
    }
}

void CChatApplication::AlljoynError(
    /* [in] */ Module m,
    /* [in] */ const String& s)
{
    Logger::E(TAG, " >> AlljoynError: %s", s.string());
    AutoLock lock(this);
    mModule = m;
    mErrorString = s;
    NotifyObservers(ALLJOYN_ERROR_EVENT);
}

Module CChatApplication::GetErrorModule()
{
    return mModule;
}

String CChatApplication::GetErrorString()
{
    return mErrorString;
}

void CChatApplication::AddFoundChannel(
    /* [in] */ const String& channel)
{
    AutoLock lock(this);
    Logger::I(TAG, "addFoundChannel(%s)", channel.string());
    RemoveFoundChannel(channel);
    mChannels.PushBack(channel);
    Logger::I(TAG, "addFoundChannel(): added %s", channel.string());
}

void CChatApplication::RemoveFoundChannel(
    /* [in] */ const String& channel)
{
    AutoLock lock(this);
    Logger::I(TAG, "removeFoundChannel(%s)", channel.string());

    List<String>::Iterator it;
    for (it = mChannels.Begin(); it != mChannels.End();) {
        if (channel.Equals(*it)) {
            it = mChannels.Erase(it);
            Logger::I(TAG, "removeFoundChannel(): removed %s", channel.string());
        }
        else {
            ++it;
        }
    }
}

AutoPtr< List<String> > CChatApplication::GetFoundChannels()
{
    AutoLock lock(this);
    Logger::I(TAG, "getFoundChannels()");

    AutoPtr< List<String> > list = new List<String>(mChannels);
    List<String>::Iterator it;
    for (it = list->Begin(); it != list->End(); ++it) {
        Logger::I(TAG, "getFoundChannels(): added %s", (*it).string());
    }
    return list;
}

void CChatApplication::HostSetChannelState(
    /* [in] */ HostChannelState state)
{
    AutoLock lock(this);
    mHostChannelState = state;
    NotifyObservers(HOST_CHANNEL_STATE_CHANGED_EVENT);
}

HostChannelState CChatApplication::HostGetChannelState()
{
    AutoLock lock(this);
    return mHostChannelState;
}

void CChatApplication::HostSetChannelName(
    /* [in] */ const String& name)
{
    AutoLock lock(this);
    mHostChannelName = name;
    NotifyObservers(HOST_CHANNEL_STATE_CHANGED_EVENT);
}

String CChatApplication::HostGetChannelName()
{
    AutoLock lock(this);
    return mHostChannelName;
}

void CChatApplication::UseSetChannelState(
    /* [in] */ UseChannelState state)
{
    AutoLock lock(this);
    mUseChannelState = state;
    NotifyObservers(USE_CHANNEL_STATE_CHANGED_EVENT);
}

UseChannelState CChatApplication::UseGetChannelState()
{
    AutoLock lock(this);
    return mUseChannelState;
}

void CChatApplication::UseSetChannelName(
    /* [in] */ const String& name)
{
    AutoLock lock(this);
    mUseChannelName = name;
    NotifyObservers(USE_CHANNEL_STATE_CHANGED_EVENT);
}

String CChatApplication::UseGetChannelName()
{
    AutoLock lock(this);
    return mUseChannelName;
}

void CChatApplication::UseJoinChannel()
{
    AutoLock lock(this);
    ClearHistory();
    NotifyObservers(USE_CHANNEL_STATE_CHANGED_EVENT);
    NotifyObservers(USE_JOIN_CHANNEL_EVENT);
}

void CChatApplication::UseLeaveChannel()
{
    AutoLock lock(this);
    NotifyObservers(USE_CHANNEL_STATE_CHANGED_EVENT);
    NotifyObservers(USE_LEAVE_CHANNEL_EVENT);
}

void CChatApplication::HostInitChannel()
{
    AutoLock lock(this);
    NotifyObservers(HOST_CHANNEL_STATE_CHANGED_EVENT);
    NotifyObservers(HOST_INIT_CHANNEL_EVENT);
}

void CChatApplication::HostStartChannel()
{
    AutoLock lock(this);
    NotifyObservers(HOST_CHANNEL_STATE_CHANGED_EVENT);
    NotifyObservers(HOST_START_CHANNEL_EVENT);
}

void CChatApplication::HostStopChannel()
{
    AutoLock lock(this);
    NotifyObservers(HOST_CHANNEL_STATE_CHANGED_EVENT);
    NotifyObservers(HOST_STOP_CHANNEL_EVENT);
}

void CChatApplication::NewLocalUserMessage(
    /* [in] */ const String& message)
{
    AutoLock lock(this);
    AddInboundItem(String("Me"), message);
    if (UseGetChannelState() == UseChannelState_JOINED) {
        AddOutboundItem(message);
    }
}

void CChatApplication::NewRemoteUserMessage(
    /* [in] */ const String& nickname,
    /* [in] */ const String& message)
{
    AutoLock lock(this);
    AddInboundItem(nickname, message);
}

String CChatApplication::GetOutboundItem()
{
    AutoLock lock(this);
    if (mOutbound.IsEmpty()) {
        return String(NULL);
    }
    else {
        List<String>::Iterator begin = mOutbound.Begin();
        String str = *begin;
        mOutbound.Erase(begin);
        return str;
    }
}

AutoPtr< List<String> > CChatApplication::GetHistory()
{
    AutoLock lock(this);
    AutoPtr< List<String> > clone = new List<String>(mHistory);
    return clone;
}

ECode CChatApplication::AddObserver(
    /* [in] */ IObserver* obs)
{
    AutoLock lock(this);
    Logger::I(TAG, "addObserver(%s)", TO_CSTR(obs));
    AutoPtr<IObserver> o = obs;
    List<AutoPtr<IObserver> >::Iterator it = Find(mObservers.Begin(), mObservers.End(), o);
    if (it == mObservers.End()) {
        mObservers.PushBack(o);
    }
    return NOERROR;
}

ECode CChatApplication::DeleteObserver(
    /* [in] */ IObserver* obs)
{
    AutoLock lock(this);
    Logger::I(TAG, "deleteObserver(%s)", TO_CSTR(obs));
    AutoPtr<IObserver> o = obs;
    mObservers.Remove(o);
    return NOERROR;
}

void CChatApplication::AddOutboundItem(
    /* [in] */ const String& message)
{
    if (mOutbound.GetSize() == OUTBOUND_MAX) {
        mOutbound.PopFront();
    }
    mOutbound.PushBack(message);
    NotifyObservers(OUTBOUND_CHANGED_EVENT);
}

void CChatApplication::AddInboundItem(
    /* [in] */ const String& nickname,
    /* [in] */ const String& message)
{
    AddHistoryItem(nickname, message);
}

void CChatApplication::AddHistoryItem(
    /* [in] */ const String& nickname,
    /* [in] */ const String& message)
{
    if (mHistory.GetSize() == HISTORY_MAX) {
        mHistory.PopFront();
    }

    AutoPtr<IDateFormat> dateFormat;
    CSimpleDateFormat::New(String("HH:mm"), (IDateFormat**)&dateFormat);
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    String dateStr;
    dateFormat->Format(date, &dateStr);
    StringBuilder sb("[");
    sb += dateStr;
    sb += "] (";
    sb += nickname;
    sb += ") : ";
    sb += message;
    mHistory.PushBack(sb.ToString());
    NotifyObservers(HISTORY_CHANGED_EVENT);
}

void CChatApplication::ClearHistory()
{
    mHistory.Clear();
    NotifyObservers(HISTORY_CHANGED_EVENT);
}

void CChatApplication::NotifyObservers(
    /* [in] */ const String& arg)
{
    Logger::I(TAG, "NotifyObservers(%s)", arg.string());
    List< AutoPtr<IObserver> >::Iterator it;
    for (it = mObservers.Begin(); it != mObservers.End(); ++it) {
        // Logger::I(TAG, "notify observer = %s", TO_CSTR(*it));
        (*it)->Update(this, CoreUtils::Convert(arg));
    }
}


} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos