#ifndef __ELASTOS_DROID_PROVIDER_CALENDARCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_CALENDARCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

class CalendarContract
{
public:
    /**
     * The content:// style URL for the top-level calendar authority
     */
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

private:
    static const String TAG;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CALENDARCONTRACT_H__
