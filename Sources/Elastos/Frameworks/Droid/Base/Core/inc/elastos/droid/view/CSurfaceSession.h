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

#ifndef __ELASTOS_DROID_VIEW_CSURFACESESSION_H__
#define __ELASTOS_DROID_VIEW_CSURFACESESSION_H__

#include "_Elastos_Droid_View_CSurfaceSession.h"
#include <elastos/core/Object.h>

#include <gui/SurfaceComposerClient.h>


namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSurfaceSession)
    , public Object
    , public ISurfaceSession
{
    friend class SurfaceControl;

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSurfaceSession();

    ~CSurfaceSession();

    CARAPI constructor();

    /**
     * Forcibly detach native resources associated with this object.
     * Unlike destroy(), after this call any surfaces that were created
     * from the session will no longer work.
     */
    CARAPI Kill();

private:
    // Note: This field is accessed by native code.
    android::sp<android::SurfaceComposerClient> mNativeClient;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CSURFACESESSION_H__
