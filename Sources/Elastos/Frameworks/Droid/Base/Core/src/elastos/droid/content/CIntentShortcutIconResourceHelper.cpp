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

#include "elastos/droid/content/CIntentShortcutIconResourceHelper.h"
#include "elastos/droid/content/CIntentShortcutIconResource.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentShortcutIconResourceHelper, Singleton, IIntentShortcutIconResourceHelper)

CAR_SINGLETON_IMPL(CIntentShortcutIconResourceHelper)

ECode CIntentShortcutIconResourceHelper::FromContext(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceId,
    /* [out] */ IIntentShortcutIconResource** shortcutIconRes)
{
    VALIDATE_NOT_NULL(shortcutIconRes)
    *shortcutIconRes = NULL;
    VALIDATE_NOT_NULL(context)

    AutoPtr<CIntentShortcutIconResource> icon;
    FAIL_RETURN(CIntentShortcutIconResource::NewByFriend((CIntentShortcutIconResource**)&icon))
    String packageName;
    FAIL_RETURN(context->GetPackageName(&packageName))
    FAIL_RETURN(icon->SetPackageName(packageName))
    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res))
    String resourceName;
    FAIL_RETURN(res->GetResourceName(resourceId, &resourceName))
    FAIL_RETURN(icon->SetResourceName(resourceName))
    *shortcutIconRes = (IIntentShortcutIconResource*)icon.Get();
    REFCOUNT_ADD(*shortcutIconRes)
    return NOERROR;
}

}
}
}

