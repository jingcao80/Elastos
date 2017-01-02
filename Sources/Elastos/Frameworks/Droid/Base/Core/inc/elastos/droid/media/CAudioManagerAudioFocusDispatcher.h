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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMANAGERAUDIOFOCUSDISPATCHER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMANAGERAUDIOFOCUSDISPATCHER_H__

#include "_Elastos_Droid_Media_CAudioManagerAudioFocusDispatcher.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioManagerAudioFocusDispatcher)
    , public Object
    , public IIAudioFocusDispatcher
    , public IBinder
{
public:
    CAudioManagerAudioFocusDispatcher();

    virtual ~CAudioManagerAudioFocusDispatcher();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IAudioManager* owner);

    CARAPI DispatchAudioFocusChange(
        /* [in] */ Int32 focusChange,
        /* [in] */ const String& id);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IWeakReference> mOwnerWeak;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMANAGERAUDIOFOCUSDISPATCHER_H__
