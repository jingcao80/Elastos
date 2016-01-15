
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntryRef.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

extern "C" const InterfaceID EIID_BasicPoolEntryRef =
        { 0x28561d66, 0xcaba, 0x42b1, { 0xaa, 0x24, 0x4f, 0x27, 0xd6, 0xf, 0xb6, 0xa8 } };

BasicPoolEntryRef::BasicPoolEntryRef(
    /* [in] */ BasicPoolEntry* entry,
    /* [in] */ IQueue* queue)
{
    // super(entry, queue);

    if (entry == NULL) {
        Logger::E("BasicPoolEntryRef", "Pool entry must not be null");
        assert(0);
        // throw new IllegalArgumentException("Pool entry must not be null.");
    }
    mRoute = entry->GetPlannedRoute();
}

PInterface BasicPoolEntryRef::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_BasicPoolEntryRef) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return Object::Probe(riid);
    }
}

AutoPtr<IHttpRoute> BasicPoolEntryRef::GetRoute()
{
    return mRoute;
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org