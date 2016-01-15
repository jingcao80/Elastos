
#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERHELPER_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERHELPER_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Content_CContentProviderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderHelper)
    , public Singleton
    , public IContentProviderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Given an IContentProvider, try to coerce it back to the real
     * ContentProvider object if it is running in the local process.  This can
     * be used if you know you are running in the same process as a provider,
     * and want to get direct access to its implementation details.  Most
     * clients should not nor have a reason to use it.
     *
     * @param abstractInterface The ContentProvider interface that is to be
     *              coerced.
     * @return If the IContentProvider is non-{@code null} and local, returns its actual
     * ContentProvider instance.  Otherwise returns {@code null}.
     * @hide
     */
    CARAPI CoerceToLocalContentProvider(
        /* [in] */ IIContentProvider* abstractInterface,
        /* [out] */ IContentProvider** localContentProvider);

    /**
     * Removes userId part from authority string. Expects format:
     * userId@some.authority
     * If there is no userId in the authority, it symply returns the argument
     * @hide
     */
    CARAPI GetAuthorityWithoutUserId(
        /* [in] */ const String& auth,
        /* [out] */ String* result);

    /** @hide */
    CARAPI GetUriWithoutUserId(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /** @hide */
    CARAPI UriHasUserId(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI MaybeAddUserId(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId,
        /* [out] */ IUri** result);

    CARAPI GetUserIdFromAuthority(
        /* [in] */ const String& auth,
        /* [in] */ Int32 defaultUserId,
        /* [out] */ Int32* userId);

    CARAPI GetUserIdFromUri(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 defaultUserId,
        /* [out] */ Int32* userId);

    CARAPI GetUserIdFromUri(
        /* [in] */ IUri* uri,
        /* [out] */ Int32* userId);

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERHELPER_H__
