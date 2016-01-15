
#ifndef __ELASTOS_TEXT_CANNOTATION_H__
#define __ELASTOS_TEXT_CANNOTATION_H__

#include "_Elastos_Text_CAnnotation.h"
#include "Object.h"


namespace Elastos {
namespace Text {

/**
 * Wrapper for a text attribute value which represents an annotation. An
 * annotation has two special aspects:
 * <ol>
 * <li>it is connected to a range of main text; if this range or the main text
 * is changed then the annotation becomes invalid,</li>
 * <li>it can not be joined with adjacent annotations even if the text attribute
 * value is the same.</li>
 * </ol>
 * <p>
 * By wrapping text attribute values into an {@code Annotation}, these aspects
 * will be taken into account when handling annotation text and the
 * corresponding main text.
 * <p>
 * Note: There is no semantic connection between this annotation class and the
 * {@code java.lang.annotation} package.
 */
CarClass(CAnnotation)
    , public Object
    , public IAnnotation
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IInterface * attribute);

    CARAPI GetValue(
        /* [out] */ IInterface ** value);

    CARAPI ToString(
        /* [out] */ String * str);

private:
    AutoPtr<IInterface> mValue;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CANNOTATION_H__
