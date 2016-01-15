
#ifndef __ELASTOS_DROID_NET_CMAILTOHELPER_H__
#define __ELASTOS_DROID_NET_CMAILTOHELPER_H__

#include "_Elastos_Droid_Net_CMailToHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CMailToHelper)
    , public Singleton
    , public IMailToHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Test to see if the given string is a mailto URL
     * @param url string to be tested
     * @return true if the string is a mailto URL
     */
    CARAPI IsMailTo(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    /**
     * Parse and decode a mailto scheme string. This parser implements
     * RFC 2368. The returned object can be queried for the parsed parameters.
     * @param url String containing a mailto URL
     * @return MailTo object
     * @exception ParseException if the scheme is not a mailto URL
     */
    CARAPI Parse(
        /* [in] */ const String& url,
        /* [out] */ IMailTo** result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CMAILTOHELPER_H__
