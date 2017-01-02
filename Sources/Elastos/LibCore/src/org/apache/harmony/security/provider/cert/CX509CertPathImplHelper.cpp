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

#include "CX509CertPathImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CAR_SINGLETON_IMPL(CX509CertPathImplHelper)

CAR_INTERFACE_IMPL(CX509CertPathImplHelper, Singleton, IX509CertPathImplHelper)

ECode CX509CertPathImplHelper::GetInstance(
    /* [in] */ Elastos::IO::IInputStream * pIs,
    /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImplHelper::GetInstanceEx(
    /* [in] */ Elastos::IO::IInputStream * pIs,
    /* [in] */ const String& encoding,
    /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImplHelper::GetInstanceEx2(
    /* [in] */ ArrayOf<Byte> * pPath,
    /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImplHelper::GetInstanceEx3(
    /* [in] */ ArrayOf<Byte> * pBytesArray,
    /* [in] */ const String& encoding,
    /* [out] */ Org::Apache::Harmony::Security::Provider::Cert::IX509CertPathImpl ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImplHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf ** ppAsn1)
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

