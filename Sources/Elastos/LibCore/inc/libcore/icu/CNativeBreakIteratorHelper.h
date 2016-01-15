
#ifndef __LIBCORE_ICU_CNATIVEBREAKITERATORHELPER_H__
#define __LIBCORE_ICU_CNATIVEBREAKITERATORHELPER_H__

#include <Singleton.h>
#include "_Libcore_ICU_CNativeBreakIteratorHelper.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CNativeBreakIteratorHelper)
    , public Singleton
    , public INativeBreakIteratorHelper
{
    CAR_SINGLETON_DECL()

public:
    CAR_INTERFACE_DECL()

    CARAPI GetCharacterInstance(
        /* [in] */ ILocale * where,
        /* [out] */ INativeBreakIterator ** outiter);

    CARAPI GetLineInstance(
        /* [in] */ ILocale * where,
        /* [out] */ INativeBreakIterator ** outiter);

    CARAPI GetSentenceInstance(
        /* [in] */ ILocale * where,
        /* [out] */ INativeBreakIterator ** outiter);

    CARAPI GetWordInstance(
        /* [in] */ ILocale * where,
        /* [out] */ INativeBreakIterator ** outiter);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CNATIVEBREAKITERATORHELPER_H__