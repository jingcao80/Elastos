//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/CDessertCaseDream.h"
#include "elastos/droid/systemui/CDessertCaseView.h"

using Elastos::Droid::Service::Dreams::EIID_IDreamService;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CAR_OBJECT_IMPL(CDessertCaseDream)
CAR_INTERFACE_IMPL(CDessertCaseDream, DreamService, IDessertCaseDream)
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
} // namepsace Droid
} // namespace Elastos