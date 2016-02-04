
#ifndef __ELASTOS_DROID_SERVER_AM_URIPERMISSION_H__
#define __ELASTOS_DROID_SERVER_AM_URIPERMISSION_H__

#include "elastos/droid/server/am/GrantUri.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Content::IUriPermission;
using Elastos::Droid::Net::IUri;
using Elastos::Core::IComparator;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class UriPermissionOwner;

class UriPermission : public Object
{
public:
    class PersistedTimeComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);
    };

    /**
     * Snapshot of {@link UriPermission} with frozen
     * {@link UriPermission#mPersistedModeFlags} state.
     */
    class Snapshot : public Object
    {
        friend class UriPermission;
    private:
        Snapshot(
            /* [in] */ UriPermission* perm)
            : mTargetUserId(perm->mTargetUserId)
            , mSourcePkg(perm->mSourcePkg)
            , mTargetPkg(perm->mTargetPkg)
            , mUri(perm->mUri)
            , mPersistedModeFlags(perm->mPersistedModeFlags)
            , mPersistedCreateTime(perm->mPersistedCreateTime)
        {
        }

    public:
        const Int32 mTargetUserId;
        const String mSourcePkg;
        const String mTargetPkg;
        const AutoPtr<GrantUri> mUri;
        const Int32 mPersistedModeFlags;
        const Int64 mPersistedCreateTime;
    };

public:
    UriPermission(
        /* [in] */ const String& sourcePkg,
        /* [in] */ const String& targetPkg,
        /* [in] */ Int32 targetUid,
        /* [in] */ GrantUri* uri);

    /**
     * Initialize persisted modes as read from file. This doesn't issue any
     * global or owner grants.
     */
    CARAPI_(void) InitPersistedModes(
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int64 createdTime);

    CARAPI_(void) GrantModes(
        /* [in] */ Int32 modeFlags,
        /* [in] */ UriPermissionOwner* owner);

    /**
     * @return if mode changes should trigger persisting.
     */
    CARAPI_(Boolean) TakePersistableModes(
        /* [in] */ Int32 modeFlags);

    CARAPI_(Boolean) ReleasePersistableModes(
        /* [in] */ Int32 modeFlags);

    /**
     * @return if mode changes should trigger persisting.
     */
    CARAPI_(Boolean) RevokeModes(
        /* [in] */ Int32 modeFlags,
        /* [in] */ Boolean includingOwners);

    /**
     * Return strength of this permission grant for the given flags.
     */
    CARAPI_(Int32) GetStrength(
        /* [in] */ Int32 modeFlags);

    /**
     * Remove given read owner, updating {@Link #modeFlags} as needed.
     */
    CARAPI_(void) RemoveReadOwner(
        /* [in] */ UriPermissionOwner* owner);

    /**
     * Remove given write owner, updating {@Link #modeFlags} as needed.
     */
    CARAPI_(void) RemoveWriteOwner(
        /* [in] */ UriPermissionOwner* owner);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(AutoPtr<Snapshot>) GetSnapshot();

    CARAPI_(AutoPtr<IUriPermission>) BuildPersistedPublicApiObject();

    static CARAPI_(Int32) PersistedTimeCompare(
        /* [in] */ UriPermission* lhs,
        /* [in] */ UriPermission* rhs);

private:
    CARAPI_(void) UpdateModeFlags();

    CARAPI_(void) AddReadOwner(
        /* [in] */ UriPermissionOwner* owner);

    CARAPI_(void) AddWriteOwner(
        /* [in] */ UriPermissionOwner* owner);

public:
    static const Int32 STRENGTH_NONE = 0;
    static const Int32 STRENGTH_OWNED = 1;
    static const Int32 STRENGTH_GLOBAL = 2;
    static const Int32 STRENGTH_PERSISTABLE = 3;

    Int32 mTargetUserId;
    const String mSourcePkg;
    const String mTargetPkg;

    /** Cached UID of {@link #targetPkg}; should not be persisted */
    const Int32 mTargetUid;

    const AutoPtr<GrantUri> mUri;

    /**
     * Allowed modes. All permission enforcement should use this field. Must
     * always be a combination of {@link #mOwnedModeFlags},
     * {@link #mGlobalModeFlags}, {@link #mPersistableModeFlags}, and
     * {@link #mPersistedModeFlags}. Mutations <em>must</em> only be performed by
     * the owning class.
     */
    Int32 mModeFlags;

    /** Allowed modes with active owner. */
    Int32 mOwnedModeFlags;
    /** Allowed modes without explicit owner. */
    Int32 mGlobalModeFlags;
    /** Allowed modes that have been offered for possible persisting. */
    Int32 mPersistableModeFlags;

    /** Allowed modes that should be persisted across device boots. */
    Int32 mPersistedModeFlags;

    /**
     * Timestamp when {@link #mPersistedModeFlags} was first defined in
     * {@link System#currentTimeMillis()} time base.
     */
    Int64 mPersistedCreateTime;

private:
    static const String TAG;
    static const Int64 INVALID_TIME;

    AutoPtr<HashSet<AutoPtr<UriPermissionOwner> > > mReadOwners;
    AutoPtr<HashSet<AutoPtr<UriPermissionOwner> > > mWriteOwners;

    String mStringName;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::UriPermission)

#endif //__ELASTOS_DROID_SERVER_AM_URIPERMISSION_H__
