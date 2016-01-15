
#ifndef __ELASTOS_DROID_CONTENT_SYNCADAPTERSCACHE_H__
#define __ELASTOS_DROID_CONTENT_SYNCADAPTERSCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/RegisteredServicesCache.h"

using namespace Elastos::Droid::Content::Pm;
using namespace Elastos::Droid::Content::Res;
using Elastos::Droid::Utility::IAttributeSet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * A cache of services that export the {@link android.content.ISyncAdapter} interface.
 * @hide
 */
class SyncAdaptersCache
    : public RegisteredServicesCache
    , public ISyncAdaptersCache
{
public:
    class MySerializer
        : public Object
        , public IXmlSerializerAndParser
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI WriteAsXml(
            /* [in] */ IInterface* item,
            /* [in] */ IXmlSerializer* serializer);

        CARAPI CreateFromXml(
            /* [in] */ IXmlPullParser* parser,
            /* [out] */ IInterface** obj);

    };

public:
    CAR_INTERFACE_DECL()

    SyncAdaptersCache();

    virtual ~SyncAdaptersCache();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI ParseServiceAttributes(
        /* [in] */ IResources* res,
        /* [in] */ const String& packageName,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** syncAdapterType);

private:
    static const String TAG;
    static const String SERVICE_INTERFACE;
    static const String SERVICE_META_DATA;
    static const String ATTRIBUTES_NAME;
    static const AutoPtr<SyncAdaptersCache::MySerializer> sSerializer;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_SYNCADAPTERSCACHE_H__
