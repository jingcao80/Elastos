
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPITEM_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPITEM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/ui/DropdownItem.h"

using Elastos::Droid::Webkit::Ui::DropdownItem;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Select popup item containing the label, the type and the enabled state
 * of an item belonging to a select popup dialog.
 */
class SelectPopupItem
    : public DropdownItem
{
public:
    SelectPopupItem(
        /* [in] */ const String& label,
        /* [in] */ Int32 type);

    //@Override
    CARAPI_(String) GetLabel();

    //@Override
    CARAPI_(String) GetSublabel();

    //@Override
    CARAPI_(Boolean) IsEnabled();

    //@Override
    CARAPI_(Boolean) IsGroupHeader();

    CARAPI_(Int32) GetType();

private:
    const String mLabel;
    const Int32 mType;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_SELECTPOPUPITEM_H__
