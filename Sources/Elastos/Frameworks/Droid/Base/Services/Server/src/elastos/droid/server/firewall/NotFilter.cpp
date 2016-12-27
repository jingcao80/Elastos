#include "elastos/droid/server/firewall/NotFilter.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"
#include "elastos/droid/internal/utility/XmlUtils.h"

using Elastos::Droid::Internal::Utility::XmlUtils;

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

ECode NotFilter::FACTORY_FilterFactory::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IFilter> child;
    Int32 outerDepth = 0;
    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        AutoPtr<IFilter> filter;
        IntentFirewall::ParseFilter(parser, (IFilter**)&filter);
        if (child == NULL) {
            child = filter;
        }
        else {
            //throw new XmlPullParserException(
            //        "<not> tag can only contain a single child filter.", parser, null);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }
    AutoPtr<NotFilter> notFilter = new NotFilter(child);
    *result = IFilter::Probe(notFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
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
    VALIDATE_NOT_NULL(ret)
    mChild->Matches(ifw, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                receivingUid, ret);
    *ret = !*ret;
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
