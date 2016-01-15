
#include "wm/AccessibilityController.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CAR_INTERFACE_IMPL_2(AccessibilityController, Object,
        IAccessibilityManagerAccessibilityStateChangeListener, IAccessibilityManagerTouchExplorationStateChangeListener)

AccessibilityController::AccessibilityController(
    /* [in] */ IContext* context)
    : mAccessibilityEnabled(FALSE)
    , mTouchExplorationEnabled(FALSE)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&service);
    AutoPtr<IAccessibilityManager> am = IAccessibilityManager::Probe(service);
    am->AddTouchExplorationStateChangeListener((IAccessibilityManagerTouchExplorationStateChangeListener*)this);
    am->AddAccessibilityStateChangeListener((IAccessibilityManagerTouchExplorationStateChangeListener*)this);
    am->IsEnabled(&mAccessibilityEnabled);
    am->IsTouchExplorationEnabled(&mTouchExplorationEnabled);
}

Boolean AccessibilityController::IsAccessibilityEnabled()
{
    return mAccessibilityEnabled;
}

Boolean AccessibilityController::IsTouchExplorationEnabled()
{
    return mTouchExplorationEnabled;
}

void AccessibilityController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println("AccessibilityController state:");
    pw->Print("  mAccessibilityEnabled=");
    pw->Println(mAccessibilityEnabled);
    pw->Print("  mTouchExplorationEnabled=");
    pw->Println(mTouchExplorationEnabled);
}

void AccessibilityController::AddStateChangedCallback(
    /* [in] */ IAccessibilityStateChangedCallback* cb)
{
    mChangeCallbacks.PushBack(cb);
    cb->OnStateChanged(mAccessibilityEnabled, mTouchExplorationEnabled);
}

void AccessibilityController::RemoveStateChangedCallback(
    /* [in] */ IAccessibilityStateChangedCallback* cb)
{
    mChangeCallbacks.Erase(cb);
}

void AccessibilityController::FireChanged()
{
    List<AutoPtr<IAccessibilityStateChangedCallback> >::Iterator it = mChangeCallbacks.Beging();
    for (; it != mChangeCallbacks.End(); ++it) {
        (*it)->OnStateChanged(mAccessibilityEnabled, mTouchExplorationEnabled);
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
}

} // Wm
} // Server
} // Droid
} // Elastos
