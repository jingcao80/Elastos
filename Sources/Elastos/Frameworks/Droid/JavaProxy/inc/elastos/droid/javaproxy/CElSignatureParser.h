
#ifndef __ELASTOS_DROID_JAVAPROXY_CELSIGNATUREPARSER_H__
#define __ELASTOS_DROID_JAVAPROXY_CELSIGNATUREPARSER_H__

#include "_Elastos_Droid_JavaProxy_CElSignatureParser.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::IElSignatureParser;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CElSignatureParser)
    , public Object
    , public IElSignatureParser
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle32 jVM);

    CARAPI GetSignature(
        /* [in] */ const String& pkg,
        /* [out] */ ISignature** signature);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;
    JavaVM* mJVM;
};

} // JavaProxy
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CELSIGNATUREPARSER_H__
