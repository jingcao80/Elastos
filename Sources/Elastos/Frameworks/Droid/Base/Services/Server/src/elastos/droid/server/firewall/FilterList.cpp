#include "elastos/droid/server/firewall/FilterList.h"

using Elastos::Droid::Internal::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

FilterList* FilterList::ReadFromXml(
    /* in */ IXmlPullParser* parser)
{
    Int32 outerDepth;

    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        ReadChild(parser);
    }
    return this;
}

void FilterList::ReadChild(
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<IFilter> filter = IntentFirewall::ParseFilter(parser);
    children->Add(TO_IInterface(filter));
}

} // Firewall
} // Server
} // Droid
} // Elastos

