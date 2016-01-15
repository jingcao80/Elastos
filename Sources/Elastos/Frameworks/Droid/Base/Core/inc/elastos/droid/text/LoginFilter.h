#ifndef __ELASTOS_DROID_TEXT_LOGINFILTER_H__
#define __ELASTOS_DROID_TEXT_LOGINFILTER_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Abstract class for filtering login-related text (user names and passwords)
 *
 */
class LoginFilter
    : public Object
    , public ILoginFilter
    , public IInputFilter
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Default constructor for LoginFilter doesn't append invalid characters.
     */
    LoginFilter();

    virtual ~LoginFilter();

    CARAPI constructor();

    /**
     * Base constructor for LoginFilter
     * @param appendInvalid whether or not to append invalid characters.
     */
    CARAPI constructor(
        /* [in] */ Boolean appendInvalid);

    /**
     * This method is called when the buffer is going to replace the
     * range <code>dstart &hellip; dend</code> of <code>dest</code>
     * with the new text from the range <code>start &hellip; end</code>
     * of <code>source</code>.  Returns the CharSequence that we want
     * placed there instead, including an empty string
     * if appropriate, or <code>null</code> to accept the original
     * replacement.  Be careful to not to reject 0-length replacements,
     * as this is what happens when you delete text.
     */
    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** cs);

    /**
     * Called when we start processing filter.
     */
    CARAPI OnStart();

    /**
     * Called whenever we encounter an invalid character.
     * @param c the invalid character
     */
    CARAPI OnInvalidCharacter(
        /* [in] */ Char32 c);

    /**
     * Called when we're done processing filter
     */
    CARAPI OnStop();

    /**
     * Returns whether or not we allow character c.
     * Subclasses must override this method.
     */
    virtual CARAPI IsAllowed(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* ret) = 0;

private:
    Boolean mAppendInvalid;  // whether to append or ignore invalid characters
};

/**
 * This filter rejects characters in the user name that are not compatible with GMail
 * account creation. It prevents the user from entering user names with characters other than
 * [a-zA-Z0-9.].
 *
 */
class UsernameFilterGMail
    : public LoginFilter
    , public IUsernameFilterGMail
{
public:
    CAR_INTERFACE_DECL()

    //@Override
    CARAPI IsAllowed(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* ret);
};

/**
 * This filter rejects characters in the user name that are not compatible with Google login.
 * It is slightly less restrictive than the above filter in that it allows [a-zA-Z0-9._-+].
 *
 */
class UsernameFilterGeneric
    : public LoginFilter
    , public IUsernameFilterGeneric
{
public:
    CAR_INTERFACE_DECL()

    //@Override
    CARAPI IsAllowed(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* ret);

private:
    static String mAllowed;// = "@_-+."; // Additional characters
};

/**
 * This filter is compatible with GMail passwords which restricts characters to
 * the Latin-1 (ISO8859-1) char set.
 *
 */
class PasswordFilterGMail
    : public LoginFilter
    , public IPasswordFilterGMail
{
public:
    CAR_INTERFACE_DECL()

    // We should reject anything not in the Latin-1 (ISO8859-1) charset
    //@Override
    CARAPI IsAllowed(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* ret);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_LOGINFILTER_H__
