
#include "elastos/droid/dialer/list/ListsFragment.h"
// #include "elastos/droid/dialer/list/CAllContactsFragment.h"
// #include "elastos/droid/dialer/list/CSpeedDialFragment.h"
// #include "elastos/droid/dialer/list/ShortcutCardsAdapter.h"
// #include "elastos/droid/dialer/list/CShortcutCardsAdapter.h"
// #include "elastos/droid/dialer/calllog/CCallLogFragment.h"
// #include "elastos/droid/dialer/calllog/CContactInfoHelper.h"
// #include "elastos/droid/dialer/calllog/CCallLogQueryHandler.h"
// #include "elastos/droid/dialer/util/DialerUtils.h"
// #include "elastos/droid/dialerbind/CObjectFactory.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
// using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::CoreUtils;
// using Elastos::Core::ISystem;
// using Elastos::Core::CSystem;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
// using Elastos::Apps::Dialer::CallLog::ICallLogQuery;
// using Elastos::Apps::Dialer::CallLog::CCallLogFragment;
// using Elastos::Apps::Dialer::CallLog::CCallLogQueryHandler;
// using Elastos::Apps::Dialer::Util::DialerUtils;
// using Elastos::Apps::Dialer::IDialtactsActivity;
// using Elastos::Apps::DialerBind::IObjectFactory;
// using Elastos::Apps::DialerBind::CObjectFactory;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogAdapterCallFetcher;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogQueryHandlerListener;
using Elastos::Droid::Dialer::Widget::EIID_IPanelSlideCallbacks;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// ListsFragment::ViewPagerAdapter
//=================================================================
ListsFragment::ViewPagerAdapter::ViewPagerAdapter(
    /* [in] */ ListsFragment* host)
    : mHost(host)
{}

ECode ListsFragment::ViewPagerAdapter::constructor(
    /* [in] */ IFragmentManager* fm)
{
    assert(0 && "TODO");
    // return FragmentPagerAdapter::constructor(fm);
    return NOERROR;
}

ECode ListsFragment::ViewPagerAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    return mHost->GetRtlPosition(position, (Int32*)id);
}

ECode ListsFragment::ViewPagerAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IFragment** item)
{
    VALIDATE_NOT_NULL(item);

    assert(0 && "TODO");
    // Int32 rtl;
    // mHost->GetRtlPosition(position, &rtl);
    // switch (rtl) {
    //     case TAB_INDEX_SPEED_DIAL:
    //         CSpeedDialFragment::New((ISpeedDialFragment**)&(mHost->mSpeedDialFragment));
    //         *item = IFragment::Probe(mHost->mSpeedDialFragment);
    //         REFCOUNT_ADD(*item);
    //         return NOERROR;
    //     case TAB_INDEX_RECENTS:
    //     {
    //         AutoPtr<ISystem> sys;
    //         CSystem::AcquireSingleton((ISystem**)&sys);
    //         Int64 value;
    //         sys->GetCurrentTimeMillis(&value);
    //         CCallLogFragment::New(ICallLogQueryHandler::CALL_TYPE_ALL, MAX_RECENTS_ENTRIES,
    //                 value - OLDEST_RECENTS_DATE, (ICallLogFragment**)&(mHost->mRecentsFragment));
    //         mHost->mRecentsFragment->SetHasFooterView(TRUE);
    //         *item = IFragment::Probe(mHost->mRecentsFragment);
    //         REFCOUNT_ADD(*item);
    //         return NOERROR;
    //     }
    //     case TAB_INDEX_ALL_CONTACTS:
    //         CAllContactsFragment::New((IAllContactsFragment**)&(mHost->mAllContactsFragment));
    //         *item = IFragment::Probe(mHost->mAllContactsFragment);
    //         REFCOUNT_ADD(*item);
    //         return NOERROR;
    // }
    // // throw new IllegalStateException("No fragment at position " + position);
    // Logger::E(ListsFragment::TAG, "No fragment at position %d", position);
    // return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ListsFragment::ViewPagerAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);

    // On rotation the FragmentManager handles rotation. Therefore getItem() isn't called.
    // Copy the fragments that the FragmentManager finds so that we can store them in
    // instance variables for later.
    AutoPtr<IInterface> fragment;
    assert(0 && "TODO");
    // FragmentPagerAdapter::InstantiateItem(container, position, (IInterface**)&fragment);
    // if (ISpeedDialFragment::Probe(fragment) != NULL) {
    //     mHost->mSpeedDialFragment = ISpeedDialFragment::Probe(fragment);
    // }
    // else if (ICallLogFragment::Probe(fragment) != NULL) {
    //     mHost->mRecentsFragment = ICallLogFragment::Probe(fragment);
    // }
    // else if (IAllContactsFragment::Probe(fragment) != NULL) {
    //     mHost->mAllContactsFragment = IAllContactsFragment::Probe(fragment);
    // }
    *item = fragment;
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode ListsFragment::ViewPagerAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = TAB_INDEX_COUNT;
    return NOERROR;
}

