
#ifndef __ELASTOS_DROID_ACCOUNTS_ACCOUNTMANAGERRESPONSE_H__
#define __ELASTOS_DROID_ACCOUNTS_ACCOUNTMANAGERRESPONSE_H__

#include "_Elastos.Droid.Accounts.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

class AccountManagerResponse
    : public Object
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    AccountManagerResponse(
        /* [in] */ IAccountManagerResponse* response);

    AccountManagerResponse(
        /* [in] */ IParcel* parcel);

    CARAPI OnResult(
        /* [in] */ IBundle* result);

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    AutoPtr<IAccountManagerResponse> mResponse;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_ACCOUNTMANAGERRESPONSE_H__
