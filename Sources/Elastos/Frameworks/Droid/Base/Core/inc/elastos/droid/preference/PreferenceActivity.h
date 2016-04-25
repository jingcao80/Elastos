
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEACTIVITY_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEACTIVITY_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Preference.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/app/ListActivity.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::App::ListActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentBreadCrumbs;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreferenceActivityHeaderAdapter;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC PreferenceActivity
    : public ListActivity
    , public IPreferenceActivity
    , public IPreferenceManagerOnPreferenceTreeClickListener
    , public IPreferenceFragmentOnPreferenceStartFragmentCallback
{
private:
    class ECO_LOCAL MHandler
        : public Handler
    {
        friend class PreferenceActivity;

    public:
        TO_STRING_IMPL("PreferenceActivity::MHandler")

        MHandler(
            /* [in] */ PreferenceActivity* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PreferenceActivity* mHost;
    };

    class ECO_LOCAL HeaderAdapter
        : public ArrayAdapter
        , public IPreferenceActivityHeaderAdapter
    {
    private:
        class HeaderViewHolder
            : public Object
        {
        public:
            AutoPtr<IImageView> mIcon;
            AutoPtr<ITextView> mTitle;
            AutoPtr<ITextView> mSummary;
        };

    public:
        CAR_INTERFACE_DECL()

        HeaderAdapter(
            /* [in] */ IContext* context,
            /* [in] */ IList* objects,
            /* [in] */ Int32 layoutResId,
            /* [in] */ Boolean removeIconBehavior);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        AutoPtr<ILayoutInflater> mInflater;
        Int32 mLayoutResId;
        Boolean mRemoveIconIfEmpty;
    };

public:
    class Header
        : public Object
        , public IPreferenceActivityHeader
        , public IParcelable
    {
    public:
        CAR_INTERFACE_DECL()

        Header();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* source);

        /**
         * Return the currently set title.  If {@link #titleRes} is set,
         * this resource is loaded from <var>res</var> and returned.  Otherwise
         * {@link #title} is returned.
         */
        CARAPI GetTitle(
            /* [in] */ IResources* res,
            /* [out] */ ICharSequence** title);

        /**
         * Return the currently set summary.  If {@link #summaryRes} is set,
         * this resource is loaded from <var>res</var> and returned.  Otherwise
         * {@link #summary} is returned.
         */
        CARAPI GetSummary(
            /* [in] */ IResources* res,
            /* [out] */ ICharSequence** summary);

        /**
         * Return the currently set bread crumb title.  If {@link #breadCrumbTitleRes} is set,
         * this resource is loaded from <var>res</var> and returned.  Otherwise
         * {@link #breadCrumbTitle} is returned.
         */
        CARAPI GetBreadCrumbTitle(
            /* [in] */ IResources* res,
            /* [out] */ ICharSequence** title);

        /**
         * Return the currently set bread crumb short title.  If
         * {@link #breadCrumbShortTitleRes} is set,
         * this resource is loaded from <var>res</var> and returned.  Otherwise
         * {@link #breadCrumbShortTitle} is returned.
         */
        CARAPI GetBreadCrumbShortTitle(
            /* [in] */ IResources* res,
            /* [out] */ ICharSequence** title);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI GetId(
            /* [out] */ Int64* id);

        CARAPI SetId(
            /* [in] */ Int64 id);

        CARAPI GetTitleRes(
            /* [out] */ Int32* titleRes);

        CARAPI SetTitleRes(
            /* [in] */ Int32 titleRes);

        CARAPI GetTitle(
            /* [out] */ ICharSequence** title);

        CARAPI SetTitle(
            /* [in] */ ICharSequence* title);

        CARAPI GetSummaryRes(
            /* [out] */ Int32* summaryRes);

        CARAPI SetSummaryRes(
            /* [in] */ Int32 summaryRes);

        CARAPI GetSummary(
            /* [out] */ ICharSequence** summary);

        CARAPI SetSummary(
            /* [in] */ ICharSequence* summary);

        CARAPI GetBreadCrumbTitleRes(
            /* [out] */ Int32* breadCrumbTitleRes);

        CARAPI SetBreadCrumbTitleRes(
            /* [in] */ Int32 breadCrumbTitleRes);

        CARAPI GetBreadCrumbTitle(
            /* [out] */ ICharSequence** breadCrumbTitle);

        CARAPI SetBreadCrumbTitle(
            /* [in] */ ICharSequence* breadCrumbTitle);

        CARAPI GetBreadCrumbShortTitleRes(
            /* [out] */ Int32* breadCrumbShortTitleRes);

        CARAPI SetBreadCrumbShortTitleRes(
            /* [in] */ Int32 breadCrumbShortTitleRes);

        CARAPI GetBreadCrumbShortTitle(
            /* [out] */ ICharSequence** breadCrumbShortTitle);

        CARAPI SetBreadCrumbShortTitle(
            /* [in] */ ICharSequence* breadCrumbShortTitle);

        CARAPI GetIconRes(
            /* [out] */ Int32* iconRes);

        CARAPI SetIconRes(
            /* [in] */ Int32 iconRes);

        CARAPI GetFragment(
            /* [out] */ String* fragment);

        CARAPI SetFragment(
            /* [in] */ const String& fragment);

        CARAPI GetFragmentArguments(
            /* [out] */ IBundle** fragmentArguments);

        CARAPI SetFragmentArguments(
            /* [in] */ IBundle* fragmentArguments);

        CARAPI GetIntent(
            /* [out] */ IIntent** intent);

        CARAPI SetIntent(
            /* [in] */ IIntent* intent);

        CARAPI GetExtras(
            /* [out] */ IBundle** extras);

        CARAPI SetExtras(
            /* [in] */ IBundle* extras);

    public:
        /**
         * Identifier for this header, to correlate with a new list when
         * it is updated.  The default value is
         * {@link PreferenceActivity#HEADER_ID_UNDEFINED}, meaning no id.
         * @attr ref android.R.styleable#PreferenceHeader_id
         */
        Int64 mId;

        /**
         * Resource ID of title of the header that is shown to the user.
         * @attr ref android.R.styleable#PreferenceHeader_title
         */
        Int32 mTitleRes;

        /**
         * Title of the header that is shown to the user.
         * @attr ref android.R.styleable#PreferenceHeader_title
         */
        AutoPtr<ICharSequence> mTitle;

        /**
         * Resource ID of optional summary describing what this header controls.
         * @attr ref android.R.styleable#PreferenceHeader_summary
         */
        Int32 mSummaryRes;

        /**
         * Optional summary describing what this header controls.
         * @attr ref android.R.styleable#PreferenceHeader_summary
         */
        AutoPtr<ICharSequence> mSummary;

        /**
         * Resource ID of optional text to show as the title in the bread crumb.
         * @attr ref android.R.styleable#PreferenceHeader_breadCrumbTitle
         */
        Int32 mBreadCrumbTitleRes;

        /**
         * Optional text to show as the title in the bread crumb.
         * @attr ref android.R.styleable#PreferenceHeader_breadCrumbTitle
         */
        AutoPtr<ICharSequence> mBreadCrumbTitle;

        /**
         * Resource ID of optional text to show as the short title in the bread crumb.
         * @attr ref android.R.styleable#PreferenceHeader_breadCrumbShortTitle
         */
        Int32 mBreadCrumbShortTitleRes;

        /**
         * Optional text to show as the short title in the bread crumb.
         * @attr ref android.R.styleable#PreferenceHeader_breadCrumbShortTitle
         */
        AutoPtr<ICharSequence> mBreadCrumbShortTitle;

        /**
         * Optional icon resource to show for this header.
         * @attr ref android.R.styleable#PreferenceHeader_icon
         */
        Int32 mIconRes;

        /**
         * Full class name of the fragment to display when this header is
         * selected.
         * @attr ref android.R.styleable#PreferenceHeader_fragment
         */
        String mFragment;

        /**
         * Optional arguments to supply to the fragment when it is
         * instantiated.
         */
        AutoPtr<IBundle> mFragmentArguments;

        /**
         * Intent to launch when the preference is selected.
         */
        AutoPtr<IIntent> mIntent;

        /**
         * Optional additional data for use by subclasses of PreferenceActivity.
         */
        AutoPtr<IBundle> mExtras;
    };

private:
    class ECO_LOCAL BackButtonListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        BackButtonListener(
            /* [in] */ PreferenceActivity* parent);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PreferenceActivity* mParent;
    };

    class ECO_LOCAL SkipButtonListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SkipButtonListener(
            /* [in] */ PreferenceActivity* parent);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PreferenceActivity* mParent;
    };

    class ECO_LOCAL NextButtonListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NextButtonListener(
            /* [in] */ PreferenceActivity* parent);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PreferenceActivity* mParent;
    };

public:
    CAR_INTERFACE_DECL()

    PreferenceActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Returns true if this activity is currently showing the header list.
     */
    CARAPI HasHeaders(
        /* [out] */ Boolean* hasHeaders);

    /**
     * Returns the Header list
     * @hide
     */
    CARAPI GetHeaders(
        /* [out] */ IList** headers);

    /**
     * Returns true if this activity is showing multiple panes -- the headers
     * and a preference fragment.
     */
    CARAPI IsMultiPane(
        /* [out] */ Boolean* isMultiPane);

    /**
     * Called to determine if the activity should run in multi-pane mode.
     * The default implementation returns true if the screen is large
     * enough.
     */
    CARAPI OnIsMultiPane(
        /* [out] */ Boolean* result);

    /**
     * Called to determine whether the header list should be hidden.
     * The default implementation returns the
     * value given in {@link #EXTRA_NO_HEADERS} or false if it is not supplied.
     * This is set to false, for example, when the activity is being re-launched
     * to show a particular preference activity.
     */
    CARAPI OnIsHidingHeaders(
        /* [out] */ Boolean* result);

    /**
     * Called to determine the initial header to be shown.  The default
     * implementation simply returns the fragment of the first header.  Note
     * that the returned Header object does not actually need to exist in
     * your header list -- whatever its fragment is will simply be used to
     * show for the initial UI.
     */
    CARAPI OnGetInitialHeader(
        /* [out] */ IPreferenceActivityHeader** header);

    /**
     * Called after the header list has been updated ({@link #onBuildHeaders}
     * has been called and returned due to {@link #invalidateHeaders()}) to
     * specify the header that should now be selected.  The default implementation
     * returns null to keep whatever header is currently selected.
     */
    CARAPI OnGetNewHeader(
        /* [out] */ IPreferenceActivityHeader** header);

    /**
     * Called when the activity needs its list of headers build.  By
     * implementing this and adding at least one item to the list, you
     * will cause the activity to run in its modern fragment mode.  Note
     * that this function may not always be called; for example, if the
     * activity has been asked to display a particular fragment without
     * the header list, there is no need to build the headers.
     *
     * <p>Typical implementations will use {@link #loadHeadersFromResource}
     * to fill in the list from a resource.
     *
     * @param target The list in which to place the headers.
     */
    CARAPI OnBuildHeaders(
        /* [in] */ IList* target);

    /**
     * Call when you need to change the headers being displayed.  Will result
     * in onBuildHeaders() later being called to retrieve the new list.
     */
    CARAPI InvalidateHeaders();

    /**
     * Parse the given XML file as a header description, adding each
     * parsed Header into the target list.
     *
     * @param resid The XML resource to load and parse.
     * @param target The list in which the parsed headers should be placed.
     */
    CARAPI LoadHeadersFromResource(
        /* [in] */ Int32 resid,
        /* [in] */ IList* target);

    /**
     * Set a footer that should be shown at the bottom of the header list.
     */
    CARAPI SetListFooter(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnContentChanged();

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    /**
     * Called when the user selects an item in the header list.  The default
     * implementation will call either
     * {@link #startWithFragment(String, Bundle, Fragment, int, int, int)}
     * or {@link #switchToHeader(Header)} as appropriate.
     *
     * @param header The header that was selected.
     * @param position The header's position in the list.
     */
    CARAPI OnHeaderClick(
        /* [in] */ IPreferenceActivityHeader* header,
        /* [in] */ Int32 position);

    /**
     * Called by {@link #startWithFragment(String, Bundle, Fragment, int, int, int)} when
     * in single-pane mode, to build an Intent to launch a new activity showing
     * the selected fragment.  The default implementation constructs an Intent
     * that re-launches the current activity with the appropriate arguments to
     * display the fragment.
     *
     * @param fragmentName The name of the fragment to display.
     * @param args Optional arguments to supply to the fragment.
     * @param titleRes Optional resource ID of title to show for this item.
     * @param shortTitleRes Optional resource ID of short title to show for this item.
     * @return Returns an Intent that can be launched to display the given
     * fragment.
     */
    CARAPI OnBuildStartFragmentIntent(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 shortTitleRes,
        /* [out] */ IIntent** intent);

    /**
     * Like {@link #startWithFragment(String, Bundle, Fragment, int, int, int)}
     * but uses a 0 titleRes.
     */
    CARAPI StartWithFragment(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ IFragment* resultTo,
        /* [in] */ Int32 resultRequestCode);

    /**
     * Start a new instance of this activity, showing only the given
     * preference fragment.  When launched in this mode, the header list
     * will be hidden and the given preference fragment will be instantiated
     * and fill the entire activity.
     *
     * @param fragmentName The name of the fragment to display.
     * @param args Optional arguments to supply to the fragment.
     * @param resultTo Option fragment that should receive the result of
     * the activity launch.
     * @param resultRequestCode If resultTo is non-null, this is the request
     * code in which to report the result.
     * @param titleRes Resource ID of string to display for the title of
     * this set of preferences.
     * @param shortTitleRes Resource ID of string to display for the short title of
     * this set of preferences.
     */
    CARAPI StartWithFragment(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ IFragment* resultTo,
        /* [in] */ Int32 resultRequestCode,
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 shortTitleRes);

    /**
     * Change the base title of the bread crumbs for the current preferences.
     * This will normally be called for you.  See
     * {@link android.app.FragmentBreadCrumbs} for more information.
     */
    CARAPI ShowBreadCrumbs(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* shortTitle);

    /**
     * Should be called after onCreate to ensure that the breadcrumbs, if any, were created.
     * This prepends a title to the fragment breadcrumbs and attaches a listener to any clicks
     * on the parent entry.
     * @param title the title for the breadcrumb
     * @param shortTitle the short title for the breadcrumb
     */
    CARAPI SetParentTitle(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* shortTitle,
        /* [in] */ IViewOnClickListener* listener);

    /**
     * When in two-pane mode, switch the fragment pane to show the given
     * preference fragment.
     *
     * @param fragmentName The name of the fragment to display.
     * @param args Optional arguments to supply to the fragment.
     */
    CARAPI SwitchToHeader(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args);

    /**
     * When in two-pane mode, switch to the fragment pane to show the given
     * preference fragment.
     *
     * @param header The new header to display.
     */
    CARAPI SwitchToHeader(
        /* [in] */ IPreferenceActivityHeader* header);

    /**
     * Start a new fragment.
     *
     * @param fragment The fragment to start
     * @param push If true, the current fragment will be pushed onto the back stack.  If false,
     * the current fragment will be replaced.
     */
    CARAPI StartPreferenceFragment(
        /* [in] */ IFragment* fragment,
        /* [in] */ Boolean push);

    /**
     * Start a new fragment containing a preference panel.  If the prefences
     * are being displayed in multi-pane mode, the given fragment class will
     * be instantiated and placed in the appropriate pane.  If running in
     * single-pane mode, a new activity will be launched in which to show the
     * fragment.
     *
     * @param fragmentClass Full name of the class implementing the fragment.
     * @param args Any desired arguments to supply to the fragment.
     * @param titleRes Optional resource identifier of the title of this
     * fragment.
     * @param titleText Optional text of the title of this fragment.
     * @param resultTo Optional fragment that result data should be sent to.
     * If non-null, resultTo.onActivityResult() will be called when this
     * preference panel is done.  The launched panel must use
     * {@link #finishPreferencePanel(Fragment, int, Intent)} when done.
     * @param resultRequestCode If resultTo is non-null, this is the caller's
     * request code to be received with the resut.
     */
    CARAPI StartPreferencePanel(
        /* [in] */ const String& fragmentClass,
        /* [in] */ IBundle* args,
        /* [in] */ Int32 titleRes,
        /* [in] */ ICharSequence* titleText,
        /* [in] */ IFragment* resultTo,
        /* [in] */ Int32 resultRequestCode);

    /**
     * Called by a preference panel fragment to finish itself.
     *
     * @param caller The fragment that is asking to be finished.
     * @param resultCode Optional result code to send back to the original
     * launching fragment.
     * @param resultData Optional result data to send back to the original
     * launching fragment.
     */
    CARAPI FinishPreferencePanel(
        /* [in] */ IFragment* caller,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    //@Override
    CARAPI OnPreferenceStartFragment(
        /* [in] */ IPreferenceFragment* caller,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

    /**
     * Returns the {@link PreferenceManager} used by this activity.
     * @return The {@link PreferenceManager}.
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI GetPreferenceManager(
        /* [out] */ IPreferenceManager** manager);

    /**
     * Sets the root of the preference hierarchy that this activity is showing.
     *
     * @param preferenceScreen The root {@link PreferenceScreen} of the preference hierarchy.
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI SetPreferenceScreen(
        /* [in] */ IPreferenceScreen* preferenceScreen);

    /**
     * Gets the root of the preference hierarchy that this activity is showing.
     *
     * @return The {@link PreferenceScreen} that is the root of the preference
     *         hierarchy.
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI GetPreferenceScreen(
        /* [out] */ IPreferenceScreen** screen);

    /**
     * Adds preferences from activities that match the given {@link Intent}.
     *
     * @param intent The {@link Intent} to query activities.
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI AddPreferencesFromIntent(
        /* [in] */ IIntent* intent);

    /**
     * Inflates the given XML resource and adds the preference hierarchy to the current
     * preference hierarchy.
     *
     * @param preferencesResId The XML resource ID to inflate.
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI AddPreferencesFromResource(
        /* [in] */ Int32 preferencesResId);

    /**
     * {@inheritDoc}
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    /**
     * Finds a {@link Preference} based on its key.
     *
     * @param key The key of the preference to retrieve.
     * @return The {@link Preference} with the key, or null.
     * @see PreferenceGroup#findPreference(CharSequence)
     *
     * @deprecated This function is not relevant for a modern fragment-based
     * PreferenceActivity.
     *
     * @Deprecated
     */
    CARAPI FindPreference(
        /* [in] */ const String& key,
        /* [out] */ IPreference** preference);

    //@Override
    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

