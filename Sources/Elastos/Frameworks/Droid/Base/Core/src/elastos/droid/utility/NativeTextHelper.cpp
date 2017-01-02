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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/utility/NativeTextHelper.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Utility {

ECode NativeTextHelper::GetLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    return GetLocalString(context, originalString, String("android"), originNamesId, localNamesId, result);
}

ECode NativeTextHelper::GetInternalLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    String packageName;
    context->GetPackageName(&packageName);
    return GetLocalString(context, originalString, packageName, originNamesId, localNamesId, result);
}

ECode NativeTextHelper::GetLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ const String& defPackage,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<String> > origNames;
    resources->GetStringArray(originNamesId, (ArrayOf<String>**)&origNames);

    AutoPtr< ArrayOf<String> > localNames;
    resources->GetStringArray(localNamesId, (ArrayOf<String>**)&localNames);
    for (Int32 i = 0; i < origNames->GetLength(); ++i) {
        if ((*origNames)[i].EqualsIgnoreCase(originalString)) {
            Int32 identify = 0;
            resources->GetIdentifier((*localNames)[i], String("string"), defPackage, &identify);
            return context->GetString(identify, result);
        }
    }
    *result = originalString;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
