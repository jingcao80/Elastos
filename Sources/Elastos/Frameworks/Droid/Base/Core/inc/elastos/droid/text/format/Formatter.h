#ifndef __ELASTOS_DROID_TEXT_FORMAT_Formatter_H__
#define __ELASTOS_DROID_TEXT_FORMAT_Formatter_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Net::IInetAddress;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

/**
 * Utility class to aid in formatting common values that are not covered
 * by the {@link java.util.Formatter} class in {@link java.util}
 */
class Formatter
{
public:
    /**
     * Formats a content size to be in the form of bytes, kilobytes, megabytes, etc
     *
     * @param context Context to use to load the localized units
     * @param number size value to be formatted
     * @return formatted string with the number
     */
    static CARAPI_(String) FormatFileSize(
        /* [in] */ IContext* context,
        /* [in] */ Int64 number);

    /**
     * Like {@link #formatFileSize}, but trying to generate shorter numbers
     * (showing fewer digits of precision).
     */
    static CARAPI_(String) FormatShortFileSize(
        /* [in] */ IContext* context,
        /* [in] */ Int64 number);

    /**
     * Returns a string in the canonical IP format ###.###.###.### from a packed integer containing
     * the IP address.  The IP address is expected to be in little-endian format (LSB first). That
     * is, 0x01020304 will return "4.3.2.1".
     *
     * @param ipv4Address the IP address as a packed integer with LSB first.
     * @return string with canonical IP address format.
     *
     * @deprecated this method doesn't support IPv6 addresses. Prefer {@link
     *     java.net.InetAddress#getHostAddress()}, which supports both IPv4 and
     *     IPv6 addresses.
     */
    //@Deprecated
    static CARAPI_(String) FormatIpAddress(
        /* [in] */ Int32 ipv4Address);

    /**
     * Returns elapsed time for the given millis, in the following format:
     * 1 day 5 hrs; will include at most two units, can go down to seconds precision.
     * @param context the application context
     * @param millis the elapsed time in milli seconds
     * @return the formatted elapsed time
     * @hide
     */
     static CARAPI_(String) FormatShortElapsedTime(
            /* [in] */ IContext* context,
            /* [in] */ Int64 millis);

private:
    Formatter();
    Formatter(const Formatter&);

    static CARAPI_(String) FormatFileSize(
        /* [in] */ IContext* context,
        /* [in] */ Int64 number,
        /* [in] */ Boolean shorter);

private:
    static const Int32 SECONDS_PER_MINUTE;// = 60;
    static const Int32 SECONDS_PER_HOUR;// = 60 * 60;
    static const Int32 SECONDS_PER_DAY;// = 24 * 60 * 60;
};



} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_Formatter_H__
