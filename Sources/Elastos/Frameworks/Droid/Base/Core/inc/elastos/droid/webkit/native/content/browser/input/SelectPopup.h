
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUP_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUP_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Handles the popup UI for the <select> HTML tag support.
 */
class SelectPopup : public Object
{
public:
    /**
     * Shows the popup.
     */
    virtual CARAPI_(void) Show() = 0;

    /**
     * Hides the popup.
     */
    virtual CARAPI_(void) Hide() = 0;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUP_H__
