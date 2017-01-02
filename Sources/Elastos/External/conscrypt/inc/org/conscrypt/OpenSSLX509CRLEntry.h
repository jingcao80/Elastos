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

#ifndef __ORG_CONSCRYPT_OPENSSLX509CRLENTRY_H__
#define __ORG_CONSCRYPT_OPENSSLX509CRLENTRY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ISet;
using Elastos::Utility::IDate;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Conscrypt {

class OpenSSLX509CRLEntry
    : public Object // public X509CRLEntry
    , public IOpenSSLX509CRLEntry
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI GetExtensionValue(
        /* [in] */ String oid,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** result);

    CARAPI GetRevocationDate(
        /* [out] */ IDate** result);

    CARAPI HasExtensions(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CRLENTRY_H__