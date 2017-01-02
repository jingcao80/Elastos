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

#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"

using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::InputMethod::EIID_IInputMethodSession;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_INTERFACE_IMPL_2(AbstractInputMethodSessionImpl, Object, IAbstractInputMethodSessionImpl, IInputMethodSession);

AbstractInputMethodSessionImpl::AbstractInputMethodSessionImpl(
    /* [in] */ AbstractInputMethodService* host)
    : mEnabled(TRUE)
    , mRevoked(FALSE)
    , mAbstractHost(host)
{}

ECode AbstractInputMethodSessionImpl::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    assert(enabled != NULL);
    *enabled = mEnabled;
    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::IsRevoked(
    /* [out] */ Boolean* revoked)
{
    assert(revoked != NULL);
    *revoked = mRevoked;
    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (!mRevoked) {
        mEnabled = enabled;
    }
    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::RevokeSelf()
{
    mRevoked = TRUE;
    mEnabled = FALSE;
    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::DispatchKeyEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* callback)
{
    assert(event != NULL && mAbstractHost != NULL);

    Boolean handled = FALSE;
    event->Dispatch(mAbstractHost, mAbstractHost->mDispatcherState, (IAbstractInputMethodSessionImpl*)this, &handled);
    if (callback != NULL) {
        return callback->FinishedEvent(seq, handled);
    }
    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::DispatchTrackballEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* callback)
{
    assert(mAbstractHost != NULL);
    Boolean handled = FALSE;
    mAbstractHost->OnTrackballEvent(event, &handled);
    if (callback != NULL) {
        return callback->FinishedEvent(seq, handled);
    }
    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::DispatchGenericMotionEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* callback)
{
    Boolean handled = FALSE;
    mAbstractHost->OnGenericMotionEvent(event, &handled);
    if (callback != NULL) {
        return callback->FinishedEvent(seq, handled);
    }
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
