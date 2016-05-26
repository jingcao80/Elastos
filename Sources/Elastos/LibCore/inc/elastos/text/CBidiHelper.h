
#ifndef __ELASTOS_TEXT_CBIDIHELPER_H__
#define __ELASTOS_TEXT_CBIDIHELPER_H__

#include "_Elastos_Text_CBidiHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Text {

CarClass(CBidiHelper)
    , public Singleton
    , public IBidiHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ReorderVisually(
        /* [in] */ ArrayOf<Byte>* levels,
        /* [in] */ Int32 levelStart,
        /* [in] */ ArrayOf<IInterface*>* objects,
        /* [in] */ Int32 objectStart,
        /* [in] */ Int32 count);

    CARAPI RequiresBidi(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [out] */ Boolean * result);
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CBIDIHELPER_H__
