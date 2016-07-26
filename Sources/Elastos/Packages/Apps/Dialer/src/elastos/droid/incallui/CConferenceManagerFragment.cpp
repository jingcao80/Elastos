
#include "elastos/droid/incallui/CConferenceManagerFragment.h"
#include "elastos/droid/incallui/ConferenceManagerPresenter.h"

using Elastos::Droid::View::EIID_IViewOnClickListener;

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
    presenter->EndConferenceConnection(rowId);
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
    presenter->SeparateConferenceConnection(rowId);
    return NOERROR;
}


//================================================================
// CConferenceManagerFragment
//================================================================
CAR_INTERFACE_IMPL_2(CConferenceManagerFragment, BaseFragment, IConferenceManagerUi, IUi);

CAR_OBJECT_IMPL(CConferenceManagerFragment);

CConferenceManagerFragment::CConferenceManagerFragment()
    : mActionBarElevation(0)
{}

ECode CConferenceManagerFragment::constructor()
{
    return NOERROR;
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
        presenter->Init(activity, calls);
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
    AutoPtr<IView> v;
    IView::Probe((*mConferenceCallList)[rowId])->FindViewById(R::id::callerPhoto, (IView**)&v);
    AutoPtr<IImageView> photoView = IImageView::Probe(v);

    final ImageView photoView = (ImageView) mConferenceCallList[rowId].findViewById(
            R.id.callerPhoto);
    final TextView nameTextView = (TextView) mConferenceCallList[rowId].findViewById(
            R.id.conferenceCallerName);
    final TextView numberTextView = (TextView) mConferenceCallList[rowId].findViewById(
            R.id.conferenceCallerNumber);
    final TextView numberTypeTextView = (TextView) mConferenceCallList[rowId].findViewById(
            R.id.conferenceCallerNumberType);

    DefaultImageRequest imageRequest = (photoUri != null) ? null :
            new DefaultImageRequest(callerName, lookupKey, true /* isCircularPhoto */);
    mContactPhotoManager.loadDirectoryPhoto(photoView, photoUri, false, true, imageRequest);

    // set the caller name
    nameTextView.setText(callerName);

    // set the caller number in subscript, or make the field disappear.
    if (TextUtils.isEmpty(callerNumber)) {
        numberTextView.setVisibility(View.GONE);
        numberTypeTextView.setVisibility(View.GONE);
    } else {
        numberTextView.setVisibility(View.VISIBLE);
        numberTextView.setText(callerNumber);
        numberTypeTextView.setVisibility(View.VISIBLE);
        numberTypeTextView.setText(callerNumberType);
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
