#ifndef __ELASTOS_APPS_DIALER_LIST_SMARTDIALNUMBERLISTADAPTER_H__
#define __ELASTOS_APPS_DIALER_LIST_SMARTDIALNUMBERLISTADAPTER_H__

#include "DialerPhoneNumberListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

/**
 * List adapter to display the SmartDial search results.
 */
class SmartDialNumberListAdapter
    : public DialerPhoneNumberListAdapter
    , public ISmartDialNumberListAdapter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Sets query for the SmartDialCursorLoader.
     */
    CARAPI ConfigureLoader(
        /* [in] */ ISmartDialCursorLoader* loader);

    /**
     * Gets Uri for the list item at the given position.
     * @param position Location of the data of interest.
     * @return Data Uri of the entry.
     */
    CARAPI GetDataUri(
        /* [in] */ Int32 position,
        /* [out] */ IUri** uri);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);

protected:
    /**
     * Sets highlight options for a List item in the SmartDial search results.
     * @param view ContactListItemView where the result will be displayed.
     * @param cursor Object containing information of the associated List item.
     */
    // @Override
    CARAPI SetHighlight(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

private:
    static const String TAG; // = SmartDialNumberListAdapter.class.getSimpleName();
    static const Boolean DEBUG; // = false;

    AutoPtr<ISmartDialNameMatcher> mNameMatcher;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_SMARTDIALNUMBERLISTADAPTER_H__
