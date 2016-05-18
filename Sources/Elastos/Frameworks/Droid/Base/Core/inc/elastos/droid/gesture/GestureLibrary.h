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
    CARAPI_(AutoPtr<Learner>) GetLearner();

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
