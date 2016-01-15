
#include "elastos/droid/server/pm/PersistentPreferredActivity.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/util/XmlUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const String PersistentPreferredActivity::ATTR_NAME("name");
const String PersistentPreferredActivity::ATTR_FILTER("filter");
const String PersistentPreferredActivity::TAG("PersistentPreferredActivity");
static const Boolean DEBUG_FILTERS;

PersistentPreferredActivity::PersistentPreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IComponentName* activity)
    : IntentFilter(filter)
    , mComponent(activity)
{}


PersistentPreferredActivity::PersistentPreferredActivity(
    /* [in] */ IXmlPullParser* parser)
{
    String shortComponent;
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &shortComponent);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->UnflattenFromString(name, (IComponentName**)&mComponent);
    if (mComponent == NULL) {
        Srring desc;
        parser->GetPositiondescription(&desc);
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("Error in package manager settings: ") +
                        "Bad activity name " + shortComponent +
                        " at " + desc);
    }
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    String tagName;
    parser->GetName(&tagName);
    Int32 type, depth;
    while ((parser->GetNext(&type), type != IXmlPullParser::END_DOCUMENT)
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
                Srring desc;
                parser->GetPositiondescription(&desc);
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Unknown element: ") + tagName + " at " + desc);
                XmlUtils::SkipCurrentTag(parser);
            }
        }
    }
    if (tagName.Equals(ATTR_FILTER)) {
        ReadFromXml(parser);
    }
    else {
        Srring desc;
        parser->GetPositiondescription(&desc);
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("Missing element filter at ") + descs);
        XmlUtils::SkipCurrentTag(parser);
    }
}

ECode PersistentPreferredActivity::WriteToXml(
    /* [in] */ IXmlSerializer* serializer)
{
    String str;
    mComponent->FlattenToShortString(&str);
    serializer->WriteAttribute(String(NULL), ATTR_NAME, str);
    serializer->WriteStartTag(String(NULL), ATTR_FILTER);
    IntentFilter::WriteToXml(serializer);
    serializer->WriteEndTag(String(NULL), ATTR_FILTER);
    return NOERROR;
}

ECode PersistentPreferredActivity::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int32 hashCode;
    system->IdentityHashCode((IObject*)this, &hashCode);
    String str;
    mComponent->FlattenToShortString(&str);
    *str = String("PersistentPreferredActivity{0x") + StringUtils::ToHexString(hashCode)
            + " " + str + "}";
    return NOERROR;
}


} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
