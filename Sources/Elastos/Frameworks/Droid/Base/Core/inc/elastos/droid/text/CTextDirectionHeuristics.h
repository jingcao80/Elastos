
#ifndef __ELASTOS_DROID_TEXT_CTEXTDIRECTIONHEURISTICS_H__
#define __ELASTOS_DROID_TEXT_CTEXTDIRECTIONHEURISTICS_H__

#include "_Elastos_Droid_Text_CTextDirectionHeuristics.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CTextDirectionHeuristics)
    , public Singleton
    , public ITextDirectionHeuristics
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetLTR(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetRTL(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetFIRSTSTRONG_LTR(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetFIRSTSTRONG_RTL(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetANYRTL_LTR(
        /* [out] */ ITextDirectionHeuristic** tdh);

    CARAPI GetLOCALE(
        /* [out] */ ITextDirectionHeuristic** tdh);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CTEXTDIRECTIONHEURISTICS_H__
