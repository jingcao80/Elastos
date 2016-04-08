
#ifndef __DIALOGDETAILSVIEW_H__
#define __DIALOGDETAILSVIEW_H__

#include "elastos/droid/app/Activity.h"
#include "DetailsHelper.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IArrayAdapter;

namespace Gallery {

class DialogDetailsView
    : public IDetailsViewContainer
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
        , public IDialogInterfaceOnDismissListener
    {
    public:
        MyListener(
            /* [in] */ DialogDetailsView* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        DialogDetailsView* mHost;
    };

friend class MyListener;

public:
    DialogDetailsView(
        /* [in] */ IActivity* activity,
        // /* [in] */ IDetailsSource* source);
        /* [in] */ ArrayOf<String>* source);

    CARAPI Show();

    CARAPI Hide();

    CARAPI ReloadDetails();

    CARAPI SetCloseListener(
        /* [in] */ ICloseListener* listener);

private:
    CARAPI_(void) SetDetails(
        /* [in] */ MediaDetails* details);

    CARAPI_(AutoPtr<IArrayAdapter>) GetArrayAdapter();

private:
    static const String TAG;

private:
    Int32 mIndex;
    AutoPtr<IActivity> mActivity;
    // private DetailsAdapter mAdapter;
    AutoPtr<IArrayAdapter> mAdapter;
    AutoPtr<MediaDetails> mDetails;
    // AutoPtr<IDetailsSource> mSource;
    AutoPtr< ArrayOf<String> > mSource;
    AutoPtr<IDialog> mDialog;
    AutoPtr<ICloseListener> mListener;
    AutoPtr<MyListener> mMyListener;
};

} // namespace Gallery

#endif // __DIALOGDETAILSVIEW_H__
