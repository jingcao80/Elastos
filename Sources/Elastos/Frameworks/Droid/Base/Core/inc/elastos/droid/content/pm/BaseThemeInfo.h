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

#ifndef __ELASTOS_DROID_CONTENT_PM_BASETHEMEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_BASETHEMEINFO_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

class BaseThemeInfo
    : public Object
    , public IBaseThemeInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BaseThemeInfo() {}

    CARAPI constructor();

    CARAPI GetResolvedString(
        /* [in] */ IResources* res,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    CARAPI GetThemeId(
        /* [out] */ String* themeId);

    CARAPI SetThemeId(
        /* [in] */ const String& themeId);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetAuthor(
        /* [out] */ String* author);

    CARAPI SetAuthor(
        /* [in] */ const String& author);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /*
     * Flatten this object in to a Parcel.
     *
     * @param dest The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     * May be 0 or {@link #PARCELABLE_WRITE_RETURN_VALUE}.
     *
     * @see android.os.Parcelable#writeToParcel(android.os.Parcel, int)
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /**
     * The theme id, which does not change when the theme is modified.
     * Specifies an Android UI Style using style name.
     *
     * @see themeId attribute
     *
     */
    String mThemeId;

    /**
     * The name of the theme (as displayed by UI).
     *
     * @see name attribute
     *
     */
    String mName;

    /**
     * The author name of the theme package.
     *
     * @see author attribute
     *
     */
    String mAuthor;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_BASETHEMEINFO_H__
