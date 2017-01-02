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

#ifndef __ELASTOS_DROID_UTILITY_CSIZE_H__
#define __ELASTOS_DROID_UTILITY_CSIZE_H__

#include "_Elastos_Droid_Utility_CSize.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CSize)
    , public Object
    , public ISize
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSize();

    CARAPI constructor(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);
    /**
     * Get the width of the size (in pixels).
     * @return width
     */
    CARAPI GetWidth(
        /* [out] */ Int32* value);

    /**
     * Get the height of the size (in pixels).
     * @return height
     */
    CARAPI GetHeight(
        /* [out] */ Int32* value);

    /**
     * Check if this size is equal to another size.
     * <p>
     * Two sizes are equal if and only if both their widths and heights are
     * equal.
     * </p>
     * <p>
     * A size object is never equal to any other type of object.
     * </p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * Return the size represented as a string with the format {@code "WxH"}
     *
     * @return string representation of the size
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * Parses the specified string as a size value.
     * <p>
     * The ASCII characters {@code \}{@code u002a} ('*') and
     * {@code \}{@code u0078} ('x') are recognized as separators between
     * the width and height.</p>
     * <p>
     * For any {@code Size s}: {@code Size.parseSize(s.toString()).equals(s)}.
     * However, the method also handles sizes expressed in the
     * following forms:</p>
     * <p>
     * "<i>width</i>{@code x}<i>height</i>" or
     * "<i>width</i>{@code *}<i>height</i>" {@code => new Size(width, height)},
     * where <i>width</i> and <i>height</i> are string integers potentially
     * containing a sign, such as "-10", "+7" or "5".</p>
     *
     * <pre>{@code
     * Size.parseSize("3*+6").equals(new Size(3, 6)) == true
     * Size.parseSize("-3x-6").equals(new Size(-3, -6)) == true
     * Size.parseSize("4 by 3") => throws NumberFormatException
     * }</pre>
     *
     * @param string the string representation of a size value.
     * @return the size value represented by {@code string}.
     *
     * @throws NumberFormatException if {@code string} cannot be parsed
     * as a size value.
     * @throws NullPointerException if {@code string} was {@code null}
     */
    static CARAPI ParseSize(
        /* [in] */ const String& string,
        /* [out] */ ISize** size);

public:
    Int32 mWidth;
    Int32 mHeight;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CSIZE_H__
