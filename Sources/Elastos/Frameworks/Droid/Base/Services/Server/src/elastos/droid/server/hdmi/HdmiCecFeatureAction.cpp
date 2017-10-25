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

#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevicePlayback.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::IRunnable;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecFeatureAction::ActionTimerHandler
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecFeatureAction::ActionTimerHandler, Handler, IHdmiCecFeatureActionActionTimer)

HdmiCecFeatureAction::ActionTimerHandler::ActionTimerHandler(
    /* [in] */ HdmiCecFeatureAction* host)
    : mHost(host)
{}

ECode HdmiCecFeatureAction::ActionTimerHandler::constructor(
    /* [in] */ ILooper* looper)
{
    return Handler::constructor(looper);
}

ECode HdmiCecFeatureAction::ActionTimerHandler::SendTimerMessage(
    /* [in] */ Int32 state,
    /* [in] */ Int64 delayMillis)
{
    // The third argument(0) is not used.
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_TIMEOUT, state, 0, (IMessage**)&msg);
    Boolean bNoUse;
    return SendMessageDelayed(msg, delayMillis, &bNoUse);
}

ECode HdmiCecFeatureAction::ActionTimerHandler::ClearTimerMessage()
{
    return RemoveMessages(MSG_TIMEOUT);
}

ECode HdmiCecFeatureAction::ActionTimerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == MSG_TIMEOUT) {
        Int32 arg1;
        msg->GetArg1(&arg1);
        mHost->HandleTimerEvent(arg1);
    }
    else {
        Slogger::W(TAG, "Unsupported message:%d", what);
    }
    return NOERROR;
}

//=============================================================================
// HdmiCecFeatureAction
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecFeatureAction, Object, IHdmiCecFeatureAction)

const String HdmiCecFeatureAction::TAG("HdmiCecFeatureAction");
const Int32 HdmiCecFeatureAction::MSG_TIMEOUT = 100;
const Int32 HdmiCecFeatureAction::STATE_NONE = 0;

HdmiCecFeatureAction::HdmiCecFeatureAction()
    : mState(STATE_NONE)
{}

ECode HdmiCecFeatureAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source)
{
    mSource = source;
    ((HdmiCecLocalDevice*)mSource.Get())->GetService((IHdmiControlService**)&mService);
    AutoPtr<ILooper> serviceLooper;
    ((HdmiControlService*)mService.Get())->GetServiceLooper((ILooper**)&serviceLooper);
    CreateActionTimer(serviceLooper, (IHdmiCecFeatureActionActionTimer**)&mActionTimer);
    return NOERROR;
}

ECode HdmiCecFeatureAction::SetActionTimer(
    /* [in] */ IHdmiCecFeatureActionActionTimer* actionTimer)
{
    mActionTimer = actionTimer;
    return NOERROR;
}

ECode HdmiCecFeatureAction::CreateActionTimer(
    /* [in] */ ILooper* looper,
    /* [out] */ IHdmiCecFeatureActionActionTimer** result)
{
    VALIDATE_NOT_NULL(result)

    *result = new ActionTimerHandler(this);
    ((ActionTimerHandler*)(*result))->constructor(looper);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecFeatureAction::AddTimer(
    /* [in] */ Int32 state,
    /* [in] */ Int32 delayMillis)
{
    return mActionTimer->SendTimerMessage(state, delayMillis);
}

ECode HdmiCecFeatureAction::Started(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mState != STATE_NONE;
    return NOERROR;
}

ECode HdmiCecFeatureAction::SendCommand(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
}

ECode HdmiCecFeatureAction::SendCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    return ((HdmiControlService*)mService.Get())->SendCecCommand(cmd, callback);
}

ECode HdmiCecFeatureAction::AddAndStartAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->AddAndStartAction(action);
}

ECode HdmiCecFeatureAction::GetActions(
    /* [in] */ ClassID clazz,
    /* [out] */ IList** result)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->GetActions(clazz, result);
}

ECode HdmiCecFeatureAction::GetCecMessageCache(
    /* [out] */ HdmiCecMessageCache** result)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->GetCecMessageCache(result);
}

