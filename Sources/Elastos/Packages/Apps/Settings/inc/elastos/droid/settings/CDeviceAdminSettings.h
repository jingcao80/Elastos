#ifndef __ELASTOS_DROID_SETTINGS_CDEVICEADMINSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CDEVICEADMINSETTINGS_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Settings_CDeviceAdminSettings.h"
#include "elastos/droid/app/ListFragment.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::App::ListFragment;
using Elastos::Droid::App::Admin::IDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDeviceAdminSettings)
    , public ListFragment
{
public:
    class ViewHolder
        : public Object
    {
    public:
        TO_STRING_IMPL("CDeviceAdminSettings::ViewHolder")

        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mName;
        AutoPtr<ICheckBox> mCheckbox;
        AutoPtr<ITextView> mDescription;
    };

    class PolicyListAdapter
        : public BaseAdapter
    {
    public:
        TO_STRING_IMPL("CDeviceAdminSettings::PolicyListAdapter")

        PolicyListAdapter(
            /* [in] */ CDeviceAdminSettings* host);

        //@Override
        CARAPI HasStableIds(
            /* [out] */ Boolean* has);

        //@Override
        CARAPI GetCount(
            /* [out] */ Int32* result);

        /**
         * The item for the given position in the list.
         *
         * @return a String object for title items and a DeviceAdminInfo object for actual device
         *         admins.
         */
        //@Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        //@Override
        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* result);

        /**
         * See {@link #getItemViewType} for the view types.
         */
        //@Override
        CARAPI GetViewTypeCount(
            /* [out] */ Int32* result);

        /**
         * Returns 1 for title items and 0 for anything else.
         */
        //@Override
        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* result);

        //@Override
        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* _convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        CARAPI_(Boolean) IsEnabled(
            /* [in] */ IInterface* o);

        CARAPI_(AutoPtr<IView>) NewDeviceAdminView(
            /* [in] */ IViewGroup* parent);

        CARAPI_(AutoPtr<IView>) NewTitleView(
            /* [in] */ IViewGroup* parent);

        CARAPI_(void) BindView(
            /* [in] */ IView* view,
            /* [in] */ IDeviceAdminInfo* item);

    protected:
        AutoPtr<ILayoutInflater> mInflater;

    private:
        CDeviceAdminSettings* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CDeviceAdminSettings();

    virtual ~CDeviceAdminSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    /**
     * Update the internal collection of available admins for all profiles associated with the
     * current user.
     */
    CARAPI_(void) UpdateList();

private:
    CARAPI_(Boolean) IsDeviceOwner(
        /* [in] */ IDeviceAdminInfo* item);

    CARAPI_(Boolean) IsProfileOwner(
        /* [in] */ IDeviceAdminInfo* item);

    CARAPI_(Boolean) IsActiveAdmin(
        /* [in] */ IDeviceAdminInfo* item);

    /**
     * Add device admins to the internal collection that belong to a profile.
     *
     * @param profileId the profile identifier.
     */
    CARAPI_(void) UpdateAvailableAdminsForProfile(
        /* [in] */ Int32 profileId);

    /**
     * Add a profile's device admins that are receivers of
     * {@code DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED} to the internal collection if they
     * haven't been added yet.
     *
     * @param alreadyAddedComponents the set of active admin component names. Receivers of
     *            {@code DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED} whose component is in this
     *            set are not added to the internal collection again.
     * @param profileId the identifier of the profile
     */
    CARAPI_(void) AddDeviceAdminBroadcastReceiversForProfile(
        /* [in] */ ICollection* alreadyAddedComponents,
        /* [in] */ Int32 profileId);

    /**
     * Add a {@link DeviceAdminInfo} object to the internal collection of available admins for all
     * active admin components associated with a profile.
     *
     * @param profileId a profile identifier.
     */
    CARAPI_(void) AddActiveAdminsForProfile(
        /* [in] */ IList* activeAdmins,
        /* [in] */ Int32 profileId);

    /**
     * Creates a device admin info object for the resolved intent that points to the component of
     * the device admin.
     *
     * @param resolved resolved intent.
     * @return new {@link DeviceAdminInfo} object or NULL if there was an error.
     */
    CARAPI_(AutoPtr<IDeviceAdminInfo>) CreateDeviceAdminInfo(
        /* [in] */ IResolveInfo* resolved);

    /**
     * Extracts the user id from a device admin info object.
     * @param adminInfo the device administrator info.
     * @return identifier of the user associated with the device admin.
     */
    CARAPI_(Int32) GetUserId(
        /* [in] */ IDeviceAdminInfo* adminInfo);

protected:
    static const String TAG;// = "DeviceAdminSettings";

    static const Int32 DIALOG_WARNING;// = 1;

private:
    AutoPtr<IDevicePolicyManager> mDPM;
    AutoPtr<IUserManager> mUm;

    /**
     * Internal collection of device admin info objects for all profiles associated with the current
     * user.
     */
    // final SparseArray<ArrayList<DeviceAdminInfo>>
    //         mAdminsByProfile = new SparseArray<ArrayList<DeviceAdminInfo>>();
    AutoPtr<ISparseArray> mAdminsByProfile;

    String mDeviceOwnerPkg;
    // private SparseArray<ComponentName> mProfileOwnerComponents = new SparseArray<ComponentName>();
    AutoPtr<ISparseArray> mProfileOwnerComponents;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDEVICEADMINSETTINGS_H__