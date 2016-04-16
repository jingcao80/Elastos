#ifndef  __ELASTOS_DROID_LAUNCHER2_ADDADAPTER_H__
#define  __ELASTOS_DROID_LAUNCHER2_ADDADAPTER_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Adapter showing the types of items that can be added to a {@link Workspace}.
 */
class AddAdapter
    : public BaseAdapter
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