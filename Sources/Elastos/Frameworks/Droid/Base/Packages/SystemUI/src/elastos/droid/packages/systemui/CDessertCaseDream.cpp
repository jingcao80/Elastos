
#include "elastos/droid/systemui/CDessertCaseDream.h"

using Elastos::Droid::Service::Dreams::EIID_IDreamService;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {

//===============================================================
// CDessertCaseDream::
//===============================================================
//CAR_INTERFACE_IMPL_2(CDessertCaseDream, Object, IDessertCaseDream, IDreamService)
CAR_INTERFACE_IMPL(CDessertCaseDream, Object, IDessertCaseDream)

ECode CDessertCaseDream::OnAttachedToWindow()
{
    assert(0 && "TODO");
//    DreamService::OnAttachedToWindow();

//    SetInteractive(FALSE);

//    mView = new DessertCaseView(this);

//    mContainer = new DessertCaseView.RescalingContainer(this);

    mContainer->SetView(mView);

//    SetContentView(IView::Probe(mContainer));
    return NOERROR;
}

ECode CDessertCaseDream::OnDreamingStarted()
{
    assert(0 && "TODO");
//    DreamService::OnDreamingStarted();
    AutoPtr<Runnable_1> r = new Runnable_1(this);
    Boolean res = FALSE;
    IView::Probe(mView)->PostDelayed(r, 1000, &res);
    return NOERROR;
}

ECode CDessertCaseDream::OnDreamingStopped()
{
    assert(0 && "TODO");
//    DreamService::OnDreamingStopped();
    mView->Stop();
    return NOERROR;
}

//===============================================================
// CDessertCaseDream::Runnable_1::
//===============================================================

CDessertCaseDream::Runnable_1::Runnable_1(
    /* [in] */ CDessertCaseDream* owner)
{
    mOwner = owner;
}

ECode CDessertCaseDream::Runnable_1::Run()
{
    mOwner->mView->Start();
    return NOERROR;
}

} // namespace SystemUI
} // namepsace Droid
} // namespace Elastos