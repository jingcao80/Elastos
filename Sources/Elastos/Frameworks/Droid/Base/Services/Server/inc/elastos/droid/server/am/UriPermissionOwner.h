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

#ifndef __ELASTOS_DROID_SERVER_AM_URIPERMISSIONOWNER_H__
#define __ELASTOS_DROID_SERVER_AM_URIPERMISSIONOWNER_H__

#include "elastos/droid/server/am/UriPermission.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

class UriPermissionOwner : public Object
{
public:
    UriPermissionOwner(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IObject* owner,
        /* [in] */ Boolean strongOwner = FALSE);

    ~UriPermissionOwner();

    CARAPI_(AutoPtr<IBinder>) GetExternalTokenLocked();

    static CARAPI_(AutoPtr<UriPermissionOwner>) FromExternalToken(
        /* [in] */ IBinder* token);

    CARAPI RemoveUriPermissionsLocked();

    CARAPI RemoveUriPermissionsLocked(
        /* [in] */ Int32 mode);

    CARAPI RemoveUriPermissionLocked(
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 mode);

    CARAPI AddReadPermission(
        /* [in] */ UriPermission* perm);

    CARAPI AddWritePermission(
        /* [in] */ UriPermission* perm);

    CARAPI RemoveReadPermission(
        /* [in] */ UriPermission* perm);

    CARAPI RemoveWritePermission(
        /* [in] */ UriPermission* perm);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

public:
    CActivityManagerService* mService;  // weak-ref
    IObject* mOwner;
    // if mStrongOwner is TRUE, this hold mOwner's reference,
    // otherwise mOwner hold this's reference
    Boolean mStrongOwner;

    AutoPtr<IBinder> mExternalToken;
    AutoPtr< HashSet< AutoPtr<UriPermission> > > mReadPerms; // special access to reading uris.
    AutoPtr< HashSet< AutoPtr<UriPermission> > > mWritePerms; // special access to writing uris.
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::UriPermissionOwner)

#endif //__ELASTOS_DROID_SERVER_AM_URIPERMISSIONOWNER_H__
