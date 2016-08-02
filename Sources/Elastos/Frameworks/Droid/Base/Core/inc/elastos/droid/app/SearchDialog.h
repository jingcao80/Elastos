
#ifndef __ELASTOS_DROID_APP_SERACH_DIALOG_H__
#define __ELASTOS_DROID_APP_SERACH_DIALOG_H__

#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IAutoCompleteTextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ISearchViewOnCloseListener;
using Elastos::Droid::Widget::ISearchViewOnQueryTextListener;
using Elastos::Droid::Widget::ISearchViewOnSuggestionListener;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Search dialog. This is controlled by the
 * SearchManager and runs in the current foreground process.
 *
 * @hide
 */
class SearchDialog
    : public Dialog
    , public ISearchDialog
{
public:
    /**
     * The root element in the search bar layout. This is a custom view just to override
     * the handling of the back button.
     */
    class SearchBar
        : public LinearLayout
        , public ISearchBar
    {
    public:
        CAR_INTERFACE_DECL()

        SearchBar();

        virtual ~SearchBar();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ IContext* context);

        CARAPI SetSearchDialog(
            /* [in] */ ISearchDialog* searchDialog);

        /**
         * Don't allow action modes in a SearchBar, it looks silly.
         */
        //@Override
        CARAPI StartActionModeForChild(
            /* [in] */ IView* child,
            /* [in] */ IActionModeCallback* cb,
            /* [out] */ IActionMode** mode);

    private:
        AutoPtr<ISearchDialog> mSearchDialog;// weak-ref?? luozhaohui
    };

private:
    class ConfChangeBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("SearchDialog::ConfChangeBroadcastReceiver")

        ConfChangeBroadcastReceiver(
            /* [in] */ SearchDialog* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SearchDialog* mHost;
    };

    class MySearchViewOnCloseListener
        : public Object
        , public ISearchViewOnCloseListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySearchViewOnCloseListener(
            /* [in] */ SearchDialog* host);

        CARAPI OnClose(
            /* [out] */ Boolean* result);
    private:
        SearchDialog* mHost;
    };

    class MySearchViewOnQueryTextListener
        : public Object
        , public ISearchViewOnQueryTextListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySearchViewOnQueryTextListener(
            /* [in] */ SearchDialog* host);

        CARAPI OnQueryTextSubmit(
            /* [in] */ const String& query,
            /* [out] */ Boolean* result);

        CARAPI OnQueryTextChange(
            /* [in] */ const String& newText,
            /* [out] */ Boolean* result);
    private:
        SearchDialog* mHost;
    };

    class MySearchViewOnSuggestionListener
        : public Object
        , public ISearchViewOnSuggestionListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySearchViewOnSuggestionListener(
            /* [in] */ SearchDialog* host);

        CARAPI OnSuggestionSelect(
            /* [in] * */ Int32 position,
            /* [out] */ Boolean* result);

        CARAPI OnSuggestionClick(
            /* [in] * */ Int32 position,
            /* [out] */ Boolean* result);
    private:
        SearchDialog* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ SearchDialog* host);

        CARAPI OnClick(
            /* [in] */ IView*view);
    private:
        SearchDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SearchDialog();

    virtual ~SearchDialog();

    static Int32 ResolveDialogTheme(
        /* [in] */ IContext* context);

    /**
     * Constructor - fires it up and makes it look like the search UI.
     *
     * @param context Application Context we can use for system acess
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ISearchManager* searchManager);

    /**
     * Create the search dialog and any resources that are used for the
     * entire lifetime of the dialog.
     */
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Set up the search dialog
     *
     * @return true if search dialog launched, false if not
     */
    CARAPI Show(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IComponentName* componentName,
        /* [in] */ IBundle* appSearchData,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnStart();

    /**
     * The search dialog is being dismissed, so handle all of the local shutdown operations.
     *
     * This function is designed to be idempotent so that dismiss() can be safely called at any time
     * (even if already closed) and more likely to really dump any memory.  No leaks!
     */
    //@Override
    CARAPI OnStop();

    /**
     * Sets the search dialog to the 'working' state, which shows a working spinner in the
     * right hand size of the text field.
     *
     * @param working true to show spinner, false to hide spinner
     */
    CARAPI SetWorking(
        /* [in] */ Boolean working);

    /**
     * Save the minimal set of data necessary to recreate the search
     *
     * @return A bundle with the state of the dialog, or {@code null} if the search
     *         dialog is not showing.
     */
    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IBundle** bundle);

    /**
     * Restore the state of the dialog from a previously saved bundle.
     *
     * @param savedInstanceState The state of the dialog previously saved by
     *     {@link #onSaveInstanceState()}.
     */
    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Called after resources have changed, e.g. after screen rotation or locale change.
     */
    CARAPI OnConfigurationChanged();

    static Boolean IsLandscapeMode(
        /* [in] */ IContext* context);

    /**
     * {@link Dialog#onTouchEvent(MotionEvent)} will cancel the dialog only when the
     * touch is outside the window. But the window includes space for the drop-down,
     * so we also cancel on taps outside the search bar when the drop-down is not showing.
     */
    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Hide();

    /**
     * Launch a search for the text in the query text field.
     */
    CARAPI LaunchQuerySearch();

    /**
     * Sets the list item selection in the AutoCompleteTextView's ListView.
     */
    CARAPI SetListSelection(
        /* [in] */ Int32 index);

    //@Override
    CARAPI OnBackPressed();

protected:
    /**
     * Launch a search for the text in the query text field.
     *
     * @param actionKey The key code of the action key that was pressed,
     *        or {@link KeyEvent#KEYCODE_UNKNOWN} if none.
     * @param actionMsg The message for the action key that was pressed,
     *        or <code>null</code> if none.
     */
    void LaunchQuerySearch(
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg);

private:
    /**
     * We recreate the dialog view each time it becomes visible so as to limit
     * the scope of any problems with the contained resources.
     */
    CARAPI CreateContentView();

    /**
     * Does the rest of the work required to show the search dialog. Called by
     * {@link #show(String, Boolean, ComponentName, Bundle)} and
     *
     * @return true if search dialog showed, false if not
     */
    Boolean DoShow(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IComponentName* componentName,
        /* [in] */ IBundle* appSearchData);

    /**
     * Sets up the search dialog and shows it.
     *
     * @return <code>true</code> if search dialog launched
     */
    Boolean Show(
        /* [in] */ IComponentName* componentName,
        /* [in] */ IBundle* appSearchData);

    using Dialog::Show;

    /**
     * Update the UI according to the info in the current value of {@link #mSearchable}.
     */
    void UpdateUI();

    /**
     * Updates the auto-complete text view.
     */
    void UpdateSearchAutoComplete();

    void UpdateSearchAppIcon();

    /**
     * Setup the search "Badge" if requested by mode flags.
     */
    void UpdateSearchBadge();

    Boolean IsOutOfBounds(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event);

    /**
     * Launches an intent, including any special intent handling.
     */
    void LaunchIntent(
        /* [in] */ IIntent* intent);

    /**
     * Constructs an intent from the given information and the search dialog state.
     *
     * @param action Intent action.
     * @param data Intent data, or <code>null</code>.
     * @param extraData Data for {@link SearchManager#EXTRA_DATA_KEY} or <code>null</code>.
     * @param query Intent query, or <code>null</code>.
     * @param actionKey The key code of the action key that was pressed,
     *        or {@link KeyEvent#KEYCODE_UNKNOWN} if none.
     * @param actionMsg The message for the action key that was pressed,
     *        or <code>null</code> if none.
     * @param mode The search mode, one of the acceptable values for
     *             {@link SearchManager#SEARCH_MODE}, or {@code null}.
     * @return The intent.
     */
    AutoPtr<IIntent> CreateIntent(
        /* [in] */ const String& action,
        /* [in] */ IUri* data,
        /* [in] */ const String& extraData,
        /* [in] */ const String& query,
        /* [in] */ Int32 actionKey,
        /* [in] */ const String& actionMsg);

    Boolean IsEmpty(
        /* [in] */ IAutoCompleteTextView* actv);

    Boolean OnClosePressed();

    /**
     * Sets the text in the query box, updating the suggestions.
     */
    void SetUserQuery(
        /* [in] */ const String& query);

private:

    // Debugging support
    static const Boolean DBG;
    static const String TAG;

    static const String INSTANCE_KEY_COMPONENT;
    static const String INSTANCE_KEY_APPDATA;
    static const String INSTANCE_KEY_USER_QUERY;

    // The string used for privateImeOptions to identify to the IME that it should not show
    // a microphone button since one already exists in the search dialog.
    static const String IME_OPTION_NO_MICROPHONE;// = "nm";

    static const Int32 SEARCH_PLATE_LEFT_PADDING_NON_GLOBAL;// = 7;

    // views & widgets
    AutoPtr<ITextView> mBadgeLabel;
    AutoPtr<IImageView> mAppIcon;
    AutoPtr<IAutoCompleteTextView> mSearchAutoComplete;
    AutoPtr<IView> mSearchPlate;
    AutoPtr<ISearchView> mSearchView;
    AutoPtr<IDrawable> mWorkingSpinner;
    AutoPtr<IView> mCloseSearch;

    // interaction with searchable application
    AutoPtr<ISearchableInfo> mSearchable;
    AutoPtr<IComponentName> mLaunchComponent;
    AutoPtr<IBundle> mAppSearchData;
    AutoPtr<IContext> mActivityContext;

    // For voice searching
    AutoPtr<IIntent> mVoiceWebSearchIntent;
    AutoPtr<IIntent> mVoiceAppSearchIntent;

    // The query entered by the user. This is not changed when selecting a suggestion
    // that modifies the contents of the text field. But if the user then edits
    // the suggestion, the resulting string is saved.
    String mUserQuery;

    // Last known IME options value for the search edit text.
    Int32 mSearchAutoCompleteImeOptions;

    AutoPtr<IBroadcastReceiver> mConfChangeListener;

    AutoPtr<ISearchViewOnCloseListener>  mOnCloseListener;
    AutoPtr<ISearchViewOnQueryTextListener> mOnQueryChangeListener;
    AutoPtr<ISearchViewOnSuggestionListener> mOnSuggestionSelectionListener;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_SERACH_DIALOG_H__