protected:
    // give subclasses access to the Next button
    /** @hide */
    CARAPI_(Boolean) HasNextButton();

    /** @hide */
    CARAPI_(AutoPtr<IButton>) GetNextButton();

    /**
     * Subclasses should override this method and verify that the given fragment is a valid type
     * to be attached to this activity. The default implementation returns <code>true</code> for
     * apps built for <code>android:targetSdkVersion</code> older than
     * {@link android.os.Build.VERSION_CODES#KITKAT}. For later versions, it will throw an exception.
     * @param fragmentName the class name of the Fragment about to be attached to this activity.
     * @return true if the fragment class name is valid for this Activity and false otherwise.
     */
    CARAPI IsValidFragment(
        /* [in] */ const String& fragmentName,
        /* [out] */ Boolean* result);

private:
    CARAPI SetSelectedHeader(
        /* [in] */ IPreferenceActivityHeader* header);

    CARAPI ShowBreadCrumbs(
        /* [in] */ IPreferenceActivityHeader* header);

    CARAPI SwitchToHeaderInner(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args);

    CARAPI FindBestMatchingHeader(
        /* [in] */ IPreferenceActivityHeader* cur,
        /* [in] */ IList* from,
        /* [out] */ IPreferenceActivityHeader** h);

    /**
     * Posts a message to bind the preferences to the list view.
     * <p>
     * Binding late is preferred as any custom preference types created in
     * {@link #onCreate(Bundle)} are able to have their views recycled.
     */
    CARAPI_(void) PostBindPreferences();

    CARAPI_(void) BindPreferences();

    CARAPI RequirePreferenceManager();

    CARAPI HandleBuildHeaders();

