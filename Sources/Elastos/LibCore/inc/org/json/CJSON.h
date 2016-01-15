
#ifndef __ORG_JSON_CJSON_H__
#define __ORG_JSON_CJSON_H__

#include "Elastos.CoreLibrary.Core.h"
#include "_Org_Json_CJSON.h"
#include "Singleton.h"

using Elastos::Core::IBoolean;
using Elastos::Core::IDouble;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;

namespace Org {
namespace Json {

/**
 * An XML pull parser with limited support for parsing internal DTDs.
 */
CarClass(CJSON)
    , public Singleton
    , public IJSON
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Returns the input if it is a JSON-permissible value; throws otherwise.
     */
    CARAPI CheckDouble(
        /* [in] */ Double d,
        /* [out] */ Double* result);

    CARAPI ToBoolean(
        /* [in] */ IInterface* value,
        /* [out] */ IBoolean** result);

    CARAPI ToDouble(
        /* [in] */ IInterface* value,
        /* [out] */ IDouble** result);

    CARAPI ToInteger32(
        /* [in] */ IInterface* value,
        /* [out] */ IInteger32** result);

    CARAPI ToInteger64(
        /* [in] */ IInterface* value,
        /* [out] */ IInteger64** result);

    CARAPI ToString(
        /* [in] */ IInterface* value,
        /* [out] */ ICharSequence** result);

    CARAPI TypeMismatch(
        /* [in] */ IInterface* indexOrName,
        /* [in] */ IInterface* actual,
        /* [in] */ const String& requiredType);

    CARAPI TypeMismatch(
        /* [in] */ IInterface* actual,
        /* [in] */ const String& requiredType);
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_CJSON_H__
