#ifndef __ELASTOS_DROID_TEXT_METHOD_HIDERETURNSTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_HIDERETURNSTRANSFORMATIONMETHOD_H__

#include "elastos/droid/text/method/ReplacementTransformationMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * This transformation method causes any carriage return characters (\r)
 * to be hidden by displaying them as zero-width non-breaking space
 * characters (\uFEFF).
 */
//public class
class HideReturnsTransformationMethod
    : public ReplacementTransformationMethod
    , public IHideReturnsTransformationMethod
{
public:
    HideReturnsTransformationMethod();

    ~HideReturnsTransformationMethod();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI GetInstance(
        /* [out] */ IHideReturnsTransformationMethod** ret);

protected:
    /**
     * The character to be replaced is \r.
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetOriginal();

    /**
     * The character that \r is replaced with is \uFEFF.
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetReplacement();

private:
    static AutoPtr<ArrayOf<Char32> > ORIGINAL;
    static AutoPtr<ArrayOf<Char32> > REPLACEMENT;
    static AutoPtr<IHideReturnsTransformationMethod> sInstance;
};


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_HIDERETURNSTRANSFORMATIONMETHOD_H__
