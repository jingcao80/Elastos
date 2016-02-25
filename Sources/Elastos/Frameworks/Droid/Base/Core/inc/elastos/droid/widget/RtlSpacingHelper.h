
#ifndef __ELASTOS_DROID_WIDGET_RTLSPACINGHELPER_H__
#define __ELASTOS_DROID_WIDGET_RTLSPACINGHELPER_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * RtlSpacingHelper manages the relationship between left/right and start/end for views
 * that need to maintain both absolute and relative settings for a form of spacing similar
 * to view padding.
 */
class RtlSpacingHelper: public Object
{
public:
    RtlSpacingHelper();

    CARAPI_(Int32) GetLeft();

    CARAPI_(Int32) GetRight();

    CARAPI_(Int32) GetStart();

    CARAPI_(Int32) GetEnd();

    CARAPI_(void) SetRelative(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) SetAbsolute(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

    CARAPI_(void) SetDirection(
        /* [in] */ Boolean isRtl);

public:
    static const Int32 UNDEFINED;

private:
    Int32 mLeft;
    Int32 mRight;
    Int32 mStart;
    Int32 mEnd;
    Int32 mExplicitLeft;
    Int32 mExplicitRight;

    Boolean mIsRtl;
    Boolean mIsRelative;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_RTLSPACINGHELPER_H__
