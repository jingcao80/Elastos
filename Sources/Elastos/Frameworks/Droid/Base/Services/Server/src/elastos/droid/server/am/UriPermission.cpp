
#include "elastos/droid/server/am/UriPermission.h"
#include "elastos/droid/server/am/UriPermissionOwner.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CUriPermission;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(UriPermission::PersistedTimeComparator, Object, IComparator)

ECode UriPermission::PersistedTimeComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    UriPermission* lhs = (UriPermission*)IObject::Probe(_lhs);
    UriPermission* rhs = (UriPermission*)IObject::Probe(_rhs);
    *result = PersistedTimeCompare(lhs, rhs);
    return NOERROR;
}

const String UriPermission::TAG("UriPermission");
const Int32 UriPermission::STRENGTH_NONE;
const Int32 UriPermission::STRENGTH_OWNED;
const Int32 UriPermission::STRENGTH_GLOBAL;
const Int32 UriPermission::STRENGTH_PERSISTABLE;
const Int64 UriPermission::INVALID_TIME = Elastos::Core::Math::INT32_MIN_VALUE;

UriPermission::UriPermission(
    /* [in] */ const String& sourcePkg,
    /* [in] */ const String& targetPkg,
    /* [in] */ Int32 targetUid,
    /* [in] */ GrantUri* uri)
    : mSourcePkg(sourcePkg)
    , mTargetPkg(targetPkg)
    , mTargetUid(targetUid)
    , mUri(uri)
    , mModeFlags(0)
    , mOwnedModeFlags(0)
    , mGlobalModeFlags(0)
    , mPersistableModeFlags(0)
    , mPersistedModeFlags(0)
    , mPersistedCreateTime(INVALID_TIME)
{
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetUserId(targetUid, &mTargetUserId);
}

void UriPermission::UpdateModeFlags()
{
    mModeFlags = mOwnedModeFlags | mGlobalModeFlags | mPersistableModeFlags | mPersistedModeFlags;
}

void UriPermission::InitPersistedModes(
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int64 createdTime)
{
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    mPersistableModeFlags = modeFlags;
    mPersistedModeFlags = modeFlags;
    mPersistedCreateTime = createdTime;

    UpdateModeFlags();
}

void UriPermission::GrantModes(
    /* [in] */ Int32 modeFlags,
    /* [in] */ UriPermissionOwner* owner)
{
    Boolean persistable = (modeFlags & IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION) != 0;
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    if (persistable) {
        mPersistableModeFlags |= modeFlags;
    }

    if (owner == NULL) {
        mGlobalModeFlags |= modeFlags;
    }
    else {
        if ((modeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0) {
            AddReadOwner(owner);
        }
        if ((modeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0) {
            AddWriteOwner(owner);
        }
    }

    UpdateModeFlags();
}

Boolean UriPermission::TakePersistableModes(
    /* [in] */ Int32 modeFlags)
{
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    if ((modeFlags & mPersistableModeFlags) != modeFlags) {
        Slogger::W(TAG, "Requested flags 0x%x, but only 0x%x are allowed",
                modeFlags, mPersistableModeFlags);
        return FALSE;
    }

    const Int32 before = mPersistedModeFlags;
    mPersistedModeFlags |= (mPersistableModeFlags & modeFlags);

    if (mPersistedModeFlags != 0) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mPersistedCreateTime);
    }

    UpdateModeFlags();
    return mPersistedModeFlags != before;
}

Boolean UriPermission::ReleasePersistableModes(
    /* [in] */ Int32 modeFlags)
{
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    const Int32 before = mPersistedModeFlags;

    mPersistableModeFlags &= ~modeFlags;
    mPersistedModeFlags &= ~modeFlags;

    if (mPersistedModeFlags == 0) {
        mPersistedCreateTime = INVALID_TIME;
    }

    UpdateModeFlags();
    return mPersistedModeFlags != before;
}

