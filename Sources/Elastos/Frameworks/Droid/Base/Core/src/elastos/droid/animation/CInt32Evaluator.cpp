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

#include "elastos/droid/animation/CInt32Evaluator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_OBJECT_IMPL(CInt32Evaluator)
CAR_INTERFACE_IMPL_2(CInt32Evaluator, Object, IInt32Evaluator, ITypeEvaluator)

CInt32Evaluator::CInt32Evaluator()
{}

CInt32Evaluator::~CInt32Evaluator()
{}

ECode CInt32Evaluator::constructor()
{
    return NOERROR;
}

ECode CInt32Evaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 startInt = 0, endInt = 0;
    IInteger32::Probe(startValue)->GetValue(&startInt);
    IInteger32::Probe(endValue)->GetValue(&endInt);
    Int32 retValue = (Int32)(startInt + fraction * (endInt - startInt));

    AutoPtr<IInteger32> obj;
    CInteger32::New(retValue, (IInteger32**)&obj);
    *result = obj.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
