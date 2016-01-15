
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_FONTSIZEUTILS_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_FONTSIZEUTILS_H__

#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

/**
 * Utility class to update the font size when the configuration has changed.
 */
class FontSizeUtils
{
public:
    static CARAPI_(void) UpdateFontSize(
        /* [in] */ IView* parent,
        /* [in] */ Int32 viewId,
        /* [in] */ Int32 dimensId);

    static CARAPI_(void) UpdateFontSize(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 dimensId);

public:
    static const Float LARGE_TEXT_SCALE;
};

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_FONTSIZEUTILS_H__