private:
    ECO_LOCAL static const String TAG;
    // Constants for state save/restore
    ECO_LOCAL static const String HEADERS_TAG;
    ECO_LOCAL static const String CUR_HEADER_TAG;
    ECO_LOCAL static const String PREFERENCES_TAG;

    ECO_LOCAL static const String BACK_STACK_PREFS;

    // show Back and Next buttons? takes boolean parameter
    // Back will then return RESULT_CANCELED and Next RESULT_OK
    ECO_LOCAL static const String EXTRA_PREFS_SHOW_BUTTON_BAR ;

    // add a Skip button?
    ECO_LOCAL static const String EXTRA_PREFS_SHOW_SKIP;

    // specify custom text for the Back or Next buttons, or cause a button to not appear
    // at all by setting it to null
    ECO_LOCAL static const String EXTRA_PREFS_SET_NEXT_TEXT;
    ECO_LOCAL static const String EXTRA_PREFS_SET_BACK_TEXT;

    /**
     * The starting request code given out to preference framework.
     */
    ECO_LOCAL static const Int32 FIRST_REQUEST_CODE = 100;

    ECO_LOCAL static const Int32 MSG_BIND_PREFERENCES = 1;
    ECO_LOCAL static const Int32 MSG_BUILD_HEADERS = 2;

    AutoPtr<IList> mHeaders;

    AutoPtr<IFrameLayout> mListFooter;

    AutoPtr<IViewGroup> mPrefsContainer;

    AutoPtr<IFragmentBreadCrumbs> mFragmentBreadCrumbs;

    Boolean mSinglePane;

    AutoPtr<IPreferenceActivityHeader> mCurHeader;

    // --- State for old mode when showing a single preference list

    AutoPtr<IPreferenceManager> mPreferenceManager;

    AutoPtr<IBundle> mSavedInstanceState;

    // --- Common state

    AutoPtr<IButton> mNextButton;
    Int32 mPreferenceHeaderItemResId;
    Boolean mPreferenceHeaderRemoveEmptyIcon;
    AutoPtr<MHandler> mHandler;
};

} // Preference
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCEACTIVITY_H__
