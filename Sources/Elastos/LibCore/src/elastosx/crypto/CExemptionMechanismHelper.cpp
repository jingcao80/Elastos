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

#include "CExemptionMechanismHelper.h"
#include "CExemptionMechanism.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CExemptionMechanismHelper, Singleton, IExemptionMechanismHelper)

CAR_SINGLETON_IMPL(CExemptionMechanismHelper)

ECode CExemptionMechanismHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IExemptionMechanism ** ExemptionMechanism)
{
	VALIDATE_NOT_NULL(ExemptionMechanism);
	return CExemptionMechanism::GetInstance(algorithm, ExemptionMechanism);
}

ECode CExemptionMechanismHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IExemptionMechanism ** ExemptionMechanism)
{
	VALIDATE_NOT_NULL(ExemptionMechanism);
	return CExemptionMechanism::GetInstance(algorithm, provider, ExemptionMechanism);
}

ECode CExemptionMechanismHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IExemptionMechanism ** ExemptionMechanism)
{
	VALIDATE_NOT_NULL(ExemptionMechanism);
	return CExemptionMechanism::GetInstance(algorithm, provider, ExemptionMechanism);
}

} // Crypto
} // Elastosx