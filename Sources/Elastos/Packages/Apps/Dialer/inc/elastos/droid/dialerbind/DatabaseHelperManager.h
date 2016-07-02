
#ifndef __ELASTOS_DROID_DIALERBIND_DATABASEHELPERMANAGER_H__
#define __ELASTOS_DROID_DIALERBIND_DATABASEHELPERMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::Database::DialerDatabaseHelper;

namespace Elastos {
namespace Droid {
namespace DialerBind {

class DatabaseHelperManager
{
public:
    static CARAPI_(AutoPtr<DialerDatabaseHelper>) GetDatabaseHelper(
        /* [in] */ IContext* context);
};

} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_DATABASEHELPERMANAGER_H__
