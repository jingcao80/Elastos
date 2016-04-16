
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_CONTENTURIUTILS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_CONTENTURIUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * This class provides methods to access content URI schemes.
 */
class ContentUriUtils
    : public Object
{
public:
    /**
     * Opens the content URI for reading, and returns the file descriptor to
     * the caller. The caller is responsible for closing the file desciptor.
     *
     * @param context {@link Context} in interest
     * @param uriString the content URI to open
     * @returns file desciptor upon sucess, or -1 otherwise.
     */
    //@CalledByNative
    static CARAPI_(Int32) OpenContentUriForRead(
        /* [in] */ IContext* context,
        /* [in] */ const String& uriString);

    /**
     * Check whether a content URI exists.
     *
     * @param context {@link Context} in interest.
     * @param uriString the content URI to query.
     * @returns true if the uri exists, or false otherwise.
     */
    //@CalledByNative
    static CARAPI_(Boolean) ContentUriExists(
        /* [in] */ IContext* context,
        /* [in] */ const String& uriString);

    /**
     * Method to resolve the display name of a content URI.
     *
     * @param uri the content URI to be resolved.
     * @param contentResolver the content resolver to query.
     * @param columnField the column field to query.
     * @returns the display name of the @code uri if present in the database
     *  or an empty string otherwise.
     */
    static CARAPI_(String) GetDisplayName(
        /* [in] */ IUri* uri,
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ const String& columnField);

    static CARAPI_(void*) ElaContentUriUtilsCallback_Init();

private:
    static CARAPI_(Int32) OpenContentUriForRead(
        /* [in] */ IInterface* context,
        /* [in] */ const String& uriString);

    static CARAPI_(Boolean) ContentUriExists(
        /* [in] */ IInterface* context,
        /* [in] */ const String& uriString);

    // Prevent instantiation.
    ContentUriUtils();

    /**
     * Helper method to open a content URI and return the ParcelFileDescriptor.
     *
     * @param context {@link Context} in interest.
     * @param uriString the content URI to open.
     * @returns ParcelFileDescriptor of the content URI, or NULL if the file does not exist.
     */
    static CARAPI_(AutoPtr<IParcelFileDescriptor>) GetParcelFileDescriptor(
        /* [in] */ IContext* context,
        /* [in] */ const String& uriString);

private:
    static const String TAG;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_CONTENTURIUTILS_H__
