#include "elastos/droid/server/firewall/FilterList.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Internal::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

CAR_INTERFACE_IMPL(FilterList, Object, IFilter);

ECode FilterList::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;

    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        ReadChild(parser);
    }
    return NOERROR;
}

ECode FilterList::ReadChild(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<IFilter> filter;
    IntentFirewall::ParseFilter(parser, (IFilter**)&filter);
    mChildren->Add(TO_IINTERFACE(filter));
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos

