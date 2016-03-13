#ifndef __ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTTYPEACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTTYPEACTIVITY_H__

#include "_Elastos.Droid.Accounts.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

class ChooseAccountTypeActivity
    : public Activity
{
private:
    class AuthInfo
        : public Object
    {
    public:
        AuthInfo(
            /* [in] */ IAuthenticatorDescription* desc,
            /* [in] */ const String& name,
            /* [in] */ IDrawable* drawable);

    public:
        AutoPtr<IAuthenticatorDescription> mDesc;
        String mName;
        AutoPtr<IDrawable> mDrawable;
    };

    class ViewHolder
        : public Object
    {
    public:
        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mText;
    };

    class AccountArrayAdapter
        : public ArrayAdapter
    {
    public:
        AccountArrayAdapter(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ IArrayList* infos);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    public:
        AutoPtr<ILayoutInflater> mLayoutInflater;
        AutoPtr<IArrayList> mInfos; // need not delete
    };

    class AdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        AdapterViewOnItemClickListener(
            /* [in] */ ChooseAccountTypeActivity* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        ChooseAccountTypeActivity* mHost;
    };

public:
    ChooseAccountTypeActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) SetResultAndFinish(
        /* [in] */ const String& type);

    CARAPI_(void) BuildTypeToAuthDescriptionMap();

private:
    static const String TAG;

    AutoPtr<IHashMap> mTypeToAuthenticatorInfo;
    AutoPtr<IArrayList> mAuthenticatorInfosToDisplay;

    friend class AdapterViewOnItemClickListener;
};

} // Accounts
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Accounts::ChooseAccountTypeActivity::AuthInfo, IInterface)

#endif //__ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTTYPEACTIVITY_H__