ECode HdmiCecFeatureAction::RemoveAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->RemoveAction(action);
}

ECode HdmiCecFeatureAction::RemoveAction(
    /* [in] */ ClassID clazz)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->RemoveActionExcept(clazz, NULL);
}

ECode HdmiCecFeatureAction::RemoveActionExcept(
    /* [in] */ ClassID clazz,
    /* [in] */ HdmiCecFeatureAction* exception)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->RemoveActionExcept(clazz, exception);
}

ECode HdmiCecFeatureAction::PollDevices(
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ Int32 pickStrategy,
    /* [in] */ Int32 retryCount)
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    ((HdmiControlService*)mService.Get())->PollDevices(callback, srcAddr, pickStrategy, retryCount);
    return NOERROR;
}

ECode HdmiCecFeatureAction::Clear()
{
    mState = STATE_NONE;
    // Clear all timers.
    mActionTimer->ClearTimerMessage();
    return NOERROR;
}

ECode HdmiCecFeatureAction::Finish()
{
    return Finish(TRUE);
}

ECode HdmiCecFeatureAction::Finish(
    /* [in] */ Boolean removeSelf)
{
    Clear();
    if (removeSelf) {
        RemoveAction(this);
    }
    if (mOnFinishedCallbacks != NULL) {
        AutoPtr<IIterator> it;
        mOnFinishedCallbacks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPair> actionCallbackPair = IPair::Probe(obj);
            AutoPtr<IInterface> first;
            actionCallbackPair->GetFirst((IInterface**)&first);
            if (((HdmiCecFeatureAction*)IObject::Probe(first))->mState != STATE_NONE) {
                AutoPtr<IInterface> second;
                actionCallbackPair->GetSecond((IInterface**)&second);
                IRunnable::Probe(second)->Run();
            }
        }
        mOnFinishedCallbacks = NULL;
    }
    return NOERROR;
}

ECode HdmiCecFeatureAction::LocalDevice(
    /* [out] */ IHdmiCecLocalDevice** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSource;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecFeatureAction::Playback(
    /* [out] */ HdmiCecLocalDevicePlayback** result)
{
    VALIDATE_NOT_NULL(result)

    *result = (HdmiCecLocalDevicePlayback*) mSource.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecFeatureAction::Tv(
    /* [out] */ IHdmiCecLocalDeviceTv** result)
{
    VALIDATE_NOT_NULL(result)

    *result = (HdmiCecLocalDeviceTv*) mSource.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecFeatureAction::GetSourceAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> info;
    ((HdmiCecLocalDevice*) mSource.Get())->GetDeviceInfo((IHdmiDeviceInfo**)&info);
    Int32 logicalAddr;
    info->GetLogicalAddress(&logicalAddr);
    *result = logicalAddr;
    return NOERROR;
}

ECode HdmiCecFeatureAction::GetSourcePath(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> info;
    ((HdmiCecLocalDevice*) mSource.Get())->GetDeviceInfo((IHdmiDeviceInfo**)&info);
    Int32 physicalAddr;
    info->GetPhysicalAddress(&physicalAddr);
    *result = physicalAddr;
    return NOERROR;
}

ECode HdmiCecFeatureAction::SendUserControlPressedAndReleased(
    /* [in] */ Int32 targetAddress,
    /* [in] */ Int32 uiCommand)
{
    return ((HdmiCecLocalDevice*) mSource.Get())->SendUserControlPressedAndReleased(targetAddress, uiCommand);
}

ECode HdmiCecFeatureAction::AddOnFinishedCallback(
    /* [in] */ HdmiCecFeatureAction* action,
    /* [in] */ IRunnable* runnable)
{
    if (mOnFinishedCallbacks == NULL) {
        CArrayList::New((IArrayList**)&mOnFinishedCallbacks);
    }
    AutoPtr<IPairHelper> helper;
    CPairHelper::AcquireSingleton((IPairHelper**)&helper);
    AutoPtr<IPair> pair;
    helper->Create(TO_IINTERFACE(action), runnable, (IPair**)&pair);
    mOnFinishedCallbacks->Add(pair);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

