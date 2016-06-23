
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSBROADCASTCONFIGINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSBROADCASTCONFIGINFO_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSmsBroadcastConfigInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * SmsBroadcastConfigInfo defines one configuration of Cell Broadcast
 * Message (CBM) to be received by the ME
 *
 * fromServiceId - toServiceId defines a range of CBM message identifiers
 * whose value is 0x0000 - 0xFFFF as defined in TS 23.041 9.4.1.2.2 for GMS
 * and 9.4.4.2.2 for UMTS. All other values can be treated as empty
 * CBM message ID.
 *
 * fromCodeScheme - toCodeScheme defines a range of CBM data coding schemes
 * whose value is 0x00 - 0xFF as defined in TS 23.041 9.4.1.2.3 for GMS
 * and 9.4.4.2.3 for UMTS.
 * All other values can be treated as empty CBM data coding scheme.
 *
 * selected false means message types specified in {@code <fromServiceId, toServiceId>}
 * and {@code <fromCodeScheme, toCodeScheme>} are not accepted, while true means accepted.
 *
 */
CarClass(CSmsBroadcastConfigInfo)
    , public Object
    , public ISmsBroadcastConfigInfo
{
public:
    CSmsBroadcastConfigInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 fromId,
        /* [in] */ Int32 toId,
        /* [in] */ Int32 fromScheme,
        /* [in] */ Int32 toScheme,
        /* [in] */ Boolean selected);

    CARAPI SetFromServiceId(
        /* [in] */ Int32 fromServiceId);

    CARAPI GetFromServiceId(
        /* [out] */ Int32* result);

    CARAPI SetToServiceId(
        /* [in] */ Int32 toServiceId);

    CARAPI GetToServiceId(
        /* [out] */ Int32* result);

    CARAPI SetFromCodeScheme(
        /* [in] */ Int32 fromCodeScheme);

    CARAPI GetFromCodeScheme(
        /* [out] */ Int32* result);

    CARAPI SetToCodeScheme(
        /* [in] */ Int32 toCodeScheme);

    CARAPI GetToCodeScheme(
        /* [out] */ Int32* result);

    CARAPI SetSelected(
        /* [in] */ Boolean selected);

    CARAPI IsSelected(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Int32 mFromServiceId;
    Int32 mToServiceId;
    Int32 mFromCodeScheme;
    Int32 mToCodeScheme;
    Boolean mSelected;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSBROADCASTCONFIGINFO_H__
