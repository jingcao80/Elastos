
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Speech.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/dialer/DialtactsActivity.h"
#include "elastos/droid/dialer/dialpad/SmartDialPrefix.h"
// #include "elastos/droid/dialer/interactions/PhoneNumberInteraction.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/dialer/widget/CSearchEditTextLayout.h"
#include "elastos/droid/dialerbind/DatabaseHelperManager.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
// #include "Elastos.Droid.Content.h"
// #include "Elastos.Droid.Os.h"
// #include "Elastos.Droid.Telephony.h"
// #include "Elastos.Droid.View.h"
// #include "Elastos.Droid.Widget.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include <elastos/droid/contacts/common/interactions/TouchPointManager.h>
#include "elastos/droid/contacts/common/interactions/ImportExportDialogFragment.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Graphics::IColor;

// using Elastos::Droid::App::IActionBar;
// using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
// using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::CallLog::ECLSID_CCallLogActivity;
using Elastos::Droid::Dialer::List::ISpeedDialFragment;
using Elastos::Droid::Net::IUri;
// using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractIntentsInsert;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::ITelecomManager;
// using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnDragListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
// using Elastos::Droid::View::IKeyEvent;
// using Elastos::Droid::View::IMenu;
// using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Support::V4::View::EIID_IViewPagerOnPageChangeListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Dialer::Dialpad::EIID_IOnDialpadQueryChangedListener;
using Elastos::Droid::Dialer::Dialpad::IDialpadFragment;
using Elastos::Droid::Dialer::Dialpad::SmartDialPrefix;
// using Elastos::Droid::Dialer::Interactions::PhoneNumberInteraction;
using Elastos::Droid::Dialer::List::IListsFragment;
using Elastos::Droid::Dialer::List::IRemoveView;
using Elastos::Droid::Dialer::List::EIID_IListsFragmentHostInterface;
using Elastos::Droid::Dialer::List::EIID_IOnDragDropListener;
using Elastos::Droid::Dialer::List::EIID_ISearchFragmentHostInterface;
using Elastos::Droid::Dialer::List::EIID_ISpeedDialFragmentHostInterface;
using Elastos::Droid::Dialer::List::ISearchFragment;
using Elastos::Droid::Dialer::List::IRegularSearchFragment;
using Elastos::Droid::Dialer::List::ISmartDialSearchFragment;
using Elastos::Droid::Dialer::Widget::ISearchEditTextLayout;
using Elastos::Droid::Dialer::Widget::CSearchEditTextLayout;
using Elastos::Droid::Dialer::Widget::EIID_IActionBarControllerActivityUi;
using Elastos::Droid::Dialer::Widget::EIID_IOnBackButtonClickedListener;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::DialerBind::DatabaseHelperManager;
using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Contacts::Common::Interactions::TouchPointManager;
using Elastos::Droid::Contacts::Common::Interactions::ImportExportDialogFragment;
using Elastos::Droid::Contacts::Common::List::EIID_IOnPhoneNumberPickerActionListener;
using Elastos::Droid::Contacts::Common::List::IContactEntryListFragment;
using Elastos::Droid::Common::Widget::ICompositeCursorAdapter;
using Elastos::Droid::Phone::Common::Animation::AnimUtils;

namespace Elastos {
namespace Droid {
namespace Dialer {

//================================================================
// DialtactsActivity::OptionsPopupMenu
//================================================================
DialtactsActivity::OptionsPopupMenu::OptionsPopupMenu(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::OptionsPopupMenu::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* anchor)
{
    return PopupMenu::constructor(context, anchor, IGravity::END);
}

ECode DialtactsActivity::OptionsPopupMenu::Show()
{
    AutoPtr<IMenu> menu;
    GetMenu((IMenu**)&menu);
    AutoPtr<IMenuItem> clearFrequents;
    menu->FindItem(R::id::menu_clear_frequents, (IMenuItem**)&clearFrequents);
    AutoPtr<ISpeedDialFragment> speedDialFragment;
    Boolean hasFrequents;
    clearFrequents->SetVisible(mHost->mListsFragment != NULL &&
            (mHost->mListsFragment->GetSpeedDialFragment((ISpeedDialFragment**)&speedDialFragment), speedDialFragment != NULL) &&
            (speedDialFragment->HasFrequents(&hasFrequents), hasFrequents));
    PopupMenu::Show();
    return NOERROR;
}


//================================================================
// DialtactsActivity::LayoutOnDragListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::LayoutOnDragListener, Object, IViewOnDragListener);

DialtactsActivity::LayoutOnDragListener::LayoutOnDragListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::LayoutOnDragListener::OnDrag(
    /* [in] */ IView* v,
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    event->GetAction(&action);
    if (action == IDragEvent::ACTION_DRAG_LOCATION) {
        Float x, y;
        event->GetX(&x);
        event->GetY(&y);
        mHost->mDragDropController->HandleDragHovered(v, (Int32) x, (Int32) y);
    }
    *result = TRUE;
    return NOERROR;
}


//================================================================
// DialtactsActivity::PhoneSearchQueryTextListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::PhoneSearchQueryTextListener, Object, ITextWatcher);

DialtactsActivity::PhoneSearchQueryTextListener::PhoneSearchQueryTextListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::PhoneSearchQueryTextListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode DialtactsActivity::PhoneSearchQueryTextListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    String newText;
    s->ToString(&newText);
    if (newText.Equals(mHost->mSearchQuery)) {
        // If the query hasn't changed (perhaps due to activity being destroyed
        // and restored, or user launching the same DIAL intent twice), then there is
        // no need to do anything here.
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onTextChange for mSearchView called with new query: %s", newText.string());
        Logger::D(TAG, "Previous Query: %s", mHost->mSearchQuery.string());
    }
    mHost->mSearchQuery = newText;

    // Show search fragment only when the query string is changed to non-empty text.
    if (!TextUtils::IsEmpty(newText)) {
        // Call enterSearchUi only if we are switching search modes, or showing a search
        // fragment for the first time.
        Boolean sameSearchMode = (mHost->mIsDialpadShown && mHost->mInDialpadSearch) ||
                (!mHost->mIsDialpadShown && mHost->mInRegularSearch);
        if (!sameSearchMode) {
            mHost->EnterSearchUi(mHost->mIsDialpadShown, mHost->mSearchQuery);
        }
    }

    Boolean visible;
    if (mHost->mSmartDialSearchFragment != NULL &&
            (mHost->mSmartDialSearchFragment->IsVisible(&visible), visible)) {
        mHost->mSmartDialSearchFragment->SetQueryString(mHost->mSearchQuery, FALSE /* delaySelection */);
    }
    else if (mHost->mRegularSearchFragment != NULL &&
            (mHost->mRegularSearchFragment->IsVisible(&visible), visible)) {
        mHost->mRegularSearchFragment->SetQueryString(mHost->mSearchQuery, FALSE /* delaySelection */);
    }
    return NOERROR;
}

ECode DialtactsActivity::PhoneSearchQueryTextListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}


//================================================================
// DialtactsActivity::SearchViewOnClickListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::SearchViewOnClickListener, Object, IViewOnClickListener);

DialtactsActivity::SearchViewOnClickListener::SearchViewOnClickListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::SearchViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean result;
    if (mHost->IsInSearchUi(&result), !result) {
        mHost->mActionBarController->OnSearchBoxTapped();
        AutoPtr<ICharSequence> text;
        ITextView::Probe(mHost->mSearchView)->GetText((ICharSequence**)&text);
        mHost->EnterSearchUi(FALSE /* smartDialSearch */, CoreUtils::Unbox(text));
    }
    return NOERROR;
}


//================================================================
// DialtactsActivity::SearchEditTextLayoutListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::SearchEditTextLayoutListener, Object, IViewOnKeyListener);

DialtactsActivity::SearchEditTextLayoutListener::SearchEditTextLayoutListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::SearchEditTextLayoutListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    AutoPtr<ICharSequence> text;
    if (keyCode == IKeyEvent::KEYCODE_BACK &&
            (event->GetAction(&action), action == IKeyEvent::ACTION_DOWN) &&
            (ITextView::Probe(mHost->mSearchView)->GetText((ICharSequence**)&text),
                TextUtils::IsEmpty(CoreUtils::Unbox(text)))) {
        mHost->MaybeExitSearchUi();
    }
    *result = FALSE;
    return NOERROR;
}


//================================================================
// DialtactsActivity::MyOnBackButtonClickedListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::MyOnBackButtonClickedListener, Object,
        IOnBackButtonClickedListener);

DialtactsActivity::MyOnBackButtonClickedListener::MyOnBackButtonClickedListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::MyOnBackButtonClickedListener::OnBackButtonClicked()
{
    return mHost->OnBackPressed();
}


//================================================================
// DialtactsActivity::MyOnGlobalLayoutListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::MyOnGlobalLayoutListener, Object,
        IOnGlobalLayoutListener);

DialtactsActivity::MyOnGlobalLayoutListener::MyOnGlobalLayoutListener(
    /* [in] */ DialtactsActivity* host,
    /* [in] */ IViewTreeObserver* observer)
    : mHost(host)
    , mObserver(observer)
{}

ECode DialtactsActivity::MyOnGlobalLayoutListener::OnGlobalLayout()
{
    Boolean isAlive;
    if (mObserver->IsAlive(&isAlive), !isAlive) {
        return NOERROR;
    }
    mObserver->RemoveOnGlobalLayoutListener(this);
    Int32 screenWidth;
    IView::Probe(mHost->mParentLayout)->GetWidth(&screenWidth);
    mHost->mFloatingActionButtonController->SetScreenWidth(screenWidth);
    mHost->UpdateFloatingActionButtonControllerAlignment(FALSE /* animate */);
    return NOERROR;
}


//================================================================
// DialtactsActivity::MyOnTouchListener
//================================================================
CAR_INTERFACE_IMPL(DialtactsActivity::MyOnTouchListener, Object, IViewOnTouchListener);

DialtactsActivity::MyOnTouchListener::MyOnTouchListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::MyOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mHost->mIsDialpadShown) {
        mHost->MaybeExitSearchUi();
    }
    *result = FALSE;
    return NOERROR;
}

//================================================================
// DialtactsActivity
//================================================================
CAR_INTERFACE_IMPL_4(DialtactsActivity::InnerListener, Object,
    IViewOnClickListener, IOnPhoneNumberPickerActionListener,
    IPopupMenuOnMenuItemClickListener, IViewPagerOnPageChangeListener)

DialtactsActivity::InnerListener::InnerListener(
    /* [in] */ DialtactsActivity* host)
    : mHost(host)
{}

ECode DialtactsActivity::InnerListener::OnPickPhoneNumberAction(
    /* [in] */ IUri* dataUri)
{
    return mHost->OnPickPhoneNumberAction(dataUri);
}

ECode DialtactsActivity::InnerListener::OnCallNumberDirectly(
    /* [in] */ const String& phoneNumber)
{
    return mHost->OnCallNumberDirectly(phoneNumber);
}

ECode DialtactsActivity::InnerListener::OnCallNumberDirectly(
    /* [in] */ const String& phoneNumber,
    /* [in] */ Boolean isVideoCall)
{
    return mHost->OnCallNumberDirectly(phoneNumber, isVideoCall);
}

ECode DialtactsActivity::InnerListener::OnShortcutIntentCreated(
    /* [in] */ IIntent* intent)
{
    return mHost->OnShortcutIntentCreated(intent);
}

ECode DialtactsActivity::InnerListener::OnHomeInActionBarSelected()
{
    return mHost->OnHomeInActionBarSelected();
}

ECode DialtactsActivity::InnerListener::OnClick(
    /* [in] */ IView* view)
{
    return mHost->OnClick(view);
}

ECode DialtactsActivity::InnerListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mHost->OnMenuItemClick(item, result);
}

ECode DialtactsActivity::InnerListener::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    return mHost->OnPageScrolled(position, positionOffset, positionOffsetPixels);
}

ECode DialtactsActivity::InnerListener::OnPageSelected(
    /* [in] */ Int32 position)
{
    return mHost->OnPageSelected(position);
}

ECode DialtactsActivity::InnerListener::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    return mHost->OnPageScrollStateChanged(state);
}

//================================================================
// DialtactsActivity
//================================================================
const String DialtactsActivity::TAG("DialtactsActivity");
const Boolean DialtactsActivity::DEBUG = TRUE;

const String DialtactsActivity::CALL_ORIGIN_DIALTACTS("Elastos.Droid.Dialer.CDialtactsActivity");

const String DialtactsActivity::KEY_IN_REGULAR_SEARCH_UI("in_regular_search_ui");
const String DialtactsActivity::KEY_IN_DIALPAD_SEARCH_UI("in_dialpad_search_ui");
const String DialtactsActivity::KEY_SEARCH_QUERY("search_query");
const String DialtactsActivity::KEY_FIRST_LAUNCH("first_launch");
const String DialtactsActivity::KEY_IS_DIALPAD_SHOWN("is_dialpad_shown");

