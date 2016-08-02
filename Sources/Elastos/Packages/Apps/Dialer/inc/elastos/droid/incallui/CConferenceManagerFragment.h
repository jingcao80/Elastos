
#ifndef __ELASTOS_DROID_INCALLUI_CCONFERENCEMANAGERFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_CCONFERENCEMANAGERFRAGMENT_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_InCallUI_CConferenceManagerFragment.h"
#include "elastos/droid/incallui/BaseFragment.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CConferenceManagerFragment)
    , public BaseFragment
    , public IConferenceManagerFragment
    , public IConferenceManagerUi
    , public IUi
{
private:
    class EndButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        EndButtonClickListener(
            /* [in] */ Int32 rowId,
            /* [in] */ CConferenceManagerFragment* host)
            : mRowId(rowId)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        Int32 mRowId;
        CConferenceManagerFragment* mHost;
    };

    class SeparateButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        SeparateButtonClickListener(
            /* [in] */ Int32 rowId,
            /* [in] */ CConferenceManagerFragment* host)
            : mRowId(rowId)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        Int32 mRowId;
        CConferenceManagerFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CConferenceManagerFragment();

    CARAPI constructor();

    // @Override
    CARAPI_(AutoPtr<IPresenter>) CreatePresenter();

    // @Override
    CARAPI_(AutoPtr<IUi>) GetUi();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI SetVisible(
        /* [in] */ Boolean on);

    // @Override
    CARAPI IsFragmentVisible(
        /* [out] */ Boolean* isVisible);

    // @Override
    CARAPI SetRowVisible(
        /* [in] */ Int32 rowId,
        /* [in] */ Boolean on);

    // @Override
    CARAPI DisplayCallerInfoForConferenceRow(
        /* [in] */ Int32 rowId,
        /* [in] */ const String& callerName,
        /* [in] */ const String& callerNumber,
        /* [in] */ const String& callerNumberType,
        /* [in] */ const String& lookupKey,
        /* [in] */ IUri* photoUri);

    // @Override
    CARAPI SetupEndButtonForRow(
        /* [in] */ Int32 rowId,
        /* [in] */ Boolean canDisconnect);

    // @Override
    CARAPI SetupSeparateButtonForRow(
        /* [in] */ Int32 rowId,
        /* [in] */ Boolean canSeparate);

private:
    AutoPtr< ArrayOf<IViewGroup*> > mConferenceCallList;
    Int32 mActionBarElevation;
    // private ContactPhotoManager mContactPhotoManager;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CCONFERENCEMANAGERFRAGMENT_H__
