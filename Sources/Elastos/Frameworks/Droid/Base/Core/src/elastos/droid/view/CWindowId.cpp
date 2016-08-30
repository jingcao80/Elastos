
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CWindowId.h"
#include "elastos/droid/view/WindowIdFocusObserver.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CWindowId);

CAR_INTERFACE_IMPL_2(CWindowId, Object, IWindowId, IParcelable);

ECode CWindowId::constructor()
{
    return NOERROR;
}

ECode CWindowId::constructor(
    /* [in] */ IIWindowId* target)
{
    mToken = target;
    return NOERROR;
}

ECode CWindowId::constructor(
    /* [in] */ IBinder* target)
{
    mToken = IIWindowId::Probe(target);
    return NOERROR;
}

ECode CWindowId::IsFocused(
    /* [out] */ Boolean* focused)
{
    VALIDATE_NOT_NULL(focused);
    ECode ec = mToken->IsFocused(focused);
    if (FAILED(ec)) {
        *focused = FALSE;
        return ec;
    }
    return NOERROR;
}

ECode CWindowId::RegisterFocusObserver(
    /* [in] */ IWindowIdFocusObserver* _observer)
{
    WindowIdFocusObserver* observer = (WindowIdFocusObserver*)_observer;
    AutoPtr<IHashMap> map = observer->mRegistrations;
    {    AutoLock syncLock(map);
        Boolean contains = FALSE;
        if (map->ContainsKey(IBinder::Probe(mToken), &contains), contains) {
            // throw new IllegalStateException(
            //         "Focus observer already registered with input token");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        map->Put(IBinder::Probe(mToken), (IWindowId*)this);
        // try {
        mToken->RegisterFocusObserver(observer->mIObserver);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CWindowId::UnregisterFocusObserver(
    /* [in] */ IWindowIdFocusObserver* _observer)
{
    WindowIdFocusObserver* observer = (WindowIdFocusObserver*)_observer;
    AutoPtr<IHashMap> map = observer->mRegistrations;
    {    AutoLock syncLock(map);
        AutoPtr<IInterface> obj;
        map->Remove(IBinder::Probe(mToken), (IInterface**)&obj);
        if (obj.Get() == NULL) {
            // throw new IllegalStateException("Focus observer not registered with input token");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        // try {
        mToken->UnregisterFocusObserver(observer->mIObserver);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CWindowId::Equals(
    /* [in] */ IInterface* otherObj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    IWindowId* wid = IWindowId::Probe(otherObj);
    if (wid) {
        AutoPtr<IBinder> binder = IBinder::Probe(((CWindowId*)wid)->mToken);
        return IObject::Probe(mToken)->Equals(binder, e);
    }
    *e = FALSE;
    return NOERROR;
}

ECode CWindowId::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    return IObject::Probe(mToken)->GetHashCode(code);
}

ECode CWindowId::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb;
    sb.Append("IntentSender{");
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int32 code = 0;
    system->IdentityHashCode(TO_IINTERFACE(this), &code);
    sb.Append(StringUtils::ToHexString(code));
    sb.Append(": ");
    if (mToken != NULL) {
        sb.Append(mToken.Get());
    }
    else {
        sb.Append("NULL");
    }
    sb.AppendChar('}');
    return sb.ToString(str);
}

ECode CWindowId::WriteToParcel(
    /* [in] */ IParcel* out)
{
    return out->WriteInterfacePtr(mToken);
}

ECode CWindowId::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr((Handle32*)&mToken);
}

ECode CWindowId::GetTarget(
    /* [out] */ IIWindowId** id)
{
    VALIDATE_NOT_NULL(id);
    *id = mToken;
    REFCOUNT_ADD(*id);
    return NOERROR;
}


} // namespace View
} // namepsace Droid
} // namespace Elastos
