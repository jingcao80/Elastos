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

#include "CNameHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_SINGLETON_IMPL(CNameHelper)

CAR_INTERFACE_IMPL(CNameHelper, Singleton, INameHelper)

ECode CNameHelper::GetASN1_RDN(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1SetOf ** ppAsn1_rdn)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameHelper::SetASN1_RDN(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1SetOf * pAsn1_rdn)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameHelper::SetASN1(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf * pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

