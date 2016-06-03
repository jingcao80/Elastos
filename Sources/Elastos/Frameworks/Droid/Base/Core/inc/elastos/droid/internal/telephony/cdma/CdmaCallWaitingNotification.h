#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLWAITINGNOTIFICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLWAITINGNOTIFICATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;
// import com.android.internal.telephony.PhoneConstants;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * Represents a Supplementary Service Notification received from the network.
  *
  * {@hide}
  */
class CdmaCallWaitingNotification
    : public Object
    , public ICdmaCallWaitingNotification
{
public:
    CAR_INTERFACE_DECL();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    static CARAPI_(Int32) PresentationFromCLIP(
        /* [in] */ Int32 cli);

public:
    static const String LOGTAG;
    String number;
    Int32 numberPresentation;
    String name;
    Int32 namePresentation;
    Int32 numberType;
    Int32 numberPlan;
    Int32 isPresent;
    Int32 signalType;
    Int32 alertPitch;
    Int32 signal;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLWAITINGNOTIFICATION_H__
