#include "elastos/droid/server/firewall/SenderPermissionFilter.h"
#include "elastos/droid/app/AppGlobals.h"
#include <elastos/droid/os/UserHandle.h>

using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::UserHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// SenderPermissionFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------
IFilter* SenderPermissionFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    String permission;
    parser->GetAttributeValue(NULL, ATTR_NAME, &permission);
    if (packageName == NULL) {
        //throw new XmlPullParserException(
        //    "A package name must be specified.", parser, null);
        return NULL;
    }

    AutoPtr<SenderPermissionFilter> spFilter = new SenderPermissionFilter(permission);
    REFCOUNT_ADD(spFilter);

    return (IFilter*)notFilter;
}

//=======================================================================================
// SenderPermissionFilter
//=======================================================================================

AutoPtr<FACTORY_FilterFactory> SenderPermissionFilter::FACTORY = new FACTORY_FilterFactory(String("sender-permission"));
String SenderPermissionFilter::ATTR_TYPE("name");

SenderPermissionFilter::SenderPermissionFilter(
    /* in */ const String& permission)
    :  mPermission(permission)
{}

ECode SenderPermissionFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    // We assume the component is exported here. If the component is not exported, then
    // ActivityManager would only resolve to this component for callers from the same uid.
    // In this case, it doesn't matter whether the component is exported or not.
    *ret = ifw->CheckComponentPermission(mPermission, callerPid, callerUid, receivingUid, TRUE);

    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
