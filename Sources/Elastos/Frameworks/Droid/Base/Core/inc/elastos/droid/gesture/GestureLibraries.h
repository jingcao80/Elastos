#ifndef __ELASTOS_DROID_GESTURE_GESTURELIBRARIES_H__
#define __ELASTOS_DROID_GESTURE_GESTURELIBRARIES_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/gesture/GestureLibrary.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Gesture::IGestureLibraries;
using Elastos::Droid::Gesture::IGestureUtils;

namespace Elastos {
namespace Droid {
namespace Gesture {

/*
 * Java: public final class GestureLibraries
 */
class GestureLibraries
{
public:
    class FileGestureLibrary
        : public GestureLibrary
    {
    public:
        FileGestureLibrary(
            /* [in] */ IFile *path);

        FileGestureLibrary();

        virtual ~FileGestureLibrary();

        CARAPI IsReadOnly(
            /* [out] */ Boolean *isReadOnly);

        CARAPI Save(
            /* [out] */ Boolean *isSaved);

        CARAPI Load(
            /* [out] */ Boolean *isLoaded);

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

        CARAPI GetLearner(
            /* [out] */ ILearner** learner);

        CARAPI Recognize(
            /* [in] */ IGesture *gesture,
            /* [out] */ IArrayList **predictions);

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
            /* [out] */ IArrayList **gestures);

    private:
        const AutoPtr<IFile> mPath;
    };

    class ResourceGestureLibrary
        : public GestureLibrary
    {
    public:
        ResourceGestureLibrary(
            /* [in] */ IContext *context,
            /* [in] */ Int32 resourceId);

        virtual ~ResourceGestureLibrary();

        CARAPI constructor(
            /* [in] */ IContext *context,
            /* [in] */ Int32 resourceId);

        CARAPI IsReadOnly(
            /* [out] */ Boolean *isReadOnly);

        CARAPI Save(
            /* [out] */ Boolean *isSaved);

        CARAPI Load(
            /* [out] */ Boolean *isLoaded);

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

        CARAPI GetLearner(
            /* [out] */ ILearner** learner);

        CARAPI Recognize(
            /* [in] */ IGesture *gesture,
            /* [out] */ IArrayList **predictions);

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
            /* [out] */ IArrayList **gestures);

    private:
        AutoPtr<IWeakReference> mContext;
        const Int32 mResourceId;
    };

public:
    static CARAPI_(AutoPtr<IGestureLibrary>) FromFile(
        /* [in] */ const String& path);

    static CARAPI_(AutoPtr<IGestureLibrary>) FromFile(
        /* [in] */ IFile* path);

    static CARAPI_(AutoPtr<IGestureLibrary>) FromPrivateFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& name);

    static CARAPI_(AutoPtr<IGestureLibrary>) FromRawResource(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId);

private:
    GestureLibraries();
    GestureLibraries(const GestureLibraries&);
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTURELIBRARIES_H__
