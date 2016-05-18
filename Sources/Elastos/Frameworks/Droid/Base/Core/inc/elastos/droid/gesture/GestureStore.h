#ifndef __ELASTOS_DROID_GESTURE_GESTURESTORE_H__
#define __ELASTOS_DROID_GESTURE_GESTURESTORE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "elastos/droid/gesture/CGesture.h"
#include "elastos/droid/gesture/Learner.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Gesture::IGestureStore;

namespace Elastos {
namespace Droid {
namespace Gesture {

class GestureStore
    : public Object
    , public IGestureStore
{
public:
    CAR_INTERFACE_DECL()

    GestureStore();

    virtual ~GestureStore();

    CARAPI constructor();

    /**
     * Specify how the gesture library will handle orientation.
     * Use ORIENTATION_INVARIANT or ORIENTATION_SENSITIVE
     *
     * @param style
     */
    CARAPI SetOrientationStyle(
        /* [in] */ Int32 style);

    CARAPI GetOrientationStyle(
        /* [out] */ Int32 *style);

    /**
     * @param type SEQUENCE_INVARIANT or SEQUENCE_SENSITIVE
     */
    CARAPI SetSequenceType(
        /* [in] */ Int32 type);

    /**
     * @return SEQUENCE_INVARIANT or SEQUENCE_SENSITIVE
     */
    CARAPI GetSequenceType(
        /* [out] */ Int32 *type);

    /**
     * Get all the gesture entry names in the library
     *
     * @return a set of strings
     */
    CARAPI GetGestureEntries(
        /* [out] */ IList **entries);

    /**
     * Recognize a gesture
     *
     * @param gesture the query
     * @return a list of predictions of possible entries for a given gesture
     */
    CARAPI Recognize(
        /* [in] */ IGesture *gesture,
        /* [out] */ IArrayList** list);

    /**
     * Add a gesture for the entry
     *
     * @param entryName entry name
     * @param gesture
     */
    CARAPI AddGesture(
        /* [in] */ const String& entryName,
        /* [in] */ IGesture *gesture);

    /**
     * Remove a gesture from the library. If there are no more gestures for the
     * given entry, the gesture entry will be removed.
     *
     * @param entryName entry name
     * @param gesture
     */
    CARAPI RemoveGesture(
        /* [in] */  const String& entryName,
        /* [in] */  IGesture *gesture);

    /**
     * Remove a entry of gestures
     *
     * @param entryName the entry name
     */
    CARAPI RemoveEntry(
        /* [in] */ const String& entryName);

    /**
     * Get all the gestures of an entry
     *
     * @param entryName
     * @return the list of gestures that is under this name
     */
    CARAPI GetGestures(
        /* [in] */ const String& entryName,
        /* [out] */ IArrayList** value);

    CARAPI HasChanged(
        /* [out] */  Boolean *hasChanged);

    /**
     * Save the gesture library
     */
    CARAPI Save(
        /* [in] */ IOutputStream *stream);

    CARAPI Save(
        /* [in] */ IOutputStream *stream,
        /* [in] */ Boolean closeStream);

    /**
     * Load the gesture library
     */
    CARAPI Load(
        /* [in] */ IInputStream *stream);

    CARAPI Load(
        /* [in] */ IInputStream *stream,
        /* [in] */ Boolean closeStream);

    CARAPI_(AutoPtr<Learner>) GetLearner();

private:
    CARAPI_(void) ReadFormatV1(
        /* [in] */ IDataInputStream *in);

private:
    static const Int16 FILE_FORMAT_VERSION;

    static const Boolean PROFILE_LOADING_SAVING;

    Int32 mSequenceType;
    Int32 mOrientationStyle;

    //private final HashMap<String, ArrayList<Gesture>> mNamedGestures =
    //        new HashMap<String, ArrayList<Gesture>>();
    typedef List<AutoPtr<IGesture> > GestureList;
    HashMap<String, AutoPtr<GestureList> > mNamedGestures;

    AutoPtr<Learner> mClassifier;

    Boolean mChanged;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTURESTORE_H__
