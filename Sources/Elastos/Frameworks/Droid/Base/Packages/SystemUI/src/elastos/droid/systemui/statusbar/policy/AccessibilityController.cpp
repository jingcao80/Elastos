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

#include "elastos/droid/systemui/statusbar/policy/AccessibilityController.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityManagerTouchExplorationStateChangeListener;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL_3(AccessibilityController, Object, IAccessibilityController \
    , IAccessibilityManagerAccessibilityStateChangeListener
    , IAccessibilityManagerTouchExplorationStateChangeListener);
AccessibilityController::AccessibilityController(
    /* [in] */ IContext* context)
    : mAccessibilityEnabled(FALSE)
    , mTouchExplorationEnabled(FALSE)
{
    CArrayList::New((IArrayList**)&mChangeCallbacks);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IAccessibilityManager> am = IAccessibilityManager::Probe(obj);
    Boolean tmp = FALSE;
    am->AddTouchExplorationStateChangeListener(this, &tmp);
    am->AddAccessibilityStateChangeListener(this, &tmp);
    am->IsEnabled(&mAccessibilityEnabled);
    am->IsTouchExplorationEnabled(&mTouchExplorationEnabled);
}

ECode AccessibilityController::IsAccessibilityEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mAccessibilityEnabled;
    return NOERROR;
}

ECode AccessibilityController::IsTouchExplorationEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mTouchExplorationEnabled;
    return NOERROR;
}

ECode AccessibilityController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("AccessibilityController state:"));
    pw->Print(String("  mAccessibilityEnabled="));
    pw->Println(mAccessibilityEnabled);
    pw->Print(String("  mTouchExplorationEnabled="));
    pw->Println(mTouchExplorationEnabled);
    return NOERROR;
}

ECode AccessibilityController::AddStateChangedCallback(
    /* [in] */ IAccessibilityStateChangedCallback* cb)
{
    mChangeCallbacks->Add(cb);
    cb->OnStateChanged(mAccessibilityEnabled, mTouchExplorationEnabled);
    return NOERROR;
}

ECode AccessibilityController::RemoveStateChangedCallback(
    /* [in] */ IAccessibilityStateChangedCallback* cb)
{
    mChangeCallbacks->Remove(cb);
    return NOERROR;
}

void AccessibilityController::FireChanged()
{
    Int32 N = 0;
    mChangeCallbacks->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mChangeCallbacks->Get(i, (IInterface**)&obj);
        IAccessibilityStateChangedCallback::Probe(obj)->OnStateChanged(mAccessibilityEnabled
                    , mTouchExplorationEnabled);
    }
}

ECode AccessibilityController::OnAccessibilityStateChanged(
    /* [in] */ Boolean enabled)
{
    mAccessibilityEnabled = enabled;
    FireChanged();
    return NOERROR;
}

ECode AccessibilityController::OnTouchExplorationStateChanged(
    /* [in] */ Boolean enabled)
{
    mTouchExplorationEnabled = enabled;
    FireChanged();
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
