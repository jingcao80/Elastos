
#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUME_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUME_H__

#include "_Elastos_Droid_Os_Storage_CStorageVolume.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageVolume)
{
public:
    CStorageVolume();

    ~CStorageVolume();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFile* path,
        /* [in] */ Int32 descriptionId,
        /* [in] */ Boolean primary,
        /* [in] */ Boolean removable,
        /* [in] */ Boolean emulated,
        /* [in] */ Int32 mtpReserveSpace,
        /* [in] */ Boolean allowMassStorage,
        /* [in] */ Int64 maxFileSize,
        /* [in] */ IUserHandle* owner);

//    static IStorageVolume* FromTemplate(
//        /* [in] */ IStorageVolume* temp,
//        /* [in] */ IFile* path,
//        /* [in] */ IUserHandle* owner);

    CARAPI GetPath(
        /* [out] */ String * pMPath);

    CARAPI GetPathFile(
        /* [out] */ IFile** pPathFile);

    CARAPI GetDescription(
        /* [in] */ IContext * pContext,
        /* [out] */ String * pDesc);

    CARAPI GetDescriptionId(
        /* [out] */ Int32 * pMDescriptionId);

    CARAPI IsPrimary(
        /* [out] */ Boolean* result);

    CARAPI IsRemovable(
        /* [out] */ Boolean * pMRemovable);

    CARAPI IsEmulated(
        /* [out] */ Boolean * pMEmulated);

    CARAPI GetStorageId(
        /* [out] */ Int32 * pMStorageId);

    CARAPI SetStorageId(
        /* [in] */ Int32 index);

    CARAPI GetMtpReserveSpace(
        /* [out] */ Int32 * pMMtpReserveSpace);

    CARAPI AllowMassStorage(
        /* [out] */ Boolean * pMAllowMassStorage);

    CARAPI GetMaxFileSize(
        /* [out] */ Int64 * pMMaxFileSize);

    CARAPI GetOwner(
        /* [out] */ IUserHandle** pOwner);

    CARAPI Equals(
        /* [in] */ IStorageVolume * pObj,
        /* [out] */ Boolean * pEqu);

    CARAPI Equals(
        /* [in] */ IInterface * pObj,
        /* [out] */ Boolean * pEqu);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pCode);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI WriteToParcel(
        /* [in] */ IParcel * pParcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

public:
    // StorageVolume extra for ACTION_MEDIA_REMOVED, ACTION_MEDIA_UNMOUNTED, ACTION_MEDIA_CHECKING,
    // ACTION_MEDIA_NOFS, ACTION_MEDIA_MOUNTED, ACTION_MEDIA_SHARED, ACTION_MEDIA_UNSHARED,
    // ACTION_MEDIA_BAD_REMOVAL, ACTION_MEDIA_UNMOUNTABLE and ACTION_MEDIA_EJECT broadcasts.
    static const String EXTRA_STORAGE_VOLUME;// = "storage_volume";

private:
    static const char* TAG;// = "StorageVolume";
    Int32 mStorageId;
    AutoPtr<IFile> mPath;
    Int32 mDescriptionId;
    Boolean mPrimary;
    Boolean mRemovable;
    Boolean mEmulated;
    Int32 mMtpReserveSpace;
    Boolean mAllowMassStorage;
    // maximum file size for the storage, or zero for no limit
    Int64 mMaxFileSize;
    /** When set, indicates exclusive ownership of this volume */
    AutoPtr<IUserHandle> mOwner;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUME_H__
