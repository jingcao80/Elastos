#include "elastos/droid/server/firewall/NotFilter.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// NotFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

NotFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

AutoPtr<IFilter> NotFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<IFilter> child;  // = NULL
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        AutoPtr<IFilter> filter = IntentFirewall::ParseFilter(parser);
        if (child == NULL) {
            child = filter;
        } else {
            //throw new XmlPullParserException(
            //        "<not> tag can only contain a single child filter.", parser, null);
            //return E_XML_PULL_PARSER_EXCEPTION;
            return NULL;
        }
    }
    AutoPtr<NotFilter> notFilter = new NotFilter(child);
    return (IFilter*)notFilter.Get();
}

//=======================================================================================
// NotFilter
//=======================================================================================

CAR_INTERFACE_IMPL(NotFilter, Object, IFilter);

INIT_PROI_3 const AutoPtr<NotFilter::FACTORY_FilterFactory> NotFilter::FACTORY = new NotFilter::FACTORY_FilterFactory(String("not"));

NotFilter::NotFilter(
    /* in */ IFilter* child)
    : mChild(child)
{}

ECode NotFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    mChild->Matches(ifw, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                receivingUid, ret);
    *ret = !*ret;
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
