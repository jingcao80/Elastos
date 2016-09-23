
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_PHONEQUERY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_PHONEQUERY_H__

#include "_Elastos.Droid.Dialer.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * The query to look up the {@link ContactInfo} for a given number in the Call Log.
 */
class PhoneQuery
{
public:
    static const AutoPtr<ArrayOf<String> > _PROJECTION = new String[] {
            PhoneLookup._ID,
            PhoneLookup.DISPLAY_NAME,
            PhoneLookup.TYPE,
            PhoneLookup.LABEL,
            PhoneLookup.NUMBER,
            PhoneLookup.NORMALIZED_NUMBER,
            PhoneLookup.PHOTO_ID,
            PhoneLookup.LOOKUP_KEY,
            PhoneLookup.PHOTO_URI};

    static const Int32 PERSON_ID = 0;
    static const Int32 NAME = 1;
    static const Int32 PHONE_TYPE = 2;
    static const Int32 LABEL = 3;
    static const Int32 MATCHED_NUMBER = 4;
    static const Int32 NORMALIZED_NUMBER = 5;
    static const Int32 PHOTO_ID = 6;
    static const Int32 LOOKUP_KEY = 7;
    static const Int32 PHOTO_URI = 8;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_PHONEQUERY_H__
