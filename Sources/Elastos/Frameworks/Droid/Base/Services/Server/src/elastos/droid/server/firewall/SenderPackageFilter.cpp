#include "elastos/droid/server/firewall/SenderPackageFilter.h"
#include "elastos/droid/app/AppGlobals.h"
#include <elastos/droid/os/UserHandle.h>

using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::UserHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// SenderPackageFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------
IFilter* SenderPackageFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    String packageName;
    parser->GetAttributeValue(NULL, ATTR_NAME, &packageName);
    if (packageName == NULL) {
        //throw new XmlPullParserException(
        //    "A package name must be specified.", parser, null);
        return NULL;
    }

    AutoPtr<SenderPackageFilter> spFilter = new SenderPackageFilter(packageName);
    REFCOUNT_ADD(spFilter);

    return (IFilter*)spFilter;
}

//=======================================================================================
// SenderPackageFilter
//=======================================================================================

AutoPtr<FACTORY_FilterFactory> SenderPackageFilter::FACTORY = new FACTORY_FilterFactory(String("sender-package"));
String SenderPackageFilter::ATTR_TYPE("name");

SenderPackageFilter::SenderPackageFilter(
    /* in */ const String& packageName)
    :  mPackageName(packageName)
{}

ECode SenderPackageFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    Int32 packageUid = -1;

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    FAIL_RETURN(pm->GetPackageUid(mPackageName, UserHandle::USER_OWNER, &packageUid));

    if (packageUid == -1)  {
        *ret = FALSE;
    } else {
        *ret = UserHandle.isSameApp(packageUid, callerUid);
    }
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