ECode ListsFragment::ViewPagerAdapter::GetPageTitle(
    /* [in] */ Int32 position,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);

    AutoPtr<ICharSequence> cs = CoreUtils::Convert((*mHost->mTabTitles)[position]);
    *title = cs;
    REFCOUNT_ADD(*title);
    return NOERROR;
}


//=================================================================
// ListsFragment::PanelSlideCallbacks
//=================================================================
CAR_INTERFACE_IMPL(ListsFragment::PanelSlideCallbacks, Object, IPanelSlideCallbacks)

ListsFragment::PanelSlideCallbacks::PanelSlideCallbacks(
    /* [in] */ ListsFragment* host)
    : mHost(host)
{}

ECode ListsFragment::PanelSlideCallbacks::OnPanelSlide(
    /* [in] */ IView* panel,
    /* [in] */ Float slideOffset)
{
    // For every 1 percent that the panel is slid upwards, clip 1 percent off the top
    // edge of the shortcut card, to achieve the animated effect of the shortcut card
    // being pushed out of view when the panel is slid upwards. slideOffset is 1 when
    // the shortcut card is fully exposed, and 0 when completely hidden.
    Float ratioCardHidden = (1 - slideOffset);
    Int32 count;
    if (IViewGroup::Probe(mHost->mShortcutCardsListView)->GetChildCount(&count), count > 0) {
        AutoPtr<IView> v;
        IViewGroup::Probe(mHost->mShortcutCardsListView)->GetChildAt(0, (IView**)&v);
        assert(0 && "TODO");
        // ((ShortcutCardsAdapter::SwipeableShortcutCard*)v.Get())->ClipCard(ratioCardHidden);
    }

    if (mHost->mActionBar != NULL) {
        // Amount of available space that is not being hidden by the bottom pane
        Int32 height;
        IView::Probe(mHost->mShortcutCardsListView)->GetHeight(&height);
        Int32 topPaneHeight = (Int32) (slideOffset * height);

        mHost->mActionBar->GetHeight(&height);
        Int32 availableActionBarHeight = Elastos::Core::Math::Min(height, topPaneHeight);

        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        IListsFragmentHostInterface::Probe(activity)->SetActionBarHideOffset(
                height - availableActionBarHeight);

        Boolean isShowing;
        if (mHost->mActionBar->IsShowing(&isShowing), !isShowing) {
            mHost->mActionBar->Show();
        }
    }

    return NOERROR;
}

ECode ListsFragment::PanelSlideCallbacks::OnPanelOpened(
    /* [in] */ IView* panel)
{
    if (ListsFragment::DEBUG) {
        Logger::D(TAG, "onPanelOpened");
    }
    mHost->mIsPanelOpen = TRUE;
    return NOERROR;
}

ECode ListsFragment::PanelSlideCallbacks::OnPanelClosed(
    /* [in] */ IView* panel)
{
    if (ListsFragment::DEBUG) {
        Logger::D(TAG, "onPanelClosed");
    }
    mHost->mIsPanelOpen = FALSE;
    return NOERROR;
}

ECode ListsFragment::PanelSlideCallbacks::OnPanelFlingReachesEdge(
    /* [in] */ Int32 velocityY)
{
    AutoPtr<IAbsListView> listView;
    mHost->GetCurrentListView((IAbsListView**)&listView);
    if (listView != NULL) {
        listView->Fling(velocityY);
    }
    return NOERROR;
}

ECode ListsFragment::PanelSlideCallbacks::IsScrollableChildUnscrolled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IAbsListView> listView;
    mHost->GetCurrentListView((IAbsListView**)&listView);
    if (listView != NULL) {
        Int32 count;
        IViewGroup::Probe(listView)->GetChildCount(&count);
        AutoPtr<IView> view;
        IViewGroup::Probe(listView)->GetChildAt(0, (IView**)&view);
        Int32 top, paddingTop;
        view->GetTop(&top);
        IView::Probe(listView)->GetPaddingTop(&paddingTop);
        *result = count == 0 || top == paddingTop;
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}


