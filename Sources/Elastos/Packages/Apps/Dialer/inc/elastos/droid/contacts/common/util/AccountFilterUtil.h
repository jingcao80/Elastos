
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_ACCOUNTFILTERUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_ACCOUNTFILTERUTILS_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Contacts::Common::List::IContactListFilter;
using Elastos::Droid::Contacts::Common::List::IContactListFilterController;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Utility class for account filter manipulation.
 */
class AccountFilterUtil
{
public:
    /**
     * Find TextView with the id "account_filter_header" and set correct text for the account
     * filter header.
     *
     * @param filterContainer View containing TextView with id "account_filter_header"
     * @return true when header text is set in the call. You may use this for conditionally
     * showing or hiding this entire view.
     */
    static CARAPI_(Boolean) UpdateAccountFilterTitleForPeople(
        /* [in] */ IView* filterContainer,
        /* [in] */ IContactListFilter* filter,
        /* [in] */ Boolean showTitleForAllAccounts);

    /**
     * Similar to {@link #updateAccountFilterTitleForPeople(View, ContactListFilter, boolean,
     * boolean)}, but for Phone UI.
     */
    static CARAPI_(Boolean) UpdateAccountFilterTitleForPhone(
        /* [in] */ IView* filterContainer,
        /* [in] */ IContactListFilter* filter,
        /* [in] */ Boolean showTitleForAllAccounts);

    /**
     * Launches account filter setting Activity using
     * {@link Activity#startActivityForResult(Intent, int)}.
     *
     * @param activity
     * @param requestCode requestCode for {@link Activity#startActivityForResult(Intent, int)}
     * @param currentFilter currently-selected filter, so that it can be displayed as activated.
     */
    static CARAPI_(void) StartAccountFilterActivityForResult(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 requestCode,
        /* [in] */ IContactListFilter* currentFilter);

    /**
     * Very similar to
     * {@link #startAccountFilterActivityForResult(Activity, int, ContactListFilter)}
     * but uses Fragment instead.
     */
    static CARAPI_(void) StartAccountFilterActivityForResult(
        /* [in] */ IFragment* fragment,
        /* [in] */ Int32 requestCode,
        /* [in] */ IContactListFilter* currentFilter);

    /**
     * Useful method to handle onActivityResult() for
     * {@link #startAccountFilterActivityForResult(Activity, int)} or
     * {@link #startAccountFilterActivityForResult(Fragment, int)}.
     *
     * This will update filter via a given ContactListFilterController.
     */
    static CARAPI_(void) HandleAccountFilterResult(
        /* [in] */ IContactListFilterController* filterController,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

private:
    static CARAPI_(Boolean) UpdateAccountFilterTitle(
        /* [in] */ IView* filterContainer,
        /* [in] */ IContactListFilter* filter,
        /* [in] */ Boolean showTitleForAllAccounts,
        /* [in] */ Boolean forPhone);

private:
    static const String TAG;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_ACCOUNTFILTERUTILS_H__
