
#include "elastos/droid/media/CRouteCategory.h"
#include <elastos/core/StringBuilder.h>;


using Elastos::Core::StringBuilder;

using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

CRouteCategory::CRouteCategory()
    : mNameResId(0)
    , mTypes(0)
    , mGroupable(FALSE)
    , mIsSystem(FALSE)
{}

ECode CRouteCategory::constructor(
    /* [in] */ ICharSequence* name,
    /* [in] */ Int32 types,
    /* [in] */ Boolean groupable)
{
    mName = name;
    mTypes = types;
    mGroupable = groupable;
    return NOERROR;
}

ECode CRouteCategory::constructor(
    /* [in] */ Int32 nameResId,
    /* [in] */ Int32 types,
    /* [in] */ Boolean groupable)
{
    mNameResId = nameResId;
    mTypes = types;
    mGroupable = groupable;
    return NOERROR;
}

ECode CRouteCategory::GetName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp = GetName(CMediaRouter::sStatic->mResources);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRouteCategory::GetName(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> temp = GetName(resources);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRouteCategory::GetRoutes(
    /* [in] */ IList* inList,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IList> list = inList;
    if (list == NULL) {
        AutoPtr<IArrayList> arrayList;
        CArrayList::New((IArrayList**)&arrayList);
        list = IList::Probe(arrayList.Get());
    }
    else {
        list->Clear();
    }

    IRouteCategory* thisObj = THIS_PROBE(IRouteCategory);
    Int32 count = CMediaRouter::GetRouteCountStatic();
    Boolean ret;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IRouteInfo> route = CMediaRouter::GetRouteAtStatic(i);
        AutoPtr<IRouteCategory> routeCategory;
        route->GetCategory((IRouteCategory**)&routeCategory);
        if (routeCategory == thisObj) {
            list->Add(route, &ret);
        }
    }

    *result = list;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRouteCategory::GetSupportedTypes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTypes;
    return NOERROR;
}

ECode CRouteCategory::IsGroupable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mGroupable;
    return NOERROR;
}

ECode CRouteCategory::IsSystem(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsSystem;
    return NOERROR;
}

ECode CRouteCategory::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String tempText;
    mName->ToString(&tempText);

    StringBuilder sb("RouteCategory{ name=");
    sb.Append(tempText);
    sb.Append(" types=");
    sb.Append(CMediaRouter::TypesToString(mTypes));
    sb.Append(" groupable=");
    sb.Append(mGroupable);
    sb.Append("}");
    *result = sb.ToString();
    return NOERROR;
}

ECode CRouteCategory::SetIsSystem(
    /* [in] */ Boolean isSystem)
{
    mIsSystem = isSystem;
    return NOERROR;
}

AutoPtr<ICharSequence> CRouteCategory::GetName(
    /* [in] */ IResources* res)
{
    AutoPtr<ICharSequence> charSequence;
    if (mNameResId != 0) {
        res->GetText(mNameResId, (ICharSequence**)&charSequence);
        return charSequence;
    }
    return mName;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
