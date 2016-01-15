#include "elastos/droid/text/method/CBaseKeyListenerHelper.h"
#include "elastos/droid/text/method/BaseKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text{
namespace Method{

CAR_SINGLETON_IMPL(CBaseKeyListenerHelper)

CAR_INTERFACE_IMPL(CBaseKeyListenerHelper, Singleton, IBaseKeyListenerHelper)

CARAPI CBaseKeyListenerHelper::MakeTextContentType(
    /* [in] */ Capitalize caps,
    /* [in] */ Boolean autoText,
    /* [out] */ Int32* ret)
{
    return BaseKeyListener::MakeTextContentType(caps, autoText, ret);
}

} // namespace Elastos
} // namespace Droid
} // namespace Text
} // namespace Method