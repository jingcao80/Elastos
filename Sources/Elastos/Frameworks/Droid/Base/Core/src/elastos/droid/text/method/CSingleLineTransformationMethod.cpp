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

AutoPtr<ArrayOf<Char32> > CSingleLineTransformationMethod::ORIGINAL = InitOriginal();
AutoPtr<ArrayOf<Char32> > CSingleLineTransformationMethod::REPLACEMENT = InitReplacement();

AutoPtr<ISingleLineTransformationMethod> CSingleLineTransformationMethod::sInstance;

CAR_INTERFACE_IMPL_3(CSingleLineTransformationMethod, Object, ISingleLineTransformationMethod, IReplacementTransformationMethod, ITransformationMethod)

CAR_OBJECT_IMPL(CSingleLineTransformationMethod)

ECode CSingleLineTransformationMethod::constructor()
{
    return NOERROR;
}

ECode CSingleLineTransformationMethod::GetInstance(
    /* [out] */ ISingleLineTransformationMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        AutoPtr<CSingleLineTransformationMethod> stm;
        CSingleLineTransformationMethod::NewByFriend((CSingleLineTransformationMethod**)&stm);
        sInstance = (ISingleLineTransformationMethod*)(stm.Get());
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > CSingleLineTransformationMethod::GetOriginal()
{
    return ORIGINAL;
}

AutoPtr< ArrayOf<Char32> > CSingleLineTransformationMethod::GetReplacement()
{
    return REPLACEMENT;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