const String DialtactsActivity::TAG_DIALPAD_FRAGMENT("dialpad");
const String DialtactsActivity::TAG_REGULAR_SEARCH_FRAGMENT("search");
const String DialtactsActivity::TAG_SMARTDIAL_SEARCH_FRAGMENT("smartdial");
const String DialtactsActivity::TAG_FAVORITES_FRAGMENT("favorites");

const String DialtactsActivity::ACTION_TOUCH_DIALER("com.android.phone.action.TOUCH_DIALER");

const Int32 DialtactsActivity::ACTIVITY_REQUEST_CODE_VOICE_SEARCH = 1;

CAR_INTERFACE_IMPL_7(DialtactsActivity, TransactionSafeActivity, IDialtactsActivity,
        IOnDialpadQueryChangedListener, IListsFragmentHostInterface,
        ISpeedDialFragmentHostInterface, ISearchFragmentHostInterface, IOnDragDropListener,
        IActionBarControllerActivityUi);

DialtactsActivity::DialtactsActivity()
    : mInDialpadSearch(FALSE)
    , mInRegularSearch(FALSE)
    , mClearSearchOnPause(FALSE)
    , mIsDialpadShown(FALSE)
    , mShowDialpadOnResume(FALSE)
    , mIsLandscape(FALSE)
    , mCurrentTabPosition(0)
    , mInCallDialpadUp(FALSE)
    , mFirstLaunch(FALSE)
    , mActionBarHeight(0)
{
}

ECode DialtactsActivity::constructor()
{
    mPhoneSearchQueryTextListener = new PhoneSearchQueryTextListener(this);
    mSearchViewOnClickListener = new SearchViewOnClickListener(this);
    mSearchEditTextLayoutListener = new SearchEditTextLayoutListener(this);
    mInnerListener = new InnerListener(this);
    return TransactionSafeActivity::constructor();
}

ECode DialtactsActivity::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed);

    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        Float x, y;
        event->GetRawX(&x);
        event->GetRawY(&y);
        TouchPointManager::GetInstance()->SetPoint((Int32)x, (Int32)y);
    }
    return TransactionSafeActivity::DispatchTouchEvent(event, isConsumed);
}

ECode DialtactsActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(TransactionSafeActivity::OnCreate(savedInstanceState));
    mFirstLaunch = TRUE;

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(
            R::dimen::action_bar_height_large, &mActionBarHeight);

    SetContentView(R::layout::dialtacts_activity);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->SetBackgroundDrawable(NULL);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    actionBar->SetCustomView(R::layout::search_edittext);
    actionBar->SetDisplayShowCustomEnabled(TRUE);
    actionBar->SetBackgroundDrawable(NULL);

    AutoPtr<IView> customView;
    actionBar->GetCustomView((IView**)&customView);
    ISearchEditTextLayout* searchEditTextLayout = ISearchEditTextLayout::Probe(customView);
    mActionBarController = new ActionBarController(
            this, (CSearchEditTextLayout*)searchEditTextLayout);

    searchEditTextLayout->SetPreImeKeyListener(mSearchEditTextLayoutListener);

    AutoPtr<IView> view;
    customView->FindViewById(R::id::search_view, (IView**)&view);
    mSearchView = IEditText::Probe(view);
    ITextView::Probe(mSearchView)->AddTextChangedListener(mPhoneSearchQueryTextListener);
    customView->FindViewById(R::id::voice_search_button, (IView**)&mVoiceSearchButton);
    view = NULL;
    customView->FindViewById(R::id::search_magnifying_glass, (IView**)&view);
    view->SetOnClickListener(mSearchViewOnClickListener);
    view = NULL;
    customView->FindViewById(R::id::search_box_start_search, (IView**)&view);
    view->SetOnClickListener(mSearchViewOnClickListener);
    AutoPtr<IOnBackButtonClickedListener> listener = new MyOnBackButtonClickedListener(this);
    searchEditTextLayout->SetOnBackButtonClickedListener(listener);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    mIsLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;

    AutoPtr<IView> floatingActionButtonContainer;
    FindViewById(R::id::floating_action_button_container,
            (IView**)&floatingActionButtonContainer);
    view = NULL;
    FindViewById(R::id::floating_action_button, (IView**)&view);
    view->SetOnClickListener(mInnerListener);
    mFloatingActionButtonController = new FloatingActionButtonController();
    mFloatingActionButtonController->constructor(this,
            floatingActionButtonContainer, view);

    view = NULL;
    customView->FindViewById(R::id::dialtacts_options_menu_button, (IView**)&view);
    view->SetOnClickListener(mInnerListener);
    mOverflowMenu = BuildOptionsMenu(customView);
    AutoPtr<IViewOnTouchListener> touchListener;
    mOverflowMenu->GetDragToOpenListener((IViewOnTouchListener**)&touchListener);
    view->SetOnTouchListener(touchListener);

    // Add the favorites fragment, and the dialpad fragment, but only if savedInstanceState
    // is null. Otherwise the fragment manager takes care of recreating these fragments.
    if (savedInstanceState == NULL) {
        AutoPtr<IFragmentManager> manager;
        GetFragmentManager((IFragmentManager**)&manager);
        AutoPtr<IFragmentTransaction> transaction;
        manager->BeginTransaction((IFragmentTransaction**)&transaction);
        AutoPtr<IFragment> listFragment;
        CListsFragment::New((IFragment**)&listFragment);
        transaction->Add(R::id::dialtacts_frame, listFragment, TAG_FAVORITES_FRAGMENT);
        AutoPtr<IFragment> dialpadFragment;
        CDialpadFragment::New((IFragment**)&dialpadFragment);
        transaction->Add(R::id::dialtacts_container, dialpadFragment, TAG_DIALPAD_FRAGMENT);
        Int32 id;
        transaction->Commit(&id);
    }
    else {
        savedInstanceState->GetString(KEY_SEARCH_QUERY, &mSearchQuery);
        savedInstanceState->GetBoolean(KEY_IN_REGULAR_SEARCH_UI, &mInRegularSearch);
        savedInstanceState->GetBoolean(KEY_IN_DIALPAD_SEARCH_UI, &mInDialpadSearch);
        savedInstanceState->GetBoolean(KEY_FIRST_LAUNCH, &mFirstLaunch);
        savedInstanceState->GetBoolean(KEY_IS_DIALPAD_SHOWN, &mShowDialpadOnResume);
        mActionBarController->RestoreInstanceState(savedInstanceState);
    }

    Boolean isLayoutRtl = DialerUtils::IsRtl();
    AutoPtr<IAnimationUtils> autils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&autils);
    if (mIsLandscape) {
        autils->LoadAnimation(this,
                isLayoutRtl ? R::anim::dialpad_slide_in_left : R::anim::dialpad_slide_in_right,
                (IAnimation**)&mSlideIn);
        autils->LoadAnimation(this,
                isLayoutRtl ? R::anim::dialpad_slide_out_left : R::anim::dialpad_slide_out_right,
                (IAnimation**)&mSlideOut);
    }
    else {
        autils->LoadAnimation(this, R::anim::dialpad_slide_in_bottom,
                (IAnimation**)&mSlideIn);
        autils->LoadAnimation(this, R::anim::dialpad_slide_out_bottom,
                (IAnimation**)&mSlideOut);
    }

    mSlideIn->SetInterpolator(AnimUtils::EASE_IN);
    mSlideOut->SetInterpolator(AnimUtils::EASE_OUT);

    mSlideOut->SetAnimationListener(mSlideOutListener);

    view = NULL;
    FindViewById(R::id::dialtacts_mainlayout, (IView**)&view);
    mParentLayout = IFrameLayout::Probe(view);
    view->SetOnDragListener(new LayoutOnDragListener(this));
    AutoPtr<IViewTreeObserver> observer;
    floatingActionButtonContainer->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->AddOnGlobalLayoutListener(new MyOnGlobalLayoutListener(this, observer));

    SetupActivityOverlay();

    mDialerDatabaseHelper = DatabaseHelperManager::GetDatabaseHelper(this);
    // TODO:
    // SmartDialPrefix::InitializeNanpSettings(this);
    return NOERROR;
}

