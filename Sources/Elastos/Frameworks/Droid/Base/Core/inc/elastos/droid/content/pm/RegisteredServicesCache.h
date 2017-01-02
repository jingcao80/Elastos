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

#ifndef __ELASTOS_DROID_CONTENT_PM_REGISTEREDSERVICESCACHE_H__
#define __ELASTOS_DROID_CONTENT_PM_REGISTEREDSERVICESCACHE_H__

#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Cache of registered services. This cache is lazily built by interrogating
 * {@link PackageManager} on a per-user basis. It's updated as packages are
 * added, removed and changed. Users are responsible for calling
 * {@link #invalidateCache(int)} when a user is started, since
 * {@link PackageManager} broadcasts aren't sent for stopped users.
 * <p>
 * The services are referred to by type V and are made available via the
 * {@link #getServiceInfo} method.
 *
 * @hide
 */
class ECO_PUBLIC RegisteredServicesCache
    : public Object
    , public IRegisteredServicesCache
{
public:
    /**
     * Value type that describes a Service. The information within can be used
     * to bind to the service.
     */
    class ServiceInfo
        : public Object
        , public IRegisteredServicesCacheServiceInfo
    {
    public:
        CAR_INTERFACE_DECL()

        /** @hide */
        ServiceInfo();

        virtual ~ServiceInfo();

        CARAPI constructor(
            /* [in] */ IInterface* type,
            /* [in] */ IComponentName* componentName,
            /* [in] */ Int32 uid);

        CARAPI ToString(
            /* [out] */ String* info);

        CARAPI SetType(
            /* [in] */ IInterface* type);

        CARAPI GetType(
            /* [out] */ IInterface** type);

        CARAPI SetComponentName(
            /* [in] */ IComponentName* cn);

        CARAPI GetComponentName(
            /* [out] */ IComponentName** cn);

        CARAPI SetUid(
            /* [in] */ Int32 uid);

        CARAPI GetUid(
            /* [out] */ Int32* uid);

    public:
        AutoPtr<IInterface> mType;
        AutoPtr<IComponentName> mComponentName;
        Int32 mUid;
    };

private:
    class ECO_LOCAL UserServices
        : public Object
    {
    public:
        UserServices();

        ~UserServices();

    public:
        // @GuardedBy("mServicesLock")
        HashMap<AutoPtr<IInterface>, AutoPtr<IInteger32> > mPersistentServices;

        // @GuardedBy("mServicesLock")
        AutoPtr<HashMap<AutoPtr<IInterface>, AutoPtr<IRegisteredServicesCacheServiceInfo> > > mServices;
    };

    class ECO_LOCAL PackageReceiver
        : public BroadcastReceiver
    {
    public:
        PackageReceiver(
            /* [in] */ RegisteredServicesCache* parent);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("RegisteredServicesCache::PackageReceiver: ")
    private:
        RegisteredServicesCache* mParent;
    };

    class ECO_LOCAL ExternalReceiver
        : public BroadcastReceiver
    {
    public:
        ExternalReceiver(
            /* [in] */ RegisteredServicesCache* parent);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("RegisteredServicesCache::ExternalReceiver: ")
    private:
        RegisteredServicesCache* mParent;
    };

    class ECO_LOCAL ListenerRunnable
        : public Runnable
    {
    public:
        ListenerRunnable(
            /* [in] */ IInterface* type,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean removed,
            /* [in] */ IRegisteredServicesCacheListener* listener);

        CARAPI Run();

    private:
        AutoPtr<IInterface> mType;

        Int32 mUserId;

        Boolean mRemoved;

        AutoPtr<IRegisteredServicesCacheListener> mListener;
    };

public:
    CAR_INTERFACE_DECL()

    RegisteredServicesCache();

    virtual ~RegisteredServicesCache();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& metaDataName,
        /* [in] */ const String& attributeName,
        /* [in] */ IXmlSerializerAndParser* serializerAndParser);

    CARAPI GetContext(
        /* [out] */ IContext** ctx);

    CARAPI SetContext(
        /* [in] */ IContext* ctx);

    CARAPI InvalidateCache(
        /* [in] */ Int32 userId);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* fout,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 userId);

    CARAPI GetListener(
        /* [out] */ IRegisteredServicesCacheListener** listener);

    CARAPI SetListener(
        /* [in] */ IRegisteredServicesCacheListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Accessor for the registered authenticators.
     * @param type the account type of the authenticator
     * @return the AuthenticatorInfo that matches the account type or null if none is present
     */
    CARAPI GetServiceInfo(
        /* [in] */ IInterface* type,
        /* [in] */ Int32 userId,
        /* [out] */ IRegisteredServicesCacheServiceInfo** info);

    /**
     * @return a collection of {@link RegisteredServicesCache.ServiceInfo} objects for all
     * registered authenticators.
     */
    CARAPI GetAllServices(
        /* [in] */ Int32 userId,
        /* [out] */ IList** list);

    virtual CARAPI ParseServiceAttributes(
            /* [in] */ IResources* res,
            /* [in] */ const String& packageName,
            /* [in] */ IAttributeSet* attrs,
            /* [out] */ IInterface** attributes) = 0;

private:
    ECO_LOCAL CARAPI_(AutoPtr<UserServices>) FindOrCreateUserLocked(
        /* [in] */ Int32 userId);

    ECO_LOCAL CARAPI_(void) NotifyListener(
        /* [in] */ IInterface* type,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean removed);

    ECO_LOCAL CARAPI_(Boolean) InSystemImage(
        /* [in] */ Int32 callerUid);

    /**
     * Populate {@link UserServices#services} by scanning installed packages for
     * given {@link UserHandle}.
     */
    ECO_LOCAL CARAPI_(void) GenerateServicesMap(
        /* [in] */ Int32 userId);

    ECO_LOCAL CARAPI_(Boolean) ContainsType(
        /* [in] */ List<AutoPtr<IRegisteredServicesCacheServiceInfo> >* serviceInfos,
        /* [in] */ IInterface* type);

    ECO_LOCAL CARAPI_(Boolean) ContainsTypeAndUid(
        /* [in] */ List<AutoPtr<IRegisteredServicesCacheServiceInfo> >* serviceInfos,
        /* [in] */ IInterface* type,
        /* [in] */ Int32 uid);

    ECO_LOCAL CARAPI ParseServiceInfo(
        /* [in] */ IResolveInfo* service,
        /* [out] */ IRegisteredServicesCacheServiceInfo** info);

    /**
     * Read all sync status back in to the initial engine state.
     */
    ECO_LOCAL CARAPI_(void) ReadPersistentServicesLocked();

    /**
     * Write all sync status to the sync status file.
     */
    ECO_LOCAL CARAPI_(void) WritePersistentServicesLocked();

    ECO_LOCAL CARAPI ErrorCheckAndClose(
        /* [in] */ ECode ec,
        /* [in] */ IFileInputStream* fos);

    ECO_LOCAL CARAPI ErrorCheckAndFailWrite(
        /* [in] */ ECode ec,
        /* [in] */ IAtomicFile* atomicFile,
        /* [in] */ IFileOutputStream* fis);

    ECO_LOCAL CARAPI HandlePackageEvent(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

public:
    AutoPtr<IContext> mContext;

private:
    ECO_LOCAL static const String TAG;

    String mInterfaceName;
    String mMetaDataName;
    String mAttributesName;
    AutoPtr<IXmlSerializerAndParser> mSerializerAndParser;

    Object mServicesLock;

    // @GuardedBy("mServicesLock")
    Boolean mPersistentServicesFileDidNotExist;

    // @GuardedBy("mServicesLock")
    HashMap<Int32, AutoPtr<UserServices> > mUserServices;

    /**
     * This file contains the list of known services. We would like to maintain this forever
     * so we store it as an XML file.
     */
    AutoPtr<IAtomicFile> mPersistentServicesFile;

    // the listener and handler are synchronized on "this" and must be updated together
    AutoPtr<IRegisteredServicesCacheListener> mListener;
    AutoPtr<IHandler> mHandler;

    AutoPtr<IBroadcastReceiver> mPackageReceiver;

    AutoPtr<IBroadcastReceiver> mExternalReceiver;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_CONTENT_PM_REGISTEREDSERVICESCACHE_H__
