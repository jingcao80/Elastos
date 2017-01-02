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

#include "elastos/droid/database/BulkCursorNative.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Database::Sqlite::ISQLiteClosable;

namespace Elastos {
namespace Droid {
namespace Database {

//==================================================================================
// BulkCursorProxy
//==================================================================================

CAR_INTERFACE_IMPL_2(BulkCursorProxy, Object, IBulkCursorProxy, IBulkCursor)

BulkCursorProxy::BulkCursorProxy(
    /* [in] */ IBinder* binder)
    : mRemote(IBulkCursor::Probe(binder))
{
    assert(mRemote != NULL);
}

ECode BulkCursorProxy::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = IBinder::Probe(mRemote);
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode BulkCursorProxy::GetWindow(
    /* [in] */ Int32 position,
    /* [out] */ ICursorWindow** result)
{
    VALIDATE_NOT_NULL(result)
    return mRemote->GetWindow(position, result);
}

ECode BulkCursorProxy::OnMove(
    /* [in] */ Int32 position)
{
    return mRemote->OnMove(position);
}

ECode BulkCursorProxy::Deactivate()
{
    return mRemote->Deactivate();
}

ECode BulkCursorProxy::Close()
{
    return mRemote->Close();
}

ECode BulkCursorProxy::Requery(
    /* [in] */ IIContentObserver* observer,
    /* [out] */ Int32* result)
{
    return mRemote->Requery(observer, result);
}

ECode BulkCursorProxy::GetExtras(
    /* [out] */ IBundle** result)
{
    return mRemote->GetExtras(result);
}

ECode BulkCursorProxy::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    return mRemote->Respond(extras, result);
}

//==================================================================================
// BulkCursorNative
//==================================================================================

CAR_INTERFACE_IMPL_2(BulkCursorNative, Object, IBulkCursor, IBinder)

AutoPtr<IBulkCursor> BulkCursorNative::AsInterface(
    /* [in] */ IBinder* binder)
{
    if (binder != NULL) {
        IProxy* proxy = (IProxy*)binder->Probe(EIID_IProxy);
        if (proxy != NULL) {
            AutoPtr<IBulkCursor> provider = new BulkCursorProxy(binder);
            return provider;
        }
    }

    return IBulkCursor::Probe(binder);
}

} //Database
} //Droid
} //Elastos