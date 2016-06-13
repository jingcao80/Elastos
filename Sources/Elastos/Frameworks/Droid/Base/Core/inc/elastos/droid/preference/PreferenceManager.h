
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEMANAGER_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEMANAGER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceManager
    : public Object
    , public IPreferenceManager
{
public:
    CAR_INTERFACE_DECL()

    PreferenceManager();

    CARAPI constructor(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 firstRequestCode);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetFragment(
        /* [in] */ IPreferenceFragment* fragment);

    CARAPI GetFragment(
        /* [out] */ IPreferenceFragment** fragment);

    CARAPI InflateFromIntent(
        /* [in] */ IIntent* queryIntent,
        /* [in] */ IPreferenceScreen* rootPreferences,
        /* [out] */ IPreferenceScreen** screen);

    CARAPI InflateFromResource(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId,
        /* [in] */ IPreferenceScreen* rootPreferences,
        /* [out] */ IPreferenceScreen** screen);

    CARAPI CreatePreferenceScreen(
        /* [in] */ IContext* context,
        /* [out] */ IPreferenceScreen** screen);

    CARAPI GetNextId(
        /* [out] */ Int64* id);

    CARAPI GetSharedPreferencesName(
        /* [out] */ String* name);

    CARAPI SetSharedPreferencesName(
        /* [in] */ const String& sharedPreferencesName);

    CARAPI GetSharedPreferencesMode(
        /* [out] */ Int32* mode);

    CARAPI SetSharedPreferencesMode(
        /* [in] */ Int32 sharedPreferencesMode);

    CARAPI GetSharedPreferences(
        /* [out] */ ISharedPreferences** sp);

    /**
     * Gets a SharedPreferences instance that points to the default file that is
     * used by the preference framework in the given context.
     *
     * @param context The context of the preferences whose values are wanted.
     * @return A SharedPreferences instance that can be used to retrieve and
     *         listen to values of the preferences.
     */
    static CARAPI GetDefaultSharedPreferences(
        /* [in] */ IContext* context,
        /* [out] */ ISharedPreferences** sp);

    CARAPI GetPreferenceScreen(
        /* [out] */ IPreferenceScreen** screen);

    CARAPI SetPreferences(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [out] */ Boolean* result);

    CARAPI FindPreference(
        /* [in] */ ICharSequence* key,
        /* [out] */ IPreference** preference);

    /**
     * Sets the default values from an XML preference file by reading the values defined
     * by each {@link Preference} item's {@code android:defaultValue} attribute. This should
     * be called by the application's main activity.
     * <p>
     *
     * @param context The context of the shared preferences.
     * @param resId The resource ID of the preference XML file.
     * @param readAgain Whether to re-read the default values.
     * If false, this method sets the default values only if this
     * method has never been called in the past (or if the
     * {@link #KEY_HAS_SET_DEFAULT_VALUES} in the default value shared
     * preferences file is false). To attempt to set the default values again
     * bypassing this check, set {@code readAgain} to true.
     *            <p class="note">
     *            Note: this will NOT reset preferences back to their default
     *            values. For that functionality, use
     *            {@link PreferenceManager#getDefaultSharedPreferences(Context)}
     *            and clear it followed by a call to this method with this
     *            parameter set to true.
     */
    static CARAPI SetDefaultValues(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId,
        /* [in] */ Boolean readAgain);

    /**
     * Similar to {@link #setDefaultValues(Context, int, boolean)} but allows
     * the client to provide the filename and mode of the shared preferences
     * file.
     *
     * @param context The context of the shared preferences.
     * @param sharedPreferencesName A custom name for the shared preferences file.
     * @param sharedPreferencesMode The file creation mode for the shared preferences file, such
     * as {@link android.content.Context#MODE_PRIVATE} or {@link
     * android.content.Context#MODE_PRIVATE}
     * @param resId The resource ID of the preference XML file.
     * @param readAgain Whether to re-read the default values.
     * If false, this method will set the default values only if this
     * method has never been called in the past (or if the
     * {@link #KEY_HAS_SET_DEFAULT_VALUES} in the default value shared
     * preferences file is false). To attempt to set the default values again
     * bypassing this check, set {@code readAgain} to true.
     *            <p class="note">
     *            Note: this will NOT reset preferences back to their default
     *            values. For that functionality, use
     *            {@link PreferenceManager#getDefaultSharedPreferences(Context)}
     *            and clear it followed by a call to this method with this
     *            parameter set to true.
     *
     * @see #setDefaultValues(Context, int, boolean)
     * @see #setSharedPreferencesName(String)
     * @see #setSharedPreferencesMode(int)
     */
    static CARAPI SetDefaultValues(
        /* [in] */ IContext* context,
        /* [in] */ const String& sharedPreferencesName,
        /* [in] */ Int32 sharedPreferencesMode,
        /* [in] */ Int32 resId,
        /* [in] */ Boolean readAgain);

    CARAPI GetEditor(
        /* [out] */ ISharedPreferencesEditor** editor);

    CARAPI ShouldCommit(
        /* [out] */ Boolean* shouldCommit);

    CARAPI GetActivity(
        /* [out] */ IActivity** activity);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI RegisterOnActivityResultListener(
        /* [in] */ IPreferenceManagerOnActivityResultListener* listener);

    CARAPI UnregisterOnActivityResultListener(
        /* [in] */ IPreferenceManagerOnActivityResultListener* listener);

    CARAPI DispatchActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI RegisterOnActivityStopListener(
        /* [in] */ IPreferenceManagerOnActivityStopListener* listener);

    CARAPI UnregisterOnActivityStopListener(
        /* [in] */ IPreferenceManagerOnActivityStopListener* listener);

    CARAPI DispatchActivityStop();

    CARAPI RegisterOnActivityDestroyListener(
        /* [in] */ IPreferenceManagerOnActivityDestroyListener* listener);

    CARAPI UnregisterOnActivityDestroyListener(
        /* [in] */ IPreferenceManagerOnActivityDestroyListener* listener);

    CARAPI DispatchActivityDestroy();

    CARAPI GetNextRequestCode(
        /* [out] */ Int32* code);

    CARAPI AddPreferencesScreen(
        /* [in] */ IDialogInterface* screen);

    CARAPI RemovePreferencesScreen(
        /* [in] */ IDialogInterface* screen);

    CARAPI DispatchNewIntent(
        /* [in] */ IIntent* intent);

    CARAPI SetOnPreferenceTreeClickListener(
        /* [in] */ IPreferenceManagerOnPreferenceTreeClickListener* listener);

    CARAPI GetOnPreferenceTreeClickListener(
        /* [out] */ IPreferenceManagerOnPreferenceTreeClickListener** listener);

private:
    CARAPI Init(
        /* [in] */ IContext* context);

    CARAPI_(AutoPtr<IList>) QueryIntentActivities(
        /* [in] */ IIntent* queryIntent);

    static CARAPI_(String) GetDefaultSharedPreferencesName(
        /* [in] */ IContext* context);

    static CARAPI_(Int32) GetDefaultSharedPreferencesMode();

    CARAPI SetNoCommit(
        /* [in] */ Boolean noCommit);

    CARAPI_(void) DismissAllScreens();

private:
    // TODO: Add your private member variables here.
    static const String TAG;

    /**
     * @see #getActivity()
     */
    AutoPtr<IActivity> mActivity;

    /**
     * Fragment that owns this instance.
     */
    AutoPtr<IPreferenceFragment> mFragment;

    /**
     * The context to use. This should always be set.
     *
     * @see #mActivity
     */
    AutoPtr<IContext> mContext;

    /**
     * The counter for unique IDs.
     */
    Int64 mNextId;

    /**
     * The counter for unique request codes.
     */
    Int32 mNextRequestCode;

    /**
     * Cached shared preferences.
     */
    AutoPtr<ISharedPreferences> mSharedPreferences;

    /**
     * If in no-commit mode, the shared editor to give out (which will be
     * committed when exiting no-commit mode).
     */
    AutoPtr<ISharedPreferencesEditor> mEditor;

    /**
     * Blocks commits from happening on the shared editor. This is used when
     * inflating the hierarchy. Do not set this directly, use {@link #setNoCommit(boolean)}
     */
    Boolean mNoCommit;

    /**
     * The SharedPreferences name that will be used for all {@link Preference}s
     * managed by this instance.
     */
    String mSharedPreferencesName;

    /**
     * The SharedPreferences mode that will be used for all {@link Preference}s
     * managed by this instance.
     */
    Int32 mSharedPreferencesMode;

    /**
     * The {@link PreferenceScreen} at the root of the preference hierarchy.
     */
    AutoPtr<IPreferenceScreen> mPreferenceScreen;

    /**
     * List of activity result listeners.
     */
    AutoPtr<List<AutoPtr<IPreferenceManagerOnActivityResultListener> > > mActivityResultListeners;

    /**
     * List of activity stop listeners.
     */
    AutoPtr<List<AutoPtr<IPreferenceManagerOnActivityStopListener> > > mActivityStopListeners;

    /**
     * List of activity destroy listeners.
     */
    AutoPtr<List<AutoPtr<IPreferenceManagerOnActivityDestroyListener> > > mActivityDestroyListeners;

    /**
     * List of dialogs that should be dismissed when we receive onNewIntent in
     * our PreferenceActivity.
     */
    AutoPtr<List<AutoPtr<IDialogInterface> > > mPreferencesScreens;

    AutoPtr<IPreferenceManagerOnPreferenceTreeClickListener> mOnPreferenceTreeClickListener;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCEMANAGER_H__
