
#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTIONFROMAVR_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTIONFROMAVR_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/SystemAudioAction.h"

using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles System Audio initiated by AVR devices.
 */
// Seq #33
class SystemAudioActionFromAvr
    : public SystemAudioAction
{
public:
    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress logical address of AVR device
     * @param targetStatus Whether to enable the system audio mode or not
     * @param callback callback interface to be notified when it's done
     * @throw IllegalArugmentException if device type of tvAddress and avrAddress is invalid
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean targetStatus,
        /* [in] */ IIHdmiControlCallback* callback);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

private:
    CARAPI HandleSystemAudioActionFromAvr();
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTIONFROMAVR_H__
