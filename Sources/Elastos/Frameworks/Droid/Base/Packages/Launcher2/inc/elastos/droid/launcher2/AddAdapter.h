#ifndef  __ELASTOS_DROID_LAUNCHER2_ADDADAPTER_H__
#define  __ELASTOS_DROID_LAUNCHER2_ADDADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Adapter showing the types of items that can be added to a {@link Workspace}.
 */
class AddAdapter
    : public BaseAdapter
    , public IAddAdapter
{
public:
    /**
     * Specific item in our list.
     */
    class ListItem
        : public Object
    {
    public:
        ListItem(
            /* [in] */ IResources* res,
            /* [in] */ Int32 textResourceId,
            /* [in] */ Int32 imageResourceId,
            /* [in] */ Int32 actionTag);
    public:
        AutoPtr<ICharSequence> mText;
        AutoPtr<IDrawable> mImage;
        Int32 mActionTag;
    };

public:
    CAR_INTERFACE_DECL();

    AddAdapter();

    CARAPI constructor(
        /* [in] */ ILauncher* launcher);

    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

private:
    AutoPtr<ILayoutInflater> mInflater;

    AutoPtr<IArrayList> mItems;

    static const Int32 ITEM_SHORTCUT;
    static const Int32 ITEM_APPWIDGET;
    static const Int32 ITEM_APPLICATION;
    static const Int32 ITEM_WALLPAPER;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_ADDADAPTER_H__