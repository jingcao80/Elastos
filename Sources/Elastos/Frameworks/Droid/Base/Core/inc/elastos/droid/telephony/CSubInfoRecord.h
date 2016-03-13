
#ifndef __ELASTOS_DROID_TELEPHONY_CSUBINFORECORD_H__
#define __ELASTOS_DROID_TELEPHONY_CSUBINFORECORD_H__

#include "_Elastos_Droid_Telephony_CSubInfoRecord.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSubInfoRecord)
    , public Object
    , public ISubInfoRecord
    , public IParcelable
{
public:
    CSubInfoRecord();

    virtual ~CSubInfoRecord();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 subId,
        /* [in] */ const String& iccId,
        /* [in] */ Int32 slotId,
        /* [in] */ const String& displayName,
        /* [in] */ Int32 nameSource,
        /* [in] */ Int32 color,
        /* [in] */ const String& number,
        /* [in] */ Int32 displayFormat,
        /* [in] */ Int32 roaming,
        /* [in] */ ArrayOf<Int32>* iconRes,
        /* [in] */ Int32 mcc,
        /* [in] */ Int32 mnc);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    /**
     * Subscription Identifier, this is a device unique number
     * and not an index into an array
     */
    Int64 mSubId;
    /** The GID for a SIM that maybe associated with this subscription, empty if unknown */
    String mIccId;
    /**
     * The slot identifier for that currently contains the subscription
     * and not necessarily unique and maybe INVALID_SLOT_ID if unknown
     */
    Int32 mSlotId;
    /**
     * The string displayed to the user that identifies this subscription
     */
    String mDisplayName;
    /**
     * The source of the name, NAME_SOURCE_UNDEFINED, NAME_SOURCE_DEFAULT_SOURCE,
     * NAME_SOURCE_SIM_SOURCE or NAME_SOURCE_USER_INPUT.
     */
    Int32 mNameSource;
    /**
     * The color to be used for when displaying to the user
     */
    Int32 mColor;
    /**
     * A number presented to the user identify this subscription
     */
    String mNumber;
    /**
     * How to display the phone number, DISPLAY_NUMBER_NONE, DISPLAY_NUMBER_FIRST,
     * DISPLAY_NUMBER_LAST
     */
    Int32 mDisplayNumberFormat;
    /**
     * Data roaming state, DATA_RAOMING_ENABLE, DATA_RAOMING_DISABLE
     */
    Int32 mDataRoaming;
    /**
     * SIM Icon resource identifiers. FIXME: Check with MTK what it really is
     */
    AutoPtr<ArrayOf<Int32> > mSimIconRes;
    /**
     * Mobile Country Code
     */
    Int32 mMcc;
    /**
     * Mobile Network Code
     */
    Int32 mMnc;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSUBINFORECORD_H__
