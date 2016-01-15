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