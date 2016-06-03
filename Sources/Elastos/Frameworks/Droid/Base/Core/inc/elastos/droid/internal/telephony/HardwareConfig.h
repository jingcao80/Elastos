
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_HARDWARECONFIG_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_HARDWARECONFIG_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IBitSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 *
 * hardware configuration information reported by the ril layer and for
 * use by the telephone framework.
 *
 * the hardware configuration is managed by the TelephonyDevController
 * (aka: the 'TDC').
 *
 * the hardware resources are:
 *    - modem: physical entity providing acces technology.
 *    - sim: physicaly entity providing a slot interface.
 */
class HardwareConfig
    : public Object
    , public IHardwareConfig
{
public:
    CAR_INTERFACE_DECL()

    /**
     * default constructor.
     */
    HardwareConfig(
        /* [in] */ Int32 type);

    /**
     * create from a resource string format.
     */
    HardwareConfig(
        /* [in] */ const String& res);

    CARAPI AssignModem(
        /* [in] */ const String& id,
        /* [in] */ Int32 state,
        /* [in] */ Int32 model,
        /* [in] */ Int32 ratBits,
        /* [in] */ Int32 maxV,
        /* [in] */ Int32 maxD,
        /* [in] */ Int32 maxS);

    CARAPI AssignSim(
        /* [in] */ const String& id,
        /* [in] */ Int32 state,
        /* [in] */ const String& link);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI CompareTo(
        /* [in] */ IHardwareConfig* hw,
        /* [out] */ Int32* result);

public:
    static const String TAG;

    /**
     * common hardware configuration.
     *
     * type - see DEV_HARDWARE_TYPE_
     * uuid - unique identifier for this hardware.
     * state - see DEV_HARDWARE_STATE_
     */
    Int32 mType;
    String mUuid;
    Int32 mState;
    /**
     * following is some specific hardware configuration based on the hardware type.
     */
    /**
     * DEV_HARDWARE_TYPE_MODEM.
     *
     * rilModel - see DEV_MODEM_RIL_MODEL_
     * rat - BitSet value, based on android.telephony.ServiceState
     * maxActiveVoiceCall - maximum number of concurent active voice calls.
     * maxActiveDataCall - maximum number of concurent active data calls.
     * maxStandby - maximum number of concurent standby connections.
     *
     * note: the maxStandby is not necessarily an equal sum of the maxActiveVoiceCall
     * and maxActiveDataCall (nor a derivative of it) since it really depends on the
     * modem capability, hence it is left for the hardware to define.
     */
    Int32 mRilModel;
    AutoPtr<IBitSet> mRat;
    Int32 mMaxActiveVoiceCall;
    Int32 mMaxActiveDataCall;
    Int32 mMaxStandby;
    /**
     * DEV_HARDWARE_TYPE_SIM.
     *
     * modemUuid - unique association to a modem for a sim.
     */
    String mModemUuid;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_HARDWARECONFIG_H__
