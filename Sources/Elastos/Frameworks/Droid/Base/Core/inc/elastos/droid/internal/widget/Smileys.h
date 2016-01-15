
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_SMILEYS_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_SMILEYS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Internal::Widget::ISmileys;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class Smileys
    : public Object
    , public ISmileys
{
public:
    CAR_INTERFACE_DECL()

    Smileys();

    static CARAPI GetSmileyResource(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

private:
    static CARAPI_(AutoPtr< ArrayOf<Int32> >) InnerInitIconds();

private:
    static const AutoPtr< ArrayOf<Int32> > sIconIds;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_SMILEYS_H__
