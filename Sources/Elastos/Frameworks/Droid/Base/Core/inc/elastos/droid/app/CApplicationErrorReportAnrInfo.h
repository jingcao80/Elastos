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

#ifndef __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTANRINFO_H__
#define __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTANRINFO_H__

#include "_Elastos_Droid_App_CApplicationErrorReportAnrInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CApplicationErrorReportAnrInfo)
    , public Object
    , public IApplicationErrorReportAnrInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create an uninitialized instance of AnrInfo.
     */
    CARAPI constructor();

    /**
     * Dump an AnrInfo instance to a Printer.
     */
    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /**
     * Activity name.
     */
    CARAPI GetActivity(
        /* [out] */ String* activity);

    CARAPI SetActivity(
        /* [in] */ const String& activity);

    /**
     * Description of the operation that timed out.
     */
    CARAPI GetCause(
        /* [out] */ String* cause);

    CARAPI SetCause(
        /* [in] */ const String& cause);

    /**
     * Additional info, including CPU stats.
     */
    CARAPI GetInfo(
        /* [out] */ String* info);

    CARAPI SetInfo(
        /* [in] */ const String& info);

private:

    /**
     * Activity name.
     */
    String mActivity;

    /**
     * Description of the operation that timed out.
     */
    String mCause;

    /**
     * Additional info, including CPU stats.
     */
    String mInfo;
};


} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTANRINFO_H__
