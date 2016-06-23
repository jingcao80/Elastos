
#include "elastos/droid/internal/telephony/dataconnection/DdsSchedulerAc.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include "elastos/droid/provider/Telephony.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::Telephony;
using Elastos::Utility::Logging::Logger;

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
    return NOERROR;
}

ECode DdsSchedulerAc::AllocateDds(
    /* [in] */ INetworkRequest* req)
{
    Logger::D(TAG, "EVENT_ADD_REQUEST = %s", TO_CSTR(req));
    SendMessage(EVENT_ADD_REQUEST, req);
    return NOERROR;
}

ECode DdsSchedulerAc::FreeDds(
    /* [in] */ INetworkRequest* req)
{
    Logger::D(TAG, "EVENT_REMOVE_REQUEST = %s", TO_CSTR(req));
    SendMessage(EVENT_REMOVE_REQUEST, req);
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
