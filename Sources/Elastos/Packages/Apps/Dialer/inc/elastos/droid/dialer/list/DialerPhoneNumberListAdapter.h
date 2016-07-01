#ifndef __ELASTOS_DROID_DIALER_LIST_DIALERPHONENUMBERLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_DIALERPHONENUMBERLISTADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace List {

/**
 * {@link PhoneNumberListAdapter} with the following added shortcuts, that are displayed as list
 * items:
 * 1) Directly calling the phone number query
 * 2) Adding the phone number query to a contact
 *
 * These shortcuts can be enabled or disabled to toggle whether or not they show up in the
 * list.
 */
class DialerPhoneNumberListAdapter
    // TODO:
    /*: public PhoneNumberListAdapter*/
    : public Object
    , public IDialerPhoneNumberListAdapter
{
public:
    CAR_INTERFACE_DECL();

    DialerPhoneNumberListAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    /**
     * @return The number of enabled shortcuts. Ranges from 0 to a maximum of SHORTCUT_COUNT
     */
    CARAPI GetShortcutCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [in] */ Int32* type);

    // @Override
    CARAPI GetViewTypeCount(
        /* [in] */ Int32* count);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * @param position The position of the item
     * @return The enabled shortcut type matching the given position if the item is a
     * shortcut, -1 otherwise
     */
    CARAPI GetShortcutTypeFromPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* result);

    /**
     * @return True if the shortcut state (disabled vs enabled) was changed by this operation
     */
    CARAPI SetShortcutEnabled(
        /* [in] */ Int32 shortcutType,
        /* [in] */ Boolean visible,
        /* [out] */ Boolean* enabled);

    CARAPI GetFormattedQueryString(
        /* [out] */ String* str);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);

private:
    // TODO:
    // CARAPI AssignShortcutToView(
    //     /* [in] */ IContactListItemView* v,
    //     /* [in] */ Int32 shortcutType);

private:
    String mFormattedQueryString;
    String mCountryIso;

    AutoPtr<ArrayOf<Boolean> > mShortcutEnabled;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_DIALERPHONENUMBERLISTADAPTER_H__
