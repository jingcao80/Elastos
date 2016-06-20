
#include "elastos/apps/dialer/calllog/CCallLogQuery.h"
#include <elquintet.h>
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ICalls;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const String _PROJECTION[] = {
    IBaseColumns::ID,                          // 0
    ICalls::NUMBER,                       // 1
    ICalls::DATE,                         // 2
    ICalls::DURATION,                     // 3
    ICalls::TYPE,                         // 4
    ICalls::COUNTRY_ISO,                  // 5
    ICalls::VOICEMAIL_URI,                // 6
    ICalls::GEOCODED_LOCATION,            // 7
    ICalls::CACHED_NAME,                  // 8
    ICalls::CACHED_NUMBER_TYPE,           // 9
    ICalls::CACHED_NUMBER_LABEL,          // 10
    ICalls::CACHED_LOOKUP_URI,            // 11
    ICalls::CACHED_MATCHED_NUMBER,        // 12
    ICalls::CACHED_NORMALIZED_NUMBER,     // 13
    ICalls::CACHED_PHOTO_ID,              // 14
    ICalls::CACHED_FORMATTED_NUMBER,      // 15
    ICalls::IS_READ,                      // 16
    ICalls::NUMBER_PRESENTATION,          // 17
    ICalls::PHONE_ACCOUNT_COMPONENT_NAME, // 18
    ICalls::PHONE_ACCOUNT_ID,             // 19
    ICalls::FEATURES,                     // 20
    ICalls::DATA_USAGE,                   // 21
    ICalls::TRANSCRIPTION                 // 22
};

CAR_INTERFACE_IMPL(CCallLogQuery, Singleton, ICallLogQuery);

CAR_SINGLETON_IMPL(CCallLogQuery);

ECode CCallLogQuery::Get_PROJECTION(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(
            (String*)_PROJECTION, 9);
    *result = projection;

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
