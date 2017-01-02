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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipCallBase.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                             SipCallBase
//=====================================================================
//CAR_INTERFACE_IMPL(SipCallBase, Call, ISipCallBase);

ECode SipCallBase::GetConnections(
    /* [out] */ IList/*<Connection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME should return Collections.unmodifiableList();
    *result = IList::Probe(mConnections);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipCallBase::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mConnections->GetSize(&size);
    *result = size > 1;
    return NOERROR;
}

ECode SipCallBase::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str;
    Call::ToString(&str);
    *result = StringUtils::ToString(mState) + ":" + str;
    return NOERROR;
}

ECode SipCallBase::ClearDisconnected()
{
    AutoPtr<IIterator> it;
    mConnections->GetIterator((IIterator**)&it);
    Boolean b;
    for (; (it->HasNext(&b), b);) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IConnection> c = IConnection::Probe(obj);
        ICallState state;
        c->GetState(&state);
        if (state == ICallState_DISCONNECTED) {
            it->Remove();
        }
    }

    if (mConnections->IsEmpty(&b), b) {
        SetState(ICallState_IDLE);
    }
    return NOERROR;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
