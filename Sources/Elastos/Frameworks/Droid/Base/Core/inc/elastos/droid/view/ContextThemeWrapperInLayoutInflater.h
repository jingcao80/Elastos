
#ifndef __ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPERINLAYOUTINFLATER_H__
#define __ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPERINLAYOUTINFLATER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/view/ContextThemeWrapper.h"

namespace Elastos {
namespace Droid {
namespace View {

class ContextThemeWrapperInLayoutInflater
    : public ContextThemeWrapper
    , public IContextThemeWrapperInLayoutInflater
{
public:
    CAR_INTERFACE_DECL();

    CARAPI ToString(
        /* [out] */ String* str);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPERINLAYOUTINFLATER_H__
