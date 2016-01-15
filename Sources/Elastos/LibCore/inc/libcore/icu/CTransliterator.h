
#ifndef __LIBCORE_ICU_CTRANSLITERATOR__
#define __LIBCORE_ICU_CTRANSLITERATOR__

#include "Transliterator.h"
#include "_Libcore_ICU_CTransliterator.h"

namespace Libcore {
namespace ICU {

CarClass(CTransliterator)
    , public Transliterator
{
public:
    CAR_OBJECT_DECL()
};

} // ICU
} // Libcore

#endif // __LIBCORE_ICU_CTRANSLITERATOR__