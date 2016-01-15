
#include "wm/CWindowId.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CAR_INTERFACE_IMPL(CWindowId, Object, IIWindowId)

CAR_OBJECT_IMPL(CWindowId)

ECode CWindowId::RegisterFocusObserver(
    /* [in] */ IIWindowFocusObserver* observer)
{
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    ((WindowState*)iowner.Get())->RegisterFocusObserver(observer);
    return NOERROR;
}

ECode CWindowId::UnregisterFocusObserver(
    /* [in] */ IIWindowFocusObserver* observer)
{
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    ((WindowState*)iowner.Get())->UnregisterFocusObserver(observer);
    return NOERROR;
}

ECode CWindowId::IsFocused(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    *result = ((WindowState*)iowner.Get())->IsFocused();
    return NOERROR;
}

ECode CWindowId::constructor(
    /* [in] */ IWindowState* owner)
{
    IWeakReferenceSource::Probe(owner)->GetWeakReference((IWeakReference**)&mWeakOwner);
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
