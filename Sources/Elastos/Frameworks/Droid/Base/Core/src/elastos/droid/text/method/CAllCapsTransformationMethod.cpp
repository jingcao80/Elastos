#include "elastos/droid/text/method/CAllCapsTransformationMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

ECode CAllCapsTransformationMethod::GetTransformation(
    /* [in] */ ICharSequence* source,
    /* [in] */ IView* view,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cs = AllCapsTransformationMethod::GetTransformation(source,view);
    *ret = cs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CAllCapsTransformationMethod::OnFocusChanged(
    /* [in] */ IView* view,
    /* [in] */ ICharSequence* sourceText,
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AllCapsTransformationMethod::OnFocusChanged(view, sourceText, focused, direction, previouslyFocusedRect);
    return NOERROR;
}

ECode CAllCapsTransformationMethod::SetLengthChangesAllowed(
    /* [in] */ Boolean allowLengthChanges)
{
    AllCapsTransformationMethod::SetLengthChangesAllowed(allowLengthChanges);
    return NOERROR;
}

ECode CAllCapsTransformationMethod::constructor(
    /* [in] */ IContext* context)
{
    AllCapsTransformationMethod::Init(context);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

