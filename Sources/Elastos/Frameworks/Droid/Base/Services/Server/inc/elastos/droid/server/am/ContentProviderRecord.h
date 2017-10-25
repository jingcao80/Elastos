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

#ifndef __ELASTOS_DROID_SERVER_AM_CONTENTPROVIDERRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_CONTENTPROVIDERRECORD_H__

#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/server/am/CContentProviderConnection.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IContentProviderHolder;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

class ContentProviderRecord : public Object
{
private:
    // This class represents a handle from an external process to a provider.
    class ExternalProcessHandle
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ExternalProcessHandle(
            /* [in] */ IBinder* token,
            /* [in] */ ContentProviderRecord* owner);

        CARAPI UnlinkFromOwnDeathLocked();

        CARAPI ProxyDied();

    public:
        static const String TAG;
        AutoPtr<IBinder> mToken;
        Int32 mAcquisitionCount;
        ContentProviderRecord* mOwner;
    };

public:
    ContentProviderRecord(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IProviderInfo* info,
        /* [in] */ IApplicationInfo* ai,
        /* [in] */ IComponentName* name,
        /* [in] */ Boolean singleton);

    ContentProviderRecord(
        /* [in] */ ContentProviderRecord* cpr);

    ~ContentProviderRecord();

    CARAPI_(AutoPtr<IContentProviderHolder>) NewHolder(
        /* [in] */ CContentProviderConnection* conn);

    CARAPI_(Boolean) CanRunHere(
        /* [in] */ ProcessRecord* app);

    CARAPI_(void) AddExternalProcessHandleLocked(
    /* [in] */ IBinder* token);

    CARAPI_(Boolean) RemoveExternalProcessHandleLocked(
    /* [in] */ IBinder* token);

    CARAPI_(Boolean) HasExternalProcessHandles();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ Boolean full);

    using Object::ToString;

    CARAPI_(String) ToString();

    CARAPI_(String) ToShortString();

private:
    CARAPI_(void) RemoveExternalProcessHandleInternalLocked(
        /* [in] */ IBinder* token);

public:
    CActivityManagerService* mService;
    AutoPtr<IProviderInfo> mInfo;
    Int32 mUid;
    AutoPtr<IApplicationInfo> mAppInfo;
    AutoPtr<IComponentName> mName;
    // number of non-framework processes supported by this provider
    Boolean mSingleton;
    AutoPtr<IIContentProvider> mProvider;
    Boolean mNoReleaseNeeded;
    // All attached clients
    List< AutoPtr<CContentProviderConnection> > mConnections;
    // Handles for non-framework processes supported by this provider
    typedef HashMap<AutoPtr<IBinder>, AutoPtr<ExternalProcessHandle> > ProcessHandleHashMap;
    typedef typename ProcessHandleHashMap::ValueType ProcessHandleValueType;
    typedef typename ProcessHandleHashMap::Iterator ProcessHandleIterator;
    AutoPtr<ProcessHandleHashMap> mExternalProcessTokenToHandle;

    // Count for external process for which we have no handles.
    Int32 mExternalProcessNoHandleCount;
    ProcessRecord* mProc; // weak-ref if non-null, hosting process.
    AutoPtr<ProcessRecord> mLaunchingApp;
    String mStringName;
    String mShortStringName;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::ContentProviderRecord)

#endif //__ELASTOS_DROID_SERVER_AM_CONTENTPROVIDERRECORD_H__
