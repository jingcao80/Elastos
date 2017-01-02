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

#ifndef __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERCONNECTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERCONNECTIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

class MediaBrowserConnectionCallback
    : public Object
    , public IMediaBrowserConnectionCallback
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Invoked after {@link MediaBrowser#connect()} when the request has successfully completed.
     */
    CARAPI OnConnected();

    /**
     * Invoked when the client is disconnected from the media browser.
     */
    CARAPI OnConnectionSuspended();

    /**
     * Invoked when the connection to the media browser failed.
     */
    CARAPI OnConnectionFailed();

};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERCONNECTIONCALLBACK_H__
