
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_DIALOG_BUILDER_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_DIALOG_BUILDER_H__

#include "_Elastos.DevSamples.DFChatDemo.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Core::Object;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

class CChatApplication;

class DialogBuilder
{
public:

    class HostNameOnEditorActionListener
        : public Object
        , public IOnEditorActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        HostNameOnEditorActionListener(
            /* [in] */ CChatApplication* application,
            /* [in] */ IDialog* dialog);

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        CChatApplication* mApplication;
        IDialog* mDialog;
    };

    class HostNameOkOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        HostNameOkOnClickListener(
            /* [in] */ CChatApplication* application,
            /* [in] */ IDialog* dialog,
            /* [in] */ ITextView* textView);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CChatApplication* mApplication;
        IDialog* mDialog;
        AutoPtr<ITextView> mTextView;
    };

    class HostStopOkOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        HostStopOkOnClickListener(
            /* [in] */ CChatApplication* application,
            /* [in] */ IDialog* dialog);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CChatApplication* mApplication;
        IDialog* mDialog;
    };

    class HostStartOkOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        HostStartOkOnClickListener(
            /* [in] */ CChatApplication* application,
            /* [in] */ IDialog* dialog);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CChatApplication* mApplication;
        IDialog* mDialog;
    };

    class DialogCancelOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DialogCancelOnClickListener(
            /* [in] */ IDialog* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        IDialog* mDialog;
    };

    class UseLeaveOKOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        UseLeaveOKOnClickListener(
            /* [in] */ CChatApplication* application,
            /* [in] */ IDialog* dialog);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CChatApplication* mApplication;
        IDialog* mDialog;
    };

    class UseJoinCancelOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        UseJoinCancelOnClickListener(
            /* [in] */ IActivity* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        IActivity* mActivity;
    };

    class UseJoinCancelOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        UseJoinCancelOnItemClickListener(
            /* [in] */ IActivity* activity,
            /* [in] */ CChatApplication* application,
            /* [in] */ IAdapterView* listView);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
             /* [in] */ Int64 id);

    private:
        IActivity* mActivity;
        CChatApplication* mApplication;
        AutoPtr<IAdapterView> mChannelList;
    };

public:
    static AutoPtr<IDialog> CreateUseJoinDialog(
        /* [in] */ IActivity* activity,
        /* [in] */ CChatApplication* application);

    static AutoPtr<IDialog> CreateUseLeaveDialog(
        /* [in] */ IActivity* activity,
        /* [in] */ CChatApplication* application);

    static AutoPtr<IDialog> CreateHostNameDialog(
        /* [in] */ IActivity* activity,
        /* [in] */ CChatApplication* application);

    static AutoPtr<IDialog> CreateHostStartDialog(
        /* [in] */ IActivity* activity,
        /* [in] */ CChatApplication* application);

    static AutoPtr<IDialog> CreateHostStopDialog(
        /* [in] */ IActivity* activity,
        /* [in] */ CChatApplication* application);

    static AutoPtr<IDialog> CreateAllJoynErrorDialog(
        /* [in] */ IActivity* activity,
        /* [in] */ CChatApplication* application);

private:
    DialogBuilder();
    DialogBuilder(const DialogBuilder&);

};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_DIALOG_BUILDER_H__
