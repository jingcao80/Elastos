
#include "elastos/droid/dialer/list/SearchFragment.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "elastos/droid/dialer/list/DialerPhoneNumberListAdapter.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/dialer/DialtactsActivity.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include "R.h"

using Elastos::Droid::Common::Widget::ICompositeCursorAdapter;
using Elastos::Droid::Contacts::Common::List::ContactListItemView;
using Elastos::Droid::Contacts::Common::Util::ViewUtil;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::DialtactsActivity;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Dialer::List::EIID_ISearchFragment;
using Elastos::Droid::Phone::Common::Animation::AnimUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::EIID_IAbsListViewOnScrollListener;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// SearchFragment::MyOnScrollListener
//=================================================================
CAR_INTERFACE_IMPL(SearchFragment::MyOnScrollListener, Object, IAbsListViewOnScrollListener);

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
SearchFragment::SearchFragment()
    : mActionBarHeight(0)
    , mShadowHeight(0)
    , mPaddingTop(0)
    , mShowDialpadDuration(0)
    , mHideDialpadDuration(0)
{}

CAR_INTERFACE_IMPL(SearchFragment, PhoneNumberPickerFragment, ISearchFragment);

ECode SearchFragment::constructor()
{
    return PhoneNumberPickerFragment::constructor();
}

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
        Logger::E("SearchFragment", "%s must implement OnListFragmentScrolledListener", str.string());
        return E_CLASS_CAST_EXCEPTION;
    }
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement OnListFragmentScrolledListener");
    // }
    return NOERROR;
}

ECode SearchFragment::OnStart()
{
    PhoneNumberPickerFragment::OnStart();
    Boolean result;
    if (IsSearchMode(&result), result) {
        AutoPtr<IInterface> adpater;
        GetAdapter((IInterface**)&adpater);
        ICompositeCursorAdapter::Probe(adpater)->SetHasHeader(0, FALSE);
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mActivity = ISearchFragmentHostInterface::Probe(activity);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    mActivity->GetActionBarHeight(&mActionBarHeight);
    AutoPtr<IDrawable> drawable;
    res->GetDrawable(Elastos::Droid::Dialer::R::drawable::search_shadow, (IDrawable**)&drawable);
    drawable->GetIntrinsicHeight(&mShadowHeight);
    res->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::search_list_padding_top, &mPaddingTop);
    res->GetInteger(Elastos::Droid::Dialer::R::integer::dialpad_slide_in_duration, &mShowDialpadDuration);
    res->GetInteger(Elastos::Droid::Dialer::R::integer::dialpad_slide_out_duration, &mHideDialpadDuration);

    AutoPtr<IView> parentView;
    GetView((IView**)&parentView);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);

    Int32 color;
    res->GetColor(Elastos::Droid::Dialer::R::color::background_dialer_results, &color);
    IView::Probe(listView)->SetBackgroundColor(color);
    IViewGroup::Probe(listView)->SetClipToPadding(FALSE);
    SetVisibleScrollbarEnabled(FALSE);
    AutoPtr<IAbsListViewOnScrollListener> listener = (IAbsListViewOnScrollListener*)new MyOnScrollListener(this);
    IAbsListView::Probe(listView)->SetOnScrollListener(listener);

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
    ViewUtil::AddBottomPaddingToListViewForFab(listView, res);
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
        ICompositeCursorAdapter::Probe(adpater)->SetHasHeader(0, FALSE);
    }
    return NOERROR;
}

ECode SearchFragment::SetAddToContactNumber(
    /* [in] */ const String& addToContactNumber)
{
    mAddToContactNumber = addToContactNumber;
    return NOERROR;
}

AutoPtr<IContactEntryListAdapter> SearchFragment::CreateListAdapter()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<DialerPhoneNumberListAdapter> adapter = new DialerPhoneNumberListAdapter();
    adapter->constructor(IContext::Probe(activity));
    adapter->SetDisplayPhotos(TRUE);
    adapter->SetUseCallableUri(PhoneNumberPickerFragment::UsesCallableUri());
    return IContactEntryListAdapter::Probe(adapter);
}

