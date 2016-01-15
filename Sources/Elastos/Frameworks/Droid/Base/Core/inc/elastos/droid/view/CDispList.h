#ifndef __ELASTOS_DROID_VIEW_CDISPLIST_H__
#define __ELASTOS_DROID_VIEW_CDISPLIST_H__

#include "_Elastos_Droid_View_CDispList.h"
#include "elastos/droid/os/Handler.h"

#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/view/CDispListDispFormat.h"
#include "elastos/droid/view/CDispListHelper.h"

using Elastos::Droid::Widget::Toast;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Os::Handler;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDispList) , public Handler
{
protected:

    class MyToast
        : public IToast
        , public Toast
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyToast(
            /* [in] */ IContext* ctx,
            /* [in] */ CDispList* host);

        CARAPI OnHide();

        CARAPI Show();

        CARAPI Cancel();

        CARAPI SetView(
            /* [in] */ IView* view);

        CARAPI GetView(
            /* [out] */ IView** view);

        CARAPI SetDuration(
            /* [in] */ Int32 duration);

        CARAPI GetDuration(
            /* [out] */ Int32* duration);

        CARAPI SetMargin(
            /* [in] */ Float horizontalMargin,
            /* [in] */ Float verticalMargin);

        CARAPI GetHorizontalMargin(
            /* [out] */ Float* horizontalMargin);

        CARAPI GetVerticalMargin(
            /* [out] */ Float* verticalMargin);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity,
            /* [in] */ Int32 xOffset,
            /* [in] */ Int32 yOffset);

        CARAPI GetGravity(
            /* [out] */ Int32* gravity);

        CARAPI GetXOffset(
            /* [out] */ Int32* xOffset);

        CARAPI GetYOffset(
            /* [out] */ Int32* yOffset);

        CARAPI SetText(
            /* [in] */ Int32 resId);

        CARAPI SetText(
            /* [in] */ ICharSequence* s);

        CARAPI IsShowing(
            /* [out] */ Boolean* showing);
    private:
        CDispList* mHost;
    };

public:
    IHANDLER_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CDispList();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI ShowItem(
            /* [in] */ IDispListDispFormat* item);

    CARAPI ShowNext();

    CARAPI IsShowing(
            /* [out] */ Boolean* showing);

    CARAPI OnHide(
            /* [in] */ IDispListDispFormat* item);

protected:

    CARAPI OnShowItem(
            /* [in] */ IDispListDispFormat* item);

    CARAPI OnShowNext();
private:

    CARAPI_(void) InitToast();
    CARAPI_(AutoPtr<CDispListHelper>) GetHelper();

private:

    static const Int32 MSG_SHOW_ITEM;//  = 0;
    static const Int32 MSG_SHOW_NEXT;//  = 1;
    static const Int32 DURATION;
private:

    AutoPtr<MyToast> mToast;
    Int32 mCurItem;
    AutoPtr<IContext> mContext;
    Boolean mToastIsInited;
};

}// namespace View
}// namespace Droid
}// namespace View
#endif
