
#include "list/SearchFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

//=================================================================
// SearchFragment::MyOnScrollListener
//=================================================================
CAR_INTERFACE_IMPL(SearchFragment::MyOnScrollListener, Object, IListViewOnScrollListener);

SearchFragment::MyOnScrollListener::MyOnScrollListener(
    /* [in] */ SearchFragment* host)
    : mHost(host)
{}

ECode SearchFragment::MyOnScrollListener::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    mHost->mActivityScrollListener->OnListFragmentScrollStateChange(scrollState);
    return NOERROR;
}

ECode SearchFragment::MyOnScrollListener::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    return NOERROR;
}

//=================================================================
// SearchFragment
//=================================================================
CAR_INTERFACE_IMPL(SearchFragment, PhoneNumberPickerFragment, ISearchFragment);

ECode SearchFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    PhoneNumberPickerFragment::OnAttach(activity);

    SetQuickContactEnabled(TRUE);
    SetAdjustSelectionBoundsEnabled(FALSE);
    SetDarkTheme(FALSE);
    SetPhotoPosition(ContactListItemView::GetDefaultPhotoPosition(FALSE /* opposite */));
    SetUseCallableUri(TRUE);
    SendScreenView();

    // try {
    mActivityScrollListener = IOnListFragmentScrolledListener::Probe(activity);
    if (mActivityScrollListener == NULL) {
        String str;
        activity->ToString(&str);
        Logger::E(TAG, "%s must implement OnListFragmentScrolledListener", str.string());
        return E_CLASS_EXCEPTION;
    }
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement OnListFragmentScrolledListener");
    // }
}

ECode SearchFragment::OnStart()
{
    PhoneNumberPickerFragment::onStart();
    Boolean result;
    if (IsSearchMode(&result), result) {
        AutoPtr<IInterface> adpater;
        GetAdapter((IInterface**)&adpater);
        IContactEntryListAdapter::Probe(adpater)->SetHasHeader(0, FALSE);
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mActivity = IHostInterface::Probe(activity);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    mActivity->GetActionBarHeight(&mActionBarHeight);
    AutoPtr<IDrawable> drawable;
    res->GetDrawable(R::drawable::search_shadow, (IDrawable**)&drawable);
    drawable->GetIntrinsicHeight(&mShadowHeight);
    res->GetDimensionPixelSize(R::dimen::search_list_padding_top, &mPaddingTop);
    res->GetInteger(R.integer.dialpad_slide_in_duration, &mShowDialpadDuration);
    res->GetInteger(R.integer.dialpad_slide_out_duration, &mHideDialpadDuration);

    AutoPtr<IView> parentView;
    GetView((IView**)&parentView);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);

    Int32 color;
    res->GetColor(R::color::background_dialer_results, &color)
    listView->SetBackgroundColor(color);
    listView->SetClipToPadding(FALSE);
    SetVisibleScrollbarEnabled(FALSE);
    listView->setOnScrollListener((IListViewOnScrollListener*)new MyOnScrollListener(this));

    UpdatePosition(FALSE /* animate */);

    return NOERROR;
}

ECode SearchFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    PhoneNumberPickerFragment::OnViewCreated(view, savedInstanceState);
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    ViewUtil->AddBottomPaddingToListViewForFab(listView, res);
    return NOERROR;
}

ECode SearchFragment::SetSearchMode(
    /* [in] */ Boolean flag)
{
    PhoneNumberPickerFragment::SetSearchMode(flag);
    // This hides the "All contacts with phone numbers" header in the search fragment
    AutoPtr<IInterface> adpater;
    GetAdapter((IInterface**)&adpater);
    if (IContactEntryListAdapter::Probe(adpater) != NULL) {
        IContactEntryListAdapter::Probe(adpater)->SetHasHeader(0, FALSE);
    }
    return NOERROR;
}

ECode SearchFragment::SetAddToContactNumber(
    /* [in] */ const String& addToContactNumber)
{
    mAddToContactNumber = addToContactNumber;
    return NOERROR;
}

