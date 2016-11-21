
#include "org/alljoyn/bus/NativeMessageContext.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Map.h>
#include <elastos/utility/etl/Pair.h>
#include <qcc/Thread.h>

using Elastos::Core::AutoLock;
using Elastos::Core::Object;
using Elastos::Utility::Etl::Map;
using Elastos::Utility::Etl::Pair;

namespace Org {
namespace Alljoyn {
namespace Bus {

Map<qcc::Thread*, ajn::Message> gMessageMap;

Object gMessageMapLock;

ajn::Message NativeMessageContext::GetMessage()
{
    AutoLock lock(gMessageMapLock);
    Map<qcc::Thread*, ajn::Message>::Iterator it =
            gMessageMap.Find(qcc::Thread::GetThread());
    assert(gMessageMap.End() != it);
    ajn::Message m = it->mSecond;
    return m;
}

NativeMessageContext::NativeMessageContext(
    /* [in] */ const ajn::Message& msg)
{
    AutoLock lock(gMessageMapLock);
    gMessageMap.Insert(Pair<qcc::Thread*, ajn::Message>(qcc::Thread::GetThread(), msg));
}

NativeMessageContext::~NativeMessageContext()
{
    AutoLock lock(gMessageMapLock);
    Map<qcc::Thread*, ajn::Message>::Iterator it =
            gMessageMap.Find(qcc::Thread::GetThread());
    gMessageMap.Erase(it);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
