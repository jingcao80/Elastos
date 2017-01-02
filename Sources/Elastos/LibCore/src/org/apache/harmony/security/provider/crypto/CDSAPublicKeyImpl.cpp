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

#include "CDSAPublicKeyImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CDSAPublicKeyImpl)

ECode CDSAPublicKeyImpl::GetAlgorithm(
    /* [out] */ String * pAlgorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::GetFormat(
    /* [out] */ String * pFormat)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::SetAlgorithm(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::SetEncoding(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::GetParams(
    /* [out] */ Elastos::Security::Interfaces::IDSAParams ** ppParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::GetY(
    /* [out] */ Elastos::Math::IBigInteger ** ppY)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::constructor(
    /* [in] */ Elastos::Security::Spec::IDSAPublicKeySpec * pKeySpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAPublicKeyImpl::constructor(
    /* [in] */ Elastos::Security::Spec::IX509EncodedKeySpec * pKeySpec)
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

