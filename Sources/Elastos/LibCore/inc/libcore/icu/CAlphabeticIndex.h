#ifndef __LIBCORE_ICU_CALPHABETICINDEX_H__
#define __LIBCORE_ICU_CALPHABETICINDEX_H__

#include "AlphabeticIndex.h"
#include "_Libcore_ICU_CAlphabeticIndex.h"

namespace Libcore {
namespace ICU {

CarClass(CAlphabeticIndex)
    , public AlphabeticIndex
{
public:
    CAR_OBJECT_DECL()
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_CALPHABETICINDEX_H__