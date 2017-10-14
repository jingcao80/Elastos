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

#ifndef __ELASTOS_DROID_GESTURE_GESTURELIBRARY_H__
#define __ELASTOS_DROID_GESTURE_GESTURELIBRARY_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/Learner.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Gesture::IGestureLibrary;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Gesture {

class GestureLibrary
    : public Object
    , public IGestureLibrary
{
public:
    CAR_INTERFACE_DECL()

    GestureLibrary();

    virtual ~GestureLibrary();

    CARAPI Save(
        /* [out] */ Boolean *isSaved) = 0;

    CARAPI Load(
        /* [out] */ Boolean *isLoaded) = 0;

    CARAPI IsReadOnly(
        /* [out] */ Boolean *isReadOnly);

    /** @hide */
    CARAPI GetLearner(
        /* [out] */ ILearner** learner);

    CARAPI SetOrientationStyle(
        /* [in] */ Int32 style);

    CARAPI GetOrientationStyle(
        /* [out] */ Int32 *style);

    CARAPI SetSequenceType(
        /* [in] */ Int32 type);

    CARAPI GetSequenceType(
        /* [out] */ Int32 *type);

    CARAPI GetGestureEntries(
        /* [out] */ IList **entries);

    CARAPI Recognize(
        /* [in] */ IGesture *gesture,
        /* [out] */ IArrayList** list);

    CARAPI AddGesture(
        /* [in] */  const String& entryName,
        /* [in] */  IGesture *gesture);

    CARAPI RemoveGesture(
        /* [in] */ const String& entryName,
        /* [in] */ IGesture *gesture);

    CARAPI RemoveEntry(
        /* [in] */ const String& entryName);

    CARAPI GetGestures(
        /* [in] */ const String& entryName,
        /* [out] */ IArrayList** gestures);
public:
    AutoPtr<IGestureStore> mStore;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTURELIBRARY_H__
