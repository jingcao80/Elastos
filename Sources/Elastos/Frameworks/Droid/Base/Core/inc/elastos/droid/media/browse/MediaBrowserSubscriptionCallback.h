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

#ifndef __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERSUBSCRIPTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERSUBSCRIPTIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

class MediaBrowserSubscriptionCallback
    : public Object
    , public IMediaBrowserSubscriptionCallback
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Called when the list of children is loaded or updated.
     */
    CARAPI OnChildrenLoaded(
        /* [in] */ const String& parentId,
        /* [in] */ IList * children);

    /**
     * Called when the id doesn't exist or other errors in subscribing.
     * <p>
     * If this is called, the subscription remains until {@link MediaBrowser#unsubscribe}
     * called, because some errors may heal themselves.
     * </p>
     */
    CARAPI OnError(
        /* [in] */ const String& id);

};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERSUBSCRIPTIONCALLBACK_H__
