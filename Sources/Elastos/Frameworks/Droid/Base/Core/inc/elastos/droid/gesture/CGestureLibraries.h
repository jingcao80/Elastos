#ifndef __ELASTOS_DROID_GESTURE_CGESTURELIBRARIES_H__
#define __ELASTOS_DROID_GESTURE_CGESTURELIBRARIES_H__

#include "_Elastos_Droid_Gesture_CGestureLibraries.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Gesture::IGestureLibraries;

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGestureLibraries)
    , public Singleton
    , public IGestureLibraries
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();

    CARAPI FromFile(
        /* [in] */  const String& path,
        /* [out] */ IGestureLibrary** gestureLib);

    CARAPI FromFile(
        /* [in] */ IFile* path,
        /* [out] */ IGestureLibrary** gestureLib);

    CARAPI FromPrivateFile(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& name,
        /* [out] */ IGestureLibrary** gestureLib);

    CARAPI FromRawResource(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 resourceId,
        /* [out] */ IGestureLibrary** gestureLib);
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTURELIBRARIES_H__
