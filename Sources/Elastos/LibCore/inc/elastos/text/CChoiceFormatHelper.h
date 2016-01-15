
#ifndef __ELASTOS_TEXT_CCHOICEFORMATHELPER_H__
#define __ELASTOS_TEXT_CCHOICEFORMATHELPER_H__

#include "_Elastos_Text_CChoiceFormatHelper.h"
#include "elastos/core/Singleton.h"


namespace Elastos {
namespace Text {

CarClass(CChoiceFormatHelper)
    , public Singleton
    , public IChoiceFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NextDouble(
        /* [in] */ Double value,
        /* [out] */ Double* nextValue);

    CARAPI NextDouble(
        /* [in] */ Double value,
        /* [in] */ Boolean increment,
        /* [out] */ Double* nextValue);

    CARAPI PreviousDouble(
        /* [in] */ Double value,
        /* [out] */ Double* previousValue);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CCHOICEFORMATHELPER_H__
