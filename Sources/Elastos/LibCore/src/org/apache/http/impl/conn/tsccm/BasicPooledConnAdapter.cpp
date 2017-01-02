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

#include "org/apache/http/impl/conn/tsccm/BasicPooledConnAdapter.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

extern "C" const InterfaceID EIID_BasicPooledConnAdapter =
        { 0x44c71d08, 0x98e9, 0x4321, { 0x8c, 0x38, 0xe6, 0x52, 0x5b, 0x19, 0xc7, 0x22 } };

BasicPooledConnAdapter::BasicPooledConnAdapter(
    /* [in] */ ThreadSafeClientConnManager* tsccm,
    /* [in] */ AbstractPoolEntry* entry)
    : AbstractPooledConnAdapter((IClientConnectionManager*)tsccm, entry)
{
    MarkReusable();
}

PInterface BasicPooledConnAdapter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_BasicPooledConnAdapter) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return AbstractPooledConnAdapter::Probe(riid);
    }
}

AutoPtr<IClientConnectionManager> BasicPooledConnAdapter::GetManager()
{
    // override needed only to make method visible in this package
    return AbstractPooledConnAdapter::GetManager();
}

AutoPtr<AbstractPoolEntry> BasicPooledConnAdapter::GetPoolEntry()
{
    return mPoolEntry;
}

void BasicPooledConnAdapter::Detach()
{
    // override needed only to make method visible in this package
    AbstractPooledConnAdapter::Detach();
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org