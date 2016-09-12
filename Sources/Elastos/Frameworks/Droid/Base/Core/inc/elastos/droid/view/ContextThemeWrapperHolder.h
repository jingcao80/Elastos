
#ifndef __ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPER_HOLDER_H__
#define __ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPER_HOLDER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/view/ContextThemeWrapper.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace View {

class ContextThemeWrapperHolder
    : public ContextThemeWrapper
    , public IContextThemeWrapperHolder
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("ContextThemeWrapperHolder")

    ContextThemeWrapperHolder();

    ~ContextThemeWrapperHolder();

    Boolean mMemoryLeakTarget;
    Boolean mMemoryCount;

};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPER_HOLDER_H__
