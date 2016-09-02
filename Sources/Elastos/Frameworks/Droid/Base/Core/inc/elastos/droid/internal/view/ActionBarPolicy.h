#ifndef __ELASTOS_DROID_INTERNAL_VIEW_ACTIONBARPOLICY_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_ACTIONBARPOLICY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class ActionBarPolicy
    : public Object
    , public IActionBarPolicy
{
public:
    static CARAPI_(AutoPtr<IActionBarPolicy>) Get(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    CARAPI GetMaxActionButtons(
        /* [out] */ Int32* rst);

    CARAPI ShowsOverflowMenuButton(
        /* [out] */ Boolean* rst);

    CARAPI GetEmbeddedMenuWidthLimit(
        /* [out] */ Int32* rst);

    CARAPI HasEmbeddedTabs(
        /* [out] */ Boolean* rst);

    CARAPI GetTabContainerHeight(
        /* [out] */ Int32* rst);

    CARAPI EnableHomeButtonByDefault(
        /* [out] */ Boolean* rst);

    CARAPI GetStackedTabMaxWidth(
        /* [out] */ Int32* rst);

private:
    ActionBarPolicy(
        /* [in] */ IContext* ctx);

private:
    IContext* mContext;
};

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_ACTIONBARPOLICY_H__
