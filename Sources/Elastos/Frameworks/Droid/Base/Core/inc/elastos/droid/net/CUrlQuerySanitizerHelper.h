
#ifndef __ELASTOS_DROID_NET_CURLQUERYSANITIZERHELPER_H__
#define __ELASTOS_DROID_NET_CURLQUERYSANITIZERHELPER_H__

#include "_Elastos_Droid_Net_CUrlQuerySanitizerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *
 * Sanitizes the Query portion of a URL. Simple example:
 * <code>
 * UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
 * sanitizer.setAllowUnregisteredParamaters(true);
 * sanitizer.parseUrl("http://example.com/?name=Joe+User");
 * String name = sanitizer.getValue("name"));
 * // name now contains "Joe_User"
 * </code>
 *
 * Register ValueSanitizers to customize the way individual
 * parameters are sanitized:
 * <code>
 * UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
 * sanitizer.registerParamater("name", UrlQuerySanitizer.createSpaceLegal());
 * sanitizer.parseUrl("http://example.com/?name=Joe+User");
 * String name = sanitizer.getValue("name"));
 * // name now contains "Joe User". (The string is first decoded, which
 * // converts the '+' to a ' '. Then the string is sanitized, which
 * // converts the ' ' to an '_'. (The ' ' is converted because the default
 * unregistered parameter sanitizer does not allow any special characters,
 * and ' ' is a special character.)
 * </code>
 *
 * There are several ways to create ValueSanitizers. In order of increasing
 * sophistication:
 * <ol>
 * <li>Call one of the UrlQuerySanitizer.createXXX() methods.
 * <li>Construct your own instance of
 * UrlQuerySanitizer.IllegalCharacterValueSanitizer.
 * <li>Subclass UrlQuerySanitizer.ValueSanitizer to define your own value
 * sanitizer.
 * </ol>
 *
 */
CarClass(CUrlQuerySanitizerHelper)
    , public Singleton
    , public IUrlQuerySanitizerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAllIllegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetAllButNulLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetAllButWhitespaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetUrlLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetUrlAndSpaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetAmpLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetAmpAndSpaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetSpaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    CARAPI GetAllButNulAndAngleBracketsLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CURLQUERYSANITIZERHELPER_H__
