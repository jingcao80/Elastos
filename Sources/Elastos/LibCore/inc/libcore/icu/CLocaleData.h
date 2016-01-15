
#ifndef __LIBCORE_ICU_ICU_CLOCALEDATA_H__
#define __LIBCORE_ICU_ICU_CLOCALEDATA_H__

#include "_Libcore_ICU_CLocaleData.h"
#include "LocaleData.h"

using Elastos::Core::IInteger32;

namespace Libcore {
namespace ICU {

CarClass(CLocaleData)
    , public LocaleData
{
public:
    CAR_OBJECT_DECL()
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_ICU_CLOCALEDATA_H__

