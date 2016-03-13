
#ifndef __ELASTOS_DROID_SETTINGS_USERSPINNERADAPTER_H__
#define __ELASTOS_DROID_SETTINGS_USERSPINNERADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"

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

/**
 * Adapter for a spinner that shows a list of users.
 */
class UserSpinnerAdapter
    : public Object
    , public ISpinnerAdapter
    , public IAdapter
{
public:
    // TODO: Update UI. See: http://b/16518801
    /** Holder for user details */
    class UserDetails
        : public Object
    {
    public:
        UserDetails(
            /* [in] */ IUserHandle* userHandle,
            /* [in] */ IUserManager* um,
            /* [in] */ IContext* context);

        ~UserDetails();

    private:
        AutoPtr<IUserHandle> mUserHandle;
        String mName;
        AutoPtr<IDrawable> mIcon;
    };

public:
    CAR_INTERFACE_DECL();

    UserSpinnerAdapter();

    ~UserSpinnerAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IArrayList* users);  /*ArrayList<UserDetails>*/

    CARAPI_(AutoPtr<IUserHandle>) GetUserHandle(
        /* [in] */ Int32 position);

    //@Override
    CARAPI GetDropDownView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ ViewGroup* parent,
        /* [out] */ IView** view);

    //@Override
    CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    //@Override
    CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    //@Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    //@Override
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    //@Override
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* itemId);

    //@Override
    CARAPI HasStableIds(
        /* [out] */ Boolean* hasStableIds);

    //@Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    //@Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* viewType);

    //@Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    //@Override
    CARAPI IsEmpty(
        /* [out] */ Boolean* isEmpty);

private:
    CARAPI_(AutoPtr<IView>) CreateUser(
        /* [in] */ IViewGroup* parent);

private:
    // ArrayList<UserDetails> data;
    AutoPtr<IArrayList> mData;
    AutoPtr<ILayoutInflater> mInflater;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_USERSPINNERADAPTER_H__
