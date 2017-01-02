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

#ifndef __ELASTOS_DINTERNAL_ROID_STATUSBAR_CSTATUSBARICON_H__
#define __ELASTOS_DINTERNAL_ROID_STATUSBAR_CSTATUSBARICON_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Internal_StatusBar_CStatusBarIcon.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace StatusBar {

CarClass(CStatusBarIcon)
    , public Object
    , public IStatusBarIcon
    , public IParcelable
{
public:
    CStatusBarIcon();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& iconPackage,
        /* [in] */ IUserHandle* user,
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 iconLevel,
        /* [in] */ Int32 number,
        /* [in] */ ICharSequence* contentDescription);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Clone(
        /* [out] */ IStatusBarIcon** object);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI GetVisible(
        /* [out] */ Boolean* visible);

    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    CARAPI GetIconPackage(
        /* [out] */ String* pkg);

    CARAPI SetIconPackage(
        /* [in] */ const String& pkg);

    CARAPI GetIconId(
        /* [out] */ Int32* id);

    CARAPI SetIconId(
        /* [in] */ Int32 id);

    CARAPI GetIconLevel(
        /* [out] */ Int32* level);

    CARAPI SetIconLevel(
        /* [in] */ Int32 level);

    CARAPI GetNumber(
        /* [out] */ Int32* number);

    CARAPI SetNumber(
        /* [in] */ Int32 number);

    CARAPI GetContentDescription(
        /* [out] */ ICharSequence** seq);

    CARAPI SetContentDescription(
        /* [in] */ ICharSequence* seq);

    CARAPI SetUser(
        /* [in] */ IUserHandle* user);

    CARAPI GetUser(
        /* [out] */ IUserHandle** user);

public:
    String mIconPackage;
    AutoPtr<IUserHandle> mUser;
    Int32 mIconId;
    Int32 mIconLevel;
    Boolean mVisible;
    Int32 mNumber;
    AutoPtr<ICharSequence> mContentDescription;

    /**
     * Parcelable.Creator that instantiates StatusBarIcon objects
     */
/*
    public static final Parcelable.Creator<StatusBarIcon> CREATOR
            = new Parcelable.Creator<StatusBarIcon>()
    {
        public StatusBarIcon createFromParcel(Parcel parcel)
        {
            return new StatusBarIcon(parcel);
        }

        public StatusBarIcon[] newArray(int size)
        {
            return new StatusBarIcon[size];
        }
    };
*/
};

} //namespace StatusBar
} //namespace Internal
} //namespace Droid
} //namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_STATUSBAR_CSTATUSBARICON_H__
