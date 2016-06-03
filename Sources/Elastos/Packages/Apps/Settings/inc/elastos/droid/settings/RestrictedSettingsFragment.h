
#ifndef __ELASTOS_DROID_SETTINGS_RESTRICTEDSETTINGSFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_RESTRICTEDSETTINGSFRAGMENT_H__

#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IRestrictionsManager;
using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Os::IUserManager;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Base class for settings screens that should be pin protected when in restricted mode.
 * The constructor for this class will take the restriction key that this screen should be
 * locked by.  If {@link RestrictionsManager->HasRestrictionsProvider()} and
 * {@link UserManager->HasUserRestriction()}, then the user will have to enter the restrictions
 * pin before seeing the Settings screen.
 *
 * If this settings screen should be pin protected whenever
 * {@link RestrictionsManager->HasRestrictionsProvider()} returns TRUE, pass in
 * {@link RESTRICT_IF_OVERRIDABLE} to the constructor instead of a restrictions key.
 */

class RestrictedSettingsFragment
    : public SettingsPreferenceFragment
{
private:
    // Receiver to clear pin status when the screen is turned off.
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ RestrictedSettingsFragment* host);

        ~MyBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        RestrictedSettingsFragment* mHost;
    };

public:
    RestrictedSettingsFragment();

    ~RestrictedSettingsFragment();

    /**
     * @param restrictionKey The restriction key to check before pin protecting
     *            this settings page. Pass in {@link RESTRICT_IF_OVERRIDABLE} if it should
     *            be protected whenever a restrictions provider is set. Pass in
     *            NULL if it should never be protected.
     */
    CARAPI constructor(
        /* [in] */ const String& restrictionKey);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

protected:
    /**
     * Returns TRUE if this activity is restricted, but no restrictions provider has been set.
     * Used to determine if the settings UI should disable UI.
     */
    CARAPI_(Boolean) IsRestrictedAndNotProviderProtected();

    CARAPI_(Boolean) HasChallengeSucceeded();

    /**
     * Returns TRUE if this restrictions key is locked down.
     */
    CARAPI_(Boolean) ShouldBeProviderProtected(
        /* [in] */ const String& restrictionKey);

    /**
     * Returns whether restricted or actionable UI elements should be removed or disabled.
     */
    CARAPI_(Boolean) IsUiRestricted();

private:
    CARAPI_(void) EnsurePin();

protected:
    static const String RESTRICT_IF_OVERRIDABLE;

private:
    // No RestrictedSettingsFragment screens should use this number in startActivityForResult.
    static const Int32 REQUEST_PIN_CHALLENGE;

    static const String KEY_CHALLENGE_SUCCEEDED;
    static const String KEY_CHALLENGE_REQUESTED;

    // If the restriction PIN is entered correctly.
    Boolean mChallengeSucceeded;
    Boolean mChallengeRequested;

    AutoPtr<IUserManager> mUserManager;
    AutoPtr<IRestrictionsManager> mRestrictionsManager;

    String mRestrictionKey;

    // Receiver to clear pin status when the screen is turned off.
    AutoPtr<BroadcastReceiver> mScreenOffReceiver;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_RESTRICTEDSETTINGSFRAGMENT_H__
