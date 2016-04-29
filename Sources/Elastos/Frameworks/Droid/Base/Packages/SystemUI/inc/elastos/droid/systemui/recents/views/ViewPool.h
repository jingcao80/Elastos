
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWPOOL_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWPOOL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::ILinkedList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {


/* A view pool to manage more views than we can visibly handle */
class ViewPool : public Object
{
public:
    /** Initializes the pool with a fixed predetermined pool size */
    ViewPool(
        /* [in] */ IContext* context,
        /* [in] */ IViewPoolConsumer* viewCreator);

    /** Returns a view into the pool */
    void ReturnViewToPool(
        /* [in] */ IInterface* v);

    /** Gets a view from the pool and prepares it */
    AutoPtr<IInterface> PickUpViewFromPool(
        /* [in] */ IInterface* preferredData,
        /* [in] */ IInterface* prepareData);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IViewPoolConsumer> mViewCreator;
    AutoPtr<ILinkedList> mPool;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWPOOL_H__
