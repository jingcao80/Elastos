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

#ifndef __ELASTOS_DROID_ACCOUNTS_IACCOUNTAUTHENTICATORCACHE_H__
#define __ELASTOS_DROID_ACCOUNTS_IACCOUNTAUTHENTICATORCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/RegisteredServicesCache.h"

using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheListener;
using Elastos::Droid::Content::Pm::RegisteredServicesCache;

namespace Elastos {
namespace Droid {
namespace Accounts {

interface IAccountAuthenticatorCache : public IInterface
{
    virtual CARAPI ParseServiceAttributes(
        /* [in] */ IResources* res,
        /* [in] */ const String& packageName,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** attributes) = 0;

    /**
     * Accessor for the {@link android.content.pm.RegisteredServicesCache.ServiceInfo} that
     * matched the specified {@link android.accounts.AuthenticatorDescription} or null
     * if none match.
     * @param type the authenticator type to return
     * @return the {@link android.content.pm.RegisteredServicesCache.ServiceInfo} that
     * matches the account type or null if none is present
     */
    virtual CARAPI_(AutoPtr<RegisteredServicesCache::ServiceInfo>) GetServiceInfo(
        /* [in] */ IAuthenticatorDescription* type,
        /* [in] */ Int32 userId) = 0;

    /**
     * @return A copy of a Collection of all the current Authenticators.
     */
    virtual CARAPI_(AutoPtr<List<AutoPtr<RegisteredServicesCache::ServiceInfo> > >)GetAllServices(
        /* [in] */ Int32 userId) = 0;

    /**
     * Dumps the state of the cache. See
     * {@link android.os.Binder#dump(java.io.FileDescriptor, java.io.PrintWriter, String[])}
     */
    virtual CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* fout,
        /* [in] */ const ArrayOf<String>& args,
        /* [in] */ Int32 userId) = 0;

    /**
     * Sets a listener that will be notified whenever the authenticator set changes
     * @param listener the listener to notify, or null
     * @param handler the {@link Handler} on which the notification will be posted. If null
     * the notification will be posted on the main thread.
     */
    virtual CARAPI_(void) SetListener(
        /* [in] */ IRegisteredServicesCacheListener* listener,
        /* [in] */ IHandler* handler) = 0;

    virtual CARAPI_(void) InvalidateCache(
        /* [in] */ Int32 userId) = 0;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_IACCOUNTAUTHENTICATORCACHE_H__
