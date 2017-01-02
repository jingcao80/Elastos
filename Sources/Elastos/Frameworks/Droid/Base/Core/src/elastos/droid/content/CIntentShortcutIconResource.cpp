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

#include "elastos/droid/content/CIntentShortcutIconResource.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CIntentShortcutIconResource, Object, IIntentShortcutIconResource, IParcelable)

CAR_OBJECT_IMPL(CIntentShortcutIconResource)

ECode CIntentShortcutIconResource::constructor()
{
    return NOERROR;
}

ECode CIntentShortcutIconResource::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackageName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::GetResourceName(
    /* [out] */ String* resourceName)
{
    VALIDATE_NOT_NULL(resourceName)
    *resourceName = mResourceName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::SetResourceName(
    /* [in] */ const String& resourceName)
{
    mResourceName = resourceName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mResourceName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mPackageName))
    FAIL_RETURN(source->ReadString(&mResourceName))
    return NOERROR;
}

ECode CIntentShortcutIconResource::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mPackageName))
    FAIL_RETURN(dest->WriteString(mResourceName))
    return NOERROR;
}

}
}
}

