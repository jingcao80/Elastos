
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/AbstractAccountAuthenticator.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String AbstractAccountAuthenticator::TAG("AccountAuthenticator");

AbstractAccountAuthenticator::AbstractAccountAuthenticator()
{
    ASSERT_SUCCEEDED(CAccountAuthenticatorTransport::NewByFriend((Handle32)this,
            (CAccountAuthenticatorTransport**)&mTransport));
}

/**
 * @return the IBinder for the AccountAuthenticator
 */
// final IBinder getIBinder();

ECode AbstractAccountAuthenticator::HandleException(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ const String& method,
    /* [in] */ const String& data,
    /* [in] */ UInt32 e)
{
    if (e == (ECode)E_NETWORK_ERROR_EXCEPTION) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, method + "(" + data + ")", e);
        // }
        response->OnError(IAccountManager::ERROR_CODE_NETWORK_ERROR,
                String(NULL)/*e.getMessage()*/);
    }
    else if (e == (ECode)E_UNSUPPORTED_OPERATION_EXCEPTION) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, method + "(" + data + ")", e);
        // }
        response->OnError(IAccountManager::ERROR_CODE_UNSUPPORTED_OPERATION,
                method + String(" not supported"));
    }
    else if (e == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, method + "(" + data + ")", e);
        // }
        response->OnError(IAccountManager::ERROR_CODE_BAD_ARGUMENTS,
                method + String(" not supported"));
    }
    else {
        // Log.w(TAG, method + "(" + data + ")", e);
        response->OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                method + String(" failed"));
    }
    return e;
}

ECode AbstractAccountAuthenticator::CheckBinderPermission()
{
    Int32 uid = Binder::GetCallingUid();
    Int32 value;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::ACCOUNT_MANAGER, &value));
    if (value != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "caller uid %d lacks %s", uid, Elastos::Droid::Manifest::permission::ACCOUNT_MANAGER.string());
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("caller uid " + uid + " lacks " + perm);
    }
    return NOERROR;
}

}   //namespace Accounts
}   //namespace Droid
}   //namespace Elastos