Boolean UriPermission::RevokeModes(
    /* [in] */ Int32 modeFlags,
    /* [in] */ Boolean includingOwners)
{
    const Boolean persistable = (modeFlags & IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION) != 0;
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    const Int32 before = mPersistedModeFlags;

    if ((modeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0) {
        if (persistable) {
            mPersistableModeFlags &= ~IIntent::FLAG_GRANT_READ_URI_PERMISSION;
            mPersistedModeFlags &= ~IIntent::FLAG_GRANT_READ_URI_PERMISSION;
        }
        mGlobalModeFlags &= ~IIntent::FLAG_GRANT_READ_URI_PERMISSION;
        if (mReadOwners != NULL && includingOwners) {
            mOwnedModeFlags &= ~IIntent::FLAG_GRANT_READ_URI_PERMISSION;
            HashSet<AutoPtr<UriPermissionOwner> >::Iterator iter;
            for (iter = mReadOwners->Begin(); iter != mReadOwners->End(); ++iter) {
                (*iter)->RemoveReadPermission(this);
            }
            mReadOwners = NULL;
        }
    }
    if ((modeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0) {
        if (persistable) {
            mPersistableModeFlags &= ~IIntent::FLAG_GRANT_WRITE_URI_PERMISSION;
            mPersistedModeFlags &= ~IIntent::FLAG_GRANT_WRITE_URI_PERMISSION;
        }
        mGlobalModeFlags &= ~IIntent::FLAG_GRANT_WRITE_URI_PERMISSION;
        if (mWriteOwners != NULL && includingOwners) {
            mOwnedModeFlags &= ~IIntent::FLAG_GRANT_WRITE_URI_PERMISSION;
            HashSet<AutoPtr<UriPermissionOwner> >::Iterator iter;
            for (iter = mWriteOwners->Begin(); iter != mWriteOwners->End(); ++iter) {
                (*iter)->RemoveWritePermission(this);
            }
            mWriteOwners = NULL;
        }
    }

    if (mPersistedModeFlags == 0) {
        mPersistedCreateTime = INVALID_TIME;
    }

    UpdateModeFlags();
    return mPersistedModeFlags != before;
}

Int32 UriPermission::GetStrength(
    /* [in] */ Int32 modeFlags)
{
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
    if ((mPersistableModeFlags & modeFlags) == modeFlags) {
        return STRENGTH_PERSISTABLE;
    }
    else if ((mGlobalModeFlags & modeFlags) == modeFlags) {
        return STRENGTH_GLOBAL;
    }
    else if ((mOwnedModeFlags & modeFlags) == modeFlags) {
        return STRENGTH_OWNED;
    }
    else {
        return STRENGTH_NONE;
    }
}

void UriPermission::AddReadOwner(
    /* [in] */ UriPermissionOwner* owner)
{
    if (mReadOwners == NULL) {
        mReadOwners = new HashSet<AutoPtr<UriPermissionOwner> >;
        mOwnedModeFlags |= IIntent::FLAG_GRANT_READ_URI_PERMISSION;
        UpdateModeFlags();
    }
    if (mReadOwners->Insert(owner).mSecond) {
        owner->AddReadPermission(this);
    }
}

void UriPermission::RemoveReadOwner(
    /* [in] */ UriPermissionOwner* owner)
{
    if (mReadOwners->Erase(owner) == 0) {
        Slogger::W/*Wtf*/(TAG, "Unknown read owner %s in %s",
            owner->ToString().string(), ToString().string());
    }
    if (mReadOwners->GetSize() == 0) {
        mReadOwners = NULL;
        mOwnedModeFlags &= ~IIntent::FLAG_GRANT_READ_URI_PERMISSION;
        UpdateModeFlags();
    }
}

void UriPermission::AddWriteOwner(
    /* [in] */ UriPermissionOwner* owner)
{
    if (mWriteOwners == NULL) {
        mWriteOwners = new HashSet<AutoPtr<UriPermissionOwner> >;
        mOwnedModeFlags |= IIntent::FLAG_GRANT_WRITE_URI_PERMISSION;
        UpdateModeFlags();
    }
    if (mWriteOwners->Insert(owner).mSecond) {
        owner->AddWritePermission(this);
    }
}

void UriPermission::RemoveWriteOwner(
    /* [in] */ UriPermissionOwner* owner)
{
    if (mWriteOwners->Erase(owner) == 0) {
        Slogger::W/*Wtf*/(TAG, "Unknown write owner %s in %s",
            owner->ToString().string(), ToString().string());
    }
    if (mWriteOwners->GetSize() == 0) {
        mWriteOwners = NULL;
        mOwnedModeFlags &= ~IIntent::FLAG_GRANT_WRITE_URI_PERMISSION;
        UpdateModeFlags();
    }
}

ECode UriPermission::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

// @Override
String UriPermission::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }

    StringBuilder sb(128);
    sb += "UriPermission{";
    sb += StringUtils::ToString((Int32)this, 16);
    sb += ' ';
    sb += mUri->ToString();
    sb += '}';
    mStringName = sb.ToString();
    return mStringName;
}

