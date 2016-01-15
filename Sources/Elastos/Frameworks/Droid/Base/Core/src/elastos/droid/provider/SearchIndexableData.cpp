#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/SearchIndexableData.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

SearchIndexableData::SearchIndexableData()
    :mEnabled(FALSE),mRank(0),mUserId(-1),mIConResId(0)
{
}

SearchIndexableData::~SearchIndexableData()
{
}

CAR_INTERFACE_IMPL(SearchIndexableData, Object, ISearchIndexableData)

ECode SearchIndexableData::constructor()
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    localeHelper->GetDefault((ILocale**)&mLocale);
    mEnabled = TRUE;
    return NOERROR;
}

ECode SearchIndexableData::constructor(
    /* [in] */ IContext* ctx)
{
    constructor();
    mContext = ctx;
    return NOERROR;
}

ECode SearchIndexableData::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode SearchIndexableData::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode SearchIndexableData::SetLocale(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    return NOERROR;
}

ECode SearchIndexableData::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode SearchIndexableData::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
    return NOERROR;
}

ECode SearchIndexableData::GetEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mEnabled;
    return NOERROR;
}

ECode SearchIndexableData::SetRank(
    /* [in] */ Int32 rank)
{
    mRank = rank;
    return NOERROR;
}

ECode SearchIndexableData::GetRank(
    /* [out] */ Int32* rank)
{
    VALIDATE_NOT_NULL(rank);
    *rank = mRank;
    return NOERROR;
}

ECode SearchIndexableData::SetKey(
    /* [in] */ const String& key)
{
    mKey = key;
    return NOERROR;
}

ECode SearchIndexableData::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    *key = mKey;
    return NOERROR;
}

ECode SearchIndexableData::SetUserId(
    /* [in] */ Int32 userId)
{
    mUserId = userId;
    return NOERROR;
}

ECode SearchIndexableData::GetUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId);
    *userId = mUserId;
    return NOERROR;
}

ECode SearchIndexableData::SetClassName(
    /* [in] */ const String& className)
{
    mClassName = className;
    return NOERROR;
}

ECode SearchIndexableData::GetClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    *className = mClassName;
    return NOERROR;
}

ECode SearchIndexableData::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode SearchIndexableData::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode SearchIndexableData::SetIconResId(
    /* [in] */ Int32 iconResId)
{
    mIConResId = iconResId;
    return NOERROR;
}

ECode SearchIndexableData::GetIconResId(
    /* [out] */ Int32* iconResId)
{
    VALIDATE_NOT_NULL(iconResId);
    *iconResId = mIConResId;
    return NOERROR;
}

ECode SearchIndexableData::SetIntentAction(
    /* [in] */ const String& intentAction)
{
    mIntentAction = intentAction;
    return NOERROR;
}

ECode SearchIndexableData::GetIntentAction(
    /* [out] */ String* intentAction)
{
    VALIDATE_NOT_NULL(intentAction);
    *intentAction = mIntentAction;
    return NOERROR;
}

ECode SearchIndexableData::SetIntentTargetPackage(
    /* [in] */ const String& intentTargetPackage)
{
    mIntentTargetPackage = intentTargetPackage;
    return NOERROR;
}

ECode SearchIndexableData::GetIntentTargetPackage(
    /* [out] */ String* intentTargetPackage)
{
    VALIDATE_NOT_NULL(intentTargetPackage);
    *intentTargetPackage = mIntentTargetPackage;
    return NOERROR;
}

ECode SearchIndexableData::SetIntentTargetClass(
    /* [in] */ const String& intentTargetClass)
{
    mIntentTargetClass = intentTargetClass;
    return NOERROR;
}

ECode SearchIndexableData::GetIntentTargetClass(
    /* [out] */ String* intentTargetClass)
{
    VALIDATE_NOT_NULL(intentTargetClass);
    *intentTargetClass = mIntentTargetClass;
    return NOERROR;
}

//@Override
ECode SearchIndexableData::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb;
    sb.Append("SearchIndexableData[mContext: ");
    sb.Append(mContext);
    sb.Append(", ");
    sb.Append("mLocale: ");
    sb.Append(mLocale);
    sb.Append(", ");
    sb.Append("mEnabled: ");
    sb.Append(mEnabled);
    sb.Append(", ");
    sb.Append("mRank: ");
    sb.Append(mRank);
    sb.Append(", ");
    sb.Append("mKey: ");
    sb.Append(mKey);
    sb.Append(", ");
    sb.Append("mUserId: ");
    sb.Append(mUserId);
    sb.Append(", ");
    sb.Append("mClassName: ");
    sb.Append(mClassName);
    sb.Append(", ");
    sb.Append("mPackageName: ");
    sb.Append(mPackageName);
    sb.Append(", ");
    sb.Append("mIConResId: ");
    sb.Append(mIConResId);
    sb.Append(", ");
    sb.Append("mIntentAction: ");
    sb.Append(mIntentAction);
    sb.Append(", ");
    sb.Append("mIntentTargetPackage: ");
    sb.Append(mIntentTargetPackage);
    sb.Append(", ");
    sb.Append("mIntentTargetClass: ");
    sb.Append(mIntentTargetClass);
    sb.Append("]");

    return sb.ToString(info);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
