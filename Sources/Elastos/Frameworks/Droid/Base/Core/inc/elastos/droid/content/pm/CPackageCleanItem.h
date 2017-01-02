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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGECLEANITEM_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGECLEANITEM_H__

#include "_Elastos_Droid_Content_Pm_CPackageCleanItem.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/** @hide */
CarClass(CPackageCleanItem)
    , public Object
    , public IPackageCleanItem
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageCleanItem();

    ~CPackageCleanItem();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 userId,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean andCode);

    CARAPI Equals(
        /* [in] */ IPackageCleanItem* obj,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetUserId(
        /* [out] */ Int32* userId);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI GetAndCode(
        /* [out] */ Boolean* andCode);

    CARAPI SetUserId(
        /* [in] */ Int32 userId);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI SetAndCode(
        /* [in] */ Boolean andCode);


public:
    Int32 mUserId;
    String mPackageName;
    Boolean mAndCode;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGECLEANITEM_H__
