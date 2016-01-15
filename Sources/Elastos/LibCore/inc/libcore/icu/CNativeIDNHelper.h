
#ifndef __LIBCORE_ICU_CNATIVEIDNHELPER_H__
#define __LIBCORE_ICU_CNATIVEIDNHELPER_H__

#include <Singleton.h>
#include "_Libcore_ICU_CNativeIDNHelper.h"

using Elastos::Core::ICharSequence;

namespace Libcore {
namespace ICU {

CarClass(CNativeIDNHelper)
    , public Singleton
    , public INativeIDNHelper
{
    CAR_SINGLETON_DECL()

public:
    CAR_INTERFACE_DECL()

    CARAPI ToASCII(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [out] */ String * str);

    CARAPI ToUnicode(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [out] */ String * str);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CNATIVEIDNHELPER_H__
