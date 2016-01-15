
#ifndef __ELASTOS_DROID_ACCOUNTS_ACCOUNTMANAGERRESPONSE_H__
#define __ELASTOS_DROID_ACCOUNTS_ACCOUNTMANAGERRESPONSE_H__

#include "Elastos.Droid.Core_server.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Accounts {

class AccountManagerResponse
    : public ElRefBase
    , public IParcelable
{
public:
    AccountManagerResponse(
        /* [in] */ IAccountManagerResponse* response);

    AccountManagerResponse(
        /* [in] */ IParcel* parcel);

    CAR_INTERFACE_DECL();

    CARAPI OnResult(
        /* [in] */ IBundle* result);

    CARAPI onError(
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