void DialtactsActivity::SetupActivityOverlay()
{
    AutoPtr<IView> activityOverlay;
    FindViewById(R::id::activity_overlay, (IView**)&activityOverlay);
    activityOverlay->SetOnTouchListener(new MyOnTouchListener(this));
}

ECode DialtactsActivity::OnResume()
{
    FAIL_RETURN(TransactionSafeActivity::OnResume());
    if (mFirstLaunch) {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        DisplayFragment(intent);
    }
    else if (!PhoneIsInUse() && mInCallDialpadUp) {
        HideDialpadFragment(FALSE, TRUE);
        mInCallDialpadUp = FALSE;
    }
    else if (mShowDialpadOnResume) {
        ShowDialpadFragment(FALSE);
        mShowDialpadOnResume = FALSE;
    }
    mFirstLaunch = FALSE;
    PrepareVoiceSearchButton();
    // TODO:
    // mDialerDatabaseHelper->StartSmartDialUpdateThread();
    UpdateFloatingActionButtonControllerAlignment(FALSE /* animate */);
    return NOERROR;
}

ECode DialtactsActivity::OnPause()
{
    if (mClearSearchOnPause) {
        HideDialpadAndSearchUi();
        mClearSearchOnPause = FALSE;
    }
    return TransactionSafeActivity::OnPause();
}

ECode DialtactsActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(TransactionSafeActivity::OnSaveInstanceState(outState));
    outState->PutString(KEY_SEARCH_QUERY, mSearchQuery);
    outState->PutBoolean(KEY_IN_REGULAR_SEARCH_UI, mInRegularSearch);
    outState->PutBoolean(KEY_IN_DIALPAD_SEARCH_UI, mInDialpadSearch);
    outState->PutBoolean(KEY_FIRST_LAUNCH, mFirstLaunch);
    outState->PutBoolean(KEY_IS_DIALPAD_SHOWN, mIsDialpadShown);
    mActionBarController->SaveInstanceState(outState);
    return NOERROR;
}

ECode DialtactsActivity::OnAttachFragment(
    /* [in] */ IFragment* fragment)
{
    if (IDialpadFragment::Probe(fragment) != NULL) {
        mDialpadFragment = (CDialpadFragment*)IDialpadFragment::Probe(fragment);
        if (!mShowDialpadOnResume) {
            AutoPtr<IFragmentManager> manager;
            GetFragmentManager((IFragmentManager**)&manager);
            AutoPtr<IFragmentTransaction> transaction;
            manager->BeginTransaction((IFragmentTransaction**)&transaction);
            transaction->Hide(mDialpadFragment);
            Int32 id;
            transaction->Commit(&id);
        }
    }
    else if (ISmartDialSearchFragment::Probe(fragment) != NULL) {
        mSmartDialSearchFragment = (SmartDialSearchFragment*)ISmartDialSearchFragment::Probe(fragment);
        mSmartDialSearchFragment->SetOnPhoneNumberPickerActionListener(mInnerListener);
    }
    else if (ISearchFragment::Probe(fragment) != NULL) {
        mRegularSearchFragment = (RegularSearchFragment*)IRegularSearchFragment::Probe(fragment);
        mRegularSearchFragment->SetOnPhoneNumberPickerActionListener(mInnerListener);
    }
    else if (IListsFragment::Probe(fragment) != NULL) {
        mListsFragment = (CListsFragment*)IListsFragment::Probe(fragment);
        mListsFragment->AddOnPageChangeListener(mInnerListener);
    }
    return NOERROR;
}

void DialtactsActivity::HandleMenuSettings()
{
    AutoPtr<IIntent> intent;
    // CIntent::New(this, ECLSID_CDialerSettingsActivity, (IIntent**)&intent);
    assert(0 && "TODO HandleMenuSettings");
    StartActivity(intent);
}

ECode DialtactsActivity::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case R::id::floating_action_button:
            if (!mIsDialpadShown) {
                mInCallDialpadUp = FALSE;
                ShowDialpadFragment(TRUE);
            }
            break;
        case R::id::voice_search_button: {
            // try {
            AutoPtr<IIntent> intent;
            CIntent::New(IRecognizerIntent::ACTION_RECOGNIZE_SPEECH, (IIntent**)&intent);
            ECode ec = StartActivityForResult(intent, ACTIVITY_REQUEST_CODE_VOICE_SEARCH);
            if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
                AutoPtr<IToastHelper> toastHelper;
                CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
                AutoPtr<IToast> toast;
                toastHelper->MakeText(this,
                        R::string::voice_search_not_available,
                        IToast::LENGTH_SHORT, (IToast**)&toast);
                toast->Show();
            }
            // } catch (ActivityNotFoundException e) {
            //     Toast.makeText(DialtactsActivity.this, R.string.voice_search_not_available,
            //             Toast.LENGTH_SHORT).show();
            // }
            break;
        }
        case R::id::dialtacts_options_menu_button:
            mOverflowMenu->Show();
            break;
        default: {
            Logger::E(TAG, "Unexpected onClick event from %s", TO_CSTR(view));
            break;
        }
    }
    return NOERROR;
}

ECode DialtactsActivity::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case R::id::menu_history:
            ShowCallHistory();
            break;
        case R::id::menu_add_contact: {
            // try {
            AutoPtr<IContactsContractContacts> contacts;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
            AutoPtr<IUri> uri;
            contacts->GetCONTENT_URI((IUri**)&uri);
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_INSERT, uri, (IIntent**)&intent);
            ECode ec = StartActivity(intent);
            if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
                AutoPtr<IToastHelper> toastHelper;
                CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
                AutoPtr<IToast> toast;
                toastHelper->MakeText(this,
                        R::string::add_contact_not_available,
                        IToast::LENGTH_SHORT, (IToast**)&toast);
                toast->Show();
            }
            // } catch (ActivityNotFoundException e) {
            //     Toast toast = Toast.makeText(this,
            //             R.string.add_contact_not_available,
            //             Toast.LENGTH_SHORT);
            //     toast.show();
            // }
            break;
        }
        case R::id::menu_import_export: {
            // We hard-code the "contactsAreAvailable" argument because doing it properly would
            // involve querying a {@link ProviderStatusLoader}, which we don't want to do right
            // now in Dialtacts for (potential) performance reasons. Compare with how it is
            // done in {@link PeopleActivity}.
            AutoPtr<IFragmentManager> manager;
            GetFragmentManager((IFragmentManager**)&manager);
            ImportExportDialogFragment::Show(manager, TRUE, IObject::Probe(TO_IINTERFACE(this)));
            *result = TRUE;
            return NOERROR;
        }
        case R::id::menu_clear_frequents: {
            AutoPtr<IFragmentManager> manager;
            GetFragmentManager((IFragmentManager**)&manager);
            assert(0 && "TODO");
            // ClearFrequentsDialog.show(manager);
            *result = TRUE;
            return NOERROR;
        }
        case R::id::menu_call_settings:
            HandleMenuSettings();
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DialtactsActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (requestCode == ACTIVITY_REQUEST_CODE_VOICE_SEARCH) {
        if (resultCode == RESULT_OK) {
            AutoPtr<IArrayList> matches;
            data->GetStringArrayListExtra(
                    IRecognizerIntent::EXTRA_RESULTS, (IArrayList**)&matches);
            Int32 size;
            if (matches->GetSize(&size), size > 0) {
                AutoPtr<IInterface> match;
                matches->Get(0, (IInterface**)&match);
                ITextView::Probe(mSearchView)->SetText(ICharSequence::Probe(match));
            }
            else {
                Logger::E(TAG, "Voice search - nothing heard");
            }
        }
        else {
            Logger::E(TAG, "Voice search failed");
        }
    }
    return TransactionSafeActivity::OnActivityResult(requestCode, resultCode, data);
}

void DialtactsActivity::ShowDialpadFragment(
    /* [in] */ Boolean animate)
{
    if (mIsDialpadShown) {
        return;
    }
    mIsDialpadShown = TRUE;
    mDialpadFragment->SetAnimate(animate);
    mDialpadFragment->SendScreenView();

    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> ft;
    manager->BeginTransaction((IFragmentTransaction**)&ft);
    ft->Show(mDialpadFragment);
    Int32 id;
    ft->Commit(&id);

    if (animate) {
        mFloatingActionButtonController->ScaleOut();
    }
    else {
        mFloatingActionButtonController->SetVisible(FALSE);
    }
    mActionBarController->OnDialpadUp();

    Boolean inSearchUi;
    if (IsInSearchUi(&inSearchUi), !inSearchUi) {
        EnterSearchUi(TRUE /* isSmartDial */, mSearchQuery);
    }
}

ECode DialtactsActivity::OnDialpadShown()
{
    Boolean result;
    if (mDialpadFragment->GetAnimate(&result), result) {
        AutoPtr<IView> view;
        mDialpadFragment->GetView((IView**)&view);
        view->StartAnimation(mSlideIn);
    }
    else {
        mDialpadFragment->SetYFraction(0);
    }

    UpdateSearchFragmentPosition();
    return NOERROR;
}

ECode DialtactsActivity::HideDialpadFragment(
    /* [in] */ Boolean animate,
    /* [in] */ Boolean clearDialpad)
{
    if (mDialpadFragment == NULL) {
        return NOERROR;
    }
    if (clearDialpad) {
        mDialpadFragment->ClearDialpad();
    }
    if (!mIsDialpadShown) {
        return NOERROR;
    }
    mIsDialpadShown = FALSE;
    mDialpadFragment->SetAnimate(animate);

    UpdateSearchFragmentPosition();

    UpdateFloatingActionButtonControllerAlignment(animate);
    if (animate) {
        AutoPtr<IView> view;
        mDialpadFragment->GetView((IView**)&view);
        view->StartAnimation(mSlideOut);
    }
    else {
        CommitDialpadFragmentHide();
    }

    mActionBarController->OnDialpadDown();

    Boolean inSearchUi;
    if (IsInSearchUi(&inSearchUi), inSearchUi) {
        if (TextUtils::IsEmpty(mSearchQuery)) {
            ExitSearchUi();
        }
    }
    return NOERROR;
}

void DialtactsActivity::CommitDialpadFragmentHide()
{
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> ft;
    manager->BeginTransaction((IFragmentTransaction**)&ft);
    ft->Hide(mDialpadFragment);
    Int32 id;
    ft->Commit(&id);

    mFloatingActionButtonController->ScaleIn(AnimUtils::NO_DELAY);
}

void DialtactsActivity::UpdateSearchFragmentPosition()
{
    AutoPtr<ISearchFragment> fragment;
    Boolean isVisible;
    if (mSmartDialSearchFragment != NULL &&
            (mSmartDialSearchFragment->IsVisible(&isVisible), isVisible)) {
        fragment = ISearchFragment::Probe(mSmartDialSearchFragment);
    }
    else if (mRegularSearchFragment != NULL &&
            (mRegularSearchFragment->IsVisible(&isVisible), isVisible)) {
        fragment = ISearchFragment::Probe(mRegularSearchFragment);
    }
    if (fragment != NULL && (IFragment::Probe(fragment)->IsVisible(&isVisible), isVisible)) {
        fragment->UpdatePosition(TRUE /* animate */);
    }
}

ECode DialtactsActivity::IsInSearchUi(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInDialpadSearch || mInRegularSearch;
    return NOERROR;
}

ECode DialtactsActivity::HasSearchQuery(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !TextUtils::IsEmpty(mSearchQuery);
    return NOERROR;
}

ECode DialtactsActivity::ShouldShowActionBar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mListsFragment->ShouldShowActionBar(result);
}

void DialtactsActivity::SetNotInSearchUi()
{
    mInDialpadSearch = FALSE;
    mInRegularSearch = FALSE;
}

void DialtactsActivity::HideDialpadAndSearchUi()
{
    if (mIsDialpadShown) {
        HideDialpadFragment(FALSE, TRUE);
    }
    else {
        ExitSearchUi();
    }
}

void DialtactsActivity::PrepareVoiceSearchButton()
{
    AutoPtr<IIntent> voiceIntent;
    CIntent::New(IRecognizerIntent::ACTION_RECOGNIZE_SPEECH, (IIntent**)&voiceIntent);
    if (CanIntentBeHandled(voiceIntent)) {
        mVoiceSearchButton->SetVisibility(IView::VISIBLE);
        mVoiceSearchButton->SetOnClickListener(mInnerListener);
    }
    else {
        mVoiceSearchButton->SetVisibility(IView::GONE);
    }
}

AutoPtr<DialtactsActivity::OptionsPopupMenu> DialtactsActivity::BuildOptionsMenu(
    /* [in] */ IView* invoker)
{
    AutoPtr<OptionsPopupMenu> popupMenu = new OptionsPopupMenu(this);
    popupMenu->constructor(this, invoker);
    popupMenu->Inflate(R::menu::dialtacts_options);
    AutoPtr<IMenu> menu;
    popupMenu->GetMenu((IMenu**)&menu);
    popupMenu->SetOnMenuItemClickListener(mInnerListener);
    return popupMenu;
}

ECode DialtactsActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    VALIDATE_NOT_NULL(allowToShow);
    if (!mPendingSearchViewQuery.IsNull()) {
        ITextView::Probe(mSearchView)->SetText(CoreUtils::Convert(mPendingSearchViewQuery));
        mPendingSearchViewQuery = NULL;
    }
    mActionBarController->RestoreActionBarOffset();
    *allowToShow = FALSE;
    return NOERROR;
}

Boolean DialtactsActivity::IsSendKeyWhileInCall(
    /* [in] */ IIntent* intent)
{
    // If there is a call in progress and the user launched the dialer by hitting the call
    // button, go straight to the in-call screen.
    String action;
    intent->GetAction(&action);
    Boolean callKey = IIntent::ACTION_CALL_BUTTON.Equals(action);

    if (callKey) {
        GetTelecomManager()->ShowInCallScreen(FALSE);
        return TRUE;
    }

    return FALSE;
}

void DialtactsActivity::DisplayFragment(
    /* [in] */ IIntent* intent)
{
    // If we got here by hitting send and we're in call forward along to the in-call activity
    if (IsSendKeyWhileInCall(intent)) {
        Finish();
        return;
    }

    if (mDialpadFragment != NULL) {
        Boolean phoneIsInUse = PhoneIsInUse();
        AutoPtr<IUri> uri;
        if (phoneIsInUse || ((intent->GetData((IUri**)&uri), uri !=  NULL) && IsDialIntent(intent))) {
            mDialpadFragment->SetStartedFromNewIntent(TRUE);
            Boolean isVisible;
            if (phoneIsInUse && (mDialpadFragment->IsVisible(&isVisible), !isVisible)) {
                mInCallDialpadUp = TRUE;
            }
            ShowDialpadFragment(FALSE);
        }
    }
}

ECode DialtactsActivity::OnNewIntent(
    /* [in] */ IIntent* newIntent)
{
    SetIntent(newIntent);
    DisplayFragment(newIntent);

    InvalidateOptionsMenu();
    return NOERROR;
}

Boolean DialtactsActivity::IsDialIntent(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_DIAL.Equals(action) || ACTION_TOUCH_DIALER.Equals(action)) {
        return TRUE;
    }
    if (IIntent::ACTION_VIEW.Equals(action)) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        String scheme;
        if (data != NULL &&
            (data->GetScheme(&scheme), IPhoneAccount::SCHEME_TEL.Equals(scheme))) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode DialtactsActivity::GetCallOrigin(
    /* [out] */ String* origin)
{
    VALIDATE_NOT_NULL(origin);
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    *origin = !IsDialIntent(intent) ? CALL_ORIGIN_DIALTACTS : String(NULL);
    return NOERROR;
}

void DialtactsActivity::EnterSearchUi(
    /* [in] */ Boolean smartDialSearch,
    /* [in] */ const String& query)
{
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    Boolean isDestroyed;
    if (manager->IsDestroyed(&isDestroyed), isDestroyed) {
        // Weird race condition where fragment is doing work after the activity is destroyed
        // due to talkback being on (b/10209937). Just return since we can't do any
        // constructive here.
        return;
    }

    if (DEBUG) {
        Logger::D(TAG, "Entering search UI - smart dial %d", smartDialSearch);
    }

    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    if (mInDialpadSearch && mSmartDialSearchFragment != NULL) {
        transaction->Remove(IFragment::Probe(mSmartDialSearchFragment));
    }
    else if (mInRegularSearch && mRegularSearchFragment != NULL) {
        transaction->Remove(IFragment::Probe(mRegularSearchFragment));
    }

    String tag;
    if (smartDialSearch) {
        tag = TAG_SMARTDIAL_SEARCH_FRAGMENT;
    }
    else {
        tag = TAG_REGULAR_SEARCH_FRAGMENT;
    }
    mInDialpadSearch = smartDialSearch;
    mInRegularSearch = !smartDialSearch;

    AutoPtr<IFragment> fragment;
    manager->FindFragmentByTag(tag, (IFragment**)&fragment);
    transaction->SetCustomAnimations(Elastos::Droid::R::animator::fade_in, 0);
    if (fragment == NULL) {
        if (smartDialSearch) {
            AutoPtr<SmartDialSearchFragment> smartFragment = new SmartDialSearchFragment();
            smartFragment->constructor();
            fragment = IFragment::Probe(smartFragment);
        }
        else {
            AutoPtr<RegularSearchFragment> regularFragment = new RegularSearchFragment();
            regularFragment->constructor();
            fragment = IFragment::Probe(regularFragment);
        }
        // transaction->Add(R::id::dialtacts_frame, fragment, tag);
    }
    else {
        transaction->Show(fragment);
    }
    // DialtactsActivity will provide the options menu
    fragment->SetHasOptionsMenu(FALSE);
    AutoPtr<IContactEntryListFragment> f = IContactEntryListFragment::Probe(fragment);
    f->SetShowEmptyListForNullQuery(TRUE);
    f->SetQueryString(query, FALSE /* delaySelection */);
    Int32 id;
    transaction->Commit(&id);

    AutoPtr<IView> view;
    mListsFragment->GetView((IView**)&view);
    AutoPtr<IViewPropertyAnimator> animator;
    view->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(0);
    animator->WithLayer();
}

