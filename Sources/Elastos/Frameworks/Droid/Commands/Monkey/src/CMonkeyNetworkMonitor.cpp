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

#include "CMonkeyNetworkMonitor.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyNetworkMonitor)

ECode CMonkeyNetworkMonitor::PerformReceive(
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle *extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return MonkeyNetworkMonitor::PerformReceive(
        intent,
        resultCode,
        data,
        extras,
        ordered,
        sticky,
        sendingUser);
}

ECode CMonkeyNetworkMonitor::Start()
{
    return MonkeyNetworkMonitor::Start();
}

ECode CMonkeyNetworkMonitor::Register(
    /* [in] */ IIActivityManager *am)
{
    return MonkeyNetworkMonitor::Register(am);
}

ECode CMonkeyNetworkMonitor::Unregister(
    /* [in] */ IIActivityManager *am)
{
    return MonkeyNetworkMonitor::Unregister(am);
}

ECode CMonkeyNetworkMonitor::Stop()
{
    return MonkeyNetworkMonitor::Stop();
}

ECode CMonkeyNetworkMonitor::Dump()
{
    return MonkeyNetworkMonitor::Dump();
}

PInterface CMonkeyNetworkMonitor::Probe(
        /* [in] */ REIID riid)
{
    return _CMonkeyNetworkMonitor::Probe(riid);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
