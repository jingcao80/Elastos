
#ifndef __ELASTOS_DROID_APP_CONKEYGUARDEXITRESULT_H__
#define __ELASTOS_DROID_APP_CONKEYGUARDEXITRESULT_H__

#include "_Elastos_Droid_App_COnKeyguardExitResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IOnKeyguardExitResult;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(COnKeyguardExitResult)
    , public Object
    , public IOnKeyguardExitResult
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IKeyguardManagerOnKeyguardExitResult* cb);

    CARAPI OnKeyguardExitResult(
        /* [in] */ Boolean success);

private:
    AutoPtr<IKeyguardManagerOnKeyguardExitResult> mCallback;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CONKEYGUARDEXITRESULT_H__
