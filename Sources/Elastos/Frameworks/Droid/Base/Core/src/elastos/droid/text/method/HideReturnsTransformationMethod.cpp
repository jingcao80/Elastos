#include "elastos/droid/text/method/HideReturnsTransformationMethod.h"
#include "elastos/droid/text/method/CHideReturnsTransformationMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IHideReturnsTransformationMethod> HideReturnsTransformationMethod::sInstance;

static AutoPtr<ArrayOf<Char32> > HInitOriginal()
{
    Char32 array[] = { '\r' };
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = '\r';
    return ch;
}

static AutoPtr<ArrayOf<Char32> > HInitReplacement()
{
    Char32 array[] = { 0xFEFF };
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = 0xFEFF;
    return ch;
}

CAR_INTERFACE_IMPL(HideReturnsTransformationMethod, ReplacementTransformationMethod, \
    IHideReturnsTransformationMethod)

HideReturnsTransformationMethod::HideReturnsTransformationMethod()
{}

HideReturnsTransformationMethod::~HideReturnsTransformationMethod()
{}

ECode HideReturnsTransformationMethod::constructor()
{
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > HideReturnsTransformationMethod::ORIGINAL = HInitOriginal();

AutoPtr<ArrayOf<Char32> > HideReturnsTransformationMethod::REPLACEMENT = HInitReplacement();

AutoPtr< ArrayOf<Char32> > HideReturnsTransformationMethod::GetOriginal()
{
    return ORIGINAL;
}

AutoPtr< ArrayOf<Char32> > HideReturnsTransformationMethod::GetReplacement()
{
    return REPLACEMENT;
}

ECode HideReturnsTransformationMethod::GetInstance(
    /* [out] */ IHideReturnsTransformationMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CHideReturnsTransformationMethod::New((IHideReturnsTransformationMethod**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
