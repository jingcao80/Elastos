#include "elastos/droid/text/method/CQwertyKeyListenerHelper.h"
#include "elastos/droid/text/method/CQwertyKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CQwertyKeyListenerHelper)

CAR_INTERFACE_IMPL(CQwertyKeyListenerHelper, Singleton, IQwertyKeyListenerHelper)

ECode CQwertyKeyListenerHelper::GetInstance(
    /* [in] */ Boolean autoText,
    /* [in] */ Capitalize cap,
    /* [out] */ IQwertyKeyListener** ret)
{
    return QwertyKeyListener::GetInstance(autoText, cap, ret);
}

ECode CQwertyKeyListenerHelper::GetInstanceForFullKeyboard(
    /* [out] */ IQwertyKeyListener** ret)
{
    return QwertyKeyListener::GetInstanceForFullKeyboard(ret);
}

ECode CQwertyKeyListenerHelper::MarkAsReplaced(
    /* [in] */ ISpannable* content,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const String& original)
{
    return QwertyKeyListener::MarkAsReplaced(content, start, end, original);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
