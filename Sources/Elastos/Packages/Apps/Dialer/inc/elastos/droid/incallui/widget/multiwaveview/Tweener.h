
#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__

#include "Elastos.Droid.Animation.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;

using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

class Tweener
    : public Object
{
public:
    static CARAPI To(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ Tweener** result);

    static CARAPI_(void) Reset();

public:
    AutoPtr<IObjectAnimator> mAnimator;

private:
    static const String TAG;
    static const Boolean DEBUG;

    static AutoPtr<IHashMap> sTweens;

    // Listener to watch for completed animations and remove them.
    static AutoPtr<IAnimatorListener> mCleanupListener;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__
