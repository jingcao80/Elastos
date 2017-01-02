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

#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CTRANSFORMATION_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CTRANSFORMATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_Animation_CTransformation.h"
#include "elastos/droid/graphics/CMatrix.h"

using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CTransformation)
    , public Object
    , public ITransformation
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTransformation();

    ~CTransformation();

    CARAPI constructor();

    CARAPI Clear();

    CARAPI GetTransformationType(
        /* [out] */ Int32* type);

    CARAPI SetTransformationType(
        /* [in] */ Int32 transformationType);

    CARAPI Set(
        /* [in] */ ITransformation* t);

    CARAPI Compose(
        /* [in] */ ITransformation* t);

    CARAPI PostCompose(
        /* [in] */ ITransformation* t);

    CARAPI GetMatrix(
        /* [out] */ IMatrix** matrix);

    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    /**
     * Sets the current Transform's clip rect
     * @hide
     */
    CARAPI SetClipRect(
        /* [in] */ IRect* r);

    /**
     * Sets the current Transform's clip rect
     * @hide
     */
    CARAPI SetClipRect(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Returns the current Transform's clip rect
     * @hide
     */
    CARAPI GetClipRect(
        /* [out] */ IRect** cr);

    /**
     * Returns whether the current Transform's clip rect is set
     * @hide
     */
    CARAPI HasClipRect(
        /* [out] */ Boolean* has);

    CARAPI GetAlpha(
        /* [out] */ Float* pAlpha);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Return a string representation of the transformation in a compact form.
     */
    CARAPI ToShortString(
        /* [out] */ String* str);

    /**
     * @hide
     */
    CARAPI ToShortString(
        /* [in] */ IStringBuilder* sb);

    /**
     * Print short string, to optimize dumping.
     * @hide
     */
    CARAPI PrintShortString(
        /* [in] */ IPrintWriter* pw);

protected:
    AutoPtr<IMatrix> mMatrix;
    Float mAlpha;
    Int32 mTransformationType;

private:
    Boolean mHasClipRect;
    AutoPtr<IRect> mClipRect;
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_ANIMATION_CTRANSFORMATION_H__
