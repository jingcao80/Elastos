#ifndef __ELASTOS_APPS_DIALER_DIALTACTSACTIVITY_H__
#define __ELASTOS_APPS_DIALER_DIALTACTSACTIVITY_H__

#include "elastos/apps/dialer/activity/TransactionSafeActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Telecomm.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnDragListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;
using Elastos::Core::IInteger64;
using Elastos::Apps::Dialer::Activity::TransactionSafeActivity;
using Elastos::Apps::Dialer::Database::IDialerDatabaseHelper;
using Elastos::Apps::Dialer::Dialpad::IDialpadFragment;
using Elastos::Apps::Dialer::Dialpad::IOnDialpadQueryChangedListener;
using Elastos::Apps::Dialer::List::IListsFragment;
using Elastos::Apps::Dialer::List::IListsFragmentHostInterface;
using Elastos::Apps::Dialer::List::ISpeedDialFragmentHostInterface;
using Elastos::Apps::Dialer::List::ISearchFragmentHostInterface;
using Elastos::Apps::Dialer::List::IOnDragDropListener;
using Elastos::Apps::Dialer::List::IPhoneFavoriteSquareTileView;
using Elastos::Apps::Dialer::List::IDragDropController;
using Elastos::Apps::Dialer::List::IRegularSearchFragment;
using Elastos::Apps::Dialer::List::ISmartDialSearchFragment;
using Elastos::Apps::Dialer::Widget::IActionBarController;
using Elastos::Apps::Dialer::Widget::IActionBarControllerActivityUi;
using Elastos::Apps::Dialer::Widget::ISearchEditTextLayoutOnBackButtonClickedListener;

namespace Elastos {
namespace Apps {
namespace Dialer {

/**
 * The dialer tab's title is 'phone', a more common name (see strings.xml).
 */
class DialtactsActivity
    : public TransactionSafeActivity
    , public IDialtactsActivity
    , public IViewOnClickListener
    , public IOnDialpadQueryChangedListener
    , public IListsFragmentHostInterface
    , public ISpeedDialFragmentHostInterface
    , public ISearchFragmentHostInterface
    , public IOnDragDropListener
    //TODO:
    // , public IOnPhoneNumberPickerActionListener
    , public IPopupMenuOnMenuItemClickListener
    //TODO:
    // , public IViewPagerOnPageChangeListener
    , public IActionBarControllerActivityUi
{
private:
    // TODO:
    // class SlideOutListener
    //     : public AnimationListenerAdapter
    // {
    // public:
    //     // @Override
    //     CARAPI OnAnimationEnd(
    //         /* [in] */ IAnimation* animation);
    // };

    class OptionsPopupMenu
        : public Object
        // TODO
        //: public PopupMenu
    {
    public:
        OptionsPopupMenu(
            /* [in] */ DialtactsActivity* host);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IView* anchor);

        // @Override
        CARAPI Show();

    private:
        DialtactsActivity* mHost;
    };

    /**
     * Listener that listens to drag events and sends their x and y coordinates to a
     * {@link DragDropController}.
     */
    class LayoutOnDragListener
        : public Object
        , public IViewOnDragListener
    {
    public:
        CAR_INTERFACE_DECL()

        LayoutOnDragListener(
            /* [in] */ DialtactsActivity* host);

        // @Override
        CARAPI OnDrag(
            /* [in] */ IView* v,
            /* [in] */ IDragEvent* event,
            /* [out] */ Boolean* result);

    private:
        DialtactsActivity* mHost;
    };

    class PhoneSearchQueryTextListener
        : public Object
        , public ITextWatcher
    {
    public:
        CAR_INTERFACE_DECL()

        PhoneSearchQueryTextListener(
            /* [in] */ DialtactsActivity* host);

        // @Override
        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        // @Override
        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        // @Override
        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

    private:
        DialtactsActivity* mHost;
    };

    /**
     * Open the search UI when the user clicks on the search box.
     */
    class SearchViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SearchViewOnClickListener(
            /* [in] */ DialtactsActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        DialtactsActivity* mHost;
    };

    /**
     * If the search term is empty and the user closes the soft keyboard, close the search UI.
     */
    class SearchEditTextLayoutListener
        : public Object
        , public IViewOnKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

        SearchEditTextLayoutListener(
            /* [in] */ DialtactsActivity* host);

        // @Override
        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        DialtactsActivity* mHost;
    };

    class MyOnBackButtonClickedListener
        : public Object
        , public ISearchEditTextLayoutOnBackButtonClickedListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnBackButtonClickedListener(
            /* [in] */ DialtactsActivity* host);

        // @Override
        CARAPI OnBackButtonClicked();

    private:
        DialtactsActivity* mHost;
    };

    class MyOnGlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnGlobalLayoutListener(
            /* [in] */ DialtactsActivity* host,
            /* [in] */ IViewTreeObserver* observer);

        // @Override
        CARAPI OnGlobalLayout();

    private:
        DialtactsActivity* mHost;
        AutoPtr<IViewTreeObserver> mObserver;
    };

    class MyOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnTouchListener(
            /* [in] */ DialtactsActivity* host);

        // @Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        DialtactsActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DialtactsActivity();

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* isConsumed);

    // @Override
    CARAPI OnAttachFragment(
        /* [in] */ IFragment* fragment);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* view);

    // @Override
    CARAPI OnMenuItemClick(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    /**
     * Callback from child DialpadFragment when the dialpad is shown.
     */
    CARAPI OnDialpadShown();

    /**
     * Initiates animations and other visual updates to hide the dialpad. The fragment is hidden in
     * a callback after the hide animation ends.
     * @see #commitDialpadFragmentHide
     */
    CARAPI HideDialpadFragment(
        /* [in] */ Boolean animate,
        /* [in] */ Boolean clearDialpad);

    // @Override
    CARAPI IsInSearchUi(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasSearchQuery(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ShouldShowActionBar(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    // @Override
    CARAPI OnNewIntent(
        /* [in] */ IIntent *intent);

    /**
     * Returns an appropriate call origin for this Activity. May return null when no call origin
     * should be used (e.g. when some 3rd party application launched the screen. Call origin is
     * for remembering the tab in which the user made a phone call, so the external app's DIAL
     * request should not be counted.)
     */
    CARAPI GetCallOrigin(
        /* [out] */ String* origin);

    /** Returns an Intent to launch Call Settings screen */
    static CARAPI_(AutoPtr<IIntent>) GetCallSettingsIntent();

    // @Override
    CARAPI OnBackPressed();

    // @Override
    CARAPI OnDialpadQueryChanged(
        /* [in] */ const String& query);

    // @Override
    CARAPI OnListFragmentScrollStateChange(
        /* [in] */ Int32 scrollState);

    // @Override
    CARAPI OnListFragmentScroll(
        /* [in] */ Int32 firstVisibleItem,
        /* [in] */ Int32 visibleItemCount,
        /* [in] */ Int32 totalItemCount);

    static CARAPI_(AutoPtr<IIntent>) GetAddNumberToContactIntent(
        /* [in] */ ICharSequence* text);

    // @Override
    CARAPI ShowCallHistory();

    /**
     * Called when the user has long-pressed a contact tile to start a drag operation.
     */
    // @Override
    CARAPI OnDragStarted(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPhoneFavoriteSquareTileView* view);

    // @Override
    CARAPI OnDragHovered(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPhoneFavoriteSquareTileView* view);

    /**
     * Called when the user has released a contact tile after long-pressing it.
     */
    // @Override
    CARAPI OnDragFinished(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // @Override
    CARAPI OnDroppedOnRemove();

    /**
     * Allows the SpeedDialFragment to attach the drag controller to mRemoveViewContainer
     * once it has been attached to the activity.
     */
    // @Override
    CARAPI SetDragDropController(
        /* [in] */ IDragDropController* dragController);

    // @Override
    CARAPI OnPickPhoneNumberAction(
        /* [in] */ IUri* dataUri);

    // @Override
    CARAPI OnCallNumberDirectly(
        /* [in] */ const String& phoneNumber);

    // @Override
    CARAPI OnCallNumberDirectly(
        /* [in] */ const String& phoneNumber,
        /* [in] */ Boolean isVideoCall);

    // @Override
    CARAPI OnShortcutIntentCreated(
        /* [in] */ IIntent* intent);

    // @Override
    CARAPI OnHomeInActionBarSelected();

    // @Override
    CARAPI OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Int32 positionOffsetPixels);

    // @Override
    CARAPI OnPageSelected(
        /* [in] */ Int32 position);

    // @Override
    CARAPI OnPageScrollStateChanged(
        /* [in] */ Int32 state);

    // @Override
    CARAPI IsActionBarShowing(
        /* [out] */ Boolean* result);

    CARAPI IsDialpadShown(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetActionBarHideOffset(
        /* [out] */ Int32* offset);

    // @Override
    CARAPI GetActionBarHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI SetActionBarHideOffset(
        /* [in] */ Int32 hideOffset);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnPause();

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI_(void) HandleMenuSettings();

    // @Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
    CARAPI_(void) SetupActivityOverlay();

    /**
     * Initiates a fragment transaction to show the dialpad fragment. Animations and other visual
     * updates are handled by a callback which is invoked after the dialpad fragment is shown.
     * @see #onDialpadShown
     */
    CARAPI_(void) ShowDialpadFragment(
        /* [in] */ Boolean animate);

    /**
     * Finishes hiding the dialpad fragment after any animations are completed.
     */
    CARAPI_(void) CommitDialpadFragmentHide();

    CARAPI_(void) UpdateSearchFragmentPosition();

    CARAPI_(void) SetNotInSearchUi();

    CARAPI_(void) HideDialpadAndSearchUi();

    CARAPI_(void) PrepareVoiceSearchButton();

    CARAPI_(AutoPtr<OptionsPopupMenu>) BuildOptionsMenu(
        /* [in] */ IView* invoker);

    /**
     * Returns true if the intent is due to hitting the green send key (hardware call button:
     * KEYCODE_CALL) while in a call.
     *
     * @param intent the intent that launched this activity
     * @return true if the intent is due to hitting the green send key while in a call
     */
    CARAPI_(Boolean) IsSendKeyWhileInCall(
        /* [in] */ IIntent* intent);

    /**
     * Sets the current tab based on the intent's request type
     *
     * @param intent Intent that contains information about which tab should be selected
     */
    CARAPI_(void) DisplayFragment(
        /* [in] */ IIntent* intent);

    /** Returns true if the given intent contains a phone number to populate the dialer with */
    CARAPI_(Boolean) IsDialIntent(
        /* [in] */ IIntent* intent);

    /**
     * Shows the search fragment
     */
    CARAPI_(void) EnterSearchUi(
        /* [in] */ Boolean smartDialSearch,
        /* [in] */ const String& query);

    /**
     * Hides the search fragment
     */
    CARAPI_(void) ExitSearchUi();

    /**
     * @return True if the search UI was exited, false otherwise
     */
    CARAPI_(Boolean) MaybeExitSearchUi();

    CARAPI_(Boolean) PhoneIsInUse();

    CARAPI_(Boolean) CanIntentBeHandled(
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr<ITelephonyManager>) GetTelephonyManager();

    CARAPI_(AutoPtr<ITelecomManager>) GetTelecomManager();

    /**
     * Updates controller based on currently known information.
     *
     * @param animate Whether or not to animate the transition.
     */
    CARAPI_(void) UpdateFloatingActionButtonControllerAlignment(
        /* [in] */ Boolean animate);
public:
    static const Boolean DEBUG; // = Log.isLoggable(TAG, Log.DEBUG);

private:
    static const String TAG; // = "DialtactsActivity";


    /** @see #getCallOrigin() */
    static const String CALL_ORIGIN_DIALTACTS; // =
            // "com.android.dialer.DialtactsActivity";

    static const String KEY_IN_REGULAR_SEARCH_UI; // = "in_regular_search_ui";
    static const String KEY_IN_DIALPAD_SEARCH_UI; // = "in_dialpad_search_ui";
    static const String KEY_SEARCH_QUERY; // = "search_query";
    static const String KEY_FIRST_LAUNCH; // = "first_launch";
    static const String KEY_IS_DIALPAD_SHOWN; // = "is_dialpad_shown";

    static const String TAG_DIALPAD_FRAGMENT; // = "dialpad";
    static const String TAG_REGULAR_SEARCH_FRAGMENT; // = "search";
    static const String TAG_SMARTDIAL_SEARCH_FRAGMENT; // = "smartdial";
    static const String TAG_FAVORITES_FRAGMENT; // = "favorites";

    /**
     * Just for backward compatibility. Should behave as same as {@link Intent#ACTION_DIAL}.
     */
    static const String ACTION_TOUCH_DIALER; // = "com.android.phone.action.TOUCH_DIALER";

    static const Int32 ACTIVITY_REQUEST_CODE_VOICE_SEARCH; // = 1;

    AutoPtr<IFrameLayout> mParentLayout;

    /**
     * Fragment containing the dialpad that slides into view
     */
    AutoPtr<IDialpadFragment> mDialpadFragment;

    /**
     * Fragment for searching phone numbers using the alphanumeric keyboard.
     */
    AutoPtr<IRegularSearchFragment> mRegularSearchFragment;

    /**
     * Fragment for searching phone numbers using the dialpad.
     */
    AutoPtr<ISmartDialSearchFragment> mSmartDialSearchFragment;

    /**
     * Animation that slides in.
     */
    AutoPtr<IAnimation> mSlideIn;

    /**
     * Animation that slides out.
     */
    AutoPtr<IAnimation> mSlideOut;

    /**
     * Listener for after slide out animation completes on dialer fragment.
     */
    //TODO:
    // AnimationListenerAdapter mSlideOutListener;

    /**
     * Fragment containing the speed dial list, recents list, and all contacts list.
     */
    AutoPtr<IListsFragment> mListsFragment;

    Boolean mInDialpadSearch;
    Boolean mInRegularSearch;
    Boolean mClearSearchOnPause;
    Boolean mIsDialpadShown;
    Boolean mShowDialpadOnResume;

    /**
     * Whether or not the device is in landscape orientation.
     */
    Boolean mIsLandscape;

    /**
     * The position of the currently selected tab in the attached {@link ListsFragment}.
     */
    Int32 mCurrentTabPosition; // = 0;

    /**
     * True if the dialpad is only temporarily showing due to being in call
     */
    Boolean mInCallDialpadUp;

    /**
     * True when this activity has been launched for the first time.
     */
    Boolean mFirstLaunch;

    /**
     * Search query to be applied to the SearchView in the ActionBar once
     * onCreateOptionsMenu has been called.
     */
    String mPendingSearchViewQuery;

    AutoPtr<IPopupMenu> mOverflowMenu;
    AutoPtr<IEditText> mSearchView;
    AutoPtr<IView> mVoiceSearchButton;

    String mSearchQuery;

    AutoPtr<IDialerDatabaseHelper> mDialerDatabaseHelper;
    AutoPtr<IDragDropController> mDragDropController;
    AutoPtr<IActionBarController> mActionBarController;

    // TODO:
    // AutoPtr<IFloatingActionButtonController> mFloatingActionButtonController;

    Int32 mActionBarHeight;

    /**
     * Listener used to send search queries to the phone search fragment.
     */
    AutoPtr<ITextWatcher> mPhoneSearchQueryTextListener;

    /**
     * Open the search UI when the user clicks on the search box.
     */
    AutoPtr<IViewOnClickListener> mSearchViewOnClickListener;

    /**
     * If the search term is empty and the user closes the soft keyboard, close the search UI.
     */
    AutoPtr<IViewOnKeyListener> mSearchEditTextLayoutListener;
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALTACTSACTIVITY_H__
