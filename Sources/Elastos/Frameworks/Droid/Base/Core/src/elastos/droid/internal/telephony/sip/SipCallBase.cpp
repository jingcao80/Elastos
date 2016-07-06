
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
