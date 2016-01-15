#ifndef __ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDACTIVITY_H__
#define __ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDACTIVITY_H__

#include "elastos/droid/content/IntentFilter.h"
#include "elastos/droid/server/pm/PreferredComponent.h"

using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Droid::Content::IntentFilter;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PersistentPreferredActivity : public IntentFilter
{
public:
    PersistentPreferredActivity(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ IComponentName* activity);

    PersistentPreferredActivity(
        /* [in] */ IXmlPullParser* parser);

    CARAPI WriteToXml(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ Boolean full);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IComponentName> mComponent;

private:
    static const String ATTR_NAME; // component name
    static const String ATTR_FILTER; // filter

    static const String TAG;

    static const Boolean DEBUG_FILTERS = FALSE;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Pm::PersistentPreferredActivity, IInterface)

#endif //__ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDACTIVITY_H__
