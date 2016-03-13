
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/AccountManagerResponse.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_INTERFACE_IMPL(AccountManagerResponse, Object, IParcelable);

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

ECode AccountManagerResponse::OnResult(
    /* [in] */ IBundle* result)
{
    return mResponse->OnResult(result);
}

ECode AccountManagerResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return mResponse->OnError(errorCode, errorMessage);
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
