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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/gesture/GestureStroke.h"
#include "elastos/droid/gesture/CGestureStroke.h"
#include "elastos/droid/gesture/GestureUtils.h"
#include "elastos/droid/gesture/CGesturePoint.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CPath.h"
#include <elastos/core/Math.h>

using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::IPath;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL_2(GestureStroke, Object, IGestureStroke, IGestureStrokeHelper);


const Float GestureStroke::TOUCH_TOLERANCE = 3;

GestureStroke::GestureStroke()
    : mLength(0)
{}

GestureStroke::~GestureStroke()
{}


ECode GestureStroke::constructor(
    /* [in] */ IArrayList *points)
{
    Int32 count;
    points->GetSize(&count);
    AutoPtr<ArrayOf<Float> > tmpPoints = ArrayOf<Float>::Alloc(count * 2);
    AutoPtr<ArrayOf<Int64> > times = ArrayOf<Int64>::Alloc(count);

    AutoPtr<IRectF> bx;
    Float len = 0;
    Int32 index = 0;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        points->Get(i, (IInterface**)&obj);
        IGesturePoint* p = IGesturePoint::Probe(obj);
        p->GetX(&(*tmpPoints)[i * 2]);
        p->GetY(&(*tmpPoints)[i * 2 + 1]);
        p->GetTimestamp(&(*times)[index]);

        Float x, y;
        p->GetX(&x);
        p->GetY(&y);
        if (bx == NULL) {
            CRectF::New(y, x, x, y, (IRectF**)&bx);
            len = 0;
        } else {
            len += Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(x - (*tmpPoints)[(i - 1) * 2], 2)
                    + Elastos::Core::Math::Pow(y - (*tmpPoints)[(i -1 ) * 2 + 1], 2));
            bx->Union(x, y);
        }
        index++;
        i++;
    }

    mTimestamps = times;
    mPoints = tmpPoints;
    mBoundingBox = bx;
    mLength = len;
    return NOERROR;
}

GestureStroke::GestureStroke(
    /* [in] */ IRectF *bbx,
    /* [in] */ Float len,
    /* [in] */ ArrayOf<Float> *pts,
    /* [in] */ ArrayOf<Int64> *times)
    : mLength(len)
{

    Float left, top, right, bottom;

    bbx->Get(&left, &top, &right, &bottom);
    CRectF::New(left, top, right, bottom, (IRectF**)&mBoundingBox);

    mPoints = pts->Clone();
    mTimestamps = times->Clone();
}

ECode GestureStroke::Draw(
    /* [in] */ ICanvas *canvas,
    /* [in] */ IPaint *paint)
{
    if (mCachedPath == NULL) {
        MakePath();
    }

    canvas->DrawPath(mCachedPath, paint);
    return NOERROR;
}

ECode GestureStroke::GetPath(
    /* [out] */ IPath **outPath)
{
    VALIDATE_NOT_NULL(outPath);
    if (mCachedPath == NULL) {
        MakePath();
    }
    *outPath = mCachedPath;
    REFCOUNT_ADD(*outPath);
    return NOERROR;
}

void GestureStroke::MakePath()
{
    AutoPtr<ArrayOf<Float> > localPoints = mPoints;
    const Int32 count = localPoints->GetLength();

    AutoPtr<IPath> path;

    Float mX = 0;
    Float mY = 0;

    for (Int32 i = 0; i < count; i += 2) {
        Float x = (*localPoints)[i];
        Float y = (*localPoints)[i + 1];
        if (path == NULL) {
            CPath::New((IPath**)&path);
            path->MoveTo(x, y);
            mX = x;
            mY = y;
        } else {
            Float dx = Elastos::Core::Math::Abs(x - mX);
            Float dy = Elastos::Core::Math::Abs(y - mY);
            if (dx >= TOUCH_TOLERANCE || dy >= TOUCH_TOLERANCE) {
                path->QuadTo(mX, mY, (x + mX) / 2, (y + mY) / 2);
                mX = x;
                mY = y;
            }
        }
    }

    mCachedPath = path;
}

