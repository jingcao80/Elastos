//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