void DialtactsActivity::ExitSearchUi()
{
    // See related bug in enterSearchUI();
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    Boolean isDestroyed;
    if (manager->IsDestroyed(&isDestroyed), isDestroyed) {
        return;
    }

    ITextView::Probe(mSearchView)->SetText((ICharSequence*)NULL);
    mDialpadFragment->ClearDialpad();
    SetNotInSearchUi();

    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    if (mSmartDialSearchFragment != NULL) {
        transaction->Remove(IFragment::Probe(mSmartDialSearchFragment));
    }
    if (mRegularSearchFragment != NULL) {
        transaction->Remove(IFragment::Probe(mRegularSearchFragment));
    }
    Int32 id;
    transaction->Commit(&id);

    AutoPtr<IView> view;
    mListsFragment->GetView((IView**)&view);
    AutoPtr<IViewPropertyAnimator> animator;
    view->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(1);
    animator->WithLayer();
    mActionBarController->OnSearchUiExited();
}

AutoPtr<IIntent> DialtactsActivity::GetCallSettingsIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelecomManager::ACTION_SHOW_CALL_SETTINGS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    return intent;
}

ECode DialtactsActivity::OnBackPressed()
{
    Boolean inSearchUi;
    if (mIsDialpadShown) {
        Boolean isVisible;
        AutoPtr<IInterface> adapter;
        Int32 count;
        if (TextUtils::IsEmpty(mSearchQuery) ||
                (mSmartDialSearchFragment != NULL &&
                        (mSmartDialSearchFragment->IsVisible(&isVisible), isVisible) &&
                        (IContactEntryListFragment::Probe(mSmartDialSearchFragment)
                                ->GetAdapter((IInterface**)&adapter),
                                ICompositeCursorAdapter::Probe(adapter)->GetCount(&count),
                                count == 0))) {
            ExitSearchUi();
        }
        HideDialpadFragment(TRUE, FALSE);
    }
    else if (IsInSearchUi(&inSearchUi), inSearchUi) {
        ExitSearchUi();
        DialerUtils::HideInputMethod(IView::Probe(mParentLayout));
    }
    else {
        TransactionSafeActivity::OnBackPressed();
    }
    return NOERROR;
}

Boolean DialtactsActivity::MaybeExitSearchUi()
{
    Boolean inSearchUi;
    if ((IsInSearchUi(&inSearchUi), inSearchUi) && TextUtils::IsEmpty(mSearchQuery)) {
        ExitSearchUi();
        DialerUtils::HideInputMethod(IView::Probe(mParentLayout));
        return TRUE;
    }
    return FALSE;
}

ECode DialtactsActivity::OnDialpadQueryChanged(
    /* [in] */ const String& query)
{
    if (mSmartDialSearchFragment != NULL) {
        mSmartDialSearchFragment->SetAddToContactNumber(query);
    }
    String normalizedQuery;
    // TODO:
    // String normalizedQuery = SmartDialNameMatcher::NormalizeNumber(query,
    //         SmartDialNameMatcher::LATIN_SMART_DIAL_MAP);

    AutoPtr<ICharSequence> text;
    ITextView::Probe(mSearchView)->GetText((ICharSequence**)&text);
    if (!TextUtils::Equals(CoreUtils::Unbox(text), normalizedQuery)) {
        if (DEBUG) {
            Logger::D(TAG, "onDialpadQueryChanged - new query: %s", query.string());
        }
        Boolean isVisible;
        if (mDialpadFragment == NULL ||
                (mDialpadFragment->IsVisible(&isVisible), !isVisible)) {
            // This callback can happen if the dialpad fragment is recreated because of
            // activity destruction. In that case, don't update the search view because
            // that would bring the user back to the search fragment regardless of the
            // previous state of the application. Instead, just return here and let the
            // fragment manager correctly figure out whatever fragment was last displayed.
            if (!TextUtils::IsEmpty(normalizedQuery)) {
                mPendingSearchViewQuery = normalizedQuery;
            }
            return NOERROR;
        }
        ITextView::Probe(mSearchView)->SetText(CoreUtils::Convert(normalizedQuery));
    }
    return NOERROR;
}

ECode DialtactsActivity::OnListFragmentScrollStateChange(
    /* [in] */ Int32 scrollState)
{
    if (scrollState == IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL) {
        HideDialpadFragment(TRUE, FALSE);
        DialerUtils::HideInputMethod(IView::Probe(mParentLayout));
    }
    return NOERROR;
}

ECode DialtactsActivity::OnListFragmentScroll(
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    // TODO: No-op for now. This should eventually show/hide the actionBar based on
    // interactions with the ListsFragments.
    return NOERROR;
}

Boolean DialtactsActivity::PhoneIsInUse()
{
    // Boolean inCall;
    // GetTelecomManager()->IsInCall(&inCall);
    // return inCall;
    return FALSE;
}

AutoPtr<IIntent> DialtactsActivity::GetAddNumberToContactIntent(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_INSERT_OR_EDIT, (IIntent**)&intent);
    intent->PutExtra(IContactsContractIntentsInsert::PHONE, text);
    intent->SetType(IContactsContractContacts::CONTENT_ITEM_TYPE);
    return intent;
}

Boolean DialtactsActivity::CanIntentBeHandled(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IPackageManager> packageManager;
    GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IList> resolveInfo;
    packageManager->QueryIntentActivities(intent,
            IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&resolveInfo);
    Int32 size;
    return resolveInfo != NULL && (resolveInfo->GetSize(&size), size > 0);
}

