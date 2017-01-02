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

#ifndef __ELASTOS_DROID_WEBKIT_URLINTERCEPTHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_URLINTERCEPTHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * @hide
 * @deprecated This interface was inteded to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
//@Deprecated
class UrlInterceptHandler : public Object
{
public:
    /**
     * Given an URL, returns the CacheResult which contains the
     * surrogate response for the request, or null if the handler is
     * not interested.
     *
     * @param url URL string.
     * @param headers The headers associated with the request. May be null.
     * @return The CacheResult containing the surrogate response.
     *
     * @hide
     * @deprecated Do not use, this interface is deprecated.
     */
    //@Deprecated
    virtual CARAPI_(AutoPtr<ICacheManagerCacheResult>) Service(
        /* [in] */ const String& url,
        /* [in] */ IObjectStingMap* headers) = 0;

    /**
     * Given an URL, returns the PluginData which contains the
     * surrogate response for the request, or null if the handler is
     * not interested.
     *
     * @param url URL string.
     * @param headers The headers associated with the request. May be null.
     * @return The PluginData containing the surrogate response.
     *
     * @hide
     * @deprecated Do not use, this interface is deprecated.
     */
    //@Deprecated
    virtual CARAPI_(AutoPtr<PluginData>) GetPluginData(
        /* [in] */ const String& url,
        /* [in] */ IObjectStingMap* headers) = 0;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_URLINTERCEPTHANDLER_H__
