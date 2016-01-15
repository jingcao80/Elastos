
#include "elastos/droid/server/am/UriPermissionOwner.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/CUriPermissionOwnerExternalToken.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

//==============================================================================
// UriPermissionOwner
//==============================================================================

UriPermissionOwner::UriPermissionOwner(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IObject* owner,
    /* [in] */ Boolean strongOwner)
    : mService(service)
    , mOwner(owner)
    , mStrongOwner(strongOwner)
{
    if (mStrongOwner)
        mOwner->AddRef();
}

UriPermissionOwner::~UriPermissionOwner()
{
    if (mStrongOwner)
        mOwner->Release();
}

AutoPtr<IBinder> UriPermissionOwner::GetExternalTokenLocked()
{
    if (mExternalToken == NULL) {
        AutoPtr<IBinder> token;
        CUriPermissionOwnerExternalToken::New((Handle32)this, (IBinder**)&token);
        // this method is only called by CActivityManagerService::NewUriPermissionOwner,
        // in NewUriPermissionOwner, token hold owner's reference. add by xihao
        //
        // mExternalToken = IUriPermissionOwnerExternalToken::Probe(token);
        return token;
    }

    return IBinder::Probe(mExternalToken);
}

AutoPtr<UriPermissionOwner> UriPermissionOwner::FromExternalToken(
    /* [in] */ IBinder* token)
{
    Handle32 owner = 0;
    if (IUriPermissionOwnerExternalToken::Probe(token) != NULL) {
        IUriPermissionOwnerExternalToken::Probe(token)->GetOwner(&owner);
    }

    return (UriPermissionOwner*)owner;
}

ECode UriPermissionOwner::RemoveUriPermissionsLocked()
{
    return RemoveUriPermissionsLocked(IIntent::FLAG_GRANT_READ_URI_PERMISSION
        | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
}

ECode UriPermissionOwner::RemoveUriPermissionsLocked(
    /* [in] */ Int32 mode)
{
    return RemoveUriPermissionLocked(NULL, mode);
}

ECode UriPermissionOwner::RemoveUriPermissionLocked(
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 mode)
{
    if ((mode & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0
            && mReadPerms != NULL) {
        HashSet< AutoPtr<UriPermission> >::Iterator it = mReadPerms->Begin();
        while (it != mReadPerms->End()) {
            UriPermission* perm = (*it);
            if (grantUri == NULL || grantUri->Equals(perm->mUri)) {
                perm->RemoveReadOwner(this);
                mService->RemoveUriPermissionIfNeededLocked(perm);
                mReadPerms->Erase(it++);
            }
            else {
                it++;
            }
        }
        if (mReadPerms->IsEmpty()) {
            mReadPerms = NULL;
        }
    }
    if ((mode & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0
            && mWritePerms != NULL) {
        HashSet< AutoPtr<UriPermission> >::Iterator it = mWritePerms->Begin();
        while (it != mWritePerms->End()) {
            UriPermission* perm = (*it);
            if (grantUri == NULL || grantUri->Equals(perm->mUri)) {
                perm->RemoveWriteOwner(this);
                mService->RemoveUriPermissionIfNeededLocked(perm);
                mWritePerms->Erase(it++);
            }
            else {
                ++it;
            }
        }
        if (mWritePerms->IsEmpty()) {
            mWritePerms = NULL;
        }
    }
    return NOERROR;
}

ECode UriPermissionOwner::AddReadPermission(
    /* [in] */ UriPermission* perm)
{
    if (mReadPerms == NULL) {
        mReadPerms = new HashSet<AutoPtr<UriPermission> >();
    }
    mReadPerms->Insert(perm);
    return NOERROR;
}

ECode UriPermissionOwner::AddWritePermission(
    /* [in] */ UriPermission* perm)
{
    if (mWritePerms == NULL) {
        mWritePerms = new HashSet< AutoPtr<UriPermission> >();
    }
    mWritePerms->Insert(perm);
    return NOERROR;
}

ECode UriPermissionOwner::RemoveReadPermission(
    /* [in] */ UriPermission* perm)
{
    if (perm != NULL) {
        if (mReadPerms != NULL) {
            mReadPerms->Erase(perm);
            if (mReadPerms->IsEmpty()) {
                mReadPerms = NULL;
            }
        }
    }
    return NOERROR;
}

ECode UriPermissionOwner::RemoveWritePermission(
    /* [in] */ UriPermission* perm)
{
    if (perm != NULL) {
        if (mWritePerms != NULL) {
            mWritePerms->Erase(perm);
            if (mWritePerms->IsEmpty()) {
                mWritePerms = NULL;
            }
        }
    }
    return NOERROR;
}

void UriPermissionOwner::Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix)
{
    if (mReadPerms != NULL) {
        pw->Print(prefix);
        pw->Print(String("readUriPermissions="));
        pw->Println(/*mReadPerms*/);
    }
    if (mWritePerms != NULL) {
        pw->Print(prefix);
        pw->Print(String("writeUriPermissions="));
        pw->Println(/*mWritePerms*/);
    }
}

ECode UriPermissionOwner::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

String UriPermissionOwner::ToString()
{
    StringBuilder sb("UriPermissionOwner:");
    sb += mOwner;
    return sb.ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
