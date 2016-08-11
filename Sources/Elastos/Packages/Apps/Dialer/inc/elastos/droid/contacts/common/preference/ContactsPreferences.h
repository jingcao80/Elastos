
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_PREFERENCE_CONTACTSPREFERENCES_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_PREFERENCE_CONTACTSPREFERENCES_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Preference {

/**
 * Manages user preferences for contacts.
 */
class ContactsPreferences
    : public Object
    , public ISharedPreferencesOnSharedPreferenceChangeListener
{
private:
    class OnChangeRunnable : public Runnable
    {
    public:
        OnChangeRunnable(
            /* [in] */ const String& key,
            /* [in] */ ContactsPreferences* host)
            : mKey(key)
            , mHost(host)
        {}

        // @Override
        CARAPI Run();

    private:
        String mKey;
        ContactsPreferences* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ContactsPreferences(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsSortOrderUserChangeable();

    CARAPI_(Int32) GetDefaultSortOrder();

    CARAPI_(Int32) GetSortOrder();

    CARAPI_(void) SetSortOrder(
        /* [in] */ Int32 sortOrder);

    CARAPI_(Boolean) IsDisplayOrderUserChangeable();

    CARAPI_(Int32) GetDefaultDisplayOrder();

    CARAPI_(Int32) GetDisplayOrder();

    CARAPI_(void) SetDisplayOrder(
        /* [in] */ Int32 displayOrder);

    CARAPI_(void) RegisterChangeListener(
        /* [in] */ IContactsPreferencesChangeListener* listener);

    CARAPI_(void) UnregisterChangeListener();

    // @Override
    CARAPI OnSharedPreferenceChanged(
        /* [in] */ ISharedPreferences* sharedPreferences,
        /* [in] */ const String& key);

private:
    /**
     * If there are currently no preferences (which means this is the first time we are run),
     * check to see if there are any preferences stored in system settings (pre-L) which can be
     * copied into our own SharedPreferences.
     */
    CARAPI_(void) MaybeMigrateSystemSettings();

public:
    /**
     * The value for the DISPLAY_ORDER key to show the given name first.
     */
    static const Int32 DISPLAY_ORDER_PRIMARY = 1;

    /**
     * The value for the DISPLAY_ORDER key to show the family name first.
     */
    static const Int32 DISPLAY_ORDER_ALTERNATIVE = 2;

    static const String DISPLAY_ORDER_KEY;

    /**
     * The value for the SORT_ORDER key corresponding to sort by given name first.
     */
    static const Int32 SORT_ORDER_PRIMARY = 1;

    static const String SORT_ORDER_KEY;

    /**
     * The value for the SORT_ORDER key corresponding to sort by family name first.
     */
    static const Int32 SORT_ORDER_ALTERNATIVE = 2;

    static const String PREF_DISPLAY_ONLY_PHONES;
    static const Boolean PREF_DISPLAY_ONLY_PHONES_DEFAULT = FALSE;

private:
    AutoPtr<IContext> mContext;
    Int32 mSortOrder;
    Int32 mDisplayOrder;
    AutoPtr<IContactsPreferencesChangeListener> mListener;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ISharedPreferences> mPreferences;
};

} // namespace Preference
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_PREFERENCE_CONTACTSPREFERENCES_H__
