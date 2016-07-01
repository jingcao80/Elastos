#ifndef __ELASTOS_DROID_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/apps/dialer/list/DialerPhoneNumberListAdapter.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;
using Elastos::Droid::Dialer::Service::ICachedContactInfo;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * List adapter to display regular search results.
 */
class RegularSearchListAdapter
    : public DialerPhoneNumberListAdapter
    , public IRegularSearchListAdapter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetContactInfo(
        /* [in] */ ICachedNumberLookupService* lookupService,
        /* [in] */ Int32 position,
        /* [out] */ ICachedContactInfo** info);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);
}

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__
