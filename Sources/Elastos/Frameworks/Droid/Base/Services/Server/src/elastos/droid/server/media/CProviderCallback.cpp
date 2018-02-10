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

#include "elastos/droid/server/media/CProviderCallback.h"
#include "elastos/droid/server/media/RemoteDisplayProviderProxy.h"

using Elastos::Droid::Media::EIID_IIRemoteDisplayCallback;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CAR_INTERFACE_IMPL_2(CProviderCallback, Object, IIRemoteDisplayCallback, IBinder)

CAR_OBJECT_IMPL(CProviderCallback)

ECode CProviderCallback::constructor(
    /* [in] */ HANDLE host)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(
            (IObject*)(RemoteDisplayProviderProxy::Connection*)host);
    return wrs->GetWeakReference((IWeakReference**)&mConnectionRef);
}

void CProviderCallback::Dispose()
{
    // mConnectionRef->Clear();
    assert(0);
}

ECode CProviderCallback::OnStateChanged(
    /* [in] */ IRemoteDisplayState * state)
{
    AutoPtr<IInterface> obj;
    mConnectionRef->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<RemoteDisplayProviderProxy::Connection> connection = (RemoteDisplayProviderProxy::Connection*)IObject::Probe(obj);
    if (connection != NULL) {
        connection->PostStateChanged(state);
    }
    return NOERROR;
}

ECode CProviderCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
