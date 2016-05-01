
#ifdef DROID_CORE
#include "elastos/droid/inputmethodservice/AbstractInputMethodImpl.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"
#else
#include "elastos/droid/inputmethodservice/AbstractInputMethodImpl.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"
#endif

using Elastos::Droid::View::InputMethod::EIID_IInputMethod;
using Elastos::Droid::View::InputMethod::EIID_IInputMethodSession;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_INTERFACE_IMPL_2(AbstractInputMethodImpl, Object, IAbstractInputMethodImpl, IInputMethod);

AbstractInputMethodImpl::AbstractInputMethodImpl(
    /* [in] */ AbstractInputMethodService* host)
    : mHost(host)
{}

ECode AbstractInputMethodImpl::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

ECode AbstractInputMethodImpl::CreateSession(
    /* [in] */ IInputMethodSessionCallback* callback)
{
    assert(mHost != NULL);
    AutoPtr<IAbstractInputMethodSessionImpl> impl;
    mHost->OnCreateInputMethodSessionInterface((IAbstractInputMethodSessionImpl**)&impl);
    return callback->SessionCreated(IInputMethodSession::Probe(impl));
}

ECode AbstractInputMethodImpl::SetSessionEnabled(
    /* [in] */ IInputMethodSession* session,
    /* [in] */ Boolean enabled)
{
    return ((AbstractInputMethodSessionImpl*)session)->SetEnabled(enabled);
}

ECode AbstractInputMethodImpl::RevokeSession(
    /* [in] */ IInputMethodSession* session)
{
    return ((AbstractInputMethodSessionImpl*)session)->RevokeSelf();
}

ECode AbstractInputMethodImpl::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Droid.InputMethodService.AbstractInputMethodImpl");
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
