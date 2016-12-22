#ifndef __ELASTOS_DROID_Server_Firewall_FilterList_H__
#define __ELASTOS_DROID_Server_Firewall_FilterList_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/utility/XmlUtils.h"

using Elastos::Droid::Internal::Utility::XmlUtils;
using Org::Xmlpull::V1::IXmlPullParser;
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
    CAR_INTERFACE_DECL()

    // throws IOException, XmlPullParserException
    CARAPI ReadFromXml(
        /* in */ IXmlPullParser* parser);

    // throws IOException, XmlPullParserException
    CARAPI_(void) ReadChild(
        /* in */ IXmlPullParser* parser);

protected:
    AutoPtr<IArrayList> children;

};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_FilterList_H__