//=================================================================
// ListsFragment
//=================================================================
const Boolean ListsFragment::DEBUG = TRUE;
const String ListsFragment::TAG("ListsFragment");

const Int32 ListsFragment::MAX_RECENTS_ENTRIES = 20;
// Oldest recents entry to display is 2 weeks old.
const Int64 ListsFragment::OLDEST_RECENTS_DATE = 1000LL * 60 * 60 * 24 * 14;

const String ListsFragment::KEY_LAST_DISMISSED_CALL_SHORTCUT_DATE("key_last_dismissed_call_shortcut_date");

CAR_INTERFACE_IMPL_3(ListsFragment, AnalyticsFragment, IListsFragment,
        ICallLogQueryHandlerListener, ICallLogAdapterCallFetcher)

ListsFragment::ListsFragment()
    : mIsPanelOpen(TRUE)
    , mLastCallShortcutDate(0)
    , mCurrentCallShortcutDate(0)
{
    CArrayList::New((IArrayList**)&mOnPageChangeListeners);
    mPanelSlideCallbacks = new PanelSlideCallbacks(this);
}

ECode ListsFragment::GetCurrentListView(
    /* [out] */ IAbsListView** listView)
{
    Int32 position;
    assert(0 && "TODO");
    // mViewPager->GetCurrentItem(&position);
    // Int32 rtl;
    // GetRtlPosition(position, &rtl);
    // switch (rtl) {
    //     case TAB_INDEX_SPEED_DIAL:
    //         mSpeedDialFragment == NULL ? *listView = NULL : mSpeedDialFragment->GetListView(listView);
    //         return NOERROR;
    //     case TAB_INDEX_RECENTS:
    //         mRecentsFragment == NULL ? *listView = NULL : mRecentsFragment->GetListView(listView);
    //         return NOERROR;
    //     case TAB_INDEX_ALL_CONTACTS:
    //         mAllContactsFragment == NULL ? *listView = NULL : mAllContactsFragment->GetListView(listView);
    //         return NOERROR;
    // }
    // throw new IllegalStateException("No fragment at position " + position);
    Logger::E(TAG, "No fragment at position %d", position);
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ListsFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(AnalyticsFragment::OnCreate(savedInstanceState));

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);
    assert(0 && "TODO");
    // CCallLogQueryHandler::New(resolver,
    //         this, 1, (ICallLogQueryHandler**)&mCallLogQueryHandler);
    // String currentCountryIso;
    // currentCountryIso = GeoUtil::GetCurrentCountryIso(activity);

    // AutoPtr<IObjectFactory> factory;
    // CObjectFactory::AcquireSingleton((IObjectFactory**)&factory);
    // AutoPtr<IContactInfoHelper> helper;
    // CContactInfoHelper::New(IContext::Probe(activity),
    //         currentCountryIso, (IContactInfoHelper**)&helper);
    // factory->NewCallLogAdapter(IContext::Probe(activity), this,
    //         helper, NULL, NULL, FALSE, (ICallLogAdapter**)&mCallLogAdapter);

    // CShortcutCardsAdapter::New(activity, this,
    //         mCallLogAdapter, (IShortcutCardsAdapter**)&mMergedAdapter);
    return NOERROR;
}

ECode ListsFragment::OnStart()
{
    return AnalyticsFragment::OnStart();
}

ECode ListsFragment::OnResume()
{
    FAIL_RETURN(AnalyticsFragment::OnResume());
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ISharedPreferences> prefs;
    IContext::Probe(activity)->GetSharedPreferences(IDialtactsActivity::SHARED_PREFS_NAME,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    prefs->GetInt64(KEY_LAST_DISMISSED_CALL_SHORTCUT_DATE, 0, &mLastCallShortcutDate);
    activity->GetActionBar((IActionBar**)&mActionBar);
    FetchCalls();
    assert(0 && "TODO");
    // mCallLogAdapter->SetLoading(TRUE);
    return NOERROR;
}

ECode ListsFragment::OnPause()
{
    // Wipe the cache to refresh the call shortcut item. This is not that expensive because
    // it only contains one item.
    assert(0 && "TODO");
    // mCallLogAdapter->InvalidateCache();
    return AnalyticsFragment::OnPause();
}

ECode ListsFragment::OnDestroy()
{
    assert(0 && "TODO");
    // mCallLogAdapter->StopRequestProcessing();
    return AnalyticsFragment::OnDestroy();
}

ECode ListsFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> parentView;
    inflater->Inflate(R::layout::lists_fragment,
            container, FALSE, (IView**)&parentView);
    AutoPtr<IView> temp;

    assert(0 && "TODO");
    // mViewPager = (ViewPager) parentView.findViewById(R.id.lists_pager);
    // mViewPager = IViewPager::Probe(temp);

    // AutoPtr<IFragmentManager> manager;
    // GetChildFragmentManager((IFragmentManager**)&manager);
    // CViewPagerAdapter::New(manager, (IViewPagerAdapter**)&mViewPagerAdapter);
    // mViewPager->SetAdapter(mViewPagerAdapter);
    // mViewPager->SetOffscreenPageLimit(2);
    // mViewPager->SetOnPageChangeListener(this);
    // Int32 rtl;
    // GetRtlPosition(TAB_INDEX_SPEED_DIAL, &rtl);
    // mViewPager->SetCurrentItem(rtl);

    mTabTitles = ArrayOf<String>::Alloc(TAB_INDEX_COUNT);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String dial, recents, contacts;
    resources->GetString(R::string::tab_speed_dial, &dial);
    mTabTitles->Set(TAB_INDEX_SPEED_DIAL, dial);
    resources->GetString(R::string::tab_recents, &recents);
    mTabTitles->Set(TAB_INDEX_RECENTS, recents);
    resources->GetString(R::string::tab_all_contacts, &contacts);
    mTabTitles->Set(TAB_INDEX_ALL_CONTACTS, contacts);

    temp = NULL;
    parentView->FindViewById(R::id::lists_pager_header, (IView**)&temp);
    assert(0 && "TODO");
    // mViewPagerTabs = IViewPagerTabs::Probe(temp);
    // mViewPagerTabs->SetViewPager(mViewPager);
    // AddOnPageChangeListener(mViewPagerTabs);

    temp = NULL;
    parentView->FindViewById(R::id::shortcut_card_list, (IView**)&temp);
    mShortcutCardsListView = IListView::Probe(temp);
    // IAdapterView::Probe(mShortcutCardsListView)->SetAdapter(mMergedAdapter);

    temp = NULL;
    parentView->FindViewById(R::id::remove_view, (IView**)&temp);
    // mRemoveView = IRemoveView::Probe(temp);
    parentView->FindViewById(R::id::remove_view_content, (IView**)&mRemoveViewContent);

    // SetupPaneLayout(IOverlappingPaneLayout::Probe(parentView));

    *view = parentView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode ListsFragment::OnVoicemailStatusFetched(
    /* [in] */ ICursor* statusCursor)
{
    // no-op
    return NOERROR;
}

ECode ListsFragment::OnCallsFetched(
    /* [in] */ ICursor* cursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    assert(0 && "TODO");
    // mCallLogAdapter->SetLoading(FALSE);

    // Save the date of the most recent call log item
    Boolean succeeded;
    if (cursor != NULL && (cursor->MoveToFirst(&succeeded), succeeded)) {
        // cursor->GetInt64(ICallLogQuery::DATE, &mCurrentCallShortcutDate);
    }

    // mCallLogAdapter->ChangeCursor(cursor);
    // mMergedAdapter->NotifyDataSetChanged();
    // Return true; took ownership of cursor
    *result = TRUE;
    return NOERROR;
}

ECode ListsFragment::FetchCalls()
{
    assert(0 && "TODO");
    return NOERROR;
    // return mCallLogQueryHandler->FetchCalls(
    //         ICallLogQueryHandler::CALL_TYPE_ALL, mLastCallShortcutDate);
}

ECode ListsFragment::DismissShortcut(
    /* [in] */ IView* view)
{
    mLastCallShortcutDate = mCurrentCallShortcutDate;
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AutoPtr<ISharedPreferences> prefs;
    context->GetSharedPreferences(IDialtactsActivity::SHARED_PREFS_NAME,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    AutoPtr<ISharedPreferencesEditor> editor;
    prefs->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutInt64(KEY_LAST_DISMISSED_CALL_SHORTCUT_DATE, mLastCallShortcutDate);
    editor->Apply();
    FetchCalls();
    return NOERROR;
}

ECode ListsFragment::AddOnPageChangeListener(
    /* [in] */ IViewPagerOnPageChangeListener* onPageChangeListener)
{
    Boolean contains;
    if (mOnPageChangeListeners->Contains(onPageChangeListener, &contains), !contains) {
        mOnPageChangeListeners->Add(onPageChangeListener);
    }
    return NOERROR;
}

ECode ListsFragment::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    Int32 count;
    mOnPageChangeListeners->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> listener;
        mOnPageChangeListeners->Get(i, (IInterface**)&listener);
        IViewPagerOnPageChangeListener::Probe(listener)->OnPageScrolled(position,
                positionOffset, positionOffsetPixels);
    }
    return NOERROR;
}

