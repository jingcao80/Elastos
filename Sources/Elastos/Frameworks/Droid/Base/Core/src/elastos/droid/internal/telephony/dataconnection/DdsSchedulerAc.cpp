
#include "elastos/droid/internal/telephony/dataconnection/DdsSchedulerAc.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DdsSchedulerAc, AsyncChannel, IDdsSchedulerAc)

DdsSchedulerAc::DdsSchedulerAc()
    : TAG("DdsSchedulerAc")
{}

ECode DdsSchedulerAc::constructor()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode DdsSchedulerAc::AllocateDds(
    /* [in] */ INetworkRequest* req)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(TAG, "EVENT_ADD_REQUEST = " + req);
        sendMessage(EVENT_ADD_REQUEST, req);

#endif
}

ECode DdsSchedulerAc::FreeDds(
    /* [in] */ INetworkRequest* req)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(TAG, "EVENT_REMOVE_REQUEST = " + req);
        sendMessage(EVENT_REMOVE_REQUEST, req);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
