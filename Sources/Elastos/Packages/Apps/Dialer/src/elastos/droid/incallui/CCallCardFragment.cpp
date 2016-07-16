
#include "elastos/droid/incallui/CCallCardFragment.h"
#include "elastos/droid/incallui/CallCardPresenter.h"

using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_INTERFACE_IMPL(CCallCardFragment::LayoutIgnoringListener, Object, IViewOnLayoutChangeListener);

ECode CCallCardFragment::LayoutIgnoringListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    v->SetLeft(oldLeft);
    v->SetRight(oldRight);
    v->SetTop(oldTop);
    v->SetBottom(oldBottom);
    return NOERROR;
}


CAR_INTERFACE_IMPL_2(CCallCardFragment, BaseFragment, ICallCardUi, IUi);

CAR_OBJECT_IMPL(CCallCardFragment);

CCallCardFragment::CCallCardFragment()
    : mRevealAnimationDuration(0)
    , mShrinkAnimationDuration(0)
    , mFabNormalDiameter(0)
    , mFabSmallDiameter(0)
    , mIsLandscape(FALSE)
    , mIsDialpadShowing(FALSE)
    , mFloatingActionButtonVerticalOffset(0)
    , mDensity(0)
    , mTranslationOffset(0)
    , mVideoAnimationDuration(0)
{}

ECode CCallCardFragment::constructor()
{
    return NOERROR;
}

AutoPtr<IUi> CCallCardFragment::GetUi()
{
    return this;
}

AutoPtr<IPresenter> CCallCardFragment::CreatePresenter()
{
    AutoPtr<IPresenter> presenter = new CallCardPresenter();
    return presenter;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
