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

#include "elastos/droid/animation/PointFEvaluator.h"
#include "elastos/droid/graphics/CPointF.h"

using Elastos::Droid::Graphics::CPointF;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(PointFEvaluator, Object, ITypeEvaluator);
PointFEvaluator::PointFEvaluator()
{
}

PointFEvaluator::PointFEvaluator(
    /* [in] */ IPointF* reuse)
    : mPoint(reuse)
{
}

ECode PointFEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** pf)
{
    VALIDATE_NOT_NULL(pf);
    Float v1 = 0.f, v2 = 0.f;
    IPointF::Probe(startValue)->GetX(&v1);
    IPointF::Probe(endValue)->GetX(&v2);
    Float x = v1 + (fraction * (v2 - v1));

    IPointF::Probe(startValue)->GetY(&v1);
    IPointF::Probe(endValue)->GetY(&v2);
    Float y = v1 + (fraction * (v2 - v1));

    if (mPoint != NULL) {
        mPoint->Set(x, y);
        *pf = mPoint;
        REFCOUNT_ADD(*pf);
        return NOERROR;
    }

    AutoPtr<IPointF> obj;
    CPointF::New(x, y, (IPointF**)&obj);
    *pf = obj.Get();
    REFCOUNT_ADD(*pf)
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
