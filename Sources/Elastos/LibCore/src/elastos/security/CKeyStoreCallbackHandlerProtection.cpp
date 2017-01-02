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

#include "CKeyStoreCallbackHandlerProtection.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStoreCallbackHandlerProtection)
CAR_INTERFACE_IMPL_2(CKeyStoreCallbackHandlerProtection, Object, IKeyStoreCallbackHandlerProtection, IKeyStoreProtectionParameter)
ECode CKeyStoreCallbackHandlerProtection::GetCallbackHandler(
    /* [out] */ ICallbackHandler **handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = mCallbackHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CKeyStoreCallbackHandlerProtection::constructor(
    /* [in] */ ICallbackHandler *handler)
{
    if (handler == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCallbackHandler = handler;
    return NOERROR;
}

}
}
