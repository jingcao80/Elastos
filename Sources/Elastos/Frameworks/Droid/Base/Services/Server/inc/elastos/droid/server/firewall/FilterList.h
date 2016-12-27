#ifndef __ELASTOS_DROID_Server_Firewall_FilterList_H__
#define __ELASTOS_DROID_Server_Firewall_FilterList_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Server::Firewall::IFilter;

using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

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
        /* [in] */ IXmlPullParser* parser);

    // throws IOException, XmlPullParserException
    CARAPI ReadChild(
        /* [in] */ IXmlPullParser* parser);

protected:
    AutoPtr<IArrayList> mChildren;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_FilterList_H__