ECode GestureStroke::ToPath(
    /* [in] */ Float width,
    /* [in] */ Float height,
    /* [in] */ Int32 numSample,
    /* [out] */ IPath **outPath)
{
    VALIDATE_NOT_NULL(outPath);

    AutoPtr<IGestureStroke> stroke = this;
    AutoPtr<ArrayOf<Float> > pts = GestureUtils::TemporalSampling(stroke, numSample);
    AutoPtr<IRectF> rect = mBoundingBox;

    Float recL, recT;
    rect->GetLeft(&recL);
    rect->GetTop(&recT);
    GestureUtils::Translate(pts, -recL, -recT);

    Float recW, recH;
    rect->GetWidth(&recW);
    rect->GetHeight(&recH);
    Float sx = width / recW;
    Float sy = height / recH;
    Float scale = sx > sy ? sy : sx;
    GestureUtils::Scale(pts, scale, scale);

    Float mX = 0;
    Float mY = 0;

    AutoPtr<IPath> path;

    const Int32 count = pts->GetLength();

    for (Int32 i = 0; i < count; i += 2) {
        Float x = (*pts)[i];
        Float y = (*pts)[i + 1];
        if (path == NULL) {
            CPath::New((IPath**)&path);
            path->MoveTo(x, y);
            mX = x;
            mY = y;
        } else {
            Float dx = Elastos::Core::Math::Abs(x - mX);
            Float dy = Elastos::Core::Math::Abs(y - mY);
            if (dx >= TOUCH_TOLERANCE || dy >= TOUCH_TOLERANCE) {
                path->QuadTo(mX, mY, (x + mX) / 2, (y + mY) / 2);
                mX = x;
                mY = y;
            }
        }
    }
    *outPath = path;
    REFCOUNT_ADD(*outPath);
    return NOERROR;
}

ECode GestureStroke::Serialize(
    /* [in] */ IDataOutputStream *out)
{
    AutoPtr<ArrayOf<Float> >pts = mPoints.Get();
    AutoPtr<ArrayOf<Int64> > times = mTimestamps.Get();
    const Int32 count = mPoints->GetLength();

    // Write number of points
    IDataOutput::Probe(out)->WriteInt32(count / 2);

    for (Int32 i = 0; i < count; i += 2) {
        // Write X
        IDataOutput::Probe(out)->WriteFloat((*pts)[i]);
        // Write Y
        IDataOutput::Probe(out)->WriteFloat((*pts)[i + 1]);
        // Write timestamp
        IDataOutput::Probe(out)->WriteInt64((*times)[i / 2]);
    }
    return NOERROR;
}

ECode GestureStroke::Deserialize(
    /* [in] */ IDataInputStream *in,
    /* [out] */ IGestureStroke **stroke)
{
    VALIDATE_NOT_NULL(stroke);

    // Number of points
    Int32 count = 0;
    IDataInput::Probe(in)->ReadInt32(&count);

    AutoPtr<IArrayList> points;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IGesturePoint> point = CGesturePoint::Deserialize(in);
        points->Add(point);
    }
    CGestureStroke::New(points, stroke);
    return NOERROR;
}

ECode GestureStroke::ClearPath()
{
    if (mCachedPath != NULL) {
        mCachedPath->Rewind();
    }
    return NOERROR;
}

ECode GestureStroke::ComputeOrientedBoundingBox(
    /* [out] */ IOrientedBoundingBox **box)
{
    VALIDATE_NOT_NULL(box);
    AutoPtr<IOrientedBoundingBox> temp = GestureUtils::ComputeOrientedBoundingBox(mPoints);
    *box = temp;
    REFCOUNT_ADD(*box);
    return NOERROR;
}

ECode GestureStroke::GetBoundingBox(
    /* [out] */ IRectF **box)
{
    VALIDATE_NOT_NULL(box);
    *box = mBoundingBox;
    REFCOUNT_ADD(*box);
    return NOERROR;
}

ECode GestureStroke::GetLength(
    /* [out] */ Float *length)
{
    VALIDATE_NOT_NULL(length);

    *length = mLength;
    return NOERROR;
}

ECode GestureStroke::GetPoints(
    /* [out, callee] */ ArrayOf<Float> **points)
{
    VALIDATE_NOT_NULL(points);
    *points = mPoints;
    REFCOUNT_ADD(*points);
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
