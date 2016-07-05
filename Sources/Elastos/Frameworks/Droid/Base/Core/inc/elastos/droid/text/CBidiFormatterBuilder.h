
#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTERBUILDER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTERBUILDER_H__

#include "_Elastos_Droid_Text_CBidiFormatterBuilder.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
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

    CBidiFormatterBuilder();

    ~CBidiFormatterBuilder();

    /**
     * Constructor.
     *
     */
    CARAPI constructor();

    /**
     * Constructor.
     *
     * @param rtlContext Whether the context directionality is RTL.
     */
    CARAPI constructor(
        /* [in] */ Boolean rtlContext);

    /**
     * Constructor.
     *
     * @param locale The context locale.
     */
    CARAPI constructor(
        /* [in] */ ILocale* locale);

    /**
     * Specifies whether the BidiFormatter to be built should also "reset" directionality before
     * a string being bidi-wrapped, not just after it. The default is false.
     */
    CARAPI StereoReset(
        /* [in] */ Boolean stereoReset);

    /**
     * Specifies the default directionality estimation algorithm to be used by the BidiFormatter.
     * By default, uses the first-strong heuristic.
     *
     * @param heuristic the {@code TextDirectionHeuristic} to use.
     * @return the builder itself.
     */
    CARAPI SetTextDirectionHeuristic(
        /* [in] */ ITextDirectionHeuristic* heuristic);

    /**
     * @return A BidiFormatter with the specified options.
     */
    CARAPI Build(
        /* [out] */ IBidiFormatter** ret);

private:
    /**
     * Initializes the builder with the given context directionality and default options.
     *
     * @param isRtlContext Whether the context is RTL or not.
     */
    CARAPI_(void) Initialize(
        /* [in] */ Boolean isRtlContext);

    static CARAPI_(AutoPtr<IBidiFormatter>) GetDefaultInstanceFromContext(
        /* [in] */ Boolean isRtlContext);

private:
    Boolean mIsRtlContext;
    Int32 mFlags;
    AutoPtr<ITextDirectionHeuristic> mTextDirectionHeuristic;
};

} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CBIDIFORMATTERBUILDER_H__
