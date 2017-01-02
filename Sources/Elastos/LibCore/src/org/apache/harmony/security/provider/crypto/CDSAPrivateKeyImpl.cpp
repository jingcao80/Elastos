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

#include "CDSAPrivateKeyImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CDSAPrivateKeyImpl)

ECode CDSAPrivateKeyImpl::GetAlgorithm(
    /* [out] */ String * pAlgorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::GetFormat(
    /* [out] */ String * pFormat)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::SetAlgorithm(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::SetEncoding(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::GetParams(
    /* [out] */ Elastos::Security::Interfaces::IDSAParams ** ppParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::GetX(
    /* [out] */ Elastos::Math::IBigInteger ** ppX)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::constructor(
    /* [in] */ Elastos::Security::Spec::IDSAPrivateKeySpec * pKeySpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPrivateKeyImpl::constructor(
    /* [in] */ Elastos::Security::Spec::IPKCS8EncodedKeySpec * pKeySpec)
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

