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

#ifndef __ELASTOS_DROID_GRAPHICS_CATLASENTRY_H__
#define __ELASTOS_DROID_GRAPHICS_CATLASENTRY_H__

#include "_Elastos_Droid_Graphics_CAtlasEntry.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CAtlasEntry)
    , public Object
    , public IAtlasEntry
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CAtlasEntry();

    CARAPI constructor();

    CARAPI SetX(
        /* [in] */ Int32 value);

    CARAPI GetX(
        /* [out] */ Int32* value);

    CARAPI SetY(
        /* [in] */ Int32 value);

    CARAPI GetY(
        /* [out] */ Int32* value);

    CARAPI SetRotated(
        /* [in] */ Boolean value);

    CARAPI GetRotated(
        /* [out] */ Boolean* value);

private:
    /**
     * Location, in pixels, of the bitmap on the X axis in the atlas.
     */
    Int32 mX;
    /**
     * Location, in pixels, of the bitmap on the Y axis in the atlas.
     */
    Int32 mY;

    /**
     * If true, the bitmap must be rotated 90 degrees in the atlas.
     */
    Boolean mRotated;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CATLASENTRY_H__
