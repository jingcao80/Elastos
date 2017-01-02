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
#include "elastos/droid/os/CMessengerImpl.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(CMessengerImpl, Object, IIMessenger, IBinder)

CAR_OBJECT_IMPL(CMessengerImpl)

ECode CMessengerImpl::constructor(
    /* [in] */ IHandler* handler)
{
    IWeakReferenceSource* source = IWeakReferenceSource::Probe(handler);
    source->GetWeakReference((IWeakReference**)&mOwner);
    return NOERROR;
}

ECode CMessengerImpl::Send(
    /* [in] */ IMessage* message)
{
    Boolean result;
    Int32 uid = Binder::GetCallingUid();
    message->SetSendingUid(uid);
    AutoPtr<IInterface> obj;
    mOwner->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj != NULL) {
        IHandler* handler = IHandler::Probe(obj);
        if (handler)
            return handler->SendMessage(message, &result);
    }
    else {
        Logger::E("CMessengerImpl", "line:%d, func:%s, handle have been destroyed,this:%p\n", __LINE__, "Send", this);
    }
    return E_REMOTE_EXCEPTION;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
