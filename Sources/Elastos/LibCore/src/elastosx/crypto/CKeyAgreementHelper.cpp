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

#include "CKeyAgreementHelper.h"
#include "CKeyAgreement.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CKeyAgreementHelper, Singleton, IKeyAgreementHelper)

CAR_SINGLETON_IMPL(CKeyAgreementHelper)

ECode CKeyAgreementHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
	VALIDATE_NOT_NULL(keyAgreement);
	return CKeyAgreement::GetInstance(algorithm, keyAgreement);
}

ECode CKeyAgreementHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
	VALIDATE_NOT_NULL(keyAgreement);
	return CKeyAgreement::GetInstance(algorithm, provider, keyAgreement);
}

ECode CKeyAgreementHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
	VALIDATE_NOT_NULL(keyAgreement);
	return CKeyAgreement::GetInstance(algorithm, provider, keyAgreement);
}

} // Crypto
} // Elastosx