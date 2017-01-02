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

#ifndef __ELASTOS_DROID_APP_BACKUP_CRESTOREDESCRIPTION_H__
#define __ELASTOS_DROID_APP_BACKUP_CRESTOREDESCRIPTION_H__

#include "_Elastos_Droid_App_Backup_CRestoreDescription.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CRestoreDescription)
    , public Object
    , public IRestoreDescription
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRestoreDescription();

    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 dataType);

    CARAPI GetPackageName(
        /* [out] */ String* name);

    CARAPI GetDataType(
        /* [out] */ Int32* type);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // ---------------------------------------
    // Parcelable implementation - not used by transport

    //@Override
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    // public static final Parcelable.Creator<RestoreDescription> CREATOR
    //         = new Parcelable.Creator<RestoreDescription>() {
    //     public RestoreDescription createFromParcel(Parcel in) {
    //         final RestoreDescription unparceled = new RestoreDescription(in);
    //         return (NO_MORE_PACKAGES_SENTINEL.equals(unparceled.mPackageName))
    //                 ? NO_MORE_PACKAGES
    //                 : unparceled;
    //     }

    //     public RestoreDescription[] newArray(int size) {
    //         return new RestoreDescription[size];
    //     }
    // };

    /**
     * Return this constant RestoreDescription from BackupTransport.nextRestorePackage()
     * to indicate that no more package data is available in the current restore operation.
     */
    static AutoPtr<IRestoreDescription> NO_MORE_PACKAGES;

private:
    CRestoreDescription(
        /* [in] */ IParcel* in);

private:
    String mPackageName;
    Int32 mDataType;

    static const String NO_MORE_PACKAGES_SENTINEL;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_APP_BACKUP_CRESTOREDESCRIPTION_H__
