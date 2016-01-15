#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_LOCALLOG_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_LOCALLOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class LocalLog
    : public Object
    , public ILocalLog
{
public:
    CAR_INTERFACE_DECL()

    LocalLog();

    CARAPI constructor(
        /* [in] */ const String& tag);

    CARAPI W(
        /* [in] */ const String& msg);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& header,
        /* [in] */ const String& prefix,
        /* [out] */ Boolean* result);

private:
    String mTag;
    Int32 mMaxLines;
    List<String> mLines;
    Object mLinesLock;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_LOCALLOG_H__
