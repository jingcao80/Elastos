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

#include "elastos/droid/server/CInputMethodManagerServiceMethodCallback.h"
#include "elastos/droid/server/CInputMethodManagerService.h"
#include <elastos/droid/os/Binder.h>

using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::View::EIID_IIInputSessionCallback;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL_2(CInputMethodManagerServiceMethodCallback, Object, IIInputSessionCallback, IBinder)

CAR_OBJECT_IMPL(CInputMethodManagerServiceMethodCallback)

ECode CInputMethodManagerServiceMethodCallback::constructor(
    /* [in] */ IIInputMethodManager* imms,
    /* [in] */ IIInputMethod* method,
    /* [in] */ IInputChannel* channel)
{
    mMethod = method;
    mChannel = channel;
    mParentIMMS = (CInputMethodManagerService*)imms;
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::SessionCreated(
    /* [in] */ IIInputMethodSession* session)
{
    Int64 ident = Binder::ClearCallingIdentity();
    mParentIMMS->OnSessionCreated(mMethod, session, mChannel);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
