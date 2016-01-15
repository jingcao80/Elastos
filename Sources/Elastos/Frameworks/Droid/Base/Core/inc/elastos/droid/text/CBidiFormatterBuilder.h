
#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTER_BUILDER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTER_BUILDER_H__

#include "_Elastos_Droid_Text_CBidiFormatterBuilder.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBidiFormatterBuilder)
    , public Object
    , public IBidiFormatterBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean rtlContext);

    CARAPI constructor(
        /* [in] */ ILocale* locale);

    /**
     * Specifies whether the BidiFormatter to be built should also "reset" directionality before
     * a string being bidi-wrapped, not just after it. The default is false.
     */
    CARAPI StereoReset(
        /* [in] */ Boolean stereoReset,
        /* [out] */ IBidiFormatterBuilder** ret);

    /**
     * Specifies the default directionality estimation algorithm to be used by the BidiFormatter.
     * By default, uses the first-strong heuristic.
     *
     * @param heuristic the {@code TextDirectionHeuristic} to use.
     * @return the builder itself.
     */
    CARAPI SetTextDirectionHeuristic(
        /* [in] */ ITextDirectionHeuristic* heuristic,
        /* [out] */ IBidiFormatterBuilder** ret);

    /**
     * @return A BidiFormatter with the specified options.
     */
    CARAPI Build(
        /* [out] */ IBidiFormatter** ret);
};

} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CBIDIFORMATTER_BUILDER_H__
