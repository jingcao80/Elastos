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

#ifndef __ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__
#define __ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

namespace Org {
namespace Conscrypt {

/**
 * Used to hold onto native OpenSSL references and run finalization on those
 * objects. Individual types must subclass this and implement finalizer.
 */
class OpenSSLNativeReference
    : public Object
    , public IOpenSSLNativeReference
{
public:
    CAR_INTERFACE_DECL();

    OpenSSLNativeReference();

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI GetNativeContext(
        /* [out] */ Int64* ctxRef);

public:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__