ECode SearchFragment::CreateListAdapter(
    /* [out] */ IContactEntryListAdapter** listAdapter)
{
    VALIDATE_NOT_NULL(listAdapter);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IDialerPhoneNumberListAdapter> adapter;
    CDialerPhoneNumberListAdapter::New(activity, (IDialerPhoneNumberListAdapter**)&adapter);
    adapter->SetDisplayPhotos(TRUE);
    Boolean result;
    PhoneNumberPickerFragment::UsesCallableUri(&result);
    adapter->SetUseCallableUri(result);
    *listAdapter = adapter;
    REFCOUNT_ADD(*listAdapter);
    return NOERROR;
}

ECode SearchFragment::OnItemClick(
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> adpater;
    GetAdapter((IInterface**)&adpater);
    Int32 shortcutType;
    IDialerPhoneNumberListAdapter::Probe(adapter)->
            GetShortcutTypeFromPosition(position, &shortcutType);
    AutoPtr<IOnPhoneNumberPickerActionListener> listener;

    switch (shortcutType) {
        case IDialerPhoneNumberListAdapter::SHORTCUT_INVALID:
            PhoneNumberPickerFragment::OnItemClick(position, id);
            break;
        case IDialerPhoneNumberListAdapter::SHORTCUT_DIRECT_CALL:
             GetOnPhoneNumberPickerListener((IOnPhoneNumberPickerActionListener**)&listener);
            if (listener != NULL) {
                String str;
                GetQueryString(&str);
                listener->OnCallNumberDirectly(str);
            }
            break;
        case IDialerPhoneNumberListAdapter::SHORTCUT_ADD_NUMBER_TO_CONTACTS:
            String number;
            if (TextUtils::IsEmpty(mAddToContactNumber)) {
                IDialerPhoneNumberListAdapter::Probe(adapter)->GetFormattedQueryString(&number);
            }
            else {
                number = mAddToContactNumber;
            }

            AutoPtr<IIntent> intent = DialtactsActivity::GetAddNumberToContactIntent(number);
            AutoPtr<IActivity> activity;
            GetActivity(&activity);
            DialerUtils::StartActivityWithErrorToast(activity, intent,
                    R::string::add_contact_not_available);
            break;
        case IDialerPhoneNumberListAdapter::SHORTCUT_MAKE_VIDEO_CALL:
             GetOnPhoneNumberPickerListener((IOnPhoneNumberPickerActionListener**)&listener);
            if (listener != NULL) {
                String str;
                GetQueryString(&str);
                listener->OnCallNumberDirectly(str, TRUE /* isVideoCall */);
            }
            break;
    }
    return NOERROR;
}

ECode SearchFragment::UpdatePosition(
    /* [in] */ Boolean animate)
{
    // Use negative shadow height instead of 0 to account for the 9-patch's shadow.
    Boolean result;
    Int32 startTranslationValue =
            mActivity->IsDialpadShown(&result), result ?
            mActionBarHeight - mShadowHeight: -mShadowHeight;
    Int32 endTranslationValue = 0;
    // Prevents ListView from being translated down after a rotation when the ActionBar is up.
    if (animate || mActivity->IsActionBarShowing(&result), result) {
        endTranslationValue =
                mActivity->IsDialpadShown(&result), result ? 0 : mActionBarHeight -mShadowHeight;
    }
    if (animate) {
        AutoPtr<IInterpolator> interpolator =
                mActivity->IsDialpadShown(&result), result ? IAnimUtils::EASE_IN : IAnimUtils::EASE_OUT ;
        Int32 duration =
                mActivity.isDialpadShown(&result), result ? mShowDialpadDuration : mHideDialpadDuration;

        AutoPtr<IView> view;
        GetView((IView**)&view);
        view->SetTranslationY(startTranslationValue);
        view->Animate()
        view->TranslationY(endTranslationValue)
        view->SetInterpolator(interpolator)
        view->SetDuration(duration);
    }
    else {
        AutoPtr<IView> view;
        GetView((IView**)&view);
        view->SetTranslationY(endTranslationValue);
    }

    // There is padding which should only be applied when the dialpad is not shown.
    Int32 paddingTop = mActivity->IsDialpadShown(&result), result ? 0 : mPaddingTop;
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    Int32 start, end, bottom;
    listView->GetPaddingStart(&start);
    listView->GetPaddingEnd(&end);bottom
    listView->GetPaddingBottom(&end);
    listView->SetPaddingRelative(start, paddingTop, end, bottom);

    return NOERROR;
}

} // List
} // Dialer
} // Apps
} // Elastos
