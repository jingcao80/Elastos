#include "elastos/droid/server/firewall/SenderFilter.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/app/AppGlobals.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/os/Process.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// SenderFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

SenderFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

IFilter* SenderFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    String typeString;
    parser->GetAttributeValue(String(NULL), ATTR_TYPE, &typeString);
    if (typeString == NULL) {
        //throw new XmlPullParserException("type attribute must be specified for <sender>",
        //        parser, null);
        return NULL;
    }
    if (typeString.Equals(VAL_SYSTEM)) {
        return SYSTEM;
    } else if (typeString.Equals(VAL_SIGNATURE)) {
        return SIGNATURE;
    } else if (typeString.Equals(VAL_SYSTEM_OR_SIGNATURE)) {
        return SYSTEM_OR_SIGNATURE;
    } else if (typeString.Equals(VAL_USER_ID)) {
        return USER_ID;
    }
    //throw new XmlPullParserException(
    //        "Invalid type attribute for <sender>: " + typeString, parser, null);
    return NULL;
}

//------------------------------------------------------------------------------
// SenderFilter::SIGNATURE_Filter
//------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(SenderFilter::SIGNATURE_Filter, Object, IFilter);

ECode SenderFilter::SIGNATURE_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    ifw->SignaturesMatch(callerUid, receivingUid, ret);
    return NOERROR;
}

//------------------------------------------------------------------------------
// SenderFilter::SYSTEM_Filter
//------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(SenderFilter::SYSTEM_Filter, Object, IFilter);

ECode SenderFilter::SYSTEM_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    *ret = IsPrivilegedApp(callerUid, callerPid);
    return NOERROR;
}

//------------------------------------------------------------------------------
// SenderFilter::SYSTEM_OR_SIGNATURE_Filter
//------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(SenderFilter::SYSTEM_OR_SIGNATURE_Filter, Object, IFilter);

ECode SenderFilter::SYSTEM_OR_SIGNATURE_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    ifw->SignaturesMatch(callerUid, receivingUid, ret);
    *ret = (IsPrivilegedApp(callerUid, callerPid) || *ret);
    return NOERROR;
}

//------------------------------------------------------------------------------
// SenderFilter::USER_ID_Filter
//------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(SenderFilter::USER_ID_Filter, Object, IFilter);

ECode SenderFilter::USER_ID_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    ifw->CheckComponentPermission(String(NULL), callerPid, callerUid, receivingUid, FALSE, ret);
    return NOERROR;
}

//=======================================================================================
// SenderFilter
//=======================================================================================

const AutoPtr<SenderFilter::FACTORY_FilterFactory> SenderFilter::FACTORY = new SenderFilter::FACTORY_FilterFactory(String("sender"));
const AutoPtr<SenderFilter::SIGNATURE_Filter> SenderFilter::SIGNATURE = new SenderFilter::SIGNATURE_Filter();
const AutoPtr<SenderFilter::SYSTEM_Filter> SenderFilter::SYSTEM = new SenderFilter::SYSTEM_Filter();
const AutoPtr<SenderFilter::SYSTEM_OR_SIGNATURE_Filter> SenderFilter::SYSTEM_OR_SIGNATURE = new SenderFilter::SYSTEM_OR_SIGNATURE_Filter();
const AutoPtr<SenderFilter::USER_ID_Filter> SenderFilter::USER_ID = new SenderFilter::USER_ID_Filter();

const String SenderFilter::ATTR_TYPE("type");

const String SenderFilter::VAL_SIGNATURE("signature");
const String SenderFilter::VAL_SYSTEM("system");
const String SenderFilter::VAL_SYSTEM_OR_SIGNATURE("system|signature");
const String SenderFilter::VAL_USER_ID("userId");

CAR_INTERFACE_IMPL(SenderFilter, Object, IFilter);

Boolean SenderFilter::IsPrivilegedApp(
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid)
{
    if (callerUid == IProcess::SYSTEM_UID || callerUid == 0 ||
            callerPid == Process::MyPid() || callerPid == 0) {
        return TRUE;
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    Int32 result;
    ECode ec = pm->GetFlagsForUid(callerUid, &result);
    if (FAILED(ec)) {
        Slogger::E(IntentFirewall::TAG, "Remote exception while retrieving uid flags");
        return FALSE;
    }

    return (result & IApplicationInfo::FLAG_PRIVILEGED) != 0;
}

} // Firewall
} // Server
} // Droid
} // Elastos

