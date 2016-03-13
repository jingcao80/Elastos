
#include "elastos/droid/settings/UserSpinnerAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Utility::IUserIcons;

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  UserSpinnerAdapter::UserDetails
//===============================================================================

UserSpinnerAdapter::UserDetails::UserDetails(
    /* [in] */ IUserHandle* userHandle,
    /* [in] */ IUserManager* um,
    /* [in] */ IContext* context)
{
    mUserHandle = userHandle;
    Int32 id;
    mUserHandle->GetIdentifier(&id);
    AutoPtr<IUserInfo> userInfo;
    um->GetUserInfo(id, (IUserInfo**)&userInfo);
    Boolean result;
    if (userInfo->IsManagedProfile(&result), result) {
        context->GetString(R::string::managed_user_title, &mName);
        AutoPtr<IResourcesHelper> helper;
        CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
        AutoPtr<IResources> resources;
        helper->GetSystem((IResources**)&resources);
        resources->GetDrawable(
                R::drawable::ic_corp_icon, (IDrawable**)&mIcon);
    }
    else {
        userInfo->GetName(&mName);
        Int32 userId;
        userInfo->GetId(&userId);
        AutoPtr<IBitmap> bit;
        um->GetUserIcon(userId, (IBitmap**)&bit);
        if (bit != NULL) {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            CBitmapDrawable::New(res, bit, (IDrawable**)&mIcon);
        }
        else {
            AutoPtr<IUserIcons> user;
            CUserIcons::AcquireSingleton((IUserIcons**)&user);
            user->GetDefaultUserIcon(userId, /* light= */ FALSE, (IDrawable**)&mIcon);
        }
    }
}

UserSpinnerAdapter::UserDetails::~UserDetails()
{}

//===============================================================================
//                  UserSpinnerAdapter
//===============================================================================

CAR_INTERFACE_IMPL_2(UserSpinnerAdapter, Object, ISpinnerAdapter, IAdapter);

UserSpinnerAdapter::UserSpinnerAdapter()
{}

UserSpinnerAdapter::~UserSpinnerAdapter()
{}

ECode UserSpinnerAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IArrayList* users)  /*ArrayList<UserDetails>*/
{
    if (users == NULL) {
        Slogger::E("UserSpinnerAdapter", "A list of user details must be provided");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mData = users;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    return NOERROR;
}

AutoPtr<IUserHandle> UserSpinnerAdapter::GetUserHandle(
    /* [in] */ Int32 position)
{
    Int32 size;
    mData->GetSize(&size);
    if (position < 0 || position >= size) {
        return NULL;
    }
    AutoPtr<IInterface> obj;
    mData->Get(position, (IInterface**)&obj);
    AutoPtr<UserDetails> details = (UserDetails*)IObject::Probe(obj);
    return details->mUserHandle;
}

ECode UserSpinnerAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ ViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> row = convertView != NULL ? convertView : CreateUser(parent);

    AutoPtr<IInterface> obj;
    mData->Get(position, (IInterface**)&obj);
    AutoPtr<UserDetails> user = (UserDetails*)IObject::Probe(obj);

    AutoPtr<IView> other;
    row->FindViewById(R::id::icon, (IView**)&other);
    IImageView::Probe(other)->SetImageDrawable(user->mIcon);
    other = NULL;
    row->FindViewById(R::id::title, (IView**)&other);
    ITextView::Probe(other)->SetText(user->mName);
    *view = row;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IView> UserSpinnerAdapter::CreateUser(
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    mInflater->Inflate(R::layout::user_preference, parent, FALSE, (IView**)&view);
    return view;
}

ECode UserSpinnerAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    // We don't support observers
    return NOERROR;
}

ECode UserSpinnerAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    // We don't support observers
    return NOERROR;
}

ECode UserSpinnerAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return mData->GetSize(count);
}

ECode UserSpinnerAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    return mData->Get(position, item);
}

ECode UserSpinnerAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    AutoPtr<IInterface> obj;
    mData->Get(position, (IInterface**)&obj);
    AutoPtr<UserDetails> details = (UserDetails*)IObject::Probe(obj);
    Int32 id;
    details->mUserHandle->GetIdentifier(&id);
    *itemId = (Int64)id;
    return NOERROR;
}

ECode UserSpinnerAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = FALSE;
    return NOERROR;
}

ECode UserSpinnerAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return GetDropDownView(position, convertView, parent, view);
}

ECode UserSpinnerAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = 0;
    return NOERROR;
}

ECode UserSpinnerAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 1;
    return NOERROR;
}

ECode UserSpinnerAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    return mData->IsEmpty(isEmpty);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
