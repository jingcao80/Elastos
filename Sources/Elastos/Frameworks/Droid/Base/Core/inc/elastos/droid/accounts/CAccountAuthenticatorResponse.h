
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORRESPONSE_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORRESPONSE_H__

#include "_Elastos_Droid_Accounts_CAccountAuthenticatorResponse.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountAuthenticatorResponse)
    , public Object
    , public IAccountAuthenticatorResponse
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIAccountAuthenticatorResponse* response);

    CARAPI OnResult(
        /* [in] */ IBundle* result);

    CARAPI OnRequestContinued();

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    static const String TAG;
    AutoPtr<IIAccountAuthenticatorResponse> mAccountAuthenticatorResponse;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORRESPONSE_H__
