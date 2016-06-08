
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                                CatLog
//=====================================================================
const Boolean CatLog::DEBUG = TRUE;

ECode CatLog::D(
    /* [in] */ IInterface* caller,
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // if (!DEBUG) {
    //     return;
    // }
    //
    // String className = caller.getClass().getName();
    // Rlog.d("CAT", className.substring(className.lastIndexOf('.') + 1) + ": "
    //         + msg);
    assert(0);
    return NOERROR;
}

ECode CatLog::D(
    /* [in] */ const String& caller,
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // if (!DEBUG) {
    //     return;
    // }
    //
    // Rlog.d("CAT", caller + ": " + msg);
    assert(0);
    return NOERROR;
}

ECode CatLog::E(
    /* [in] */ IInterface* caller,
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // String className = caller.getClass().getName();
    // Rlog.e("CAT", className.substring(className.lastIndexOf('.') + 1) + ": "
    //         + msg);
    assert(0);
    return NOERROR;
}

ECode CatLog::E(
    /* [in] */ const String& caller,
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.e("CAT", caller + ": " + msg);
    assert(0);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


