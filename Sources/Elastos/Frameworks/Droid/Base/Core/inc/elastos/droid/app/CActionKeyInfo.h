#ifndef __ELASTOS_DROID_APP_CACTIONKEYINFO_H__
#define __ELASTOS_DROID_APP_CACTIONKEYINFO_H__

#include "_Elastos_Droid_App_CActionKeyInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActionKeyInfo)
    , public Object
    , public IActionKeyInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActionKeyInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetKeyCode(
        /* [out] */ Int32* keyCode);

    CARAPI GetQueryActionMsg(
        /* [out] */ String* actionMsg);

    CARAPI GetSuggestActionMsg(
        /* [out] */ String* actionMsg);

    CARAPI GetSuggestActionMsgColumn(
        /* [out] */ String* column);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

private:
    Int32 mKeyCode;
    String mQueryActionMsg;
    String mSuggestActionMsg;
    String mSuggestActionMsgColumn;
};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif
