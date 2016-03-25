
#include "elastos/droid/server/pm/PreferredActivity.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::ILogHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const String PreferredActivity::TAG("PreferredActivity");
const Boolean PreferredActivity::DEBUG_FILTERS;

PreferredActivity::PreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Boolean always)
    : IntentFilter(filter)
{
    mPref = new PreferredComponent(this, match, set, activity, always);
}

UInt32 PreferredActivity::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 PreferredActivity::Release()
{
    return ElRefBase::Release();
}

PInterface PreferredActivity::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(PreferredComponent::ICallbacks*)this;
    }
    else if (riid == EIID_IPreferredComponentCallbacks) {
        return (PreferredComponent::ICallbacks*)this;
    }
    return IntentFilter::Probe(riid);
}

ECode PreferredActivity::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == (IInterface*)(PreferredComponent::ICallbacks*)this) {
        *iid = EIID_IPreferredComponentCallbacks;
        return NOERROR;
    }
    return IntentFilter::GetInterfaceID(object, iid);
}

// CAR_INTERFACE_IMPL(PreferredActivity, IntentFilter, PreferredComponent::ICallbacks)

PreferredActivity::PreferredActivity(
    /* [in] */ IXmlPullParser* parser)
{
    mPref = new PreferredComponent(this, parser);
}

ECode PreferredActivity::WriteToXml(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ Boolean full)
{
    FAIL_RETURN(mPref->WriteToXml(serializer, full))
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("filter")))
    FAIL_RETURN(IntentFilter::WriteToXml(serializer))
    return serializer->WriteEndTag(String(NULL), String("filter"));
}

ECode PreferredActivity::OnReadTag(
    /* [in] */ const String& tagName,
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (tagName.Equals("filter")) {
        if (DEBUG_FILTERS) {
            Slogger::I(TAG, "Starting to parse filter...");
        }
        FAIL_RETURN(IntentFilter::ReadFromXml(parser))
        if (DEBUG_FILTERS) {
            Int32 depth;
            parser->GetDepth(&depth);
            String name;
            parser->GetName(&name);
            Slogger::I(TAG, "Finished filter: depth=%d tag=%s", depth, name.string());
        }
    }
    else {
        String name;
        parser->GetName(&name);
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("Unknown element under <preferred-activities>: ") + name);
        XmlUtils::SkipCurrentTag(parser);
    }
    *result = TRUE;
    return NOERROR;
}

ECode PreferredActivity::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String s;
    mPref->mComponent->FlattenToShortString(&s);
    StringBuilder sb("PreferredActivity{0x");
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
