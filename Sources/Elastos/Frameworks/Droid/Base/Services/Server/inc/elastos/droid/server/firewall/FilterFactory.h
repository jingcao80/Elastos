#ifndef __ELASTOS_DROID_Server_Firewall_FilterFactory_H__
#define __ELASTOS_DROID_Server_Firewall_FilterFactory_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Firewall::IFilter;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class FilterFactory
    : public Object
{
public:
    CARAPI_(String) GetTagName();

    virtual CARAPI NewFilter(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IFilter** result) = 0;

protected:
    FilterFactory();

    CARAPI constructor(
        /* [in] */ const String& tag);

private:
    String mTag;
};

} // Firewall
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Firewall::FilterFactory, IInterface)

#endif // __ELASTOS_DROID_Server_Firewall_FilterFactory_H__
