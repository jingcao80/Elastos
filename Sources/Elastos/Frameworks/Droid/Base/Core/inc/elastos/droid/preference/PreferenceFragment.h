#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAGMENT_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAGMENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Preference::IPreferenceManagerOnPreferenceTreeClickListener;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC PreferenceFragment
    : public Fragment
    , public IPreferenceFragment
    , public IPreferenceManagerOnPreferenceTreeClickListener
{
private:
    class ECO_LOCAL PreferenceFragmentHandler
        : public Handler
    {
    public:
        PreferenceFragmentHandler(
            /* [in] */ PreferenceFragment* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PreferenceFragment* mHost;
    };

    class ECO_LOCAL RequestFocus
        : public Runnable
    {
    public:
        RequestFocus(
            /* [in] */ PreferenceFragment* host);

        CARAPI Run();

    private:
        PreferenceFragment* mHost;
    };

    class ECO_LOCAL PreferenceFragmentOnKeyListener
        : public Object
        , public IViewOnKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

        PreferenceFragmentOnKeyListener(
            /* [in] */ PreferenceFragment* host);

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out]*/ Boolean* result);

    private:
        PreferenceFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PreferenceFragment();

    //@Override
    CARAPI OnCreate(
        /*[in]*/ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /*[in]*/ ILayoutInflater* inflater,
        /*[in]*/ IViewGroup* container,
        /*[in]*/ IBundle* savedInstanceState,
        /*[out]*/ IView** view);

    //@Override
    CARAPI OnActivityCreated(
        /*[in]*/ IBundle* savedInstanceState);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnDestroy();

    CARAPI OnSaveInstanceState(
        /*[in]*/ IBundle* outState);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    /**
     * Returns the {@link PreferenceManager} used by this fragment.
     * @return The {@link PreferenceManager}.
     */
    CARAPI GetPreferenceManager(
        /*[out]*/ IPreferenceManager** preferencemanager);

    /**
     * Sets the root of the preference hierarchy that this fragment is showing.
     *
     * @param preferenceScreen The root {@link PreferenceScreen} of the preference hierarchy.
     */
    CARAPI SetPreferenceScreen(
        /*[in]*/ IPreferenceScreen* preferenceScreen);

    /**
     * Gets the root of the preference hierarchy that this fragment is showing.
     *
     * @return The {@link PreferenceScreen} that is the root of the preference
     *         hierarchy.
     */
    CARAPI GetPreferenceScreen(
        /*[out]*/ IPreferenceScreen** preferencescreen);

    /**
     * Adds preferences from activities that match the given {@link Intent}.
     *
     * @param intent The {@link Intent} to query activities.
     */
    CARAPI AddPreferencesFromIntent(
        /*[in]*/ IIntent* intent);

    /**
     * Inflates the given XML resource and adds the preference hierarchy to the current
     * preference hierarchy.
     *
     * @param preferencesResId The XML resource ID to inflate.
     */
    CARAPI AddPreferencesFromResource(
        /*[in]*/ Int32 preferencesResId);

    /**
     * {@inheritDoc}
     */
    CARAPI OnPreferenceTreeClick(
        /*[in]*/ IPreferenceScreen* preferenceScreen,
        /*[in]*/ IPreference* preference,
        /*[out]*/ Boolean* result);

    /**
     * Finds a {@link Preference} based on its key.
     *
     * @param key The key of the preference to retrieve.
     * @return The {@link Preference} with the key, or null.
     * @see PreferenceGroup#findPreference(CharSequence)
     */
    CARAPI FindPreference(
        /*[in]*/ ICharSequence* key,
        /*[out]*/ IPreference** preference);

    /** @hide */
    CARAPI GetListView(
        /*[out]*/ IListView** listview);

    /** @hide */
    CARAPI HasListView(
        /* [out] */ Boolean* result);

protected:
    /** @hide */
    CARAPI OnBindPreferences();

    /** @hide */
    CARAPI OnUnbindPreferences();

private:
    ECO_LOCAL CARAPI RequirePreferenceManager();

    ECO_LOCAL CARAPI_(void) PostBindPreferences();

    ECO_LOCAL CARAPI_(void) BindPreferences();

    ECO_LOCAL CARAPI EnsureList();

private:
    ECO_LOCAL static const String PREFERENCES_TAG;
    /**
     * The starting request code given out to preference framework.
     */
    ECO_LOCAL static const Int32 FIRST_REQUEST_CODE = 100;

    ECO_LOCAL static const Int32 MSG_BIND_PREFERENCES = 1;

    AutoPtr<IPreferenceManager>  mPreferenceManager;
    AutoPtr<IListView> mList;
    Boolean mHavePrefs;
    Boolean mInitDone;
    Int32 mLayoutResId;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRunnable> mRequestFocus;
    AutoPtr<IViewOnKeyListener> mListOnKeyListener;
};

}
}
}

#endif //__ELASTOS_DROID_PREFERENCE_PREFERENCEFRAGMENT_H__
