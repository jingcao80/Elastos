#ifndef __ELASTOS_APPS_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__

#include "SearchFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

/**
 * Implements a fragment to load and display SmartDial search results.
 */
class SmartDialSearchFragment
    : public SearchFragment
    , public ISmartDialSearchFragment
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a SmartDialCursorLoader object to load query results.
     */
    // @Override
    CARAPI OnCreateLoader(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args,
        /* [out] */ ILoader** loader);

protected:
    /**
     * Creates a SmartDialListAdapter to display and operate on search results.
     */
    // @Override
    CARAPI CreateListAdapter(
        /* [out] */ IContactEntryListAdapter** adapter);

    /**
     * Gets the Phone Uri of an entry for calling.
     * @param position Location of the data of interest.
     * @return Phone Uri to establish a phone call.
     */
    // @Override
    CARAPI GetPhoneUri(
        /* [in] */ Int32 position,
        /* [out] */ IUri** uri);

private:
    static const String TAG; // = SmartDialSearchFragment.class.getSimpleName();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__
