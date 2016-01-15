#include "elastos/droid/text/method/CTextKeyListenerHelper.h"
#include "elastos/droid/text/method/TextKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CTextKeyListenerHelper)

CAR_INTERFACE_IMPL(CTextKeyListenerHelper, Singleton, ITextKeyListenerHelper)

ECode CTextKeyListenerHelper::Clear(
    /* [in] */ IEditable* e)
{
    return TextKeyListener::Clear(e);
}

ECode CTextKeyListenerHelper::GetInstance(
    /* [out] */ ITextKeyListener** ret)
{
    return TextKeyListener::GetInstance(ret);
}

ECode CTextKeyListenerHelper::GetInstance(
    /* [in] */ Boolean autotext,
    /* [in] */ Capitalize cap,
    /* [out] */ ITextKeyListener** ret)
{
    return TextKeyListener::GetInstance(autotext, cap, ret);
}

ECode CTextKeyListenerHelper::ShouldCap(
    /* [in] */ Capitalize cap,
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 off,
    /* [out] */ Boolean* ret)
{
    return TextKeyListener::ShouldCap(cap, cs, off, ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

