
#ifndef __ELASTOS_DROID_CONTENT_CRESTRICTIONS_MANAGER_H__
#define __ELASTOS_DROID_CONTENT_CRESTRICTIONS_MANAGER_H__

#include "_Elastos_Droid_Content_CRestrictionsManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Provides a mechanism for apps to query restrictions imposed by an entity that
 * manages the user. Apps can also send permission requests to a local or remote
 * device administrator to override default app-specific restrictions or any other
 * operation that needs explicit authorization from the administrator.
 * <p>
 * Apps can expose a set of restrictions via an XML file specified in the manifest.
 * <p>
 * If the user has an active Restrictions Provider, dynamic requests can be made in
 * addition to the statically imposed restrictions. Dynamic requests are app-specific
 * and can be expressed via a predefined set of request types.
 * <p>
 * The RestrictionsManager forwards the dynamic requests to the active
 * Restrictions Provider. The Restrictions Provider can respond back to requests by calling
 * {@link #notifyPermissionResponse(String, PersistableBundle)}, when
 * a response is received from the administrator of the device or user.
 * The response is relayed back to the application via a protected broadcast,
 * {@link #ACTION_PERMISSION_RESPONSE_RECEIVED}.
 * <p>
 * Static restrictions are specified by an XML file referenced by a meta-data attribute
 * in the manifest. This enables applications as well as any web administration consoles
 * to be able to read the list of available restrictions from the apk.
 * <p>
 * The syntax of the XML format is as follows:
 * <pre>
 * &lt;?xml version="1.0" encoding="utf-8"?&gt;
 * &lt;restrictions xmlns:android="http://schemas.android.com/apk/res/android" &gt;
 *     &lt;restriction
 *         android:key="string"
 *         android:title="string resource"
 *         android:restrictionType=["bool" | "string" | "integer"
 *                                         | "choice" | "multi-select" | "hidden"]
 *         android:description="string resource"
 *         android:entries="string-array resource"
 *         android:entryValues="string-array resource"
 *         android:defaultValue="reference"
 *         /&gt;
 *     &lt;restriction ... /&gt;
 *     ...
 * &lt;/restrictions&gt;
 * </pre>
 * <p>
 * The attributes for each restriction depend on the restriction type.
 * <p>
 * <ul>
 * <li><code>key</code>, <code>title</code> and <code>restrictionType</code> are mandatory.</li>
 * <li><code>entries</code> and <code>entryValues</code> are required if <code>restrictionType
 * </code> is <code>choice</code> or <code>multi-select</code>.</li>
 * <li><code>defaultValue</code> is optional and its type depends on the
 * <code>restrictionType</code></li>
 * <li><code>hidden</code> type must have a <code>defaultValue</code> and will
 * not be shown to the administrator. It can be used to pass along data that cannot be modified,
 * such as a version code.</li>
 * <li><code>description</code> is meant to describe the restriction in more detail to the
 * administrator controlling the values, if the title is not sufficient.</li>
 * </ul>
 * <p>
 * In your manifest's <code>application</code> section, add the meta-data tag to point to
 * the restrictions XML file as shown below:
 * <pre>
 * &lt;application ... &gt;
 *     &lt;meta-data android:name="android.content.APP_RESTRICTIONS"
 *                   android:resource="@xml/app_restrictions" /&gt;
 *     ...
 * &lt;/application&gt;
 * </pre>
 *
 * @see RestrictionEntry
 * @see RestrictionsReceiver
 * @see DevicePolicyManager#setRestrictionsProvider(ComponentName, ComponentName)
 * @see DevicePolicyManager#setApplicationRestrictions(ComponentName, String, Bundle)
 */
CarClass(CRestrictionsManager)
    , public Object
    , public IRestrictionsManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRestrictionsManager();

    virtual ~CRestrictionsManager();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIRestrictionsManager* service);

    /**
     * Returns any available set of application-specific restrictions applicable
     * to this application.
     * @return the application restrictions as a Bundle. Returns null if there
     * are no restrictions.
     */
    CARAPI GetApplicationRestrictions(
        /* [out] */ IBundle** bundle);

    /**
     * Called by an application to check if there is an active Restrictions Provider. If
     * there isn't, {@link #requestPermission(String, String, PersistableBundle)} is not available.
     *
     * @return whether there is an active Restrictions Provider.
     */
    CARAPI HasRestrictionsProvider(
        /* [out] */ Boolean* provider);

    /**
     * Called by an application to request permission for an operation. The contents of the
     * request are passed in a Bundle that contains several pieces of data depending on the
     * chosen request type.
     *
     * @param requestType The type of request. The type could be one of the
     * predefined types specified here or a custom type that the specific
     * Restrictions Provider might understand. For custom types, the type name should be
     * namespaced to avoid collisions with predefined types and types specified by
     * other Restrictions Providers.
     * @param requestId A unique id generated by the app that contains sufficient information
     * to identify the parameters of the request when it receives the id in the response.
     * @param request A PersistableBundle containing the data corresponding to the specified request
     * type. The keys for the data in the bundle depend on the request type.
     *
     * @throws IllegalArgumentException if any of the required parameters are missing.
     */
    CARAPI RequestPermission(
        /* [in] */ const String& requestType,
        /* [in] */ const String& requestId,
        /* [in] */ IPersistableBundle* request);

    CARAPI CreateLocalApprovalIntent(
        /* [out] */ IIntent** intent);

    /**
     * Called by the Restrictions Provider to deliver a response to an application.
     *
     * @param packageName the application to deliver the response to. Cannot be null.
     * @param response the bundle containing the response status, request ID and other information.
     *                 Cannot be null.
     *
     * @throws IllegalArgumentException if any of the required parameters are missing.
     */
    CARAPI NotifyPermissionResponse(
        /* [in] */ const String& packageName,
        /* [in] */ IPersistableBundle* response);

    /**
     * Parse and return the list of restrictions defined in the manifest for the specified
     * package, if any.
     *
     * @param packageName The application for which to fetch the restrictions list.
     * @return The list of RestrictionEntry objects created from the XML file specified
     * in the manifest, or null if none was specified.
     */
    CARAPI GetManifestRestrictions(
        /* [in] */ const String& packageName,
        /* [out] */ IList** restrictions);

private:
    CARAPI LoadManifestRestrictions(
        /* [in] */ const String& packageName,
        /* [in] */ IXmlResourceParser* xml,
        /* [out] */ IList** restrictions);

    CARAPI LoadRestriction(
        /* [in] */ IContext* appContext,
        /* [in] */ ITypedArray* ta,
        /* [out] */ IRestrictionEntry** restrictions);

private:
    static const String TAG;// = "RestrictionsManager";

    static const String TAG_RESTRICTION;// = "restriction";

    AutoPtr<IContext> mContext;
    AutoPtr<IIRestrictionsManager> mService;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CRESTRICTIONS_MANAGER_H__
