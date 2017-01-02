//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/AbstractAccountAuthenticator.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String AbstractAccountAuthenticator::TAG("AccountAuthenticator");

//===============================================================
// AbstractAccountAuthenticator::MyRunnable::
//===============================================================
CAR_INTERFACE_IMPL(AbstractAccountAuthenticator::MyRunnable, Object, IRunnable)

AbstractAccountAuthenticator::MyRunnable::MyRunnable(
    /* [in] */ IAccountAuthenticatorResponse* response)
{
    mResponse = response;
}

ECode AbstractAccountAuthenticator::MyRunnable::Run()
{
    AutoPtr<IBundle> result;
    CBundle::New((IBundle**)&result);
    result->PutBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE);
    mResponse->OnResult(result);
    return NOERROR;
}

//===============================================================
// AbstractAccountAuthenticator::
//===============================================================
AbstractAccountAuthenticator::AbstractAccountAuthenticator()
{
    ASSERT_SUCCEEDED(CAccountAuthenticatorTransport::NewByFriend((Handle32)this,
            (CAccountAuthenticatorTransport**)&mTransport));
}

ECode AbstractAccountAuthenticator::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode AbstractAccountAuthenticator::HandleException(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ const String& method,
    /* [in] */ const String& data,
    /* [in] */ UInt32 e)
{
    if (e == E_NETWORK_ERROR_EXCEPTION) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, method + "(" + data + ")", e);
        // }
        response->OnError(IAccountManager::ERROR_CODE_NETWORK_ERROR,
                String(NULL)/*e.getMessage()*/);
    }
    else if (e == E_UNSUPPORTED_OPERATION_EXCEPTION) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, method + "(" + data + ")", e);
        // }
        response->OnError(IAccountManager::ERROR_CODE_UNSUPPORTED_OPERATION,
                method + String(" not supported"));
    }
    else if (e == E_ILLEGAL_ARGUMENT_EXCEPTION) {
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

ECode AbstractAccountAuthenticator::GetAccountCredentialsForCloning(
    /* [in] */ IAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IRunnable> r = new MyRunnable(response);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
    *result = NULL;
    return NOERROR;
}

ECode AbstractAccountAuthenticator::AddAccountFromCredentials(
    /* [in] */ IAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* accountCredentials,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IRunnable> r = new MyRunnable(response);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
    *result = NULL;
    return NOERROR;
}

}   //namespace Accounts
}   //namespace Droid
}   //namespace Elastos
