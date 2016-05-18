
#ifndef __ELASTOS_DROID_WEBKIT_MIMETYPEMAP_H__
#define __ELASTOS_DROID_WEBKIT_MIMETYPEMAP_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Two-way map that maps MIME-types to file extensions and vice versa.
 *
 * <p>See also {@link java.net.URLConnection#guessContentTypeFromName}
 * and {@link java.net.URLConnection#guessContentTypeFromStream}. This
 * class and {@code URLConnection} share the same MIME-type database.
 */
class MimeTypeMap
    : public Object
    , public IMimeTypeMap
{
    friend class CMimeTypeMapHelper;
public:
    CAR_INTERFACE_DECL()

    /**
     * Return true if the given MIME type has an entry in the map.
     * @param mimeType A MIME type (i.e. text/plain)
     * @return True iff there is a mimeType entry in the map.
     */
    CARAPI HasMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean* result);

    /**
     * Return the MIME type for the given extension.
     * @param extension A file extension without the leading '.'
     * @return The MIME type for the given extension or null iff there is none.
     */
    CARAPI GetMimeTypeFromExtension(
        /* [in] */ const String& extension,
        /* [out] */ String* mimeType);

    /**
     * Return true if the given extension has a registered MIME type.
     * @param extension A file extension without the leading '.'
     * @return True iff there is an extension entry in the map.
     */
    CARAPI HasExtension(
        /* [in] */ const String& extension,
        /* [out] */ Boolean* result);

    /**
     * Return the registered extension for the given MIME type. Note that some
     * MIME types map to multiple extensions. This call will return the most
     * common extension for the given MIME type.
     * @param mimeType A MIME type (i.e. text/plain)
     * @return The extension for the given MIME type or null iff there is none.
     */
    CARAPI GetExtensionFromMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ String* extension);

    CARAPI ToString(
        /* [out] */ String* info);

public:
    /**
     * Returns the file extension or an empty string iff there is no
     * extension. This method is a convenience method for obtaining the
     * extension of a url and has undefined results for other Strings.
     * @param url
     * @return The file extension of the given url.
     */
    static CARAPI_(String) GetFileExtensionFromUrl(
        /* [in] */ const String& url);

    /**
     * If the given MIME type is null, or one of the "generic" types (text/plain
     * or application/octet-stream) map it to a type that Android can deal with.
     * If the given type is not generic, return it unchanged.
     *
     * @param mimeType MIME type provided by the server.
     * @param url URL of the data being loaded.
     * @param contentDisposition Content-disposition header given by the server.
     * @return The MIME type that should be used for this data.
     */
    /* package */
    virtual CARAPI_(String) RemapGenericMimeType(
        /* [in] */ const String& mimeType,
        /* [in] */ const String& url,
        /* [in] */ const String& contentDisposition);

    /**
     * Get the singleton instance of MimeTypeMap.
     * @return The singleton instance of the MIME-type map.
     */
    static CARAPI_(AutoPtr<MimeTypeMap>) GetSingleton();

//private:
    // Static method called by jni.
    static CARAPI_(String) MimeTypeFromExtension(
        /* [in] */ const String& extension);

private:
    MimeTypeMap();

private:
    static AutoPtr<MimeTypeMap> sMimeTypeMap;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_MIMETYPEMAP_H__
