#include "elastos/droid/text/method/CHideReturnsTransformationMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IHideReturnsTransformationMethod> CHideReturnsTransformationMethod::sInstance;

ECode CHideReturnsTransformationMethod::GetTransformation(
    /* [in] */ ICharSequence* source,
    /* [in] */ IView* view,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cs = HideReturnsTransformationMethod::GetTransformation(source,view);
    *ret = cs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CHideReturnsTransformationMethod::OnFocusChanged(
    /* [in] */ IView* view,
    /* [in] */ ICharSequence* sourceText,
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    HideReturnsTransformationMethod::OnFocusChanged(view, sourceText, focused, direction, previouslyFocusedRect);
    return NOERROR;
}

AutoPtr<IHideReturnsTransformationMethod> CHideReturnsTransformationMethod::GetInstance()
{
    if (sInstance == NULL) {
        AutoPtr<CHideReturnsTransformationMethod> hrtm;
        CHideReturnsTransformationMethod::NewByFriend((CHideReturnsTransformationMethod**)&hrtm);
        sInstance = (IHideReturnsTransformationMethod*)(hrtm.Get());
    }
    return sInstance;
}

} //namespace Method
} //namespace Text
} //namespace Droid
} //namespace Elastos

