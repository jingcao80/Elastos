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
