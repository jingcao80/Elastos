
#include "elastos/droid/server/hdmi/HdmiCecStandbyModeHandler.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include <Elastos.Droid.Utility.h>

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return true;

#endif
}

//=============================================================================
// HdmiCecStandbyModeHandler::Bypasser
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::Bypasser, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

ECode HdmiCecStandbyModeHandler::Bypasser::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return true;

#endif
}

//=============================================================================
// HdmiCecStandbyModeHandler::Aborter
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::Aborter, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

HdmiCecStandbyModeHandler::Aborter::Aborter(
    /* [in] */ Int32 reason)
    : mReason(reason)
{}

ECode HdmiCecStandbyModeHandler::Aborter::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mService.maySendFeatureAbortCommand(message, mReason);
                return true;

#endif
}

//=============================================================================
// HdmiCecStandbyModeHandler::AutoOnHandler
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::AutoOnHandler, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

ECode HdmiCecStandbyModeHandler::AutoOnHandler::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                if (!mTv.getAutoWakeup()) {
                    mAborterRefused.handle(message);
                    return true;
                }
                return false;

#endif
}

//=============================================================================
// HdmiCecStandbyModeHandler::UserControlProcessedHandler
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecStandbyModeHandler::UserControlProcessedHandler, Object, IHdmiCecStandbyModeHandlerCecMessageHandler)

ECode HdmiCecStandbyModeHandler::UserControlProcessedHandler::Handle(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                // The power status here is always standby.
                if (HdmiCecLocalDevice.isPowerOnOrToggleCommand(message)) {
                    return false;
                } else if (HdmiCecLocalDevice.isPowerOffOrToggleCommand(message)) {
                    return true;
                }
                return mAborterIncorrectMode.handle(message);

#endif
}

//=============================================================================
// HdmiCecStandbyModeHandler
//=============================================================================
HdmiCecStandbyModeHandler::HdmiCecStandbyModeHandler()
{
#if 0 // TODO: Translate codes below
    private final SparseArray<CecMessageHandler> mCecMessageHandlers = new SparseArray<>();

    private final CecMessageHandler mDefaultHandler = new Aborter(
            Constants.ABORT_UNRECOGNIZED_OPCODE);
    private final CecMessageHandler mAborterIncorrectMode = new Aborter(
            Constants.ABORT_NOT_IN_CORRECT_MODE);
    private final CecMessageHandler mAborterRefused = new Aborter(Constants.ABORT_REFUSED);
    private final CecMessageHandler mAutoOnHandler = new AutoOnHandler();
    private final CecMessageHandler mBypasser = new Bypasser();
    private final CecMessageHandler mBystander = new Bystander();
    private final UserControlProcessedHandler
            mUserControlProcessedHandler = new UserControlProcessedHandler();
#endif
}

ECode HdmiCecStandbyModeHandler::constructor(
    /* [in] */ IHdmiControlService* service,
    /* [in] */ HdmiCecLocalDeviceTv* tv)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService = service;
        mTv = tv;
        addHandler(Constants::MESSAGE_IMAGE_VIEW_ON, mAutoOnHandler);
        addHandler(Constants::MESSAGE_TEXT_VIEW_ON, mAutoOnHandler);
        addHandler(Constants::MESSAGE_ACTIVE_SOURCE, mBystander);
        addHandler(Constants::MESSAGE_REQUEST_ACTIVE_SOURCE, mBystander);
        addHandler(Constants::MESSAGE_ROUTING_CHANGE, mBystander);
        addHandler(Constants::MESSAGE_ROUTING_INFORMATION, mBystander);
        addHandler(Constants::MESSAGE_SET_STREAM_PATH, mBystander);
        addHandler(Constants::MESSAGE_STANDBY, mBystander);
        addHandler(Constants::MESSAGE_SET_MENU_LANGUAGE, mBystander);
        addHandler(Constants::MESSAGE_DEVICE_VENDOR_ID, mBystander);
        addHandler(Constants::MESSAGE_USER_CONTROL_RELEASED, mBystander);
        addHandler(Constants::MESSAGE_REPORT_POWER_STATUS, mBystander);
        addHandler(Constants::MESSAGE_FEATURE_ABORT, mBystander);
        addHandler(Constants::MESSAGE_INACTIVE_SOURCE, mBystander);
        addHandler(Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS, mBystander);
        addHandler(Constants::MESSAGE_REPORT_AUDIO_STATUS, mBystander);
        // If TV supports the following messages during power-on, ignore them and do nothing,
        // else reply with <Feature Abort>["Unrecognized Opcode"]
        // <Deck Status>, <Tuner Device Status>, <Tuner Cleared Status>, <Timer Status>
        addHandler(Constants::MESSAGE_RECORD_STATUS, mBystander);
        // If TV supports the following messages during power-on, reply with <Feature Abort>["Not
        // in correct mode to respond"], else reply with <Feature Abort>["Unrecognized Opcode"]
        // <Give Tuner Device Status>, <Select Digital Service>, <Tuner Step Decrement>,
        // <Tuner Stem Increment>, <Menu Status>.
        addHandler(Constants::MESSAGE_RECORD_TV_SCREEN, mAborterIncorrectMode);
        addHandler(Constants::MESSAGE_INITIATE_ARC, mAborterIncorrectMode);
        addHandler(Constants::MESSAGE_TERMINATE_ARC, mAborterIncorrectMode);
        addHandler(Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS, mBypasser);
        addHandler(Constants::MESSAGE_GET_MENU_LANGUAGE, mBypasser);
        addHandler(Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS, mBypasser);
        addHandler(Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID, mBypasser);
        addHandler(Constants::MESSAGE_GIVE_OSD_NAME, mBypasser);
        addHandler(Constants::MESSAGE_SET_OSD_NAME, mBypasser);
        addHandler(Constants::MESSAGE_USER_CONTROL_PRESSED, mUserControlProcessedHandler);
        addHandler(Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS, mBypasser);
        addHandler(Constants::MESSAGE_ABORT, mBypasser);
        addHandler(Constants::MESSAGE_GET_CEC_VERSION, mBypasser);
        addHandler(Constants::MESSAGE_VENDOR_COMMAND_WITH_ID, mAborterIncorrectMode);
        addHandler(Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE, mAborterIncorrectMode);

#endif
}

ECode HdmiCecStandbyModeHandler::AddHandler(
    /* [in] */ Int32 opcode,
    /* [in] */ IHdmiCecStandbyModeHandlerCecMessageHandler* handler)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCecMessageHandlers.put(opcode, handler);

#endif
}

ECode HdmiCecStandbyModeHandler::HandleCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        CecMessageHandler handler = mCecMessageHandlers.get(message.getOpcode());
        if (handler != NULL) {
            return handler.handle(message);
        }
        return mDefaultHandler.handle(message);

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