void UriPermission::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("targetUserId="));
    pw->Print(mTargetUserId);
    pw->Print(String(" sourcePkg="));
    pw->Print(mSourcePkg);
    pw->Print(String(" targetPkg="));
    pw->Println(mTargetPkg);

    pw->Print(prefix);
    pw->Print(String("mode=0x"));
    pw->Print(StringUtils::ToString(mModeFlags, 16));
    pw->Print(String(" owned=0x"));
    pw->Print(StringUtils::ToString(mOwnedModeFlags, 16));
    pw->Print(String(" global=0x"));
    pw->Print(StringUtils::ToString(mGlobalModeFlags, 16));
    pw->Print(String(" persistable=0x"));
    pw->Print(StringUtils::ToString(mPersistableModeFlags, 16));
    pw->Print(String(" persisted=0x"));
    pw->Print(StringUtils::ToString(mPersistedModeFlags, 16));
    if (mPersistedCreateTime != INVALID_TIME) {
        pw->Print(String(" persistedCreate="));
        pw->Print(mPersistedCreateTime);
    }
    pw->Println();

    if (mReadOwners != NULL) {
        pw->Print(prefix);
        pw->Println(String("readOwners:"));
        HashSet<AutoPtr<UriPermissionOwner> >::Iterator iter;
        for (iter = mReadOwners->Begin(); iter != mReadOwners->End(); ++iter) {
            pw->Print(prefix);
            pw->Print(String("  * "));
            pw->Println((*iter)->ToString());
        }
    }
    if (mWriteOwners != NULL) {
        pw->Print(prefix);
        pw->Println(String("writeOwners:"));
        HashSet<AutoPtr<UriPermissionOwner> >::Iterator iter;
        for (iter = mWriteOwners->Begin(); iter != mWriteOwners->End(); ++iter) {
            pw->Print(prefix);
            pw->Println(String("  * "));
            pw->Print((*iter)->ToString());
        }
    }
}

AutoPtr<UriPermission::Snapshot> UriPermission::GetSnapshot()
{
    return new Snapshot(this);
}

AutoPtr<IUriPermission> UriPermission::BuildPersistedPublicApiObject()
{
    AutoPtr<IUriPermission> perm;
    CUriPermission::New(mUri->mUri, mPersistedModeFlags, mPersistedCreateTime,
        (IUriPermission**)&perm);
    return perm;
}

Int32 UriPermission::PersistedTimeCompare(
    /* [in] */ UriPermission* lhs,
    /* [in] */ UriPermission* rhs)
{
    assert(lhs && rhs);
    return lhs->mPersistedCreateTime < rhs->mPersistedCreateTime ?
        -1 : (lhs->mPersistedCreateTime == rhs->mPersistedCreateTime ? 0 : 1);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
