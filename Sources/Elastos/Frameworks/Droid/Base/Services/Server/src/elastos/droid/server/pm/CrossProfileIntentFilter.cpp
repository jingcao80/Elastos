
#include "elastos/droid/server/pm/CrossProfileIntentFilter.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/server/pm/Settings.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::ILogHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const String CrossProfileIntentFilter::ATTR_TARGET_USER_ID("targetUserId");
const String CrossProfileIntentFilter::ATTR_FLAGS("flags");
const String CrossProfileIntentFilter::ATTR_OWNER_USER_ID("ownerUserId");
const String CrossProfileIntentFilter::ATTR_OWNER_PACKAGE("wnerPackage");
const String CrossProfileIntentFilter::ATTR_FILTER("filter");
const String CrossProfileIntentFilter::TAG("CrossProfileIntentFilter");

CrossProfileIntentFilter::CrossProfileIntentFilter(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& ownerPackage,
    /* [in] */ Int32 ownerUserId,
    /* [in] */ Int32 targetUserId,
    /* [in] */ Int32 flags)
    : IntentFilter(filter)
    , mTargetUserId(targetUserId)
    , mOwnerUserId(ownerUserId)
    , mOwnerPackage(ownerPackage)
    , mFlags(flags)
{}

CrossProfileIntentFilter::CrossProfileIntentFilter(
    /* [in] */ IXmlPullParser* parser)
    : mTargetUserId(0)
    , mOwnerUserId(0)
    , mFlags(0)
{
    mTargetUserId = GetInt32FromXml(parser, ATTR_TARGET_USER_ID, IUserHandle::USER_NULL);
    mOwnerUserId = GetInt32FromXml(parser, ATTR_OWNER_USER_ID, IUserHandle::USER_NULL);
    mOwnerPackage = GetStringFromXml(parser, ATTR_OWNER_PACKAGE, String(""));
    mFlags = GetInt32FromXml(parser, ATTR_FLAGS, 0);

    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    String tagName;
    parser->GetName(&tagName);
    Int32 type, depth;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        parser->GetName(&tagName);
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        else if (type == IXmlPullParser::START_TAG) {
            if (tagName.Equals(ATTR_FILTER)) {
                break;
            }
            else {
                String des;
                parser->GetPositionDescription(&des);
                String msg = String("Unknown element under ")
                        + Settings::TAG_CROSS_PROFILE_INTENT_FILTERS + ": " + tagName + " at "
                        + des;
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, msg);
                XmlUtils::SkipCurrentTag(parser);
            }
        }
    }
    if (tagName.Equals(ATTR_FILTER)) {
        ReadFromXml(parser);
    }
    else {
        String des;
        parser->GetPositionDescription(&des);
        String msg = String("Missing element under ") + TAG + ": " + ATTR_FILTER +
                " at " + des;
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, msg);
        XmlUtils::SkipCurrentTag(parser);
    }
}

Int32 CrossProfileIntentFilter::GetTargetUserId()
{
    return mTargetUserId;
}

Int32 CrossProfileIntentFilter::GetFlags()
{
    return mFlags;
}

Int32 CrossProfileIntentFilter::GetOwnerUserId()
{
    return mOwnerUserId;
}

String CrossProfileIntentFilter::GetOwnerPackage()
{
    return mOwnerPackage;
}

String CrossProfileIntentFilter::GetStringFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& attribute,
    /* [in] */ const String& defaultValue)
{
    String value;
    parser->GetAttributeValue(String(NULL), attribute, &value);
    if (value.IsNull()) {
        String des;
        parser->GetPositionDescription(&des);
        String msg = String("Missing element under ") + TAG +": " + attribute + " at " + des;
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, msg);
        return defaultValue;
    }
    else {
        return value;
    }
}

Int32 CrossProfileIntentFilter::GetInt32FromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue)
{
    String stringValue = GetStringFromXml(parser, attribute, String(NULL));
    if (!stringValue.IsNull()) {
        return StringUtils::ParseInt32(stringValue);
    }
    return defaultValue;
}

ECode CrossProfileIntentFilter::WriteToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    serializer->WriteAttribute(String(NULL), ATTR_TARGET_USER_ID, StringUtils::ToString(mTargetUserId));
    serializer->WriteAttribute(String(NULL), ATTR_FLAGS, StringUtils::ToString(mFlags));
    serializer->WriteAttribute(String(NULL), ATTR_OWNER_USER_ID, StringUtils::ToString(mOwnerUserId));
    serializer->WriteAttribute(String(NULL), ATTR_OWNER_PACKAGE, mOwnerPackage);
    serializer->WriteStartTag(String(NULL), ATTR_FILTER);
    IntentFilter::WriteToXml(serializer);
    serializer->WriteEndTag(String(NULL), ATTR_FILTER);
    return NOERROR;
}

ECode CrossProfileIntentFilter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CrossProfileIntentFilter{0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " ";
    sb += mTargetUserId;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}


} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
