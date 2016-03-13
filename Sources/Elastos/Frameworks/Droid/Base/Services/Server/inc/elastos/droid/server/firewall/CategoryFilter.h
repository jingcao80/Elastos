#ifndef __ELASTOS_DROID_Server_Firewall_CategoryFilter_H__
#define __ELASTOS_DROID_Server_Firewall_CategoryFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"

using Elastos::Droid::internal.util.XmlUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Server::Firewall::IFilter;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class CategoryFilter
    : public Object
    , public IFilter
{
public:
    class FACTORY_FilterFactory {
        // throws IOException, XmlPullParserException;
        CARAPI_(Filter*) NewFilter(
            /* in */ IXmlPullParser* parser);
    };

    //@Override
    CARAPI Matches(
        /* [in] */ IIntentFirewall* ifw,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid
        /* [out] */ Boolean *ret);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY = new FACTORY_FilterFactory(String("category"));

private:
    CategoryFilter(
        /* in */ const String& categoryName);

private:
    static const String ATTR_NAME;      // = "name";

    String mCategoryName;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_CategoryFilter_H__
