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

#ifndef __ELASTOS_DROID_RENDERSCRIPT_CELEMENTBUILDER_H__
#define __ELASTOS_DROID_RENDERSCRIPT_CELEMENTBUILDER_H__

#include "_Elastos_Droid_Os_CElementBuilder.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace RenderScript {

CarClass(CElementBuilder)
    , public Object
    , public IElementBuilder
{
public:
    CElementBuilder();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IRenderScript* rs);

    /**
     * Add an array of elements to this element.
     *
     * @param element
     * @param name
     * @param arraySize
     */
    CARAPI Add(
        /* [in] */ IElement* element,
        /* [in] */ const String& name,
        /* [in] */ Int32 arraySize,
        /* [out] */ IElementBuilder** builder);

    /**
     * Add a single element to this Element.
     *
     * @param element
     * @param name
     */
    CARAPI Add(
        /* [in] */ IElement* element,
        /* [in] */ const String& name,
        /* [out] */ IElementBuilder** builder);

    /**
     * Create the element from this builder.
     *
     *
     * @return Element
     */
    CARAPI Create(
        /* [out] */ IElement** e);

private:
    AutoPtr<IRenderScript> mRS;
    AutoPtr<ArrayOf<IElement*> > mElements;
    AutoPtr<ArrayOf<String> > mElementNames;
    AutoPtr<ArrayOf<Int32> > mArraySizes;
    Int32 mCount;
    Int32 mSkipPadding;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_RENDERSCRIPT_CELEMENTBUILDER_H__
