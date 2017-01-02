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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/gesture/Instance.h"
#include "elastos/droid/gesture/GestureUtils.h"
#include <elastos/core/Math.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Gesture {

static AutoPtr<ArrayOf<Float> > InitORIENTATIONS()
{
    AutoPtr<ArrayOf<Float> > orientations = ArrayOf<Float>::Alloc(10);
    (*orientations)[0] = 0.0;
    (*orientations)[1] = (Float)(Elastos::Core::Math::PI / 4.0);
    (*orientations)[2] = (Float)(Elastos::Core::Math::PI / 2.0);
    (*orientations)[3] = (Float)(Elastos::Core::Math::PI * 3.0 / 4.0);
    (*orientations)[4] = (Float)(Elastos::Core::Math::PI);
    (*orientations)[5] = -0.0;
    (*orientations)[6] = (Float)(-Elastos::Core::Math::PI / 4.0);
    (*orientations)[7] = (Float)(-Elastos::Core::Math::PI / 2.0);
    (*orientations)[8] = (Float)(-Elastos::Core::Math::PI * 3.0 / 4.0);
    (*orientations)[9] = (Float)(-Elastos::Core::Math::PI);
    return orientations;
}

const AutoPtr<ArrayOf<Float> > Instance::ORIENTATIONS = InitORIENTATIONS();

CAR_INTERFACE_IMPL(Instance, Object, IInstance);

Instance::Instance()
{}

Instance::~Instance()
{}

ECode Instance::constructor()
{
    return NOERROR;
}

ECode Instance::constructor(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Float>* sample,
    /* [in] */ const String& sampleName)
{
    mVector = sample;
    mLabel = sampleName;
    mId = id;

    return NOERROR;
}

ECode Instance::Normalize()
{
    AutoPtr<ArrayOf<Float> > sample = mVector;
    Float sum = 0;

    Int32 size = sample->GetLength();
    for (Int32 i = 0; i < size; i++) {
        sum += (*sample)[i] * (*sample)[i];
    }

    Float magnitude = (Float)Elastos::Core::Math::Sqrt(sum);
    for (Int32 i = 0; i < size; i++) {
        (*sample)[i] /= magnitude;
    }

    return NOERROR;
}

AutoPtr<Instance> Instance::CreateInstance(
    /* [in] */ Int32 sequenceType,
    /* [in] */ Int32 orientationType,
    /* [in] */ IGesture* gesture,
    /* [in] */ const String& label)
{
    AutoPtr<ArrayOf<Float> > pts;
    AutoPtr<Instance> instance;
    Int64 id;

    if (sequenceType == IGestureStore::SEQUENCE_SENSITIVE) {
        pts = TemporalSampler(orientationType, gesture);
        gesture->GetID(&id);
        instance = new Instance();
        instance->constructor(id, pts, label);
        instance->Normalize();
    }
    else {
        SpatialSampler(gesture, (ArrayOf<Float>** )&pts);
        gesture->GetID(&id);
        instance = new Instance();
        instance->constructor(id, pts, label);
    }

    return instance;
}

ECode Instance::CreateInstance(
    /* [in] */ Int32 sequenceType,
    /* [in] */ Int32 orientationType,
    /* [in] */ IGesture* gesture,
    /* [in] */ const String& label,
    /* [out] */ IInstance** obj)
{
    AutoPtr<IInstance> p = CreateInstance(sequenceType, orientationType, gesture, label);
    *obj = p;
    REFCOUNT_ADD(*obj)

    return NOERROR;
}

ECode Instance::SpatialSampler(
    /* [in] */ IGesture* gesture,
    /* [out, callee] */ ArrayOf<Float>** sampler)
{
    VALIDATE_NOT_NULL(sampler);
    AutoPtr<ArrayOf<Float> > temp = GestureUtils::SpatialSampling(gesture, PATCH_SAMPLE_SIZE, FALSE);
    *sampler = temp;
    REFCOUNT_ADD(*sampler);
    return NOERROR;
}

AutoPtr<ArrayOf<Float> > Instance::TemporalSampler(
    /* [in] */ Int32 orientationType,
    /* [in] */ IGesture* gesture)
{
    AutoPtr<IArrayList> strokes;
    gesture->GetStrokes((IArrayList**)&strokes);
    AutoPtr<IInterface> obj;
    strokes->Get(0, (IInterface**)&obj);
    IGestureStroke* stroke = IGestureStroke::Probe(obj);

    AutoPtr<ArrayOf<Float> > pts = GestureUtils::TemporalSampling(stroke,
            SEQUENCE_SAMPLE_SIZE);
    AutoPtr<ArrayOf<Float> > center = GestureUtils::ComputeCentroid(pts);
    Float orientation = (Float)Elastos::Core::Math::Atan2((*pts)[1] - (*center)[1], (*pts)[0] - (*center)[0]);

    Float adjustment = -orientation;
    if (orientationType != IGestureStore::ORIENTATION_INVARIANT) {
        Int32 count = ORIENTATIONS->GetLength();
        for (Int32 i = 0; i < count; i++) {
            Float delta = (*ORIENTATIONS)[i] - orientation;
            if (Elastos::Core::Math::Abs(delta) < Elastos::Core::Math::Abs(adjustment)) {
                adjustment = delta;
            }
        }
    }

    GestureUtils::Translate(pts, -(*center)[0], -(*center)[1]);
    GestureUtils::Rotate(pts, adjustment);

    return pts;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
