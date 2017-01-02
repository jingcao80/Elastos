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

#ifndef __ELASTOS_SECURITY_CKEYSTORECALLBACKHANDLERPROTECTION_H__
#define __ELASTOS_SECURITY_CKEYSTORECALLBACKHANDLERPROTECTION_H__

#include "_Elastos_Security_CKeyStoreCallbackHandlerProtection.h"

using Elastosx::Security::Auth::Callback::ICallbackHandler;

namespace Elastos {
namespace Security {

CarClass(CKeyStoreCallbackHandlerProtection)
    , public Object
    , public IKeyStoreCallbackHandlerProtection
    , public IKeyStoreProtectionParameter
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCallbackHandler(
        /* [out] */ ICallbackHandler **handler);

    CARAPI constructor(
        /* [in] */ ICallbackHandler *handler);

private:
    // Store CallbackHandler
    const AutoPtr<ICallbackHandler> mCallbackHandler;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYSTORECALLBACKHANDLERPROTECTION_H__
