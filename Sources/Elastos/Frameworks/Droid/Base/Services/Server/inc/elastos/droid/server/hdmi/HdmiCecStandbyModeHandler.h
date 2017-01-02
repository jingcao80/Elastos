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
            /* [in] */ HdmiCecStandbyModeHandler* host,
            /* [in] */ Int32 reason);

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);

    private:
        HdmiCecStandbyModeHandler* mHost;
        const Int32 mReason;
    };

    class AutoOnHandler
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        AutoOnHandler(
            /* [in] */ HdmiCecStandbyModeHandler* host);

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);

    private:
        HdmiCecStandbyModeHandler* mHost;
    };

    class UserControlProcessedHandler
        : public Object
        , public IHdmiCecStandbyModeHandlerCecMessageHandler
    {
    public:
        CAR_INTERFACE_DECL()

        UserControlProcessedHandler(
            /* [in] */ HdmiCecStandbyModeHandler* host);

        // @Override
        CARAPI Handle(
            /* [in] */ IHdmiCecMessage* message,
            /* [out] */ Boolean* result);

    private:
        HdmiCecStandbyModeHandler* mHost;
    };

public:
    HdmiCecStandbyModeHandler();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service,
        /* [in] */ IHdmiCecLocalDeviceTv* tv);

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

    AutoPtr<IHdmiCecLocalDeviceTv> mTv;

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
