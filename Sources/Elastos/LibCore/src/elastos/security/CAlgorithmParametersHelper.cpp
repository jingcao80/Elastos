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

#include "CAlgorithmParametersHelper.h"
#include "AlgorithmParameters.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CAlgorithmParametersHelper);
CAR_INTERFACE_IMPL(CAlgorithmParametersHelper, Singleton, IAlgorithmParametersHelper)

ECode CAlgorithmParametersHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IAlgorithmParameters **result)
{
    return AlgorithmParameters::GetInstance(algorithm, result);
}

ECode CAlgorithmParametersHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IAlgorithmParameters **result)
{
    return AlgorithmParameters::GetInstance(algorithm, provider, result);
}

ECode CAlgorithmParametersHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider *provider,
    /* [out] */ IAlgorithmParameters **result)
{
    return AlgorithmParameters::GetInstance(algorithm, provider, result);
}

} // namespace Security
} // namespace Elastos