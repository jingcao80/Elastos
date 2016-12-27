#include "elastos/droid/server/firewall/OrFilter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// OrFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

OrFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode OrFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    AutoPtr<OrFilter> orFilter = new OrFilter();
    orFilter->ReadFromXml(parser);
    *result = IFilter::Probe(orFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// OrFilter
//=======================================================================================

INIT_PROI_3 const AutoPtr<OrFilter::FACTORY_FilterFactory> OrFilter::FACTORY = new OrFilter::FACTORY_FilterFactory(String("or"));

ECode OrFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 size = 0;
    mChildren->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> filter;
        mChildren->Get(i, (IInterface**)&filter);
        Boolean bRes = FALSE;
        IFilter::Probe(filter)->Matches(ifw, resolvedComponent, intent, callerUid, callerPid,
                resolvedType, receivingUid, &bRes);
        if (bRes) {
            *ret = TRUE;
            return NOERROR;
        }
    }

    *ret = FALSE;
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
