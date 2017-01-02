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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CPathInterpolator.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/utility/PathParser.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Utility::PathParser;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

const Float CPathInterpolator::PRECISION = 0.002f;

CAR_OBJECT_IMPL(CPathInterpolator);

CAR_INTERFACE_IMPL_3(CPathInterpolator, Object, IPathInterpolator, IInterpolator, ITimeInterpolator);

CPathInterpolator::CPathInterpolator()
{}

CPathInterpolator::~CPathInterpolator()
{}

ECode CPathInterpolator::constructor(
    /* [in] */ IPath* path)
{
    return InitPath(path);
}

ECode CPathInterpolator::constructor(
    /* [in] */ Float controlX,
    /* [in] */ Float controlY)
{
    return InitQuad(controlX, controlY);
}

ECode CPathInterpolator::constructor(
    /* [in] */ Float controlX1,
    /* [in] */ Float controlY1,
    /* [in] */ Float controlX2,
    /* [in] */ Float controlY2)
{
    return InitCubic(controlX1, controlY1, controlX2, controlY2);
}

ECode CPathInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    return constructor(res, theme, attrs);
}

ECode CPathInterpolator::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::PathInterpolator);
    AutoPtr<ITypedArray> a;

    if (theme != NULL) {
        theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    }
    else {
        res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&a);
    }

    ECode ec = ParseInterpolatorFromTypeArray(a);
    a->Recycle();

    if (FAILED(ec)) {
        Slogger::E("CPathInterpolator", "Failed to create CPathInterpolator! ec=%08x", ec);
    }
    return ec;
}

ECode CPathInterpolator::ParseInterpolatorFromTypeArray(
    /* [in] */ ITypedArray* a)
{
    // If there is pathData defined in the xml file, then the controls points
    // will be all coming from pathData.
    Boolean has = FALSE;
    if (a->HasValue(R::styleable::PathInterpolator_pathData, &has), has) {
        String pathData;
        a->GetString(R::styleable::PathInterpolator_pathData, &pathData);
        AutoPtr<IPath> path = PathParser::CreatePathFromPathData(pathData);
        if (path == NULL) {
            Slogger::E("CPathInterpolator", "The path is null, which is created from %s", pathData.string());
            return E_INFLATE_EXCEPTION;
        }

        ECode ec = InitPath(path);
        if (FAILED(ec)) {
            Slogger::E("CPathInterpolator", "Failed to init path: %s", pathData.string());
        }
        return ec;
    }
    else {
        if (a->HasValue(R::styleable::PathInterpolator_controlX1, &has), !has) {
            Slogger::E("CPathInterpolator", "pathInterpolator requires the controlX1 attribute");
            return E_INFLATE_EXCEPTION;
        }
        else if (a->HasValue(R::styleable::PathInterpolator_controlY1, &has), !has) {
            Slogger::E("CPathInterpolator", "pathInterpolator requires the controlY1 attribute");
            return E_INFLATE_EXCEPTION;
        }
        Float x1, y1;
        a->GetFloat(R::styleable::PathInterpolator_controlX1, 0, &x1);
        a->GetFloat(R::styleable::PathInterpolator_controlY1, 0, &y1);

        Boolean hasX2 = FALSE, hasY2 = FALSE;
        a->HasValue(R::styleable::PathInterpolator_controlX2, &hasX2);
        a->HasValue(R::styleable::PathInterpolator_controlY2, &hasY2);

        if (hasX2 != hasY2) {
            Slogger::E("CPathInterpolator", "pathInterpolator requires both controlX2 and controlY2 for cubic Beziers.");
            return E_INFLATE_EXCEPTION;
        }

        if (!hasX2) {
            ECode ec = InitQuad(x1, y1);
            if (FAILED(ec)) {
                Slogger::E("CPathInterpolator", "Failed to init Quad: (%.2f, %.2f)", x1, y1);
            }
            return ec;
        }
        else {
            Float x2, y2;
            a->GetFloat(R::styleable::PathInterpolator_controlX2, 0, &x2);
            a->GetFloat(R::styleable::PathInterpolator_controlY2, 0, &y2);
            ECode ec = InitCubic(x1, y1, x2, y2);
            if (FAILED(ec)) {
                Slogger::E("CPathInterpolator", "Failed to init cubic: (%.2f, %.2f), (%.2f, %.2f)", x1, y1, x2, y2);
            }
            return ec;
        }
    }
    return NOERROR;
}

ECode CPathInterpolator::InitQuad(
    /* [in] */ Float controlX,
    /* [in] */ Float controlY)
{
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    path->MoveTo(0, 0);
    path->QuadTo(controlX, controlY, 1, 1);
    return InitPath(path);
}

ECode CPathInterpolator::InitCubic(
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2)
{
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    path->MoveTo(0, 0);
    path->CubicTo(x1, y1, x2, y2, 1, 1);
    return InitPath(path);
}

ECode CPathInterpolator::InitPath(
    /* [in] */ IPath* path)
{
    AutoPtr< ArrayOf<Float> > pointComponents;
    path->Approximate(PRECISION, (ArrayOf<Float>**)&pointComponents);

    Int32 numPoints = pointComponents->GetLength() / 3;
    if ((*pointComponents)[1] != 0 || (*pointComponents)[2] != 0
            || (*pointComponents)[pointComponents->GetLength() - 2] != 1
            || (*pointComponents)[pointComponents->GetLength() - 1] != 1) {
        Slogger::E("CPathInterpolator", "The Path must start at (0,0) and end at (1,1)");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mX = ArrayOf<Float>::Alloc(numPoints);
    mY = ArrayOf<Float>::Alloc(numPoints);
    Float prevX = 0;
    Float prevFraction = 0;
    Int32 componentIndex = 0;
    for (Int32 i = 0; i < numPoints; i++) {
        Float fraction = (*pointComponents)[componentIndex++];
        Float x = (*pointComponents)[componentIndex++];
        Float y = (*pointComponents)[componentIndex++];
        if (fraction == prevFraction && x != prevX) {
            Slogger::E("CPathInterpolator", "The Path cannot have discontinuity in the X axis.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (x < prevX) {
            Slogger::E("CPathInterpolator", "The Path cannot loop back on itself.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        (*mX)[i] = x;
        (*mY)[i] = y;
        prevX = x;
        prevFraction = fraction;
    }

    return NOERROR;
}

ECode CPathInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation);
    *interpolation = 0;
    if (t <= 0) {
        return NOERROR;
    }
    else if (t >= 1) {
        *interpolation = 1;
        return NOERROR;
    }
    // Do a binary search for the correct x to interpolate between.
    Int32 startIndex = 0;
    Int32 endIndex = mX->GetLength() - 1;

    while (endIndex - startIndex > 1) {
        Int32 midIndex = (startIndex + endIndex) / 2;
        if (t < (*mX)[midIndex]) {
            endIndex = midIndex;
        }
        else {
            startIndex = midIndex;
        }
    }

    Float xRange = (*mX)[endIndex] - (*mX)[startIndex];
    if (xRange == 0) {
        *interpolation = (*mY)[startIndex];
        return NOERROR;
    }

    Float tInRange = t - (*mX)[startIndex];
    Float fraction = tInRange / xRange;

    Float startY = (*mY)[startIndex];
    Float endY = (*mY)[endIndex];
    *interpolation = startY + (fraction * (endY - startY));
    return NOERROR;
}

ECode CPathInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
