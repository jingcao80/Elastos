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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/BaseThemeInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(BaseThemeInfo, Object, IBaseThemeInfo, IParcelable)

ECode BaseThemeInfo::constructor()
{
    return NOERROR;
}

ECode BaseThemeInfo::GetResolvedString(
    /* [in] */ IResources* res,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 resId;
    attrs->GetAttributeResourceValue(index, 0, &resId);
    if (resId != 0) {
        return res->GetString(resId, str);
    }
    return attrs->GetAttributeValue(index, str);
}

ECode BaseThemeInfo::GetThemeId(
    /* [out] */ String* themeId)
{
    VALIDATE_NOT_NULL(themeId)
    *themeId = mThemeId;
    return NOERROR;
}

ECode BaseThemeInfo::SetThemeId(
    /* [in] */ const String& themeId)
{
    mThemeId = themeId;
    return NOERROR;
}

ECode BaseThemeInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode BaseThemeInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode BaseThemeInfo::GetAuthor(
    /* [out] */ String* author)
{
    VALIDATE_NOT_NULL(author)
    *author = mAuthor;
    return NOERROR;
}

ECode BaseThemeInfo::SetAuthor(
    /* [in] */ const String& author)
{
    mAuthor = author;
    return NOERROR;
}

ECode BaseThemeInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadString(&mThemeId);
    source->ReadString(&mName);
    source->ReadString(&mAuthor);
    return NOERROR;
}

ECode BaseThemeInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteString(mThemeId);
    dest->WriteString(mName);
    dest->WriteString(mAuthor);
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