ECode ListsFragment::OnPageSelected(
    /* [in] */ Int32 position)
{
    assert(0 && "TODO");
    // if (position == TAB_INDEX_SPEED_DIAL && mSpeedDialFragment != NULL) {
    //     mSpeedDialFragment->SendScreenView();
    // }
    // else if (position == TAB_INDEX_RECENTS && mRecentsFragment != NULL) {
    //     mRecentsFragment->SendScreenView();
    // }
    // else if (position == TAB_INDEX_ALL_CONTACTS && mAllContactsFragment != NULL) {
    //     mAllContactsFragment->SendScreenView();
    // }
    Int32 count;
    mOnPageChangeListeners->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> listener;
        mOnPageChangeListeners->Get(i, (IInterface**)&listener);
        IViewPagerOnPageChangeListener::Probe(listener)->OnPageSelected(position);
    }
    return NOERROR;
}

ECode ListsFragment::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    Int32 count;
    mOnPageChangeListeners->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> listener;
        mOnPageChangeListeners->Get(i, (IInterface**)&listener);
        IViewPagerOnPageChangeListener::Probe(listener)->OnPageScrollStateChanged(state);
    }
    return NOERROR;
}

ECode ListsFragment::ShowRemoveView(
    /* [in] */ Boolean show)
{
    mRemoveViewContent->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    assert(0 && "TODO");
    // mRemoveView->SetAlpha(show ? 0 : 1);
    AutoPtr<IViewPropertyAnimator> animator;
    // mRemoveView->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(show ? 1 : 0);
    animator->Start();

    Int32 count;
    if (IViewGroup::Probe(mShortcutCardsListView)->GetChildCount(&count), count > 0) {
        AutoPtr<IView> v;
        IViewGroup::Probe(mShortcutCardsListView)->GetChildAt(0, (IView**)&v);
        animator = NULL;
        v->Animate((IViewPropertyAnimator**)&animator);
        animator->WithLayer();
        animator->Alpha(show ? REMOVE_VIEW_SHOWN_ALPHA : REMOVE_VIEW_HIDDEN_ALPHA);
        animator->Start();
    }
    return NOERROR;
}

ECode ListsFragment::ShouldShowActionBar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsPanelOpen && mActionBar != NULL;
    return NOERROR;
}

ECode ListsFragment::IsPaneOpen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsPanelOpen;
    return NOERROR;
}

// void ListsFragment::SetupPaneLayout(
//     /* [in] */ IOverlappingPaneLayout* paneLayout)
// {
//     // TODO: Remove the notion of a capturable view. The entire view be slideable, once
//     // the framework better supports nested scrolling.
//     assert(0 && "TODO");
//     // paneLayou->SetCapturableView(mViewPagerTabs);
//     paneLayout->OpenPane();
//     paneLayout->SetPanelSlideCallbacks(mPanelSlideCallbacks);

//     AutoPtr<IActivity> activity;
//     GetActivity((IActivity**)&activity);
//     Int32 height;
//     IListsFragmentHostInterface::Probe(activity)->GetActionBarHeight(&height);
//     paneLayout->SetIntermediatePinnedOffset(height);

//     AutoPtr<ILayoutTransition> transition;
//     paneLayout->GetLayoutTransition((ILayoutTransition**)&transition);
//     // Turns on animations for all types of layout changes so that they occur for
//     // height changes.
//     transition->EnableTransitionType(ILayoutTransition::CHANGING);
// }

ECode ListsFragment::GetSpeedDialFragment(
    /* [out] */ ISpeedDialFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    assert(0 && "TODO");
    // *fragment = mSpeedDialFragment;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode ListsFragment::GetRemoveView(
    /* [out] */ IRemoveView** removeView)
{
    VALIDATE_NOT_NULL(removeView);
    assert(0 && "TODO");
    // *removeView = mRemoveView;
    REFCOUNT_ADD(*removeView);
    return NOERROR;
}

ECode ListsFragment::GetRtlPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    assert(0 && "TODO");
    // if (DialerUtils::IsRtl()) {
    //     *result = TAB_INDEX_COUNT - 1 - position;
    //     return NOERROR;
    // }
    *result = position;
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
