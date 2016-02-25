
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECSTANDBYMODEHANDLER_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECSTANDBYMODEHANDLER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDeviceTv;
/**
 * This class handles the incoming messages when HdmiCecService is in the standby mode.
 */
class HdmiCecStandbyModeHandler
    : public Object
{
private:
    class Bystander
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);
    };

    class Bypasser
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);
    };

    class Aborter
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        Aborter(
            /* [in] */ Int32 reason);

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);

    private:
        const Int32 mReason;
    };

    class AutoOnHandler
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);
    };

    class UserControlProcessedHandler
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);
    };

public:
    HdmiCecStandbyModeHandler();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service,
        /* [in] */ HdmiCecLocalDeviceTv* tv);

    /**
     * Handles the CEC message in the standby mode.
     *
     * @param message {@link HdmiCecMessage} to be processed
     * @return true if the message is handled in the handler, false means that the message is need
     *         to be dispatched to the local device.
     */
    CARAPI HandleCommand(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

private:
    CARAPI AddHandler(
        /* [in] */ Int32 opcode,
        /* [in] */ IHdmiCecStandbyModeHandlerCecMessageHandler* handler);

private:
    AutoPtr<IHdmiControlService> mService;

    AutoPtr<HdmiCecLocalDeviceTv> mTv;

    AutoPtr<ISparseArray> mCecMessageHandlers;

    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> mDefaultHandler;

    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> mAborterIncorrectMode;

    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> mAborterRefused;

    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> mAutoOnHandler;

    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> mBypasser;

    AutoPtr<IHdmiCecStandbyModeHandlerCecMessageHandler> mBystander;

    AutoPtr<UserControlProcessedHandler> mUserControlProcessedHandler;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECSTANDBYMODEHANDLER_H__
