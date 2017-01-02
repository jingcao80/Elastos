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

#ifndef __ELASTOS_DROID_MEDIA_PROJECTION_MEDIAPROJECTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_PROJECTION_MEDIAPROJECTIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

class MediaProjectionCallback
    : public Object
    , public IMediaProjectionCallback
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Called when the MediaProjection session is no longer valid.
     * <p>
     * Once a MediaProjection has been stopped, it's up to the application to release any
     * resources it may be holding (e.g. {@link android.hardware.display.VirtualDisplay}s).
     * </p>
     */
    CARAPI OnStop();

};

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_PROJECTION_MEDIAPROJECTIONCALLBACK_H__
