
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCONTACTINFO_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCONTACTINFO_H__

#include "_Elastos_Droid_Dialer_CallLog_CContactInfo.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Information for a contact as needed by the Call Log.
 */
CarClass(CContactInfo)
    , public Object
    , public IContactInfo
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CContactInfo();

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

public:
    AutoPtr<IUri> mLookupUri;
    String mLookupKey;
    String mName;
    Int32 mType;
    String mLabel;
    String mNumber;
    String mFormattedNumber;
    String mNormalizedNumber;
    /** The photo for the contact, if available. */
    Int64 mPhotoId;
    /** The high-res photo for the contact, if available. */
    AutoPtr<IUri> mPhotoUri;
    Boolean mIsBadData;
    String mObjectId;

    static AutoPtr<IContactInfo> EMPTY; // = new ContactInfo();

    static String GEOCODE_AS_LABEL; // = "";

    Int32 mSourceType; // = 0;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCONTACTINFO_H__