void SearchFragment::OnItemClick(
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> adapter;
    GetAdapter((IInterface**)&adapter);
    AutoPtr<IDialerPhoneNumberListAdapter> listAdapter = IDialerPhoneNumberListAdapter::Probe(adapter);
    Int32 shortcutType;
    listAdapter->GetShortcutTypeFromPosition(position, &shortcutType);
    AutoPtr<IOnPhoneNumberPickerActionListener> listener;

    switch (shortcutType) {
        case IDialerPhoneNumberListAdapter::SHORTCUT_INVALID:
            PhoneNumberPickerFragment::OnItemClick(position, id);
            break;
        case IDialerPhoneNumberListAdapter::SHORTCUT_DIRECT_CALL:
            listener = GetOnPhoneNumberPickerListener();
            if (listener != NULL) {
                String str;
                GetQueryString(&str);
                listener->OnCallNumberDirectly(str);
            }
            break;
        case IDialerPhoneNumberListAdapter::SHORTCUT_ADD_NUMBER_TO_CONTACTS:{
            String number;
            if (TextUtils::IsEmpty(mAddToContactNumber)) {
                listAdapter->GetFormattedQueryString(&number);
            }
            else {
                number = mAddToContactNumber;
            }

            AutoPtr<ICharSequence> cs;
            CString::New(number, (ICharSequence**)&cs);
            AutoPtr<IIntent> intent = DialtactsActivity::GetAddNumberToContactIntent(cs);
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            DialerUtils::StartActivityWithErrorToast(IContext::Probe(activity), intent,
                    Elastos::Droid::Dialer::R::string::add_contact_not_available);
            break;
        }
        case IDialerPhoneNumberListAdapter::SHORTCUT_MAKE_VIDEO_CALL:
            listener = GetOnPhoneNumberPickerListener();
            if (listener != NULL) {
                String str;
                GetQueryString(&str);
                listener->OnCallNumberDirectly(str, TRUE /* isVideoCall */);
            }
            break;
    }
}

ECode SearchFragment::UpdatePosition(
    /* [in] */ Boolean animate)
{
    // Use negative shadow height instead of 0 to account for the 9-patch's shadow.
    Boolean result;
    Int32 startTranslationValue =
            (mActivity->IsDialpadShown(&result), result) ?
            mActionBarHeight - mShadowHeight: -mShadowHeight;
    Int32 endTranslationValue = 0;
    // Prevents ListView from being translated down after a rotation when the ActionBar is up.
    if (animate || mActivity->IsActionBarShowing(&result), result) {
        endTranslationValue =
                (mActivity->IsDialpadShown(&result), result) ? 0 : mActionBarHeight -mShadowHeight;
    }
    if (animate) {
        AutoPtr<IInterpolator> interpolator =
                (mActivity->IsDialpadShown(&result), result) ? AnimUtils::EASE_IN : AnimUtils::EASE_OUT ;
        Int32 duration =
                (mActivity->IsDialpadShown(&result), result) ? mShowDialpadDuration : mHideDialpadDuration;

        AutoPtr<IView> view;
        GetView((IView**)&view);
        view->SetTranslationY(startTranslationValue);
        AutoPtr<IViewPropertyAnimator> animator;
        view->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationY(endTranslationValue);
        animator->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        animator->SetDuration(duration);
    }
    else {
        AutoPtr<IView> view;
        GetView((IView**)&view);
        view->SetTranslationY(endTranslationValue);
    }

    // There is padding which should only be applied when the dialpad is not shown.
    Int32 paddingTop = (mActivity->IsDialpadShown(&result), result) ? 0 : mPaddingTop;
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IView> v = IView::Probe(listView);
    Int32 start, end, bottom;
    v->GetPaddingStart(&start);
    v->GetPaddingEnd(&end);
    v->GetPaddingBottom(&bottom);
    v->SetPaddingRelative(start, paddingTop, end, bottom);

    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
