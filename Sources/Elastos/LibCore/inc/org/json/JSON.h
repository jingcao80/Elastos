
#ifndef __ORG_JSON_JSON_H__
#define __ORG_JSON_JSON_H__

#include "Elastos.CoreLibrary.Core.h"

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
class JSON
{
public:
    /**
     * Returns the input if it is a JSON-permissible value; throws otherwise.
     */
    static CARAPI CheckDouble(
        /* [in] */ Double d);

    static CARAPI ToBoolean(
        /* [in] */ IInterface* value,
        /* [out] */ IBoolean** result);

    static CARAPI ToDouble(
        /* [in] */ IInterface* value,
        /* [out] */ IDouble** result);

    static CARAPI ToInteger32(
        /* [in] */ IInterface* value,
        /* [out] */ IInteger32** result);

    static CARAPI ToInteger64(
        /* [in] */ IInterface* value,
        /* [out] */ IInteger64** result);

    static CARAPI ToString(
        /* [in] */ IInterface* value,
        /* [out] */ String* result);

    static CARAPI TypeMismatch(
        /* [in] */ IInterface* indexOrName,
        /* [in] */ IInterface* actual,
        /* [in] */ const String& requiredType);

    static CARAPI TypeMismatch(
        /* [in] */ IInterface* actual,
        /* [in] */ const String& requiredType);
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_JSON_H__
