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

#ifndef __ELASTOS_DROID_GRAPHICS_INSETS_H__
#define __ELASTOS_DROID_GRAPHICS_INSETS_H__

#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class Insets
    : public Object
    , public IInsets
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Return an Insets instance with the appropriate values.
     *
     * @param left the left inset
     * @param top the top inset
     * @param right the right inset
     * @param bottom the bottom inset
     *
     * @return Insets instance with the appropriate values
     */
    static CARAPI_(AutoPtr<Insets>) Of(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Return an Insets instance with the appropriate values.
     *
     * @param r the rectangle from which to take the values
     *
     * @return an Insets instance with the appropriate values
     */
    static CARAPI_(AutoPtr<Insets>) Of(
        /* [in] */ IRect* r);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* e);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hc);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetLeft(
        /* [out] */ Int32* left);

    CARAPI GetTop(
        /* [out] */ Int32* top);

    CARAPI GetRight(
        /* [out] */ Int32* right);

    CARAPI GetBottom(
        /* [out] */ Int32* bottom);

private:
    Insets(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

public:
    static const AutoPtr<Insets> NONE;

    const Int32 mLeft;
    const Int32 mTop;
    const Int32 mRight;
    const Int32 mBottom;
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_INSETS_H__
