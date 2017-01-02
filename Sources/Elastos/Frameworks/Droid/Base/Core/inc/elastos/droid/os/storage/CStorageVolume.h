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

#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUME_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUME_H__

#include "_Elastos_Droid_Os_Storage_CStorageVolume.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageVolume)
    , public Object
    , public IStorageVolume
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

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

    CARAPI SetUuid(
        /* [in] */ const String& uuid);

    CARAPI GetUuid(
        /* [out] */ String* uuid);

    /**
     * Parse and return volume UUID as FAT volume ID, or return -1 if unable to
     * parse or UUID is unknown.
     */
    CARAPI GetFatVolumeId(
        /* [out] */ Int32* id);

    CARAPI SetUserLabel(
        /* [in] */ const String& userLabel);

    CARAPI GetUserLabel(
        /* [out] */ String* userLabel);

    CARAPI SetState(
        /* [in] */ const String& state);

    CARAPI GetState(
        /* [out] */ String* state);

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

    String mUuid;
    String mUserLabel;
    String mState;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUME_H__
