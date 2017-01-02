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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CInterpolator.h"
#include "SkScalar.h"
#include "SkTemplates.h"
#include "elastos/droid/os/SystemClock.h"
#include <skia/utils/SkInterpolator.h>

using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CInterpolator);
CAR_INTERFACE_IMPL(CInterpolator, Object, IInterpolator);
ECode CInterpolator::constructor(
    /* [in] */ Int32 valueCount)
{
    mValueCount = valueCount;
    mFrameCount = 2;
    mNativeInstance = NativeConstructor(valueCount, 2);
    return NOERROR;
}

ECode CInterpolator::constructor(
    /* [in] */ Int32 valueCount,
    /* [in] */ Int32 frameCount)
{
    mValueCount = valueCount;
    mFrameCount = frameCount;
    mNativeInstance = NativeConstructor(valueCount, frameCount);
    return NOERROR;
}

CInterpolator::~CInterpolator()
{
    NativeDestructor(mNativeInstance);
}

ECode CInterpolator::GetKeyFrameCount(
    /* [out] */ Int32* count)
{
    *count = mFrameCount;
    return NOERROR;
}

ECode CInterpolator::GetValueCount(
    /* [out] */ Int32* count)
{
    *count = mValueCount;
    return NOERROR;
}

ECode CInterpolator::Reset(
    /* [in] */ Int32 valueCount)
{
    Reset(valueCount, 2);
    return NOERROR;
}

ECode CInterpolator::Reset(
    /* [in] */ Int32 valueCount,
    /* [in] */ Int32 frameCount)
{
    mValueCount = valueCount;
    mFrameCount = frameCount;
    NativeReset(mNativeInstance, valueCount, frameCount);
    return NOERROR;
}

ECode CInterpolator::SetKeyFrame(
    /* [in] */ Int32 index,
    /* [in] */ Int32 msec,
    /* [in] */ ArrayOf<Float>* values)
{
    SetKeyFrame(index, msec, values, NULL);
    return NOERROR;
}

ECode CInterpolator::SetKeyFrame(
    /* [in] */ Int32 index,
    /* [in] */ Int32 msec,
    /* [in] */ ArrayOf<Float>* values,
    /* [in] */ ArrayOf<Float>* blendArray)
{
    if (index < 0 || index >= mFrameCount) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (values->GetLength() < mValueCount) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (blendArray != NULL && blendArray->GetLength() < 4) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    NativeSetKeyFrame(mNativeInstance, index, msec, values, blendArray);
    return NOERROR;
}

ECode CInterpolator::SetRepeatMirror(
    /* [in] */ Float repeatCount,
    /* [in] */ Boolean mirror)
{
    if (repeatCount >= 0) {
        NativeSetRepeatMirror(mNativeInstance, repeatCount, mirror);
    }
    return NOERROR;
}

ECode CInterpolator::TimeToValues(
    /* [out] */ ArrayOf<Float>* values,
    /* [out] */ InterpolatorResult* result)
{
    return TimeToValues((Int32)SystemClock::GetUptimeMillis(), values, result);
}

ECode CInterpolator::TimeToValues(
    /* [in] */ Int32 msec,
    /* [out] */ ArrayOf<Float>* values,
    /* [out] */ InterpolatorResult* result)
{
    VALIDATE_NOT_NULL(result);

    if (values != NULL && values->GetLength() < mValueCount) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    switch (NativeTimeToValues(mNativeInstance, msec, values)) {
        case 0: *result = InterpolatorResult_NORMAL;
        case 1: *result = InterpolatorResult_FREEZE_START;
        default: *result = InterpolatorResult_FREEZE_END;
    }
    return NOERROR;
}

Int64 CInterpolator::NativeConstructor(
    /* [in] */ Int32 valueCount,
    /* [in] */ Int32 frameCount)
{
    return reinterpret_cast<Int64>(new SkInterpolator(valueCount, frameCount));
}

void CInterpolator::NativeDestructor(
    /* [in] */ Int64 interpHandle)
{
    SkInterpolator* interp = reinterpret_cast<SkInterpolator*>(interpHandle);
    delete interp;
}

void CInterpolator::NativeReset(
    /* [in] */ Int64 interpHandle,
    /* [in] */ Int32 valueCount,
    /* [in] */ Int32 frameCount)
{
    SkInterpolator* interp = reinterpret_cast<SkInterpolator*>(interpHandle);
    interp->reset(valueCount, frameCount);
}

void CInterpolator::NativeSetKeyFrame(
    /* [in] */ Int64 interpHandle,
    /* [in] */ Int32 index,
    /* [in] */ Int32 msec,
    /* [in] */ ArrayOf<Float>* valueArray,
    /* [in] */ ArrayOf<Float>* blendArray)
{
    SkInterpolator* interp = reinterpret_cast<SkInterpolator*>(interpHandle);

    assert(blendArray == NULL || blendArray->GetLength() >= 4);
#ifdef SK_SCALAR_IS_FLOAT
    SkScalar* scalars = valueArray->GetPayload();
    SkScalar* blend = NULL;
    if (blendArray) {
        blend = blendArray->GetPayload();
    }
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif

    interp->setKeyFrame(index, msec, scalars, blend);
}

void CInterpolator::NativeSetRepeatMirror(
    /* [in] */ Int64 interpHandle,
    /* [in] */ Float repeatCount,
    /* [in] */ Boolean mirror)
{
    SkInterpolator* interp = reinterpret_cast<SkInterpolator*>(interpHandle);
    if (repeatCount > 32000)
        repeatCount = 32000;

    interp->setRepeatCount(repeatCount);
    interp->setMirror(mirror != 0);
}

Int32 CInterpolator::NativeTimeToValues(
    /* [in] */ Int64 interpHandle,
    /* [in] */ Int32 msec,
    /* [in] */ ArrayOf<Float>* valueArray)
{
    SkInterpolator* interp = reinterpret_cast<SkInterpolator*>(interpHandle);
    SkInterpolatorBase::Result result;

    float* values = valueArray ? valueArray->GetPayload() : NULL;
    result = interp->timeToValues(msec, (SkScalar*)values);

    if (valueArray) {
        int n = valueArray->GetLength();
        for (int i = 0; i < n; i++) {
            values[i] = SkScalarToFloat(*(SkScalar*)&values[i]);
        }
    }

    return static_cast<Int32>(result);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
