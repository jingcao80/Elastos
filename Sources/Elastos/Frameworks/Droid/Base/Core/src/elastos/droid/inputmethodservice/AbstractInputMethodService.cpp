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
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"
#include "elastos/droid/inputmethodservice/CIInputMethodWrapper.h"
#include "elastos/droid/view/CDispatcherState.h"
#else
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"
#endif

using Elastos::Droid::View::CDispatcherState;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::Internal::View::IIInputMethod;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_INTERFACE_IMPL_2(AbstractInputMethodService, Service, IAbstractInputMethodService, IKeyEventCallback);

AbstractInputMethodService::AbstractInputMethodService()
{
    CDispatcherState::New((IDispatcherState**)&mDispatcherState);
}

ECode AbstractInputMethodService::GetKeyDispatcherState(
    /* [out] */ IDispatcherState** dispatcherState)
{
    assert(dispatcherState != NULL);
    *dispatcherState = mDispatcherState;
    REFCOUNT_ADD(*dispatcherState);

    return NOERROR;
}

ECode AbstractInputMethodService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    assert(binder != NULL);

    if (mInputMethod == NULL) {
        AutoPtr<IAbstractInputMethodImpl> mtdImpl;
        OnCreateInputMethodInterface((IAbstractInputMethodImpl**)&mtdImpl);
        mInputMethod = IInputMethod::Probe(mtdImpl.Get());
    }

    AutoPtr<IIInputMethod> method;
    CIInputMethodWrapper::New(this, mInputMethod, (IIInputMethod**)&method);
    *binder = IBinder::Probe(method);
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

ECode AbstractInputMethodService::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* state)
{
    assert(state != NULL);
    *state = FALSE;

    return NOERROR;
}

ECode AbstractInputMethodService::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* state)
{
    assert(state != NULL);
    *state = FALSE;

    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
