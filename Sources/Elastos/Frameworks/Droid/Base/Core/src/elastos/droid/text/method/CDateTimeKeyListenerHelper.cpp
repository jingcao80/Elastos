#include "elastos/droid/text/method/CDateTimeKeyListenerHelper.h"
#include "elastos/droid/text/method/DateTimeKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CDateTimeKeyListenerHelper)

CAR_INTERFACE_IMPL(CDateTimeKeyListenerHelper, Singleton, IDateTimeKeyListenerHelper)

ECode CDateTimeKeyListenerHelper::GetCHARACTERS(
    /* [out, callee] */ ArrayOf<Char32>** ret)
{
    return DateTimeKeyListener::GetCHARACTERS(ret);
}

ECode CDateTimeKeyListenerHelper::GetInstance(
    /* [out] */ IDateTimeKeyListener** ret)
{
    return DateTimeKeyListener::GetInstance(ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

