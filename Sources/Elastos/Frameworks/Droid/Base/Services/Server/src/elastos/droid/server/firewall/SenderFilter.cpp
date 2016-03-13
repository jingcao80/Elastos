#include "elastos/droid/server/firewall/SenderFilter.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// PortFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------
IFilter* PortFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    String typeString;
    parser->GetAttributeValue(null, ATTR_TYPE, &typeString);
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
// PortFilter::SIGNATURE_Filter
//------------------------------------------------------------------------------
ECode PortFilter::SIGNATURE_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    *ret = ifw->SignaturesMatch(callerUid, receivingUid);
    return NOERROR;
}

//------------------------------------------------------------------------------
// PortFilter::SYSTEM_Filter
//------------------------------------------------------------------------------
ECode PortFilter::SIGNATURE_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    *ret = IsPrivilegedApp(callerUid, callerPid);
    return NOERROR;
}

//------------------------------------------------------------------------------
// PortFilter::SYSTEM_OR_SIGNATURE_Filter
//------------------------------------------------------------------------------
ECode PortFilter::SIGNATURE_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    *ret = (IsPrivilegedApp(callerUid, callerPid) || ifw->SignaturesMatch(callerUid, receivingUid));
    return NOERROR;
}

//------------------------------------------------------------------------------
// PortFilter::USER_ID_Filter
//------------------------------------------------------------------------------
ECode PortFilter::SIGNATURE_Filter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    *ret = ifw->CheckComponentPermission(NULL, callerPid, callerUid, receivingUid, FALSE);
    return NOERROR;
}

//=======================================================================================
// SenderFilter
//=======================================================================================

AutoPtr<FACTORY_FilterFactory> SenderFilter::FACTORY = new FACTORY_FilterFactory(String("sender"));

String SenderFilter::ATTR_TYPE("type");

String SenderFilter::VAL_SIGNATURE("signature");
String SenderFilter::VAL_SYSTEM("system");
String SenderFilter::VAL_SYSTEM_OR_SIGNATURE("system|signature");
String SenderFilter::VAL_USER_ID("userId");

Boolean SenderFilter::IsPrivilegedApp(
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid)
{
    if (callerUid == Process.SYSTEM_UID || callerUid == 0 ||
            callerPid == Process->MyPid() || callerPid == 0) {
        return TRUE;
    }

    IPackageManager pm = AppGlobals::GetPackageManager();
    Int32 result;
    ECode ec = pm->GetFlagsForUid(callerUid, &result);
    if (FAILED(ec)) {
        Slogger::E(IntentFirewall.TAG, "Remote exception while retrieving uid flags");
        return FALSE;
    }

    return (result & ApplicationInfo::FLAG_PRIVILEGED) != 0;
}

} // Firewall
} // Server
} // Droid
} // Elastos

