#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CCalendarContractEventDays.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

const String CCalendarContractEventDays::SELECTION = String("selected=1");

CAR_SINGLETON_IMPL(CCalendarContractEventDays)

CAR_INTERFACE_IMPL(CCalendarContractEventDays, Singleton, ICalendarContractEventDays)

ECode CCalendarContractEventDays::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/instances/groupbyday";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CCalendarContractEventDays::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int32 startDay,
    /* [in] */ Int32 numDays,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    if (numDays < 1) {
        *cursor = NULL;
        return NOERROR;
    }
    Int32 endDay = startDay + numDays - 1;
    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    AutoPtr<IUriBuilder> builder;
    FAIL_RETURN(_uri->BuildUpon((IUriBuilder**)&builder))
    AutoPtr<IContentUris> helper;
    FAIL_RETURN(CContentUris::AcquireSingleton((IContentUris**)&helper))
    FAIL_RETURN(helper->AppendId(builder, startDay))
    FAIL_RETURN(helper->AppendId(builder, endDay))
    AutoPtr<IUri> uri;
    FAIL_RETURN(builder->Build((IUri**)&uri))
    return cr->Query(uri, projection, SELECTION, NULL /* selection args */, ICalendarContractEventDaysColumns::STARTDAY, cursor);
}

} //Provider
} //Droid
} //Elastos