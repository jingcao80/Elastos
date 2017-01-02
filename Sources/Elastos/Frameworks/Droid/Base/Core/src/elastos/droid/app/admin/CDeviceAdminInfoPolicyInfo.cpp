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

#include "elastos/droid/app/admin/CDeviceAdminInfoPolicyInfo.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {


ECode CDeviceAdminInfoPolicyInfo::constructor(
    /* [in] */ Int32 identIn,
    /* [in] */ const String& tagIn,
    /* [in] */ Int32 labelIn,
    /* [in] */ Int32 descriptionIn)
{
    mIdent = identIn;
    mTag = tagIn;
    mLabel = labelIn;
    mDescription = descriptionIn;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::GetIdent(
    /* [out] */ Int32* ident)
{
    VALIDATE_NOT_NULL(ident);
    *ident = mIdent;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::SetIdent(
    /* [in] */ Int32 ident)
{
    mIdent = ident;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::SetTag(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::GetLabel(
    /* [out] */ Int32* label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::SetLabel(
    /* [in] */ Int32 label)
{
    mLabel = label;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::GetDescription(
    /* [out] */ Int32* description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescription;
    return NOERROR;
}

ECode CDeviceAdminInfoPolicyInfo::SetDescription(
    /* [in] */ Int32 description)
{
    mDescription = description;
    return NOERROR;
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos
