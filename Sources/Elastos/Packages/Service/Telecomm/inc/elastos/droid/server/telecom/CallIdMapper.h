
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLIDMAPPER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLIDMAPPER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/** Utility to map {@link Call} objects to unique IDs. IDs are generated when a call is added. */
class CallIdMapper
    : public Object
    , public ICallIdMapper
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& callIdPrefix);

    CARAPI ReplaceCall(
        /* [in] */ ICall* newCall,
        /* [in] */ ICall* callToReplace);

    CARAPI AddCall(
        /* [in] */ ICall* call,
        /* [in] */ const String& id);

    CARAPI AddCall(
        /* [in] */ ICall* call);

    CARAPI RemoveCall(
        /* [in] */ ICall* call);

    CARAPI RemoveCall(
        /* [in] */ const String& callId);

    CARAPI GetCallId(
        /* [in] */ ICall* call,
        /* [out] */ String* result);

    CARAPI GetCall(
        /* [in] */ IInterface* objId,
        /* [out] */ ICall** result);

    CARAPI Clear();

    CARAPI IsValidCallId(
        /* [in] */ const String& callId,
        /* [out] */ Boolean* result);

    CARAPI IsValidConferenceId(
        /* [in] */ const String& callId,
        /* [out] */ Boolean* result);

    CARAPI GetNewId(
        /* [out] */ String* result);

private:
    AutoPtr<IHashMap> mCalls; // HashBiMap

    String mCallIdPrefix;

    static Int32 sIdCount;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLIDMAPPER_H__
