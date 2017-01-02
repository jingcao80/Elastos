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

#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGER_STACKINFO_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGER_STACKINFO_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_App_CActivityManagerStackInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can retrieve about an ActivityStack in the system.
 * @hide
 */
CarClass(CActivityManagerStackInfo)
    , public Object
    , public IActivityManagerStackInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerStackInfo();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetStackId(
        /* [out] */ Int32* result);

    CARAPI SetStackId(
        /* [in] */ Int32 id);

    CARAPI GetBounds(
        /* [out] */ IRect** rect);

    CARAPI SetBounds(
        /* [in] */ IRect* rect);

    CARAPI GetTaskIds(
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI SetTaskId(
        /* [in] */ ArrayOf<Int32>* ids);

    CARAPI GetTaskNames(
        /* [out, callee] */ ArrayOf<String>** names);

    CARAPI SetTaskNames(
        /* [in] */ ArrayOf<String>* names);

    CARAPI GetDisplayId(
        /* [out] */ Int32* id);

    CARAPI SetDisplayId(
        /* [in] */ Int32 id);

    CARAPI ToString(
        /* [in] */ const String& prefix,
        /* [out] */ String* str);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int32 mStackId;
    AutoPtr<IRect> mBounds;// = new Rect();
    AutoPtr<ArrayOf<Int32> > mTaskIds;
    AutoPtr<ArrayOf<String> > mTaskNames;
    Int32 mDisplayId;

};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_APP_CACTIVITYMANAGER_STACKINFO_H__
