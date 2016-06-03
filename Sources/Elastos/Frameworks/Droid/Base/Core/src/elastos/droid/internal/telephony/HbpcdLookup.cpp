
#include "elastos/droid/internal/telephony/HbpcdLookup.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

AutoPtr<IUri> InitUri(
    /* [in] */ const String& value)
{
    AutoPtr<IUri> uri;
    Uri::Parse(value, (IUri**)&uri);
    return uri;
}

//class HbpcdLookup
const String HbpcdLookup::AUTHORITY("hbpcd_lookup");

AutoPtr<IUri> HbpcdLookup::CONTENT_URI = InitUri(String("content://") + AUTHORITY);

const String HbpcdLookup::PATH_MCC_IDD("idd");
const String HbpcdLookup::PATH_MCC_LOOKUP_TABLE("lookup");
const String HbpcdLookup::PATH_MCC_SID_CONFLICT("conflict");
const String HbpcdLookup::PATH_MCC_SID_RANGE("range");
const String HbpcdLookup::PATH_NANP_AREA_CODE("nanp");
const String HbpcdLookup::PATH_ARBITRARY_MCC_SID_MATCH("arbitrary");
const String HbpcdLookup::PATH_USERADD_COUNTRY("useradd");

const String HbpcdLookup::ID("_id");
const Int32 HbpcdLookup::IDINDEX = 0;

//class HbpcdLookup::MccIdd
AutoPtr<IUri> HbpcdLookup::MccIdd::CONTENT_URI =
    InitUri(String("content://") + AUTHORITY + "/" + PATH_MCC_IDD);
const String HbpcdLookup::MccIdd::DEFAULT_SORT_ORDER("MCC ASC");

const String HbpcdLookup::MccIdd::MCC("MCC");
const String HbpcdLookup::MccIdd::IDD("IDD");

//class HbpcdLookup::MccLookup
AutoPtr<IUri> HbpcdLookup::MccLookup::CONTENT_URI =
    InitUri(String("content://") + AUTHORITY + "/" + PATH_MCC_LOOKUP_TABLE);
const String HbpcdLookup::MccLookup::DEFAULT_SORT_ORDER("MCC ASC");

const String HbpcdLookup::MccLookup::MCC("MCC");
const String HbpcdLookup::MccLookup::COUNTRY_CODE("Country_Code");
const String HbpcdLookup::MccLookup::COUNTRY_NAME("Country_Name");
const String HbpcdLookup::MccLookup::NDD("NDD");
const String HbpcdLookup::MccLookup::NANPS("NANPS");
const String HbpcdLookup::MccLookup::GMT_OFFSET_LOW("GMT_Offset_Low");
const String HbpcdLookup::MccLookup::GMT_OFFSET_HIGH("GMT_Offset_High");
const String HbpcdLookup::MccLookup::GMT_DST_LOW("GMT_DST_Low");
const String HbpcdLookup::MccLookup::GMT_DST_HIGH("GMT_DST_High");

//class HbpcdLookup::MccSidConflicts
AutoPtr<IUri> HbpcdLookup::MccSidConflicts::CONTENT_URI =
    InitUri(String("content://") + AUTHORITY + "/" + PATH_MCC_SID_CONFLICT);
const String HbpcdLookup::MccSidConflicts::DEFAULT_SORT_ORDER("MCC ASC");

const String HbpcdLookup::MccSidConflicts::MCC("MCC");
const String HbpcdLookup::MccSidConflicts::SID_CONFLICT("SID_Conflict");

//class HbpcdLookup::MccSidRange
AutoPtr<IUri> HbpcdLookup::MccSidRange::CONTENT_URI =
    InitUri(String("content://") + AUTHORITY + "/" + PATH_MCC_SID_RANGE);
const String HbpcdLookup::MccSidRange::DEFAULT_SORT_ORDER("MCC ASC");

const String HbpcdLookup::MccSidRange::MCC("MCC");
const String HbpcdLookup::MccSidRange::RANGE_LOW("SID_Range_Low");
const String HbpcdLookup::MccSidRange::RANGE_HIGH("SID_Range_High");

//class HbpcdLookup::ArbitraryMccSidMatch
AutoPtr<IUri> HbpcdLookup::ArbitraryMccSidMatch::CONTENT_URI =
    InitUri(String("content://") + AUTHORITY + "/" + PATH_ARBITRARY_MCC_SID_MATCH);
const String HbpcdLookup::ArbitraryMccSidMatch::DEFAULT_SORT_ORDER("MCC ASC");

const String HbpcdLookup::ArbitraryMccSidMatch::MCC("MCC");
const String HbpcdLookup::ArbitraryMccSidMatch::SID("SID");

//class HbpcdLookup::NanpAreaCode
AutoPtr<IUri> HbpcdLookup::NanpAreaCode::CONTENT_URI =
    InitUri(String("content://") + AUTHORITY + "/" + PATH_NANP_AREA_CODE);
const String HbpcdLookup::NanpAreaCode::DEFAULT_SORT_ORDER("Area_Code ASC");

const String HbpcdLookup::NanpAreaCode::AREA_CODE("Area_Code");

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
