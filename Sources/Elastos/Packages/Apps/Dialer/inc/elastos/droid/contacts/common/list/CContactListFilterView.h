#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTERVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTERVIEW_H__

#include "_Elastos_Droid_Contacts_Common_List_CContactListFilterView.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Contacts::Common::Model::IAccountTypeManager;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * Contact list filter parameters.
 */
CarClass(CContactListFilterView)
    , public LinearLayout
    , public IContactListFilterView
{
public:
    CContactListFilterView()
        : mSingleAccount(FALSE)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetContactListFilter(
        /* [in] */ IContactListFilter* filter);

    CARAPI GetContactListFilter(
        /* [out] */ IContactListFilter** filter);

    CARAPI SetSingleAccount(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI SetActivated(
        /* [in] */ Boolean activated);

    CARAPI BindView(
        /* [in] */ IAccountTypeManager* accountTypes);

private:
    CARAPI_(void) BindView(
        /* [in] */ Int32 iconResource,
        /* [in] */ Int32 textResource);

private:
    static const String TAG;

    AutoPtr<IImageView> mIcon;
    AutoPtr<ITextView> mAccountType;
    AutoPtr<ITextView> mAccountUserName;
    AutoPtr<IRadioButton> mRadioButton;
    AutoPtr<IContactListFilter> mFilter;
    Boolean mSingleAccount;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTERVIEW_H__
