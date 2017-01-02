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
