
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_CQSDETAILITEMS_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_CQSDETAILITEMS_H__

#include "_Elastos_Droid_SystemUI_Qs_CQSDetailItems.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/**
 * Quick settings common detail view with line items.
 */
CarClass(CQSDetailItems)
    , public FrameLayout
    , public IQSDetailItems
{

public:
    class Item
        : public Object
        , public IQSDetailItemsItem
    {
    public:
        CAR_INTERFACE_DECL()

        Item();

    public:
        Int32 mIcon;
        String mLine1;
        String mLine2;
        AutoPtr<IInterface> mTag;
        Boolean mCanDisconnect;
    };

private:
    class H: public Handler
    {
    public:
        TO_STRING_IMPL("CQSDetailItems::H")

        H(
            /* [in] */ CQSDetailItems* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SET_ITEMS = 1;
        static const Int32 SET_CALLBACK = 2;
        static const Int32 SET_ITEMS_VISIBLE = 3;

    private:
        CQSDetailItems* mHost;
    };

    class OnClickListener1
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnClickListener1(
            /* [in] */ CQSDetailItems* host,
            /* [in] */ IQSDetailItemsItem* item);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CQSDetailItems* mHost;
        AutoPtr<IQSDetailItemsItem> mItem;
    };

    class OnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnClickListener2(
            /* [in] */ CQSDetailItems* host,
            /* [in] */ IQSDetailItemsItem* item);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CQSDetailItems* mHost;
        AutoPtr<IQSDetailItemsItem> mItem;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CQSDetailItems();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    static CARAPI_(AutoPtr<IQSDetailItems>) ConvertOrInflate(
        /* [in] */ IContext* context,
        /* [in] */ IView* convert,
        /* [in] */ IViewGroup* parent);

    CARAPI SetTagSuffix(
        /* [in] */ const String& suffix);

    CARAPI SetEmptyState(
        /* [in] */ Int32 icon,
        /* [in] */ Int32 text);

    CARAPI SetCallback(
        /* [in] */ IQSDetailItemsCallback* callback);

    CARAPI SetItems(
        /* [in] */ IArrayList* items);

    CARAPI SetItemsVisible(
        /* [in] */ Boolean visible);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) HandleSetCallback(
        /* [in] */ IQSDetailItemsCallback* callback);

    CARAPI_(void) HandleSetItems(
        /* [in] */ IArrayList* items);

    CARAPI_(void) HandleSetItemsVisible(
        /* [in] */ Boolean visible);

    CARAPI_(void) Bind(
        /* [in] */ IQSDetailItemsItem* item,
        /* [in] */ IView* view);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IContext> mContext;
    AutoPtr<H> mHandler;

    String mTag;
    AutoPtr<IQSDetailItemsCallback> mCallback;
    Boolean mItemsVisible;
    AutoPtr<ILinearLayout> mItems;
    AutoPtr<IView> mEmpty;
    AutoPtr<ITextView> mEmptyText;
    AutoPtr<IImageView> mEmptyIcon;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_CQSDETAILITEMS_H__
