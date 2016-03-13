#include "elastos/droid/server/firewall/NotFilter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// NotFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------
IFilter* NotFilter::FACTORY_FilterFactory::NewFilter()
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<Filter> child;  // = NULL
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        AutoPtr<Filter> filter = IntentFirewall::ParseFilter(parser);
        if (child == NULL) {
            child = filter;
        } else {
            //throw new XmlPullParserException(
            //        "<not> tag can only contain a single child filter.", parser, null);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }
    AutoPtr<NotFilter> notFilter = new NotFilter(child);
    REFCOUNT_ADD(notFilter);

    return (IFilter*)notFilter;
}

//=======================================================================================
// NotFilter
//=======================================================================================

AutoPtr<FACTORY_FilterFactory> NotFilter::FACTORY = new FACTORY_FilterFactory(String("not"));

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
    /* [in] */ Int32 receivingUid
    /* [out] */ Boolean *ret)
{
    *ret = !mChild->Matches(ifw, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                receivingUid);
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
