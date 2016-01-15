
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/AccountManagerResponse.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

AccountManagerResponse::AccountManagerResponse(
    /* [in] */ IAccountManagerResponse* response)
    : mResponse(response)
{}

AccountManagerResponse::AccountManagerResponse(
    /* [in] */ IParcel* parcel)
{
    ASSERT_SUCCEEDED(parcel->ReadInterfacePtr(
            (Handle32*)(IAccountManagerResponse**)&mResponse));
}

CAR_INTERFACE_IMPL(AccountManagerResponse, IParcelable);

ECode AccountManagerResponse::OnResult(
    /* [in] */ IBundle* result)
{
    // try {
    return mResponse->OnResult(result);
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode AccountManagerResponse::onError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    // try {
    return mResponse->OnError(errorCode, errorMessage);
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode AccountManagerResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mResponse);
}

ECode AccountManagerResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr((Handle32*)(IAccountManagerResponse**)&mResponse);
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
