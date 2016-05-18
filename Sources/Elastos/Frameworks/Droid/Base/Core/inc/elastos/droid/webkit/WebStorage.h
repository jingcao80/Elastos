
#ifndef __ELASTOS_DROID_WEBKIT_WEBSTORAGE_H__
#define __ELASTOS_DROID_WEBKIT_WEBSTORAGE_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/WebViewFactory.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

class WebStorageClassic;

/**
 * This class is used to manage the JavaScript storage APIs provided by the
 * {@link WebView}. It manages the Application Cache API, the Web SQL Database
 * API and the HTML5 Web Storage API.
 *
 * The Application Cache API provides a mechanism to create and maintain an
 * application cache to power offline Web applications. Use of the Application
 * Cache API can be attributed to an origin {@link WebStorage.Origin}, however
 * it is not possible to set per-origin quotas. Note that there can be only
 * one application cache per application.
 *
 * The Web SQL Database API provides storage which is private to a given origin.
 * Similar to the Application Cache, use of the Web SQL Database can be attributed
 * to an origin. It is also possible to set per-origin quotas.
 */
class WebStorage
    : public Object
    , public IWebStorage
{
public:
    /**
     * This class encapsulates information about the amount of storage
     * currently used by an origin for the JavaScript storage APIs.
     * An origin comprises the host, scheme and port of a URI.
     * See {@link WebStorage} for details.
     */
    class Origin
        : public Object
        , public IWebStorageOrigin
    {
        friend class WebStorageClassic;
    public:
        CAR_INTERFACE_DECL()

        /**
         * Gets the string representation of this origin.
         *
         * @return the string representation of this origin
         */
        // An origin string is created using WebCore::SecurityOrigin::toString().
        // Note that WebCore::SecurityOrigin uses 0 (which is not printed) for
        // the port if the port is the default for the protocol. Eg
        // http://www.google.com and http://www.google.com:80 both record a port
        // of 0 and hence toString() == 'http://www.google.com' for both.
        CARAPI GetOrigin(
            /* [out] */ String* origin);

        /**
         * Gets the quota for this origin, for the Web SQL Database API, in
         * bytes. If this origin does not use the Web SQL Database API, this
         * quota will be set to zero.
         *
         * @return the quota, in bytes
         */
        CARAPI GetQuota(
            /* [out] */ Int64* quota);

        /**
         * Gets the total amount of storage currently being used by this origin,
         * for all JavaScript storage APIs, in bytes.
         *
         * @return the total amount of storage, in bytes
         */
        CARAPI GetUsage(
            /* [out] */ Int64* usage);

        CARAPI ToString(
            /* [out] */ String* info);

    protected:
        /** @hide */
        Origin(
            /* [in] */ const String& origin,
            /* [in] */ Int64 quota,
            /* [in] */ Int64 usage);

        /** @hide */
        Origin(
            /* [in] */ const String& origin,
            /* [in] */ Int64 quota);

        /** @hide */
        Origin(
            /* [in] */ const String& origin);

    private:
        String mOrigin;
        Int64 mQuota;
        Int64 mUsage;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * This class should not be instantiated directly, applications must only use
     * {@link #getInstance()} to obtain the instance.
     * Note this constructor was erroneously public and published in SDK levels prior to 16, but
     * applications using it would receive a non-functional instance of this class (there was no
     * way to call createHandler() and createUIHandler(), so it would not work).
     * @hide
     */
    WebStorage();

    CARAPI constructor();

    /*
     * When calling getOrigins(), getUsageForOrigin() and getQuotaForOrigin(),
     * we need to get the values from WebCore, but we cannot block while doing so
     * as we used to do, as this could result in a full deadlock (other WebCore
     * messages received while we are still blocked here, see http://b/2127737).
     *
     * We have to do everything asynchronously, by providing a callback function.
     * We post a message on the WebCore thread (mHandler) that will get the result
     * from WebCore, and we post it back on the UI thread (using mUIHandler).
     * We can then use the callback function to return the value.
     */

    /**
     * Gets the origins currently using either the Application Cache or Web SQL
     * Database APIs. This method operates asynchronously, with the result
     * being provided via a {@link ValueCallback}. The origins are provided as
     * a map, of type {@code Map<String, WebStorage.Origin>}, from the string
     * representation of the origin to a {@link WebStorage.Origin} object.
     */
    virtual CARAPI GetOrigins(
        /* [in] */ IValueCallback* callBack);

    /**
     * Gets the amount of storage currently being used by both the Application
     * Cache and Web SQL Database APIs by the given origin. The amount is given
     * in bytes and the origin is specified using its string representation.
     * This method operates asynchronously, with the result being provided via
     * a {@link ValueCallback}.
     */
    virtual CARAPI GetUsageForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callBack);

    /**
     * Gets the storage quota for the Web SQL Database API for the given origin.
     * The quota is given in bytes and the origin is specified using its string
     * representation. This method operates asynchronously, with the result
     * being provided via a {@link ValueCallback}. Note that a quota is not
     * enforced on a per-origin basis for the Application Cache API.
     */
    virtual CARAPI GetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callBack);

    /**
     * Sets the storage quota for the Web SQL Database API for the given origin.
     * The quota is specified in bytes and the origin is specified using its string
     * representation. Note that a quota is not enforced on a per-origin basis
     * for the Application Cache API.
     */
    virtual CARAPI SetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ Int64 quota);

    /**
     * Clears the storage currently being used by both the Application Cache and
     * Web SQL Database APIs by the given origin. The origin is specified using
     * its string representation.
     */
    virtual CARAPI DeleteOrigin(
        /* [in] */ const String& origin);

    /**
     * Clears all storage currently being used by the JavaScript storage APIs.
     * This includes the Application Cache, Web SQL Database and the HTML5 Web
     * Storage APIs.
     */
    virtual CARAPI DeleteAllData();

    /**
     * Gets the singleton instance of this class.
     *
     * @return the singleton {@link WebStorage} instance
     */
    static CARAPI_(AutoPtr<IWebStorage>) GetInstance();

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBSTORAGE_H__
