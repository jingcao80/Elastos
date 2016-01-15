#ifndef __ELASTOS_DROID_DATABASE_ABSTRACTWINDOWEDCURSOR_H__
#define __ELASTOS_DROID_DATABASE_ABSTRACTWINDOWEDCURSOR_H__

#include "elastos/droid/database/AbstractCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * A base class for Cursors that store their data in {@link CursorWindow}s.
 */
class AbstractWindowedCursor
    : public AbstractCursor
    , public IAbstractWindowedCursor
{
public:
    CAR_INTERFACE_DECL()

    //@Override
    virtual CARAPI GetBlob(
        /* [in] */ Int32 columnIndex,
        /* [out] */ ArrayOf<Byte>** blob);

    //@Override
    virtual CARAPI GetString(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* str);

    //@Override
    virtual CARAPI CopyStringToBuffer(
        /* [in] */ Int32 columnIndex,
        /* [in] */ ICharArrayBuffer* buffer);

    //@Override
    virtual CARAPI GetInt16(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int16* value);

    //@Override
    virtual CARAPI GetInt32(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int32* value);

    //@Override
    virtual CARAPI GetInt64(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int64* value);

    //@Override
    virtual CARAPI GetFloat(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Float* value);

    //@Override
    virtual CARAPI GetDouble(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Double* value);

    //@Override
    virtual CARAPI IsNull(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isNull);

    virtual CARAPI IsBlob(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isBlob);

    virtual CARAPI IsString(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isString);

    virtual CARAPI IsInt64(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isInt64);

    virtual CARAPI IsFloat(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isFloat);

    virtual CARAPI GetType(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int32* type);

    //@Override
    virtual CARAPI GetWindow(
        /* [out] */ ICursorWindow** window);

    /**
     * Set a new cursor window to cursor, usually set a remote cursor window
     * @param window cursor window
     */
    virtual CARAPI SetWindow(
        /* [in] */ ICursorWindow* window);

    virtual CARAPI HasWindow(
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI CheckPosition();

    /**
     * Closes the cursor window and sets {@link #mWindow} to null.
     * @hide
     */
    CARAPI CloseWindow();

    /**
     * If there is a window, clear it.
     * Otherwise, creates a new window.
     *
     * @param name The window name.
     * @hide
     */
    CARAPI ClearOrCreateWindow(
        /* [in] */ const String& name);

    /** @hide */
    //@Override
    CARAPI OnDeactivateOrClose();

protected:
    /**
     * The cursor window owned by this cursor.
     */
    AutoPtr<ICursorWindow> mWindow;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_ABSTRACTWINDOWEDCURSOR_H__
