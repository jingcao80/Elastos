#ifndef __ELASTOS_DROID_TEXT_METHOD_CBASEKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CBASEKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Text_Method_CBaseKeyListenerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text{
namespace Method{

CarClass(CBaseKeyListenerHelper)
    , public Singleton
    , public IBaseKeyListenerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI MakeTextContentType(
        /* [in] */ Capitalize caps,
        /* [in] */ Boolean autoText,
        /* [out] */ Int32* ret);
};

} // namespace Elastos
} // namespace Droid
} // namespace Text
} // namespace Method

#endif // __ELASTOS_DROID_TEXT_METHOD_CBASEKEYLISTENERHELPER_H__