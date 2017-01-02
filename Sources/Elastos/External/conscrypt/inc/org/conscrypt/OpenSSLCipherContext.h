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

#ifndef __ORG_CONSCRYPT_OPENSSLCIPHERCONTEXT_H__
#define __ORG_CONSCRYPT_OPENSSLCIPHERCONTEXT_H__

#include <elastos/coredef.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

namespace Org {
namespace Conscrypt {

class OpenSSLCipherContext
    : public IOpenSSLCipherContext
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLCipherContext();

    ~OpenSSLCipherContext();

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI GetContext(
        /* [out] */ Int64* result);

private:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLCIPHERCONTEXT_H__
