#ifndef __ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTTYPEACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTTYPEACTIVITY_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
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

namespace Elastos {
namespace Droid {
namespace Accounts {

class ChooseAccountTypeActivity : public Activity
{
private:
    class AuthInfo
        : public ElRefBase
        , public IInterface
    {
    public:
        AuthInfo(
            /* [in] */ IAuthenticatorDescription* desc,
            /* [in] */ const String& name,
            /* [in] */ IDrawable* drawable);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    public:
        AutoPtr<IAuthenticatorDescription> mDesc;
        String mName;
        AutoPtr<IDrawable> mDrawable;
    };

    class ViewHolder
        : public ElRefBase
        , public IInterface
    {
    public:
        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    public:
        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mText;
    };

    class AccountArrayAdapter
        : public ElRefBase
        , public IArrayAdapter
        , public ISpinnerAdapter
        , public ArrayAdapter
    {
    public:
        AccountArrayAdapter(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ List<AutoPtr<AuthInfo> >* infos);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI Add(
            /* [in] */ IInterface* object);

        CARAPI AddAll(
            /* [in] */ IObjectContainer* collection);

        CARAPI AddAll(
            /* [in] */ ArrayOf<IInterface* >* items) ;

        CARAPI Insert(
            /* [in] */ IInterface* object,
            /* [in] */ Int32 index);

        CARAPI Remove(
            /* [in] */ IInterface* object);

        CARAPI Clear();

        CARAPI Sort(
            /* [in] */ IComparator* comparator);

        CARAPI SetNotifyOnChange(
            /* [in] */ Boolean notifyOnChange);

        CARAPI GetContext(
            /* [out] */ IContext** context);

        CARAPI GetPosition(
            /* [in] */ IInterface* item,
            /* [out] */ Int32* position);

        CARAPI SetDropDownViewResource(
            /* [in] */ Int32 resource);

        CARAPI NotifyDataSetChanged();

        CARAPI NotifyDataSetInvalidated();

        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        CARAPI GetDropDownView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI RegisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI UnregisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI HasStableIds(
            /* [out] */ Boolean* hasStableIds);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* viewType);

        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        CARAPI IsEmpty(
             /* [out] */ Boolean* isEmpty);

    public:
        AutoPtr<ILayoutInflater> mLayoutInflater;
        AutoPtr< List<AutoPtr<AuthInfo> > > mInfos; // need not delete
    };

    class AdapterViewOnItemClickListener
        : public ElRefBase
        , public IAdapterViewOnItemClickListener
    {
    public:
        AdapterViewOnItemClickListener(
            /* [in] */ ChooseAccountTypeActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        AutoPtr<ChooseAccountTypeActivity> mHost;
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

    HashMap<String, AutoPtr<AuthInfo> > mTypeToAuthenticatorInfo;
    AutoPtr< List<AutoPtr<AuthInfo> > > mAuthenticatorInfosToDisplay;

    friend class AdapterViewOnItemClickListener;
};

} // Accounts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTTYPEACTIVITY_H__
