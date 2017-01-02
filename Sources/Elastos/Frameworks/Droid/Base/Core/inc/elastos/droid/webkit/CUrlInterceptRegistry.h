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

#ifndef __ELASTOS_DROID_WEBKIT_CURLINTERCEPTREGISTRY_H__
#define __ELASTOS_DROID_WEBKIT_CURLINTERCEPTREGISTRY_H__

#include "_Elastos_Droid_Webkit_CUrlInterceptRegistry.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Singleton.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CUrlInterceptRegistry)
    , public Singleton
    , public IUrlInterceptRegistry
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetUrlInterceptDisabled(
        /* [in] */ Boolean disabled);

    CARAPI UrlInterceptDisabled(
        /* [out] */ Boolean* disabled);

    CARAPI RegisterHandler(
        /* [in] */ IUrlInterceptHandler* handler,
        /* [out] */ Boolean* result);

    CARAPI UnregisterHandler(
        /* [in] */ IUrlInterceptHandler* handler,
        /* [out] */ Boolean* result);

    CARAPI GetSurrogate(
        /* [in] */ const String& url,
        /* [in] */ IMap* headers,
        /* [out] */ ICacheManagerCacheResult** cacheResult);

    CARAPI GetPluginData(
        /* [in] */ const String& url,
        /* [in] */ IMap* headers,
        /* [out] */ IPluginData** pluginData);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CURLINTERCEPTREGISTRY_H__
