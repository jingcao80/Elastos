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

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreinterpret-base-class"
#endif

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

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

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