
#ifndef __ELASTOS_DROID_APP_ADMIN_CDEVICEADMININFO_H__
#define __ELASTOS_DROID_APP_ADMIN_CDEVICEADMININFO_H__

#include "_Elastos_Droid_App_Admin_CDeviceAdminInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/SparseArray.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Droid::App::Admin::IDeviceAdminInfoPolicyInfo;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

CarClass(CDeviceAdminInfo)
    , public Object
    , public IDeviceAdminInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDeviceAdminInfo();

    CARAPI constructor(
        /* [in] */ IParcel* source);

    /**
     * Constructor.
     *
     * @param context The Context in which we are parsing the device admin.
     * @param receiver The ResolveInfo returned from the package manager about
     * this device admin's component.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* receiver);

    /**
     * Return the .apk package that implements this device admin.
     */
    CARAPI GetPackageName(
        /* [out] */ String* name);

    /**
     * Return the class name of the receiver component that implements
     * this device admin.
     */
    CARAPI GetReceiverName(
        /* [out] */ String* name);

    /**
     * Return the raw information about the receiver implementing this
     * device admin.  Do not modify the returned object.
     */
    CARAPI GetActivityInfo(
        /* [out] */ IActivityInfo** info);

    /**
     * Return the component of the receiver that implements this device admin.
     */
    CARAPI GetComponent(
        /* [out] */ IComponentName** name);

    /**
     * Load the user-displayed label for this device admin.
     *
     * @param pm Supply a PackageManager used to load the device admin's
     * resources.
     */
    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    /**
     * Load user-visible description associated with this device admin.
     *
     * @param pm Supply a PackageManager used to load the device admin's
     * resources.
     */
    CARAPI LoadDescription(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** description);

    /**
     * Load the user-displayed icon for this device admin.
     *
     * @param pm Supply a PackageManager used to load the device admin's
     * resources.
     */
    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * Returns whether this device admin would like to be visible to the
     * user, even when it is not enabled.
     */
    CARAPI IsVisible(
        /* [out] */ Boolean* visible);

    /**
     * Return true if the device admin has requested that it be able to use
     * the given policy control.  The possible policy identifier inputs are:
     * {@link #USES_POLICY_LIMIT_PASSWORD}, {@link #USES_POLICY_WATCH_LOGIN},
     * {@link #USES_POLICY_RESET_PASSWORD}, {@link #USES_POLICY_FORCE_LOCK},
     * {@link #USES_POLICY_WIPE_DATA},
     * {@link #USES_POLICY_EXPIRE_PASSWORD}, {@link #USES_ENCRYPTED_STORAGE},
     * {@link #USES_POLICY_DISABLE_CAMERA}.
     */
    CARAPI UsesPolicy(
        /* [in] */ Int32 policyIdent,
        /* [out] */ Boolean* result);

    /**
     * Return the XML tag name for the given policy identifier.  Valid identifiers
     * are as per {@link #usesPolicy(int)}.  If the given identifier is not
     * known, null is returned.
     */
    CARAPI GetTagForPolicy(
        /* [in] */ Int32 policyIdent,
        /* [out] */ String* tag);

    /** @hide */
    CARAPI GetUsedPolicies(
        /* [out] */ IArrayList** policies);

    /** @hide */
    CARAPI WritePoliciesToXml(
        /* [in] */ IXmlSerializer* out);

    /** @hide */
    CARAPI ReadPoliciesFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* string);

    static Boolean Init();


private:
    static List<AutoPtr<IDeviceAdminInfoPolicyInfo> > sPoliciesDisplayOrder;
    static HashMap<String, AutoPtr<IInteger32> > sKnownPolicies;
    static AutoPtr<SparseArray> sRevKnownPolicies; // SparseArray<PolicyInfo> sRevKnownPolicies = new SparseArray<PolicyInfo>();

    static Boolean sIsInited;
    /**
     * The BroadcastReceiver that implements this device admin component.
     */
    AutoPtr<IResolveInfo> mReceiver;

    /**
     * Whether this should be visible to the user.
     */
    Boolean mVisible;

    /**
     * The policies this administrator needs access to.
     */
    Int32 mUsesPolicies;

};

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ADMIN_CDEVICEADMININFO_H__
