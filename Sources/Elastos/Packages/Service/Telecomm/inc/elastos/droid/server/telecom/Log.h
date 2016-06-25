
#ifndef __ELASTOS_DROID_SERVER_TELECOM_LOG_H__
#define __ELASTOS_DROID_SERVER_TELECOM_LOG_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Manages logging for the entire module.
 */
class Log
    : public Object
{
public:
    static CARAPI_(Boolean) IsLoggable(
        /* [in] */ Int32 level);

    static CARAPI D(
        /* [in] */ const char * tag,
        /* [in] */ const char * fmt, ...);

    static CARAPI D(
        /* [in] */ IInterface* objectPrefix,
        /* [in] */ const char * fmt, ...);

    static CARAPI I(
        /* [in] */ const char * tag,
        /* [in] */ const char * fmt, ...);

    static CARAPI I(
        /* [in] */ IInterface* objectPrefix,
        /* [in] */ const char * fmt, ...);

    static CARAPI V(
        /* [in] */ const char * tag,
        /* [in] */ const char * fmt, ...);

    static CARAPI V(
        /* [in] */ IInterface* objectPrefix,
        /* [in] */ const char * fmt, ...);

    static CARAPI W(
        /* [in] */ const char * tag,
        /* [in] */ const char * fmt, ...);

    static CARAPI W(
        /* [in] */ IInterface* objectPrefix,
        /* [in] */ const char * fmt, ...);

    static CARAPI E(
        /* [in] */ const char * tag,
        /* [in] */ const char * fmt, ...);

    static CARAPI E(
        /* [in] */ IInterface* objectPrefix,
        /* [in] */ const char * fmt, ...);

    static CARAPI Wtf(
        /* [in] */ const char * tag,
        /* [in] */ const char * fmt, ...);

    static CARAPI Wtf(
        /* [in] */ IInterface* objectPrefix,
        /* [in] */ const char * fmt, ...);

    static CARAPI_(String) PiiHandle(
        /* [in] */ IInterface* pii);

    /**
     * Redact personally identifiable information for production users.
     * If we are running in verbose mode, return the original string, otherwise
     * return a SHA-1 hash of the input string.
     */
    static CARAPI_(String) Pii(
        /* [in] */ IInterface* pii);

private:
    Log();

    static CARAPI SecureHash(
        /* [in] */ ArrayOf<Byte>* input,
        /* [out] */ String* result);

    static CARAPI_(String) EncodeHex(
        /* [in] */ ArrayOf<Byte>* bytes);

    static CARAPI_(String) GetPrefixFromObject(
        /* [in] */ IInterface* obj);

public:
    static const Boolean FORCE_LOGGING; /* STOP SHIP if true */
    static const Boolean DEBUG;
    static const Boolean INFO;
    static const Boolean VERBOSE;
    static const Boolean WARN;
    static const Boolean ERROR;

private:
    // Generic tag for all In Call logging
    static const String TAG;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_LOG_H__
