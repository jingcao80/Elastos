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

#ifndef __ORG_CONSCRYPT_COPENSSLPROVIDER_H__
#define __ORG_CONSCRYPT_COPENSSLPROVIDER_H__

#include "_Org_Conscrypt_COpenSSLProvider.h"
#include "elastos/security/Provider.h"

using Elastos::Security::Provider;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLProvider) , public Provider
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& providerName);

    CAR_OBJECT_DECL()

private:
    using Provider::Put;

    CARAPI Put(
        /* [in] */ const char* key,
        /* [in] */ const char* value);

    CARAPI Put(
        /* [in] */ const char* key,
        /* [in] */ const String& value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

public:
    static const String PROVIDER_NAME;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLPROVIDER_H__
