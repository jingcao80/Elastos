#ifndef __ELASTOS_DROID_TEXT_METHOD_CSINGLELINETRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_CSINGLELINETRANSFORMATIONMETHOD_H__

#include "_Elastos_Droid_Text_Method_CSingleLineTransformationMethod.h"
#include "elastos/droid/text/method/ReplacementTransformationMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CSingleLineTransformationMethod)
    , public ReplacementTransformationMethod
    , public ISingleLineTransformationMethod
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    static CARAPI GetInstance(
        /* [out] */ ISingleLineTransformationMethod** ret);

protected:
    /**
     * The characters to be replaced are \n and \r.
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetOriginal();

    /**
     * The character \n is replaced with is space;
     * the character \r is replaced with is FEFF (zero width space).
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetReplacement();

private:
    static AutoPtr<ArrayOf<Char32> > ORIGINAL;
    static AutoPtr<ArrayOf<Char32> > REPLACEMENT;
    static AutoPtr<ISingleLineTransformationMethod> sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CSINGLELINETRANSFORMATIONMETHOD_H__
