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

#include "elastos/droid/server/media/projection/CMediaProjectionManager.h"
#include "elastos/droid/server/media/projection/CMediaProjection.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::Projection::EIID_IIMediaProjection;
using Elastos::Droid::Media::Projection::EIID_IIMediaProjectionManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {
namespace Projection {

CAR_INTERFACE_IMPL_2(CMediaProjectionManager, Object, IIMediaProjectionManager, IBinder)

CAR_OBJECT_IMPL(CMediaProjectionManager)

ECode CMediaProjectionManager::constructor(
    /* [in] */ HANDLE host)
{
    mHost = (MediaProjectionManagerService*)host;
    return NOERROR;
}

ECode CMediaProjectionManager::HasProjectionPermission(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 token = Binder::ClearCallingIdentity();
    Boolean hasPermission = FALSE;
    // try {
    Int32 value;
    mHost->mAppOps->NoteOpNoThrow(IAppOpsManager::OP_PROJECT_MEDIA, uid, packageName, &value);
    hasPermission |= CheckPermission(packageName, Elastos::Droid::Manifest::permission::CAPTURE_VIDEO_OUTPUT)
            || value == IAppOpsManager::MODE_ALLOWED;
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    *result = hasPermission;
    return NOERROR;
}

ECode CMediaProjectionManager::CreateProjection(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 type,
    /* [in] */ Boolean isPermanentGrant,
    /* [out] */ IIMediaProjection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 perm;
    mHost->mContext->CheckCallingPermission(
            Elastos::Droid::Manifest::permission::MANAGE_MEDIA_PROJECTION, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E("CMediaProjectionManager", "Requires MANAGE_MEDIA_PROJECTION in order to grant projection permission");
        return E_SECURITY_EXCEPTION;
    }
    if (packageName.IsNull() || packageName.IsEmpty()) {
        Slogger::E("CMediaProjectionManager", "package name must not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 callingToken = Binder::ClearCallingIdentity();
    AutoPtr<CMediaProjection> projection;
    // try {
    CMediaProjection::NewByFriend(type, uid, packageName, (HANDLE)this, (CMediaProjection**)&projection);
    if (isPermanentGrant) {
        mHost->mAppOps->SetMode(IAppOpsManager::OP_PROJECT_MEDIA,
                projection->mUid, projection->mPackageName, IAppOpsManager::MODE_ALLOWED);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(callingToken);
    // }
    Binder::RestoreCallingIdentity(callingToken);
    *result = (IIMediaProjection*)projection->Probe(EIID_IIMediaProjection);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaProjectionManager::IsValidMediaProjection(
    /* [in] */ IIMediaProjection* projection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->IsValidMediaProjection(IBinder::Probe(projection));
    return NOERROR;
}

ECode CMediaProjectionManager::GetActiveProjectionInfo(
    /* [out] */ IMediaProjectionInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Int32 perm;
    mHost->mContext->CheckCallingPermission(
            Elastos::Droid::Manifest::permission::MANAGE_MEDIA_PROJECTION, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E("CMediaProjectionManager", "Requires MANAGE_MEDIA_PROJECTION in order to add projection callbacks");
        return E_SECURITY_EXCEPTION;
    }
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IMediaProjectionInfo> temp = mHost->GetActiveProjectionInfo();
    *result = temp;
    REFCOUNT_ADD(*result)
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
}

ECode CMediaProjectionManager::StopActiveProjection()
{
    Int32 perm;
    mHost->mContext->CheckCallingPermission(
            Elastos::Droid::Manifest::permission::MANAGE_MEDIA_PROJECTION, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E("CMediaProjectionManager", "Requires MANAGE_MEDIA_PROJECTION in order to add projection callbacks");
        return E_SECURITY_EXCEPTION;
    }
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    if (mHost->mProjectionGrant != NULL) {
        mHost->mProjectionGrant->Stop();
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaProjectionManager::AddCallback(
    /* [in] */ IIMediaProjectionWatcherCallback* cb)
{
    Int32 perm;
    mHost->mContext->CheckCallingPermission(
            Elastos::Droid::Manifest::permission::MANAGE_MEDIA_PROJECTION, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E("CMediaProjectionManager", "Requires MANAGE_MEDIA_PROJECTION in order to add projection callbacks");
        return E_SECURITY_EXCEPTION;
    }
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    mHost->AddCallback(cb);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaProjectionManager::RemoveCallback(
    /* [in] */ IIMediaProjectionWatcherCallback* cb)
{
    Int32 perm;
    mHost->mContext->CheckCallingPermission(
            Elastos::Droid::Manifest::permission::MANAGE_MEDIA_PROJECTION, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E("CMediaProjectionManager", "Requires MANAGE_MEDIA_PROJECTION in order to remove projection callbacks");
        return E_SECURITY_EXCEPTION;
    }
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    mHost->RemoveCallback(cb);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

Boolean CMediaProjectionManager::CheckPermission(
    /* [in] */ const String& packageName,
    /* [in] */ const String& permission)
{
    AutoPtr<IPackageManager> pm;
    mHost->mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 perm;
    pm->CheckPermission(permission, packageName, &perm);
    return perm == IPackageManager::PERMISSION_GRANTED;
}

ECode CMediaProjectionManager::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Projection
} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
