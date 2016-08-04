#include "elastos/droid/text/method/SingleLineTransformationMethod.h"
#include "elastos/droid/text/method/CSingleLineTransformationMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

static AutoPtr<ArrayOf<Char32> > InitOriginal()
{
    Char32 array[] = {'\n', '\r'};
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = '\n';
    (*ch)[1] = '\r';
    return ch;
}

static AutoPtr<ArrayOf<Char32> > InitReplacement()
{
    Char32 array[] = {' ', 0xFEFF};
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = ' ';
    (*ch)[1] = 0xFEFF;
    return ch;
}

AutoPtr<ArrayOf<Char32> > SingleLineTransformationMethod::ORIGINAL = InitOriginal();
AutoPtr<ArrayOf<Char32> > SingleLineTransformationMethod::REPLACEMENT = InitReplacement();

AutoPtr<ISingleLineTransformationMethod> SingleLineTransformationMethod::sInstance;

CAR_INTERFACE_IMPL(SingleLineTransformationMethod, ReplacementTransformationMethod, \
    ISingleLineTransformationMethod)

ECode SingleLineTransformationMethod::constructor()
{
    return NOERROR;
}

ECode SingleLineTransformationMethod::GetInstance(
    /* [out] */ ISingleLineTransformationMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CSingleLineTransformationMethod::New((ISingleLineTransformationMethod**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > SingleLineTransformationMethod::GetOriginal()
{
    return ORIGINAL;
}

AutoPtr< ArrayOf<Char32> > SingleLineTransformationMethod::GetReplacement()
{
    return REPLACEMENT;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

