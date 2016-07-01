
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_TELEPHONYDEVCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_TELEPHONYDEVCONTROLLER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * TelephonyDevController - provides a unified view of the
 * telephony hardware resources on a device.
 *
 * manages the set of HardwareConfig for the framework.
 */
class TelephonyDevController
    : public Handler
    , public ITelephonyDevController
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI_(AutoPtr<ITelephonyDevController>) Create();

    static CARAPI_(AutoPtr<ITelephonyDevController>) GetInstance();

    /**
     * each RIL call this interface to register/unregister the unsolicited hardware
     * configuration callback data it can provide.
     */
    static CARAPI RegisterRIL(
        /* [in] */ ICommandsInterface* cmdsIf);

    static CARAPI UnregisterRIL(
        /* [in] */ ICommandsInterface* cmdsIf);

    /**
     * handle callbacks from RIL.
     */
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * get total number of registered modem.
     */
    static CARAPI_(Int32) GetModemCount();

    /**
     * get modem at index 'index'.
     */
    CARAPI GetModem(
        /* [in] */ Int32 index,
        /* [out] */ IHardwareConfig** result);

    /**
     * get total number of registered sims.
     */
    CARAPI GetSimCount(
        /* [out] */ Int32* result);

    /**
     * get sim at index 'index'.
     */
    CARAPI GetSim(
        /* [in] */ Int32 index,
        /* [out] */ IHardwareConfig** result);

    /**
     * get modem associated with sim index 'simIndex'.
     */
    CARAPI GetModemForSim(
        /* [in] */ Int32 simIndex,
        /* [out] */ IHardwareConfig** result);

    /**
     * get all sim's associated with modem at index 'modemIndex'.
     */
    CARAPI GetAllSimsForModem(
        /* [in] */ Int32 modemIndex,
        /* [out] */ IArrayList** result);

    /**
     * get all modem's registered.
     */
    CARAPI GetAllModems(
        /* [out] */ IArrayList** result);

    /**
     * get all sim's registered.
     */
    CARAPI GetAllSims(
        /* [out] */ IArrayList** result);

private:
    static CARAPI_(void) Logd(
        /* [in] */ String s);

    static CARAPI_(void) Loge(
        /* [in] */ String s);

    CARAPI_(void) InitFromResource();

    /**
     * hardware configuration update or insert.
     */
    static CARAPI_(void) UpdateOrInsert(
        /* [in] */ IHardwareConfig* hw,
        /* [in] */ IArrayList* list);

    /**
     * hardware configuration changed.
     */
    static CARAPI_(void) HandleGetHardwareConfigChanged(
        /* [in] */ AsyncResult* ar);

private:
    static const String LOGTAG;
    static const Boolean DBG;
    static Object mLock;

    static const Int32 EVENT_HARDWARE_CONFIG_CHANGED;

    static AutoPtr<ITelephonyDevController> sTelephonyDevController;
    static AutoPtr<IArrayList> mModems;
    static AutoPtr<IArrayList> mSims;

    static AutoPtr<IMessage> sRilHardwareConfig;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_TELEPHONYDEVCONTROLLER_H__
