#include "elastos/droid/server/firewall/AndFilter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// AndFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

AndFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

IFilter* AndFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<AndFilter> filter = new AndFilter();
    return (IFilter*)(filter->ReadFromXml(parser));
}

//=======================================================================================
// AndFilter
//=======================================================================================

const AutoPtr<AndFilter::FACTORY_FilterFactory> AndFilter::FACTORY = new AndFilter::FACTORY_FilterFactory(String("and"));

ECode AndFilter::Matches(
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
        if (! *ret) {
            return NOERROR;
        }
    }

    *ret = TRUE;
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
