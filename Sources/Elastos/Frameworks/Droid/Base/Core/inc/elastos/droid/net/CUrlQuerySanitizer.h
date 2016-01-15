
#ifndef __ELASTOS_DROID_NET_CURLQUERYSANITIZER_H__
#define __ELASTOS_DROID_NET_CURLQUERYSANITIZER_H__

#include "_Elastos_Droid_Net_CUrlQuerySanitizer.h"
#include "elastos/droid/net/UrlQuerySanitizer.h"

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
CarClass(CUrlQuerySanitizer)
    , public UrlQuerySanitizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CURLQUERYSANITIZER_H__
