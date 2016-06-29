#ifndef __ELASTOS_APPS_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__

#include "_Elastos.Apps.Dialer.h"
#include "elastos/apps/dialer/list/SearchFragment.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Apps {
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
    CAR_INTERFACE_DECL();

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
    // TODO:
    // CARAPI CreateListAdapter(
    //     /* [out] */ IContactEntryListAdapter** adapter);

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
