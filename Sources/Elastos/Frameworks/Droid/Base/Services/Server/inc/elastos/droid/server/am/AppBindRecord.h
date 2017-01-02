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

#ifndef __ELASTOS_DROID_SERVER_AM_APPBINDRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_APPBINDRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashSet.h>
#include "elastos/droid/server/am/ConnectionRecord.h"

using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CServiceRecord;
class IntentBindRecord;
class ProcessRecord;

/**
 * An association between a service and one of its client applications.
 */
class AppBindRecord : public Object
{
public:
    AppBindRecord(
        /* [in] */ CServiceRecord* service,
        /* [in] */ IntentBindRecord* intent,
        /* [in] */ ProcessRecord* client);

    void Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    void DumpInIntentBind(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<CServiceRecord> mService;   // The running service.
    AutoPtr<IntentBindRecord> mIntent;  // The intent we are bound to.
    ProcessRecord* mClient;     // Who has started/bound the service.

    HashSet< AutoPtr<ConnectionRecord> > mConnections;// All ConnectionRecord for this client.
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPBINDRECORD_H__
