
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
