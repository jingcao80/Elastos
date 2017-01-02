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

#ifndef __ELASTOS_DROID_GRAPHICS_CPOINTF_H__
#define __ELASTOS_DROID_GRAPHICS_CPOINTF_H__

#include "_Elastos_Droid_Graphics_CPointF.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * PointF holds two float coordinates
 */
CarClass(CPointF)
    , public Object
    , public IPointF
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI constructor(
        /* [in] */ IPoint* src);

    /**
     * Set the point's x and y coordinates
     */
    CARAPI Set(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI Get(
        /* [out] */ Float* x,
        /* [out] */ Float* y);

    /**
     * Set the point's x and y coordinates to the coordinates of p
     */
    CARAPI Set(
        /* [in] */ IPointF* p);

    CARAPI GetX(
        /* [out] */ Float* x);

    CARAPI GetY(
        /* [out] */ Float* x);

    CARAPI SetX(
        /* [in] */ Float x);

    CARAPI SetY(
        /* [in] */ Float y);

    CARAPI Negate();

    CARAPI Offset(
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    /**
     * Returns true if the point's coordinates equal (x,y)
     */
    CARAPI Equals(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IPointF* x,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* x,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Return the euclidian distance from (0,0) to the point
     */
    CARAPI Length(
        /* [out] */ Float* length);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [out] */ IParcel* dest);

    static CARAPI_(Float) Length(
        /* [in] */ Float x,
        /* [in] */ Float y);

public:
    Float mX;
    Float mY;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPOINTF_H__
