
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
        /* [in] */ HdmiCecLocalDeviceTv* source,
        /* [in] */ IIHdmiControlCallback* callback,
        /* [out] */ ActiveSourceHandler** result);

    /**
     * Handles the incoming active source command.
     *
     * @param newActive new active source information
     */
    CARAPI Process(
        /* [in] */ HdmiCecLocalDevice::ActiveSource* newActive);

private:
    ActiveSourceHandler(
        /* [in] */ HdmiCecLocalDeviceTv* source,
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI GetSourceAddress(
        /* [out] */ Int32* result);

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    // AutoPtr<HdmiCecLocalDeviceTv> mSource;

    // AutoPtr<HdmiControlService> mService;

    // @Nullable
    AutoPtr<IIHdmiControlCallback> mCallback;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_ACTIVESOURCEHANDLER_H__
