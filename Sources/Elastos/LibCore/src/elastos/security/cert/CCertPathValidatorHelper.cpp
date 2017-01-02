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

#include "CCertPathValidatorHelper.h"
#include "CertPathValidator.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CCertPathValidatorHelper, Singleton, ICertPathValidatorHelper)
CAR_SINGLETON_IMPL(CCertPathValidatorHelper)
ECode CCertPathValidatorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ICertPathValidator **validator)
{
    return CertPathValidator::GetInstance(algorithm, validator);
}

ECode CCertPathValidatorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ICertPathValidator **validator)
{
    return CertPathValidator::GetInstance(algorithm, provider, validator);
}

ECode CCertPathValidatorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider *provider,
    /* [out] */ ICertPathValidator **validator)
{
    return CertPathValidator::GetInstance(algorithm, provider, validator);
}

ECode CCertPathValidatorHelper::GetDefaultType(
    /* [out] */ String *type)
{
    return CertPathValidator::GetDefaultType(type);
}

}
}
}
