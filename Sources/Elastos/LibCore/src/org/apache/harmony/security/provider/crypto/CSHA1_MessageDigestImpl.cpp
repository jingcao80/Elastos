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

#include "CSHA1_MessageDigestImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CSHA1_MessageDigestImpl)

ECode CSHA1_MessageDigestImpl::Clone(
    /* [out] */ IInterface ** ppObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineGetDigestLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineUpdate(
    /* [in] */ Byte input)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineUpdateEx(
    /* [in] */ ArrayOf<Byte> * pInput,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineUpdateEx2(
    /* [in] */ Elastos::IO::IByteBuffer * pInput)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineDigest(
    /* [out, callee] */ ArrayOf<Byte> ** ppDigest)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineDigestEx(
    /* [in] */ ArrayOf<Byte> * pBuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * pDigest)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::EngineReset()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1_MessageDigestImpl::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

