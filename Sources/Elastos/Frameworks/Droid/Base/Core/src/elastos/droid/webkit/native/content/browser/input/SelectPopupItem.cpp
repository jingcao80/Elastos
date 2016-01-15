
#include "elastos/droid/webkit/native/content/browser/input/SelectPopupItem.h"
#include "elastos/droid/webkit/native/content/browser/input/PopupItemType.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

SelectPopupItem::SelectPopupItem(
    /* [in] */ const String& label,
    /* [in] */ Int32 type)
    : mLabel(label)
    , mType(type)
{
}

//@Override
String SelectPopupItem::GetLabel()
{
    return mLabel;
}

//@Override
String SelectPopupItem::GetSublabel()
{
    return String(NULL);
}

//@Override
Boolean SelectPopupItem::IsEnabled()
{
    return mType == PopupItemType::ENABLED || mType == PopupItemType::GROUP;
}

//@Override
Boolean SelectPopupItem::IsGroupHeader()
{
    return mType == PopupItemType::GROUP;
}

Int32 SelectPopupItem::GetType()
{
    return mType;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
