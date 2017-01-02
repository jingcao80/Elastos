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

#include "CAlgorithmParameterGeneratorHelper.h"
#include "CAlgorithmParameterGenerator.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CAlgorithmParameterGeneratorHelper);
CAR_INTERFACE_IMPL(CAlgorithmParameterGeneratorHelper, Singleton, IAlgorithmParameterGeneratorHelper)
ECode CAlgorithmParameterGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/
{
    return CAlgorithmParameterGenerator::GetInstance(algorithm, generator);
}

ECode CAlgorithmParameterGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    return CAlgorithmParameterGenerator::GetInstance(algorithm, provider, generator);
}

ECode CAlgorithmParameterGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/
{
    return CAlgorithmParameterGenerator::GetInstance(algorithm, provider, generator);
}

} // namespace Security
} // namespace Elastos
