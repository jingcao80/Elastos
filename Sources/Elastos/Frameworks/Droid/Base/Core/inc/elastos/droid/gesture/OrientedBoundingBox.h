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

#ifndef __ELASTOS_DROID_GESTURE_ORIENTEDBOUNDINGBOX_H__
#define __ELASTOS_DROID_GESTURE_ORIENTEDBOUNDINGBOX_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Gesture::IOrientedBoundingBox;
using Elastos::Droid::Graphics::IPath;

namespace Elastos {
namespace Droid {
namespace Gesture {

class OrientedBoundingBox
    : public Object
    , public IOrientedBoundingBox
{
public:
    CAR_INTERFACE_DECL()

    OrientedBoundingBox();

    virtual ~OrientedBoundingBox();

    CARAPI constructor(
    /* [in] */ Float angle,
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Float w,
    /* [in] */ Float h);

	CARAPI ToPath(
	    /* [out] */ IPath **outPath);

	CARAPI GetSquareness(
	    /* [out] */ Float* squareness);

	CARAPI GetWidth(
	    /* [out] */ Float *width);

	CARAPI GetHeight(
	    /* [out] */ Float *height);

	CARAPI GetOrientation(
	    /* [out] */ Float *orientation);

	CARAPI GetCenterX(
	    /* [out] */ Float *centerX);

	CARAPI GetCenterY(
	    /* [out] */ Float *centerY);
private:
    Float mSquareness;
    Float mWidth;
    Float mHeight;
    Float mOrientation;
    Float mCenterX;
    Float mCenterY;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_ORIENTEDBOUNDINGBOX_H__
