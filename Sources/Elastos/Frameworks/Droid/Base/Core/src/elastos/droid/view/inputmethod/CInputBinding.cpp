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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/inputmethod/CInputBinding.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CInputBinding::
//========================================================================================
CAR_INTERFACE_IMPL_2(CInputBinding, Object, IInputBinding, IParcelable)

CAR_OBJECT_IMPL(CInputBinding)

CInputBinding::CInputBinding()
    : mUid(0)
    , mPid(0)
{
}

ECode CInputBinding::constructor(
    /* [in] */ IInputConnection* conn,
    /* [in] */ IBinder* connToken,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    mConnection = conn;
    mConnectionToken = connToken;
    mUid = uid;
    mPid = pid;
    return NOERROR;
}

ECode CInputBinding::constructor(
    /* [in] */ IInputConnection* conn,
    /* [in] */ IInputBinding* binding)
{
    mConnection = conn;
    assert(binding != NULL);
    binding->GetConnectionToken((IBinder**)&mConnectionToken);
    binding->GetUid(&mUid);
    binding->GetPid(&mPid);

    return NOERROR;
}

ECode CInputBinding::constructor()
{
    mConnection = NULL;
    return NOERROR;
}

ECode CInputBinding::GetConnection(
    /* [out] */ IInputConnection** inputConnection)
{
    VALIDATE_NOT_NULL(inputConnection);

    *inputConnection = mConnection;
    REFCOUNT_ADD(*inputConnection);

    return NOERROR;
}

ECode CInputBinding::GetConnectionToken(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    *binder = mConnectionToken;
    REFCOUNT_ADD(*binder);

    return NOERROR;
}

ECode CInputBinding::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);

    *uid = mUid;
    return NOERROR;
}

ECode CInputBinding::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);

    *pid = mPid;
    return NOERROR;
}

ECode CInputBinding::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    AutoPtr<IInterface> obj;
    FAIL_RETURN(source->ReadInterfacePtr((HANDLE*)&obj));
    mConnectionToken = IBinder::Probe(obj.Get());
    FAIL_RETURN(source->ReadInt32(&mUid));
    FAIL_RETURN(source->ReadInt32(&mPid));
    return NOERROR;
}

ECode CInputBinding::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    FAIL_RETURN(dest->WriteInterfacePtr((IInterface*)mConnectionToken.Get()));
    FAIL_RETURN(dest->WriteInt32(mUid));
    FAIL_RETURN(dest->WriteInt32(mPid));
    return NOERROR;
}

ECode CInputBinding::ToString(
    /* [out] */ String* str)
{
    StringBuilder builder;
    builder += "InputBinding{";
    String str1;
    mConnectionToken->ToString(&str1);
    builder += str1;
    builder += " / uid ";
    builder += mUid;
    builder += " / pid ";
    builder += mPid;
    builder += "}";

    *str = builder.ToString();

    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
