
#ifndef __Elastos_DevSamples_ContentProviderDemo_Utils_H__
#define __Elastos_DevSamples_ContentProviderDemo_Utils_H__

#include <Elastos.Droid.Net.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

class Utils
{
public:
    static const String DB_NAME;
    static const String TABLE_NAME;
    static const Int32 VERSION;

    static const String TAG_ID;
    static const String USERNAME;
    static const String SEX;
    static const String EMAIL;

    static const String AUTOHORITY;
    static const Int32 ITEM = 1;
    static const Int32 ITEM_ID = 2;

    static const String CONTENT_TYPE;
    static const String CONTENT_ITEM_TYPE;

    static const AutoPtr<IUri> CONTENT_URI;

private:
    Utils();
    Utils(const Utils&);
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ContentProviderDemo_Utils_H__
