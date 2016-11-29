
#include "org/alljoyn/bus/NativeSignalHandler.h"

namespace Org {
namespace Alljoyn {
namespace Bus {


NativeSignalHandler::NativeSignalHandler(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* method)
{}

NativeSignalHandler::~NativeSignalHandler()
{
}

bool NativeSignalHandler::IsSameObject(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* method)
{
    return FALSE;
}

QStatus NativeSignalHandler::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    return ER_OK;
}

void NativeSignalHandler::SignalHandler(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ const char* sourcePath,
    /* [in] */ ajn::Message& msg)
{}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org