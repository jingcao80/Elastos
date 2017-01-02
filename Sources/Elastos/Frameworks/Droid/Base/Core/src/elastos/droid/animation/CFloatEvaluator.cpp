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

#include "elastos/droid/animation/CFloatEvaluator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::INumber;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_OBJECT_IMPL(CFloatEvaluator)
CAR_INTERFACE_IMPL_2(CFloatEvaluator, Object, IFloatEvaluator, ITypeEvaluator)

CFloatEvaluator::CFloatEvaluator()
{}

CFloatEvaluator::~CFloatEvaluator()
{}

ECode CFloatEvaluator::constructor()
{
    return NOERROR;
}

ECode CFloatEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Float startFloat = 0.0f, endFloat = 0.0f;
    INumber::Probe(startValue)->FloatValue(&startFloat);
    INumber::Probe(endValue)->FloatValue(&endFloat);
    Float retValue = startFloat + fraction * (endFloat - startFloat);

    AutoPtr<IFloat> r;
    CFloat::New(retValue, (IFloat**)&r);
    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
