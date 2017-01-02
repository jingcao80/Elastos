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

#include "elastos/droid/animation/CArgbEvaluator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Animation {

AutoPtr<IArgbEvaluator> CArgbEvaluator::sInstance;

CAR_OBJECT_IMPL(CArgbEvaluator)
CAR_INTERFACE_IMPL_2(CArgbEvaluator, Object, IArgbEvaluator, ITypeEvaluator)

CArgbEvaluator::CArgbEvaluator()
{}

CArgbEvaluator::~CArgbEvaluator()
{}

ECode CArgbEvaluator::constructor()
{
    return NOERROR;
}

AutoPtr<IArgbEvaluator> CArgbEvaluator::GetInstance()
{
    if (sInstance == NULL) {
        CArgbEvaluator::New((IArgbEvaluator**)&sInstance);
    }
    return sInstance;
}

ECode CArgbEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out, callee] */ IInterface** result)
{
    assert(startValue != NULL && IInteger32::Probe(startValue) != NULL);
    assert(endValue != NULL && IInteger32::Probe(endValue) != NULL);

    Int32 startInt = 0;
    IInteger32::Probe(startValue)->GetValue(&startInt);

    Int32 startA = (startInt >> 24) & 0xff;
    Int32 startR = (startInt >> 16) & 0xff;
    Int32 startG = (startInt >> 8) & 0xff;
    Int32 startB = startInt & 0xff;

    Int32 endInt = 0;
    IInteger32::Probe(endValue)->GetValue(&endInt);

    Int32 endA = (endInt >> 24) & 0xff;
    Int32 endR = (endInt >> 16) & 0xff;
    Int32 endG = (endInt >> 8) & 0xff;
    Int32 endB = endInt & 0xff;

    Int32 retValue = (Int32)((startA + (Int32)(fraction * (endA - startA))) << 24) |
            (Int32)((startR + (Int32)(fraction * (endR - startR))) << 16) |
            (Int32)((startG + (Int32)(fraction * (endG - startG))) << 8) |
            (Int32)((startB + (Int32)(fraction * (endB - startB))));

    AutoPtr<IInteger32> r;
    CInteger32::New(retValue, (IInteger32**)&r);
    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
