
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSIMTLV_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSIMTLV_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSimTlv.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * SIM Tag-Length-Value record
 * TS 102 223 Annex C
 *
 * {@hide}
 *
 */
CarClass(CSimTlv)
    , public ISimTlv
    , public Object
{
public:
    CSimTlv();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* record,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI NextObject(
        /* [out] */ Boolean* result);

    CARAPI IsValidObject(
        /* [out] */ Boolean* result);

    /**
     * Returns the tag for the current TLV object
     * Return 0 if !isValidObject()
     * 0 and 0xff are invalid tag values
     * valid tags range from 1 - 0xfe
     */
    CARAPI GetTag(
        /* [out] */ Int32* result);

    /**
     * Returns data associated with current TLV object
     * returns null if !isValidObject()
     */
    CARAPI GetData(
        /* [out, callee] */ ArrayOf<Byte>** result);

private:
    /**
     * Updates curDataLength and curDataOffset
     * @return false on invalid record, true on valid record
     */

    CARAPI_(Boolean) ParseCurrentTlvObject();

public:
    AutoPtr<ArrayOf<Byte> > mRecord;
    Int32 mTlvOffset;
    Int32 mTlvLength;
    Int32 mCurOffset;
    Int32 mCurDataOffset;
    Int32 mCurDataLength;
    Boolean mHasValidTlvObject;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSIMTLV_H__
