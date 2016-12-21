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

IFilter* OrFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<OrFilter> orFilter;
    AutoPtr<IFilter>  filter;
    AutoPtr<FilterList> filterList;

    orFilter = new OrFilter();
    filterList = orFilter->ReadFromXml(parser);
    filter = (IFilter*)filterList;
    REFCOUNT_ADD(filter);

    return (IFilter*)filter;
}

//=======================================================================================
// OrFilter
//=======================================================================================

const AutoPtr<OrFilter::FACTORY_FilterFactory> OrFilter::FACTORY = new OrFilter::FACTORY_FilterFactory(String("or"));

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
    Int32 size;

    children->GetSize(&size);

    for (Int32 i = 0;  i < size;  i++) {

        AutoPtr<IInterface> filter;
        children->Get(i, (IInterface**)&filter);

        IFilter::Probe(filter)->Matches(ifw, resolvedComponent, intent, callerUid, callerPid,
                resolvedType, receivingUid, ret);
        if (*ret) {
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
