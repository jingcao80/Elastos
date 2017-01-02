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

#ifndef __ELASTOS_DROID_SERVER_AM_INTENTBINDRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_INTENTBINDRECORD_H__

#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ProcessRecord;
class CServiceRecord;

/**
 * A particular Intent that has been bound to a Service.
 */
class IntentBindRecord : public Object
{
public:
    IntentBindRecord(
        /* [in] */ CServiceRecord* service,
        /* [in] */ IIntentFilterComparison* intent);

    virtual ~IntentBindRecord();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpInService(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(Int32) CollectFlags();

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
     /** The running service. */
    CServiceRecord* mService;
    /** The intent that is bound.*/
    AutoPtr<IIntentFilterComparison> mIntent; //
     /** All apps that have bound to this Intent. */
    HashMap< AutoPtr<ProcessRecord>, AutoPtr<AppBindRecord> > mApps;
    /** Binder published from service. */
    AutoPtr<IBinder> mBinder;
    /** Set when we have initiated a request for this binder. */
    Boolean mRequested;
    /** Set when we have received the requested binder. */
    Boolean mReceived;
    /** Set when we still need to tell the service all clients are unbound. */
    Boolean mHasBound;
    /** Set when the service's onUnbind() has asked to be told about new clients. */
    Boolean mDoRebind;

    String mStringName;      // caching of toString
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_INTENTBINDRECORD_H__
