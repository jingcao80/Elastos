
#ifndef __ELASTOS_DROID_TRANSITION_CSCENEHELPER_H__
#define __ELASTOS_DROID_TRANSITION_CSCENEHELPER_H__

#include "_Elastos_Droid_Transition_CSceneHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Content::IContext;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass (CSceneHelper)
    , public Singleton
    , public ISceneHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSceneForLayout(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ Int32 layoutId,
        /* [in] */ IContext* context,
        /* [out] */ IScene** result);

    CARAPI SetCurrentScene(
        /* [in] */ IView* vw,
        /* [in] */ IScene* scene);

    CARAPI GetCurrentScene(
        /* [in] */ IView* view,
        /* [out] */ IScene** result);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CSCENEHELPER_H__