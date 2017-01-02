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

#ifndef __ELASTOS_DROID_SERVER_HDMI_ACTIVESOURCEHANDLER_H__
#define __ELASTOS_DROID_SERVER_HDMI_ACTIVESOURCEHANDLER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"

using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDeviceTv;
class HdmiControlService;
/**
 * Handles CEC command &lt;Active Source&gt;.
 * <p>
 * Used by feature actions that need to handle the command in their flow. Only for TV
 * local device.
 */
class ActiveSourceHandler
    : public Object
{
public:
    static CARAPI Create(
        /* [in] */ IHdmiCecLocalDeviceTv* source,
        /* [in] */ IIHdmiControlCallback* callback,
        /* [out] */ ActiveSourceHandler** result);

    /**
     * Handles the incoming active source command.
     *
     * @param newActive new active source information
     */
    CARAPI Process(
        /* [in] */ IHdmiCecLocalDeviceActiveSource* newActive);

private:
    ActiveSourceHandler(
        /* [in] */ IHdmiCecLocalDeviceTv* source,
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI GetSourceAddress(
        /* [out] */ Int32* result);

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    AutoPtr<IHdmiCecLocalDeviceTv> mSource;

    AutoPtr<IHdmiControlService> mService;

    // @Nullable
    AutoPtr<IIHdmiControlCallback> mCallback;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_ACTIVESOURCEHANDLER_H__
