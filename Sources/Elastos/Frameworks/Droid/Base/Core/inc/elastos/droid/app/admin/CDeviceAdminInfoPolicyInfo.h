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

#ifndef __ELASTOS_DROID_APP_ADMIN_CDEVICEADMININFOPOLICYINFO_H__
#define __ELASTOS_DROID_APP_ADMIN_CDEVICEADMININFOPOLICYINFO_H__

#include "_Elastos_Droid_App_Admin_CDeviceAdminInfoPolicyInfo.h"
#include "elastos/droid/ext/frameworkext.h"


namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

CarClass(CDeviceAdminInfoPolicyInfo)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 identIn,
        /* [in] */ const String& tagIn,
        /* [in] */ Int32 labelIn,
        /* [in] */ Int32 descriptionIn);

    CARAPI GetIdent(
        /* [out] */ Int32* ident);

    CARAPI SetIdent(
        /* [in] */ Int32 ident);

    CARAPI GetTag(
        /* [out] */ String* tag);

    CARAPI SetTag(
        /* [in] */ const String& tag);

    CARAPI GetLabel(
        /* [out] */ Int32* label);

    CARAPI SetLabel(
        /* [in] */ Int32 label);

    CARAPI GetDescription(
        /* [out] */ Int32* description);

    CARAPI SetDescription(
        /* [in] */ Int32 description);

private:
    Int32 mIdent;
    String mTag;
    Int32 mLabel;
    Int32 mDescription;
};

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ADMIN_CDEVICEADMININFOPOLICYINFO_H__
