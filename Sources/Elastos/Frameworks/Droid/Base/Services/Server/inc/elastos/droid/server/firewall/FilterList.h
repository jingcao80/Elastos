#ifndef __ELASTOS_DROID_Server_Firewall_FilterList_H__
#define __ELASTOS_DROID_Server_Firewall_FilterList_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"

using Elastos::Droid::internal.util.XmlUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Server::Firewall::IFilter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class FilterList
    : public Object
    , public IFilter
{
public:
    // throws IOException, XmlPullParserException
    CARAPI_(FilterList*) ReadFromXml(
        /* in */ IXmlPullParser* parser);

    // throws IOException, XmlPullParserException
    CARAPI_(void) ReadChild(
        /* in */ IXmlPullParser* parser);

protected:
    AutoPtr<IArrayList<IFilter> > children;

};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_FilterList_H__
