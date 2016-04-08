
#include "CEditActivity.h"
#include "R.h"

using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace MusicPlayer {

//=======================================================================
//              CEditActivity::MyListener
//=======================================================================
CAR_INTERFACE_IMPL(CEditActivity::MyListener, IViewOnClickListener)

CEditActivity::MyListener::MyListener(
    /* [in] */ CEditActivity* host)
    : mActivity(host)
{
}

ECode CEditActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    mActivity->Finish();
    return NOERROR;
}

//=======================================================================
//              CEditActivity
//=======================================================================
ECode CEditActivity::OnCreate(
/* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_edit);

    AutoPtr<IView> imageView = FindViewById(R::edit::image_view);
    assert(imageView != NULL);
    AutoPtr<MyListener> l = new MyListener(this);
    imageView->SetOnClickListener(l);

    return NOERROR;
}

} // namespace MusicPlayer
