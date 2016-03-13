
#include "elastos/droid/systemui/statusbar/policy/KeyguardMonitor.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(KeyguardMonitor, Object, IKeyguardMonitor);
KeyguardMonitor::KeyguardMonitor()
    : mShowing(FALSE)
    , mSecure(FALSE)
{
    CArrayList::New((IArrayList**)&mCallbacks);
}

ECode KeyguardMonitor::AddCallback(
    /* [in] */ IKeyguardMonitorCallback* callback)
{
    mCallbacks->Add(callback);
    return NOERROR;
}

ECode KeyguardMonitor::RemoveCallback(
    /* [in] */ IKeyguardMonitorCallback* callback)
{
    mCallbacks->Remove(callback);
    return NOERROR;
}

ECode KeyguardMonitor::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mShowing;
    return NOERROR;
}

ECode KeyguardMonitor::IsSecure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSecure;
    return NOERROR;
}

ECode KeyguardMonitor::NotifyKeyguardState(
    /* [in] */ Boolean showing,
    /* [in] */ Boolean secure)
{
    if (mShowing == showing && mSecure == secure) return NOERROR;
    mShowing = showing;
    mSecure = secure;

    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        IKeyguardMonitorCallback::Probe(callback)->OnKeyguardChanged();
    }
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
