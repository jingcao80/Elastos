
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCESCREEN_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCESCREEN_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/preference/PreferenceGroup.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceScreen
    : public PreferenceGroup
    , public IPreferenceScreen
{
private:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("PreferenceScreen::InnerListener")

        InnerListener(
            /* [in] */ PreferenceScreen* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);
    private:
        PreferenceScreen* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PreferenceScreen();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetRootAdapter(
        /* [out] */ IListAdapter** adapter);

    CARAPI OnCreateRootAdapter(
        /* [out] */ IListAdapter** adapter);

    CARAPI Bind(
        /* [in] */ IListView* listView);

    //@Override
    CARAPI OnClick();

    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    CARAPI GetDialog(
        /* [out] */ IDialog** dialog);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    //@Override
    CARAPI IsOnSameScreenAsChildren(
        /* [out] */ Boolean* isOnSameScreenAsChildren);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    CARAPI ShowDialog(
        /* [in] */ IBundle* state);

private:
    AutoPtr<IListAdapter> mRootAdapter;
    AutoPtr<IDialog> mDialog;
    AutoPtr<IListView> mListView;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCESCREEN_H__
