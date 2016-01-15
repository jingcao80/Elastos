
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntry.h"
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntryRef.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

extern "C" const InterfaceID EIID_BasicPoolEntry =
        { 0x9da434e4, 0x990e, 0x47f5, { 0x87, 0xf, 0x24, 0x14, 0x56, 0x96, 0x2a, 0x92 } };

BasicPoolEntry::BasicPoolEntry(
    /* [in] */ IClientConnectionOperator* op,
    /* [in] */ IHttpRoute* route,
    /* [in] */ IQueue* queue)
    : AbstractPoolEntry(op, route)
{
    if (route == NULL) {
        Logger::E("BasicPoolEntry", "HTTP route may not be null");
        assert(0);
        // throw new IllegalArgumentException("HTTP route may not be null");
    }
    mReference = new BasicPoolEntryRef(this, queue);
}

PInterface BasicPoolEntry::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_BasicPoolEntry) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return AbstractPoolEntry::Probe(riid);
    }
}

AutoPtr<IOperatedClientConnection> BasicPoolEntry::GetConnection()
{
    return mConnection;
}

AutoPtr<IHttpRoute> BasicPoolEntry::GetPlannedRoute()
{
    return mRoute;
}

AutoPtr<BasicPoolEntryRef> BasicPoolEntry::GetWeakRef()
{
    return mReference;
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org