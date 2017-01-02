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

#include "elastos/droid/internal/app/AlertActivity.h"
#include "elastos/droid/internal/app/CAlertControllerAlertParams.h"
#include "elastos/droid/internal/app/CAlertController.h"

using Elastos::Droid::Content::EIID_IDialogInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL_2(AlertActivity, Activity, IAlertActivity, IDialogInterface)

ECode AlertActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    CAlertController::New(this, this, GetWindow(), (IAlertController**)&mAlert);
    CAlertControllerAlertParams::New(this, (IAlertControllerAlertParams**)&mAlertParams);
    return NOERROR;
}

ECode AlertActivity::Cancel()
{
    return Finish();
}

ECode AlertActivity::Dismiss()
{
    // This is called after the click, since we finish when handling the
    // click, don't do that again here.
    Boolean finished;
    IsFinishing(&finished);
    if (!finished) {
        Finish();
    }
    return NOERROR;
}

void AlertActivity::SetupAlert()
{
    mAlertParams->Apply(mAlert);
    mAlert->InstallContent();
}

ECode AlertActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* keyDown)
{
    Boolean result;
    mAlert->OnKeyDown(keyCode, event, &result);
    if (result) {
        *keyDown = TRUE;
        return NOERROR;
    }
    return Activity::OnKeyDown(keyCode, event, keyDown);
}

ECode AlertActivity::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* keyUp)
{
    Boolean result;
    mAlert->OnKeyUp(keyCode, event, &result);
    if (result) {
        *keyUp = TRUE;
        return NOERROR;
    }
    return Activity::OnKeyUp(keyCode, event, keyUp);
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
