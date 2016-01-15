
#include "elastos/droid/webkit/WebStorage.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//                       WebStorage::Origin
//===============================================================

CAR_INTERFACE_IMPL(WebStorage::Origin, Object, IWebStorageOrigin);

/** @hide */
WebStorage::Origin::Origin(
    /* [in] */ const String& origin,
    /* [in] */ Int64 quota,
    /* [in] */ Int64 usage)
    : mOrigin(origin)
    , mQuota(quota)
    , mUsage(usage)
{
}

/** @hide */
WebStorage::Origin::Origin(
    /* [in] */ const String& origin,
    /* [in] */ Int64 quota)
    : mOrigin(origin)
    , mQuota(quota)
    , mUsage(0)
{
}

/** @hide */
WebStorage::Origin::Origin(
    /* [in] */ const String& origin)
    : mOrigin(origin)
    , mQuota(0)
    , mUsage(0)
{
}

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
ECode WebStorage::Origin::GetOrigin(
    /* [out] */ String* origin)
{
    VALIDATE_NOT_NULL(origin);
    *origin = mOrigin;
    return NOERROR;
}

/**
 * Gets the quota for this origin, for the Web SQL Database API, in
 * bytes. If this origin does not use the Web SQL Database API, this
 * quota will be set to zero.
 *
 * @return the quota, in bytes
 */
ECode WebStorage::Origin::GetQuota(
    /* [out] */ Int64* quota)
{
    VALIDATE_NOT_NULL(quota);
    *quota = mQuota;
    return NOERROR;
}

/**
 * Gets the total amount of storage currently being used by this origin,
 * for all JavaScript storage APIs, in bytes.
 *
 * @return the total amount of storage, in bytes
 */
ECode WebStorage::Origin::GetUsage(
    /* [out] */ Int64* usage)
{
    VALIDATE_NOT_NULL(usage);
    *usage = mUsage;
    return NOERROR;
}

ECode WebStorage::Origin::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebStorage::Origin";
    return NOERROR;
}

//===============================================================
//                           WebStorage
//===============================================================

CAR_INTERFACE_IMPL(WebStorage, Object, IWebStorage);

/**
 * This class should not be instantiated directly, applications must only use
 * {@link #getInstance()} to obtain the instance.
 * Note this constructor was erroneously public and published in SDK levels prior to 16, but
 * applications using it would receive a non-functional instance of this class (there was no
 * way to call createHandler() and createUIHandler(), so it would not work).
 * @hide
 */
WebStorage::WebStorage()
{
}

ECode WebStorage::constructor()
{
    return NOERROR;
}

/**
 * Gets the origins currently using either the Application Cache or Web SQL
 * Database APIs. This method operates asynchronously, with the result
 * being provided via a {@link ValueCallback}. The origins are provided as
 * a map, of type {@code Map<String, WebStorage.Origin>}, from the string
 * representation of the origin to a {@link WebStorage.Origin} object.
 */
ECode WebStorage::GetOrigins(
    /* [in] */ IValueCallback* callBack)
{
    // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    return NOERROR;
}

/**
 * Gets the amount of storage currently being used by both the Application
 * Cache and Web SQL Database APIs by the given origin. The amount is given
 * in bytes and the origin is specified using its string representation.
 * This method operates asynchronously, with the result being provided via
 * a {@link ValueCallback}.
 */
ECode WebStorage::GetUsageForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callBack)
{
    // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    return NOERROR;
}

/**
 * Gets the storage quota for the Web SQL Database API for the given origin.
 * The quota is given in bytes and the origin is specified using its string
 * representation. This method operates asynchronously, with the result
 * being provided via a {@link ValueCallback}. Note that a quota is not
 * enforced on a per-origin basis for the Application Cache API.
 */
ECode WebStorage::GetQuotaForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callBack)
{
    // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    return NOERROR;
}

/**
 * Sets the storage quota for the Web SQL Database API for the given origin.
 * The quota is specified in bytes and the origin is specified using its string
 * representation. Note that a quota is not enforced on a per-origin basis
 * for the Application Cache API.
 */
ECode WebStorage::SetQuotaForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ Int64 quota)
{
    // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    return NOERROR;
}

/**
 * Clears the storage currently being used by both the Application Cache and
 * Web SQL Database APIs by the given origin. The origin is specified using
 * its string representation.
 */
ECode WebStorage::DeleteOrigin(
    /* [in] */ const String& origin)
{
    // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    return NOERROR;
}

/**
 * Clears all storage currently being used by the JavaScript storage APIs.
 * This includes the Application Cache, Web SQL Database and the HTML5 Web
 * Storage APIs.
 */
ECode WebStorage::DeleteAllData()
{
    // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    return NOERROR;
}

/**
 * Gets the singleton instance of this class.
 *
 * @return the singleton {@link WebStorage} instance
 */
AutoPtr<IWebStorage> WebStorage::GetInstance()
{
    AutoPtr<IWebStorage> webStorage;
    WebViewFactory::GetProvider()->GetWebStorage((IWebStorage**)&webStorage);
    return webStorage;
}

ECode WebStorage::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebStorage";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos