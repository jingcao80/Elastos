
#include "elastos/droid/accounts/CAccountAuthenticatorResponse.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

const String CAccountAuthenticatorResponse::TAG("AccountAuthenticator");

CAR_OBJECT_IMPL(CAccountAuthenticatorResponse)

CAR_INTERFACE_IMPL(CAccountAuthenticatorResponse, Object, IAccountAuthenticatorResponse)

ECode CAccountAuthenticatorResponse::OnResult(
    /* [in] */ IBundle* result)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "AccountAuthenticatorResponse.onResult: "
    //             + AccountManager.sanitizeResult(result));
    // }
    // try {
    return mAccountAuthenticatorResponse->OnResult(result);
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode CAccountAuthenticatorResponse::OnRequestContinued()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "AccountAuthenticatorResponse.onRequestContinued");
    // }
    // try {
    return mAccountAuthenticatorResponse->OnRequestContinued();
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode CAccountAuthenticatorResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "AccountAuthenticatorResponse.onRequestContinued");
    // }
    // try {
    return mAccountAuthenticatorResponse->OnRequestContinued();
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode CAccountAuthenticatorResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mAccountAuthenticatorResponse);
}

ECode CAccountAuthenticatorResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr(
            (Handle32*)(IIAccountAuthenticatorResponse**)&mAccountAuthenticatorResponse);
}

ECode CAccountAuthenticatorResponse::constructor(
    /* [in] */ IIAccountAuthenticatorResponse* response)
{
    mAccountAuthenticatorResponse = response;
    return NOERROR;
}

ECode CAccountAuthenticatorResponse::constructor()
{
    return NOERROR;
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
