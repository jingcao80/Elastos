
#ifndef __ELASTOS_DROID_TEXT_CAUTOTEXTHELPER_H__
#define __ELASTOS_DROID_TEXT_CAUTOTEXTHELPER_H__

#include "_Elastos_Droid_Text_CAutoTextHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CAutoTextHelper)
    , public Singleton
    , public IAutoTextHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Get(
        /* [in] */ ICharSequence* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IView* view,
        /* [out] */ String* ret);

    CARAPI GetSize(
        /* [in] */ IView* view,
        /* [out] */ Int32* ret);

};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CAUTOTEXTHELPER_H__
