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

#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecStandbyModeHandler.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Utility::CSparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecStandbyModeHandler::Bystander
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::Bystander, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

ECode HdmiCecStandbyModeHandler::Bystander::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

//=============================================================================
// HdmiCecStandbyModeHandler::Bypasser
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::Bypasser, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

ECode HdmiCecStandbyModeHandler::Bypasser::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

//=============================================================================
// HdmiCecStandbyModeHandler::Aborter
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::Aborter, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

HdmiCecStandbyModeHandler::Aborter::Aborter(
    /* [in] */ HdmiCecStandbyModeHandler* host,
    /* [in] */ Int32 reason)
    : mHost(host)
    , mReason(reason)
{}

ECode HdmiCecStandbyModeHandler::Aborter::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ((HdmiControlService*)mHost->mService.Get())->MaySendFeatureAbortCommand(message, mReason);
    *result = TRUE;
    return NOERROR;
}

//=============================================================================
// HdmiCecStandbyModeHandler::AutoOnHandler
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::AutoOnHandler, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

HdmiCecStandbyModeHandler::AutoOnHandler::AutoOnHandler(
    /* [in] */ HdmiCecStandbyModeHandler* host)
    : mHost(host)
{}

ECode HdmiCecStandbyModeHandler::AutoOnHandler::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isAutoWakeup;
    ((HdmiCecLocalDeviceTv*) mHost->mTv.Get())->GetAutoWakeup(&isAutoWakeup);
    if (!isAutoWakeup) {
        Boolean bNoUse;
        mHost->mAborterRefused->Handle(message, &bNoUse);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=============================================================================
// HdmiCecStandbyModeHandler::UserControlProcessedHandler
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::UserControlProcessedHandler, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

HdmiCecStandbyModeHandler::UserControlProcessedHandler::UserControlProcessedHandler(
    /* [in] */ HdmiCecStandbyModeHandler* host)
    : mHost(host)
{}

ECode HdmiCecStandbyModeHandler::UserControlProcessedHandler::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // The power status here is always standby.
    Boolean isPowerOnOrToggleCommand;
    HdmiCecLocalDevice::IsPowerOnOrToggleCommand(message, &isPowerOnOrToggleCommand);
    Boolean isPowerOffOrToggleCommand;
    HdmiCecLocalDevice::IsPowerOffOrToggleCommand(message, &isPowerOffOrToggleCommand);
    if (isPowerOnOrToggleCommand) {
        *result = FALSE;
        return NOERROR;
    } else if (isPowerOffOrToggleCommand) {
        *result = TRUE;
        return NOERROR;
    }
    return mHost->mAborterIncorrectMode->Handle(message, result);
}

//=============================================================================
// HdmiCecStandbyModeHandler
//=============================================================================
HdmiCecStandbyModeHandler::HdmiCecStandbyModeHandler()
{
    CSparseArray::New((ISparseArray**)&mCecMessageHandlers);

    mDefaultHandler = new Aborter(this, Constants::ABORT_UNRECOGNIZED_OPCODE);
    mAborterIncorrectMode = new Aborter(this, Constants::ABORT_NOT_IN_CORRECT_MODE);
    mAborterRefused = new Aborter(this, Constants::ABORT_REFUSED);
    mAutoOnHandler = new AutoOnHandler(this);
    mBypasser = new Bypasser();
    mBystander = new Bystander();
    mUserControlProcessedHandler = new UserControlProcessedHandler(this);
}

ECode HdmiCecStandbyModeHandler::constructor(
    /* [in] */ IHdmiControlService* service,
    /* [in] */ IHdmiCecLocalDeviceTv* tv)
{
    mService = service;
    mTv = tv;
    AddHandler(Constants::MESSAGE_IMAGE_VIEW_ON, mAutoOnHandler);
    AddHandler(Constants::MESSAGE_TEXT_VIEW_ON, mAutoOnHandler);
    AddHandler(Constants::MESSAGE_ACTIVE_SOURCE, mBystander);
    AddHandler(Constants::MESSAGE_REQUEST_ACTIVE_SOURCE, mBystander);
    AddHandler(Constants::MESSAGE_ROUTING_CHANGE, mBystander);
    AddHandler(Constants::MESSAGE_ROUTING_INFORMATION, mBystander);
    AddHandler(Constants::MESSAGE_SET_STREAM_PATH, mBystander);
    AddHandler(Constants::MESSAGE_STANDBY, mBystander);
    AddHandler(Constants::MESSAGE_SET_MENU_LANGUAGE, mBystander);
    AddHandler(Constants::MESSAGE_DEVICE_VENDOR_ID, mBystander);
    AddHandler(Constants::MESSAGE_USER_CONTROL_RELEASED, mBystander);
    AddHandler(Constants::MESSAGE_REPORT_POWER_STATUS, mBystander);
    AddHandler(Constants::MESSAGE_FEATURE_ABORT, mBystander);
    AddHandler(Constants::MESSAGE_INACTIVE_SOURCE, mBystander);
    AddHandler(Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS, mBystander);
    AddHandler(Constants::MESSAGE_REPORT_AUDIO_STATUS, mBystander);
    // If TV supports the following messages during power-on, ignore them and do nothing,
    // else reply with <Feature Abort>["Unrecognized Opcode"]
    // <Deck Status>, <Tuner Device Status>, <Tuner Cleared Status>, <Timer Status>
    AddHandler(Constants::MESSAGE_RECORD_STATUS, mBystander);
    // If TV supports the following messages during power-on, reply with <Feature Abort>["Not
    // in correct mode to respond"], else reply with <Feature Abort>["Unrecognized Opcode"]
    // <Give Tuner Device Status>, <Select Digital Service>, <Tuner Step Decrement>,
    // <Tuner Stem Increment>, <Menu Status>.
    AddHandler(Constants::MESSAGE_RECORD_TV_SCREEN, mAborterIncorrectMode);
    AddHandler(Constants::MESSAGE_INITIATE_ARC, mAborterIncorrectMode);
    AddHandler(Constants::MESSAGE_TERMINATE_ARC, mAborterIncorrectMode);
    AddHandler(Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS, mBypasser);
    AddHandler(Constants::MESSAGE_GET_MENU_LANGUAGE, mBypasser);
    AddHandler(Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS, mBypasser);
    AddHandler(Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID, mBypasser);
    AddHandler(Constants::MESSAGE_GIVE_OSD_NAME, mBypasser);
    AddHandler(Constants::MESSAGE_SET_OSD_NAME, mBypasser);
    AddHandler(Constants::MESSAGE_USER_CONTROL_PRESSED, mUserControlProcessedHandler);
    AddHandler(Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS, mBypasser);
    AddHandler(Constants::MESSAGE_ABORT, mBypasser);
    AddHandler(Constants::MESSAGE_GET_CEC_VERSION, mBypasser);
    AddHandler(Constants::MESSAGE_VENDOR_COMMAND_WITH_ID, mAborterIncorrectMode);
    AddHandler(Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE, mAborterIncorrectMode);
    return NOERROR;
}

ECode HdmiCecStandbyModeHandler::AddHandler(
    /* [in] */ Int32 opcode,
    /* [in] */ IHdmiCecStandbyModeHandlerCecMessageHandler* handler)
{
    mCecMessageHandlers->Put(opcode, handler);
    return NOERROR;
}

ECode HdmiCecStandbyModeHandler::HandleCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 opcode;
    message->GetOpcode(&opcode);
    AutoPtr<IInterface> obj;
    mCecMessageHandlers->Get(opcode, (IInterface**)&obj);
    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> handler = IHdmiCecStandbyModeHandlerCecMessageHandler::Probe(obj);
    if (handler != NULL) {
        return handler->Handle(message, result);
    }
    return mDefaultHandler->Handle(message, result);
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
