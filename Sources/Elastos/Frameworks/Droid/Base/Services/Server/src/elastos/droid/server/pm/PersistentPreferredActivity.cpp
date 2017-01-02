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

#include "elastos/droid/server/pm/PersistentPreferredActivity.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const String PersistentPreferredActivity::ATTR_NAME("name");
const String PersistentPreferredActivity::ATTR_FILTER("filter");
const String PersistentPreferredActivity::TAG("PersistentPreferredActivity");
const Boolean PersistentPreferredActivity::DEBUG_FILTERS;

ECode PersistentPreferredActivity::constructor(
    /* [in] */ IIntentFilter* filter)
{
    return IntentFilter::constructor(filter);
}

PersistentPreferredActivity::PersistentPreferredActivity(
    /* [in] */ IComponentName* activity)
    : mComponent(activity)
{}


PersistentPreferredActivity::PersistentPreferredActivity(
    /* [in] */ IXmlPullParser* parser)
{
    String shortComponent;
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &shortComponent);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->UnflattenFromString(shortComponent, (IComponentName**)&mComponent);
    if (mComponent == NULL) {
        String desc;
        parser->GetPositionDescription(&desc);
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
                String desc;
                parser->GetPositionDescription(&desc);
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
        String desc;
        parser->GetPositionDescription(&desc);
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("Missing element filter at ") + desc);
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
    VALIDATE_NOT_NULL(str)
    String s;
    mComponent->FlattenToShortString(&s);
    StringBuilder sb("PersistentPreferredActivity{0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " ";
    sb += s;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}


} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