ECode DialtactsActivity::ShowCallHistory()
{
    // Use explicit CallLogActivity intent instead of ACTION_VIEW +
    // CONTENT_TYPE, so that we always open our call log from our dialer
    AutoPtr<IIntent> intent;
    CIntent::New(this, ECLSID_CCallLogActivity, (IIntent**)&intent);
    StartActivity(intent);
    return NOERROR;
}

ECode DialtactsActivity::OnDragStarted(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* view)
{
    Boolean isPaneOpen;
    if (mListsFragment->IsPaneOpen(&isPaneOpen), isPaneOpen) {
        mActionBarController->SetAlpha(IListsFragment::REMOVE_VIEW_SHOWN_ALPHA);
    }
    mListsFragment->ShowRemoveView(TRUE);
    return NOERROR;
}

ECode DialtactsActivity::OnDragHovered(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* view)
{
    return NOERROR;
}

ECode DialtactsActivity::OnDragFinished(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Boolean isPaneOpen;
    if (mListsFragment->IsPaneOpen(&isPaneOpen), isPaneOpen) {
        mActionBarController->SetAlpha(IListsFragment::REMOVE_VIEW_HIDDEN_ALPHA);
    }
    mListsFragment->ShowRemoveView(FALSE);
    return NOERROR;
}

ECode DialtactsActivity::OnDroppedOnRemove()
{
    return NOERROR;
}

ECode DialtactsActivity::SetDragDropController(
    /* [in] */ IDragDropController* dragController)
{
    mDragDropController = (DragDropController*)dragController;
    AutoPtr<IRemoveView> removeView;
    mListsFragment->GetRemoveView((IRemoveView**)&removeView);
    removeView->SetDragDropController(dragController);
    return NOERROR;
}

ECode DialtactsActivity::OnPickPhoneNumberAction(
    /* [in] */ IUri* dataUri)
{
    // Specify call-origin so that users will see the previous tab instead of
    // CallLog screen (search UI will be automatically exited).
    String origin;
    GetCallOrigin(&origin);
    assert(0 && "TODO");
    // PhoneNumberInteraction::StartInteractionForPhoneCall(
    //         DialtactsActivity.this, dataUri, origin);
    mClearSearchOnPause = TRUE;
    return NOERROR;
}

ECode DialtactsActivity::OnCallNumberDirectly(
    /* [in] */ const String& phoneNumber)
{
    return OnCallNumberDirectly(phoneNumber, FALSE /* isVideoCall */);
}

ECode DialtactsActivity::OnCallNumberDirectly(
    /* [in] */ const String& phoneNumber,
    /* [in] */ Boolean isVideoCall)
{
    String origin;
    GetCallOrigin(&origin);

    AutoPtr<IIntent> intent = isVideoCall ?
            CallUtil::GetVideoCallIntent(phoneNumber, origin) :
            CallUtil::GetCallIntent(phoneNumber, origin);
    DialerUtils::StartActivityWithErrorToast(this, intent);
    mClearSearchOnPause = TRUE;
    return NOERROR;
}

ECode DialtactsActivity::OnShortcutIntentCreated(
    /* [in] */ IIntent* intent)
{
    Logger::W(TAG, "Unsupported intent has come (%s). Ignoring.", TO_CSTR(intent));
    return NOERROR;
}

ECode DialtactsActivity::OnHomeInActionBarSelected()
{
    ExitSearchUi();
    return NOERROR;
}

ECode DialtactsActivity::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    mListsFragment->GetRtlPosition(position, &position);
    // Only scroll the button when the first tab is selected. The button should scroll from
    // the middle to right position only on the transition from the first tab to the second
    // tab.
    // If the app is in RTL mode, we need to check against the second tab, rather than the
    // first. This is because if we are scrolling between the first and second tabs, the
    // viewpager will report that the starting tab position is 1 rather than 0, due to the
    // reversal of the order of the tabs.
    Boolean isLayoutRtl = DialerUtils::IsRtl();
    Boolean shouldScrollButton = position == (isLayoutRtl
            ? IListsFragment::TAB_INDEX_RECENTS : IListsFragment::TAB_INDEX_SPEED_DIAL);
    if (shouldScrollButton && !mIsLandscape) {
        mFloatingActionButtonController->OnPageScrolled(
                isLayoutRtl ? 1 - positionOffset : positionOffset);
    }
    else if (position != IListsFragment::TAB_INDEX_SPEED_DIAL) {
        mFloatingActionButtonController->OnPageScrolled(1);
    }
    return NOERROR;
}

ECode DialtactsActivity::OnPageSelected(
    /* [in] */ Int32 position)
{
    mListsFragment->GetRtlPosition(position, &position);
    mCurrentTabPosition = position;
    return NOERROR;
}

ECode DialtactsActivity::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    return NOERROR;
}

AutoPtr<ITelephonyManager> DialtactsActivity::GetTelephonyManager()
{
    AutoPtr<IInterface> service;
    GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    return ITelephonyManager::Probe(service);
}

AutoPtr<ITelecomManager> DialtactsActivity::GetTelecomManager()
{
    AutoPtr<IInterface> service;
    GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    return ITelecomManager::Probe(service);
}

ECode DialtactsActivity::IsActionBarShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mActionBarController->IsActionBarShowing();
    return NOERROR;
}

ECode DialtactsActivity::IsDialpadShown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsDialpadShown;
    return NOERROR;
}

ECode DialtactsActivity::GetActionBarHideOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    actionBar->GetHideOffset(offset);
    return NOERROR;
}

ECode DialtactsActivity::GetActionBarHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mActionBarHeight;
    return NOERROR;
}

ECode DialtactsActivity::SetActionBarHideOffset(
    /* [in] */ Int32 hideOffset)
{
    mActionBarController->SetHideOffset(hideOffset);
    return NOERROR;
}

void DialtactsActivity::UpdateFloatingActionButtonControllerAlignment(
    /* [in] */ Boolean animate)
{
    Int32 align = (!mIsLandscape && mCurrentTabPosition == IListsFragment::TAB_INDEX_SPEED_DIAL) ?
            FloatingActionButtonController::ALIGN_MIDDLE :
                    FloatingActionButtonController::ALIGN_END;
    mFloatingActionButtonController->Align(align, 0 /* offsetX */, 0 /* offsetY */, animate);
}

ECode DialtactsActivity::GetActionBar(
    /* [out] */ IActionBar** bar)
{
    VALIDATE_NOT_NULL(bar);
    return TransactionSafeActivity::GetActionBar(bar);
}

} // Dialer
} // Droid
} // Elastos
