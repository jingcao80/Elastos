
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/incallui/CConferenceManagerFragment.h"
#include "elastos/droid/incallui/ConferenceManagerPresenter.h"
#include "R.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Dialer::R;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CConferenceManagerFragment::EndButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(CConferenceManagerFragment::EndButtonClickListener, Object, IViewOnClickListener);

ECode CConferenceManagerFragment::EndButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    ConferenceManagerPresenter* presenter = (ConferenceManagerPresenter*)mHost->GetPresenter().Get();
    presenter->EndConferenceConnection(mRowId);
    return NOERROR;
}


//================================================================
// CConferenceManagerFragment::SeparateButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(CConferenceManagerFragment::SeparateButtonClickListener, Object, IViewOnClickListener);

ECode CConferenceManagerFragment::SeparateButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    ConferenceManagerPresenter* presenter = (ConferenceManagerPresenter*)mHost->GetPresenter().Get();
    presenter->SeparateConferenceConnection(mRowId);
    return NOERROR;
}


//================================================================
// CConferenceManagerFragment
//================================================================
CAR_INTERFACE_IMPL_3(CConferenceManagerFragment, BaseFragment, IConferenceManagerFragment, IConferenceManagerUi, IUi);

CAR_OBJECT_IMPL(CConferenceManagerFragment);

CConferenceManagerFragment::CConferenceManagerFragment()
    : mActionBarElevation(0)
{}

ECode CConferenceManagerFragment::constructor()
{
    return BaseFragment::constructor();
}

AutoPtr<IPresenter> CConferenceManagerFragment::CreatePresenter()
{
    // having a singleton instance.
    AutoPtr<IPresenter> presenter = new ConferenceManagerPresenter();
    return presenter;
}

AutoPtr<IUi> CConferenceManagerFragment::GetUi()
{
    return this;
}

ECode CConferenceManagerFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    return BaseFragment::OnCreate(savedInstanceState);
}

ECode CConferenceManagerFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> parent;
    inflater->Inflate(R::layout::conference_manager_fragment, container,
            FALSE, (IView**)&parent);

    // Create list of conference call widgets
    ConferenceManagerPresenter* presenter = (ConferenceManagerPresenter*)GetPresenter().Get();
    mConferenceCallList = ArrayOf<IViewGroup*>::Alloc(presenter->GetMaxCallersInConference());
    AutoPtr< ArrayOf<Int32> > viewGroupIdList = ArrayOf<Int32>::Alloc(5);
    (*viewGroupIdList)[0] = R::id::caller0;
    (*viewGroupIdList)[1] = R::id::caller1;
    (*viewGroupIdList)[2] = R::id::caller2;
    (*viewGroupIdList)[3] = R::id::caller3;
    (*viewGroupIdList)[4] = R::id::caller4;
    for (Int32 i = 0; i < presenter->GetMaxCallersInConference(); i++) {
        AutoPtr<IView> v;
        parent->FindViewById((*viewGroupIdList)[i], (IView**)&v);
        mConferenceCallList->Set(i, IViewGroup::Probe(v));
    }

    assert(0 && "TODO");
    // mContactPhotoManager =
    //         ContactPhotoManager.getInstance(getActivity().getApplicationContext());

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Float elevation;
    res->GetDimension(R::dimen::incall_action_bar_elevation, &elevation);
    mActionBarElevation = elevation;

    *view = parent;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CConferenceManagerFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    return BaseFragment::OnActivityCreated(savedInstanceState);
}

ECode CConferenceManagerFragment::SetVisible(
    /* [in] */ Boolean on)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IActionBar> actionBar;
    activity->GetActionBar((IActionBar**)&actionBar);

    if (on) {
        actionBar->SetTitle(R::string::manageConferenceLabel);
        actionBar->SetElevation(mActionBarElevation);
        actionBar->SetHideOffset(0);
        actionBar->Show();

        AutoPtr<CallList> calls = CallList::GetInstance();
        ConferenceManagerPresenter* presenter = (ConferenceManagerPresenter*)GetPresenter().Get();
        presenter->Init(IContext::Probe(activity), calls);
        AutoPtr<IView> v;
        GetView((IView**)&v);
        v->SetVisibility(IView::VISIBLE);
    }
    else {
        AutoPtr<IView> v;
        GetView((IView**)&v);
        v->SetVisibility(IView::GONE);

        actionBar->SetElevation(0);
        Int32 height;
        actionBar->GetHeight(&height);
        actionBar->SetHideOffset(height);
    }
    return NOERROR;
}

ECode CConferenceManagerFragment::IsFragmentVisible(
    /* [out] */ Boolean* isVisible)
{
    VALIDATE_NOT_NULL(isVisible);
    return IsVisible(isVisible);
}

ECode CConferenceManagerFragment::SetRowVisible(
    /* [in] */ Int32 rowId,
    /* [in] */ Boolean on)
{
    if (on) {
        IView::Probe((*mConferenceCallList)[rowId])->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe((*mConferenceCallList)[rowId])->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode CConferenceManagerFragment::DisplayCallerInfoForConferenceRow(
    /* [in] */ Int32 rowId,
    /* [in] */ const String& callerName,
    /* [in] */ const String& callerNumber,
    /* [in] */ const String& callerNumberType,
    /* [in] */ const String& lookupKey,
    /* [in] */ IUri* photoUri)
{
    AutoPtr<IView> conferenceView = IView::Probe((*mConferenceCallList)[rowId]);
    AutoPtr<IView> v;
    conferenceView->FindViewById(R::id::callerPhoto, (IView**)&v);
    AutoPtr<IImageView> photoView = IImageView::Probe(v);
    v = NULL;
    conferenceView->FindViewById(R::id::conferenceCallerName, (IView**)&v);
    AutoPtr<ITextView> nameTextView = ITextView::Probe(v);
    v = NULL;
    conferenceView->FindViewById(R::id::conferenceCallerNumber, (IView**)&v);
    AutoPtr<ITextView> numberTextView = ITextView::Probe(v);
    v = NULL;
    conferenceView->FindViewById(R::id::conferenceCallerNumberType, (IView**)&v);
    AutoPtr<ITextView> numberTypeTextView = ITextView::Probe(v);

    assert(0 && "TODO");
    // DefaultImageRequest imageRequest = (photoUri != null) ? null :
    //         new DefaultImageRequest(callerName, lookupKey, true /* isCircularPhoto */);
    // mContactPhotoManager.loadDirectoryPhoto(photoView, photoUri, false, true, imageRequest);

    // set the caller name
    nameTextView->SetText(CoreUtils::Convert(callerName));

    // set the caller number in subscript, or make the field disappear.
    if (TextUtils::IsEmpty(callerNumber)) {
        IView::Probe(numberTextView)->SetVisibility(IView::GONE);
        IView::Probe(numberTypeTextView)->SetVisibility(IView::GONE);
    }
    else {
        IView::Probe(numberTextView)->SetVisibility(IView::VISIBLE);
        numberTextView->SetText(CoreUtils::Convert(callerNumber));
        IView::Probe(numberTypeTextView)->SetVisibility(IView::VISIBLE);
        numberTypeTextView->SetText(CoreUtils::Convert(callerNumberType));
    }
    return NOERROR;
}

ECode CConferenceManagerFragment::SetupEndButtonForRow(
    /* [in] */ Int32 rowId,
    /* [in] */ Boolean canDisconnect)
{
    AutoPtr<IView> endButton;
    IView::Probe((*mConferenceCallList)[rowId])->FindViewById(
            R::id::conferenceCallerDisconnect, (IView**)&endButton);

    // Comment
    if (canDisconnect) {
        endButton->SetOnClickListener(new EndButtonClickListener(rowId, this));
        endButton->SetVisibility(IView::VISIBLE);
    }
    else {
        endButton->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

ECode CConferenceManagerFragment::SetupSeparateButtonForRow(
    /* [in] */ Int32 rowId,
    /* [in] */ Boolean canSeparate)
{
    AutoPtr<IView> separateButton;
    IView::Probe((*mConferenceCallList)[rowId])->FindViewById(
            R::id::conferenceCallerSeparate, (IView**)&separateButton);

    if (canSeparate) {
        separateButton->SetOnClickListener(new SeparateButtonClickListener(rowId, this));
        separateButton->SetVisibility(IView::VISIBLE);
    }
    else {
        separateButton->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
