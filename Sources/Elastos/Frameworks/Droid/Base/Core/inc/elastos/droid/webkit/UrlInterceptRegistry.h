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

#ifndef __ELASTOS_DROID_WEBKIT_URLINTERCEPTREGISTRY_H__
#define __ELASTOS_DROID_WEBKIT_URLINTERCEPTREGISTRY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Webkit {

class UrlInterceptRegistry
{
public:
    static CARAPI SetUrlInterceptDisabled(
        /* [in] */ Boolean disabled);

    static CARAPI_(Boolean) UrlInterceptDisabled();

    static CARAPI_(Boolean) RegisterHandler(
        /* [in] */ IUrlInterceptHandler* handler);

    static CARAPI_(Boolean) UnregisterHandler(
        /* [in] */ IUrlInterceptHandler* handler);

    static CARAPI_(AutoPtr<ICacheManagerCacheResult>) GetSurrogate(
        /* [in] */ const String& url,
        /* [in] */ IMap* headers);

    static CARAPI_(AutoPtr<IPluginData>) GetPluginData(
        /* [in] */ const String& url,
        /* [in] */ IMap* headers);

private:
    static String LOGTAG;

    static Boolean mDisabled;

    static List< AutoPtr<IInterface> > mHandlerList;

    static Object sLock;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_URLINTERCEPTREGISTRY_H__
