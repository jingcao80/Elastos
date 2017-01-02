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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1_MESSAGEDIGESTIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1_MESSAGEDIGESTIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CSHA1_MessageDigestImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CSHA1_MessageDigestImpl)
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface ** ppObject);

    CARAPI EngineGetDigestLength(
        /* [out] */ Int32 * pLength);

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdateEx(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineUpdateEx2(
        /* [in] */ Elastos::IO::IByteBuffer * pInput);

    CARAPI EngineDigest(
        /* [out, callee] */ ArrayOf<Byte> ** ppDigest);

    CARAPI EngineDigestEx(
        /* [in] */ ArrayOf<Byte> * pBuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * pDigest);

    CARAPI EngineReset();

    CARAPI constructor();

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1_MESSAGEDIGESTIMPL_H__
