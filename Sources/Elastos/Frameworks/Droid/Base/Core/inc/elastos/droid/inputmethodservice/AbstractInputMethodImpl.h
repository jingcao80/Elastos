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

#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODIMPL_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODIMPL_H__

#include "Elastos.Droid.InputMethodService.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"

using Elastos::Droid::InputMethodService::AbstractInputMethodService;
using Elastos::Droid::InputMethodService::IAbstractInputMethodImpl;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::View::InputMethod::IInputMethodSessionCallback;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

class ECO_PUBLIC AbstractInputMethodImpl
    : public Object
    , public IAbstractInputMethodImpl
    , public IInputMethod
{
public:
    CAR_INTERFACE_DECL()

    AbstractInputMethodImpl(
        /* [in] */ AbstractInputMethodService* host);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    CARAPI CreateSession(
        /* [in] */ IInputMethodSessionCallback* callback);

    CARAPI SetSessionEnabled(
        /* [in] */ IInputMethodSession* session,
        /* [in] */ Boolean enabled);

    CARAPI RevokeSession(
        /* [in] */ IInputMethodSession* session);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    AbstractInputMethodService* mHost;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODIMPL_H__
