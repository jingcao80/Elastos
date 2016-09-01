
#ifndef __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALCURSORLOADER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALCURSORLOADER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/dialpad/SmartDialNameMatcher.h"
#include <elastos/droid/content/AsyncTaskLoader.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"

using Elastos::Droid::Content::AsyncTaskLoader;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

/**
 * Implements a Loader<Cursor> class to asynchronously load SmartDial search results.
 */
class SmartDialCursorLoader
    : public AsyncTaskLoader
    , public ISmartDialCursorLoader
{
public:
    CAR_INTERFACE_DECL()

    SmartDialCursorLoader(
        /* [in] */ IContext* context);

    /**
     * Configures the query string to be used to find SmartDial matches.
     * @param query The query string user typed.
     */
    CARAPI ConfigureQuery(
        /* [in] */ const String& query);

    /**
     * Queries the SmartDial database and loads results in background.
     * @return Cursor of contacts that matches the SmartDial query.
     */
    // @Override
    CARAPI LoadInBackground(
        /* [out] */ IInterface** result);

    // @Override
    CARAPI DeliverResult(
        /* [in] */ IInterface* data);

    // @Override
    CARAPI OnCanceled(
        /* [in] */ IInterface* data);

protected:
    // @Override
    CARAPI OnStartLoading();

    // @Override
    CARAPI OnStopLoading();

    // @Override
    CARAPI OnReset();

private:
    CARAPI_(void) ReleaseResources(
        /* [in] */ ICursor* cursor);

private:
    static const String TAG; // = SmartDialCursorLoader.class.getSimpleName();
    static const Boolean DEBUG = FALSE; // = false;

    AutoPtr<IContext> mContext;

    AutoPtr<ICursor> mCursor;

    String mQuery;
    AutoPtr<SmartDialNameMatcher> mNameMatcher;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALCURSORLOADER_H__
