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

#include "elastos/droid/graphics/drawable/shapes/PathShape.h"
#include "elastos/droid/graphics/CPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(PathShape, Shape, IPathShape);
PathShape::PathShape()
    : mStdWidth(0)
    , mStdHeight(0)
    , mScaleX(0)
    , mScaleY(0)
{}

PathShape::PathShape(
    /* [in] */ IPath* path,
    /* [in] */ Float stdWidth,
    /* [in] */ Float stdHeight)
    : mPath(path)
    , mStdWidth(stdWidth)
    , mStdHeight(stdHeight)
{}

ECode PathShape::constructor(
    /* [in] */ IPath* path,
    /* [in] */ Float stdWidth,
    /* [in] */ Float stdHeight)
{
    mPath = path;
    mStdWidth = stdWidth;
    mStdHeight = stdHeight;
    return NOERROR;
}

ECode PathShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    Int32 count;
    canvas->Save(&count);
    canvas->Scale(mScaleX, mScaleY);
    canvas->DrawPath(mPath, paint);
    canvas->Restore();
    return NOERROR;
}

void PathShape::OnResize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    mScaleX = width / mStdWidth;
    mScaleY = height / mStdHeight;
}

ECode PathShape::CloneImpl(
    /* [in] */ IPathShape* _other)
{
    PathShape* other = (PathShape*)_other;
    Shape::CloneImpl(IShape::Probe(other));
    CPath::New(mPath, (IPath**)&other->mPath);
    other->mStdWidth = mStdWidth;
    other->mStdHeight = mStdHeight;
    other->mScaleX = mScaleX;
    other->mScaleY = mScaleY;
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
