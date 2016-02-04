
#include "elastos/droid/packages/systemui/CDessertCaseDream.h"
#include "elastos/droid/packages/systemui/CDessertCaseView.h"

using Elastos::Droid::Service::Dreams::EIID_IDreamService;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

CAR_OBJECT_IMPL(CDessertCaseDream);
CAR_INTERFACE_IMPL(CDessertCaseDream, DreamService, IDessertCaseDream);
ECode CDessertCaseDream::OnAttachedToWindow()
{
    DreamService::OnAttachedToWindow();
    SetInteractive(FALSE);

    CDessertCaseView::New(this, (IDessertCaseView**)&mView);

    mContainer = new CDessertCaseView::RescalingContainer(this);

    IDessertCaseViewRescalingContainer::Probe(mContainer)->SetView(mView);

    SetContentView(IView::Probe(mContainer));
    return NOERROR;
}

ECode CDessertCaseDream::OnDreamingStarted()
{
    DreamService::OnDreamingStarted();
    AutoPtr<Runnable_1> r = new Runnable_1(this);
    Boolean res = FALSE;
    IView::Probe(mView)->PostDelayed(r, 1000, &res);
    return NOERROR;
}

ECode CDessertCaseDream::OnDreamingStopped()
{
    DreamService::OnDreamingStopped();
    mView->Stop();
    return NOERROR;
}

//===============================================================
// CDessertCaseDream::Runnable_1
//===============================================================
CDessertCaseDream::Runnable_1::Runnable_1(
    /* [in] */ CDessertCaseDream* owner)
    : mOwner(owner)
{}

ECode CDessertCaseDream::Runnable_1::Run()
{
    mOwner->mView->Start();
    return NOERROR;
}

} // namespace SystemUI
} // namespace Packages
} // namepsace Droid
} // namespace Elastos