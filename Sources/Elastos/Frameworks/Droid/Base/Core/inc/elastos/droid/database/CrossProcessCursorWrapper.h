#ifndef __ELASTOS_DROID_DATABASE_CROSSPROCESSCURSORWRAPPER_H__
#define __ELASTOS_DROID_DATABASE_CROSSPROCESSCURSORWRAPPER_H__

#include "elastos/droid/database/CursorWrapper.h"

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Cursor wrapper that implements {@link CrossProcessCursor}.
 * <p>
 * If the wrapped cursor implements {@link CrossProcessCursor}, then the wrapper
 * delegates {@link #fillWindow}, {@link #getWindow()} and {@link #onMove} to it.
 * Otherwise, the wrapper provides default implementations of these methods that
 * traverse the contents of the cursor similar to {@link AbstractCursor#fillWindow}.
 * </p><p>
 * This wrapper can be used to adapt an ordinary {@link Cursor} into a
 * {@link CrossProcessCursor}.
 * </p>
 */
class CrossProcessCursorWrapper
    : public CursorWrapper
    , public ICrossProcessCursorWrapper
    , public ICrossProcessCursor
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("CrossProcessCursorWrapper")

    //@Override
    virtual CARAPI FillWindow(
        /* [in] */ Int32 position,
        /* [in] */ ICursorWindow* window);

    //@Override
    virtual CARAPI GetWindow(
        /* [out] */ ICursorWindow** window);

    //@Override
    virtual CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* result);
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CROSSPROCESSCURSORWRAPPER_H__
