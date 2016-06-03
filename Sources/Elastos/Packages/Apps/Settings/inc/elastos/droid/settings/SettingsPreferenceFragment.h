#ifndef __ELASTOS_DROID_SETTINGS_SETTINGSPREFERENCEFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_SETTINGSPREFERENCEFRAGMENT_H__

#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/preference/PreferenceFragment.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::PreferenceFragment;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Base class for Settings fragments, with some helper functions and dialog management.
 */
class SettingsPreferenceFragment
    : public PreferenceFragment
    , public ISettingsPreferenceFragment
    , public IDialogCreatable
{
public:
    class SettingsDialogFragment
        : public DialogFragment
    {
        friend class SettingsPreferenceFragment;
    public:
        SettingsDialogFragment();

        ~SettingsDialogFragment();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IDialogCreatable* fragment,
            /* [in] */ Int32 dialogId);

        //@Override
        CARAPI OnSaveInstanceState(
            /* [in] */ IBundle* outState);

        //@Override
        CARAPI OnStart();

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** dialog);

        //@Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

        CARAPI_(Int32) GetDialogId();

        //@Override
        CARAPI OnDetach();

    private:
        static const String KEY_DIALOG_ID;
        static const String KEY_PARENT_FRAGMENT_ID;

        Int32 mDialogId;

        AutoPtr<IFragment> mParentFragment;

        AutoPtr<IDialogInterfaceOnCancelListener> mOnCancelListener;
        AutoPtr<IDialogInterfaceOnDismissListener> mOnDismissListener;
    };

private:
    class InitDataSetObserver
        : public DataSetObserver
    {
    public:
        InitDataSetObserver(
            /* [in] */ SettingsPreferenceFragment* host);

        ~InitDataSetObserver();

        //@Override
        CARAPI OnChanged();

        //@Override
        CARAPI OnInvalidated();

    private:
        SettingsPreferenceFragment* mHost;
    };

    class RunnableInHighlightPreference
        : public Runnable
    {
    public:
        RunnableInHighlightPreference(
            /* [in] */ IListView* listView,
            /* [in] */ Int32 position,
            /* [in] */ IDrawable* highlight);

        ~RunnableInHighlightPreference();

        //@Override
        CARAPI Run();

    private:
        AutoPtr<IListView> mListView;
        Int32 mPosition;
        AutoPtr<IDrawable> mHighlight;
    };

    class RunnableInHighlightPreference2
        : public Runnable
    {
    public:
        RunnableInHighlightPreference2(
            /* [in] */ IListView* listView,
            /* [in] */ Int32 position,
            /* [in] */ IDrawable* highlight);

        ~RunnableInHighlightPreference2();

        //@Override
        CARAPI Run();

    private:
        AutoPtr<IListView> mListView;
        Int32 mPosition;
        AutoPtr<IDrawable> mHighlight;
    };

public:
    CAR_INTERFACE_DECL();

    SettingsPreferenceFragment();

    ~SettingsPreferenceFragment();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    CARAPI SetPinnedHeaderView(
        /* [in] */ IView* pinnedHeader);

    CARAPI ClearPinnedHeaderView();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnStop();

    CARAPI RegisterObserverIfNeeded();

    CARAPI UnregisterObserverIfNeeded();

    CARAPI HighlightPreferenceIfNeeded();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    /*
     * The name is intentionally made different from Activity#Finish(), so that
     * users won't misunderstand its meaning.
     */
    CARAPI_(void) FinishFragment();

    //@Override
    CARAPI OnDetach();

    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** dialog);

    CARAPI OnDialogShowing();

    CARAPI Finish();

    CARAPI_(Boolean) StartFragment(
        /* [in] */ IFragment* caller,
        /* [in] */ const String& fragmentClass,
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* extras);

protected:
    //@Override
    CARAPI OnBindPreferences();

    //@Override
    CARAPI OnUnbindPreferences();

    CARAPI_(void) RemovePreference(
        /* [in] */ const String& key);

    /**
     * Override this if you want to show a help item in the menu, by returning the resource id.
     * @return the resource id for the help url
     */
    CARAPI_(Int32) GetHelpResource();

    // Some helpers for functions used by the settings fragments when they were activities

    /**
     * Returns the ContentResolver from the owning Activity.
     */
    CARAPI_(AutoPtr<IContentResolver>) GetContentResolver();

    /**
     * Returns the specified system service from the owning Activity.
     */
    CARAPI_(AutoPtr<IInterface>) GetSystemService(
        /* [in] */ const String& name);
    /**
     * Returns the PackageManager from the owning Activity.
     */
    CARAPI_(AutoPtr<IPackageManager>) GetPackageManager();

    // Dialog management

    CARAPI_(void) ShowDialog(
        /* [in] */ Int32 dialogId);

    CARAPI_(void) RemoveDialog(
        /* [in] */ Int32 dialogId);

    /**
     * Sets the OnCancelListener of the dialog shown. This method can only be
     * called after ShowDialog(Int32) and before RemoveDialog(Int32). The method
     * does nothing otherwise.
     */
    CARAPI_(void) SetOnCancelListener(
        /* [in] */ IDialogInterfaceOnCancelListener* listener);

    /**
     * Sets the OnDismissListener of the dialog shown. This method can only be
     * called after ShowDialog(Int32) and before RemoveDialog(Int32). The method
     * does nothing otherwise.
     */
    CARAPI_(void) SetOnDismissListener(
        /* [in] */ IDialogInterfaceOnDismissListener* listener);

    CARAPI_(Boolean) HasNextButton();

    CARAPI_(AutoPtr<IButton>) GetNextButton();

private:
    CARAPI_(AutoPtr<IDrawable>) GetHighlightDrawable();

    /**
     * Return a valid ListView position or -1 if none is found
     */
    CARAPI_(Int32) CanUseListViewForHighLighting(
        /* [in] */ const String& key);

    CARAPI_(void) HighlightPreference(
        /* [in] */ const String& key);

    CARAPI_(Int32) FindListPositionFromKey(
        /* [in] */ IListAdapter* adapter,
        /* [in] */ const String& key);

private:
    static const String TAG;

    static const Int32 MENU_HELP;
    static const Int32 DELAY_HIGHLIGHT_DURATION_MILLIS;

    static const String SAVE_HIGHLIGHTED_KEY;

    AutoPtr<SettingsDialogFragment> mDialogFragment;

    String mHelpUrl;

    // Cache the content resolver for async callbacks
    AutoPtr<IContentResolver> mContentResolver;

    String mPreferenceKey;
    Boolean mPreferenceHighlighted;
    AutoPtr<IDrawable> mHighlightDrawable;

    AutoPtr<IListAdapter> mCurrentRootAdapter;
    Boolean mIsDataSetObserverRegistered;
    AutoPtr<IDataSetObserver> mDataSetObserver;

    AutoPtr<IViewGroup> mPinnedHeaderFrameLayout;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SETTINGSPREFERENCEFRAGMENT_H__
