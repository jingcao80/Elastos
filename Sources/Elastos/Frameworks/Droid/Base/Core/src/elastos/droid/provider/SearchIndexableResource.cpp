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

#include "elastos/droid/provider/SearchIndexableResource.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

SearchIndexableResource::SearchIndexableResource()
    :mXmlResId(0)
{}

SearchIndexableResource::~SearchIndexableResource()
{}

CAR_INTERFACE_IMPL_2(SearchIndexableResource, Object, ISearchIndexableResource, ISearchIndexableData)

ECode SearchIndexableResource::constructor(
    /* [in] */ IContext* context)
{
    return SearchIndexableData::constructor(context);
}

ECode SearchIndexableResource::constructor(
    /* [in] */ Int32 rank,
    /* [in] */ Int32 xmlResId,
    /* [in] */ const String& className,
    /* [in] */ Int32 iconResId)
{
    SearchIndexableData::constructor();
    mRank = rank;
    mXmlResId = xmlResId;
    mClassName = className;
    mIConResId = iconResId;
    return NOERROR;
}

//@Override
ECode SearchIndexableResource::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb;
    sb.Append("SearchIndexableResource[");
    String str;
    SearchIndexableData::ToString(&str);
    sb.Append(str);
    sb.Append(", ");
    sb.Append("mXmlResId: ");
    sb.Append(mXmlResId);
    sb.Append("]");

    return sb.ToString(info);
}

ECode SearchIndexableResource::SetXmlResId(
    /* [in] */ Int32 xmlResId)
{
    mXmlResId = xmlResId;
    return NOERROR;
}

ECode SearchIndexableResource::GetXmlResId(
    /* [out] */ Int32* xmlResId)
{
    *xmlResId = mXmlResId;
    return NOERROR;
}

//override
ECode SearchIndexableResource::SetKey(
    /* [in] */ const String& key)
{
    return SearchIndexableData::SetKey(key);
}

//override
ECode SearchIndexableResource::SetClassName(
    /* [in] */ const String& className)
{
    return SearchIndexableData::SetClassName(className);
}

//override
ECode SearchIndexableResource::SetIntentAction(
    /* [in] */ const String& intentAction)
{
    return SearchIndexableData::SetIntentAction(intentAction);
}

//override
ECode SearchIndexableResource::SetIntentTargetPackage(
    /* [in] */ const String& intentTargetPackage)
{
    return SearchIndexableData::SetIntentTargetPackage(intentTargetPackage);
}

ECode SearchIndexableResource::GetIntentTargetClass(
    /* [out] */ String* intentTargetClass)
{
    return SearchIndexableData::GetIntentTargetClass(intentTargetClass);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos