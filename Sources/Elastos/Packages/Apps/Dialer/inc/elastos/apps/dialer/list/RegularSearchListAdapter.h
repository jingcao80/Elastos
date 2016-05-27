#ifndef __ELASTOS_APPS_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__
#define __ELASTOS_APPS_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__

#include "DialerPhoneNumberListAdapter.h"

namespace Elastos{
namespace Apps{
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
        /* [out] */ ICachedNumberLookupServiceCachedContactInfo** info);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);
}

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__
