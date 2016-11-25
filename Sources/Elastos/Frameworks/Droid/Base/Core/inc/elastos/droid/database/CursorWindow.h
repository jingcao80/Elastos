#ifndef __ELASTOS_DROID_DATABASE_CURSORWINDOW_H__
#define __ELASTOS_DROID_DATABASE_CURSORWINDOW_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/database/sqlite/SQLiteClosable.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Database::Sqlite::SQLiteClosable;
using Elastos::Core::ICloseGuard;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Database {

class CCursorWindow;

/**
 * A buffer containing multiple cursor rows.
 */
class CursorWindow
    : public SQLiteClosable
    , public ICursorWindow
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CursorWindow();

    virtual ~CursorWindow();

    CARAPI constructor();

    /**
     * Creates a new empty cursor window and gives it a name.
     * <p>
     * The cursor initially has no rows or columns.  Call {@link #setNumColumns(int)} to
     * set the number of columns before adding any rows to the cursor.
     * </p>
     *
     * @param name The name of the cursor window, or null if none.
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Creates a new empty cursor window.
     * <p>
     * The cursor initially has no rows or columns.  Call {@link #setNumColumns(int)} to
     * set the number of columns before adding any rows to the cursor.
     * </p>
     *
     * @param localWindow True if this window will be used in this process only,
     * false if it might be sent to another processes.  This argument is ignored.
     *
     * @deprecated There is no longer a distinction between local and remote
     * cursor windows.  Use the {@link #CursorWindow(String)} constructor instead.
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ Boolean localWindow);

    /**
     * Gets the name of this cursor window, never null.
     * @hide
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Clears out the existing contents of the window, making it safe to reuse
     * for new data.
     * <p>
     * The start position ({@link #getStartPosition()}), number of rows ({@link #getNumRows()}),
     * and number of columns in the cursor are all reset to zero.
     * </p>
     */
    virtual CARAPI Clear();

    /**
     * Gets the start position of this cursor window.
     * <p>
     * The start position is the zero-based index of the first row that this window contains
     * relative to the entire result set of the {@link Cursor}.
     * </p>
     *
     * @return The zero-based start position.
     */
    virtual CARAPI GetStartPosition(
        /* [out] */ Int32* pos);

    /**
     * Set the start position of cursor window
     * @param pos
     */
    virtual CARAPI SetStartPosition(
        /* [in] */ Int32 pos);

    /**
     * Returns the number of rows in this window.
     *
     * @return the number of rows in this window.
     */
    virtual CARAPI GetNumRows(
        /* [out] */ Int32* num);

    /**
     * Sets the number of columns in this window.
     * <p>
     * This method must be called before any rows are added to the window, otherwise
     * it will fail to set the number of columns if it differs from the current number
     * of columns.
     * </p>
     *
     * @param columnNum The new number of columns.
     * @return True if successful.
     */
    virtual CARAPI SetNumColumns(
        /* [in] */ Int32 columnNum,
        /* [out] */ Boolean* succeeded);

    /**
     * Allocates a new row at the end of this cursor window.
     *
     * @return True if successful, false if the cursor window is out of memory.
     */
    virtual CARAPI AllocRow(
        /* [out] */ Boolean* succeeded);

    /**
     * Frees the last row in this cursor window.
     */
    virtual CARAPI FreeLastRow();

    /**
     * Returns true if the field at the specified row and column index
     * has type {@link Cursor#FIELD_TYPE_NULL}.
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if the field has type {@link Cursor#FIELD_TYPE_NULL}.
     * @deprecated Use {@link #getType(int, int)} instead.
     */
    //@Deprecated
    virtual CARAPI IsNull(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* isNull);

    /**
     * Returns true if the field at the specified row and column index
     * has type {@link Cursor#FIELD_TYPE_BLOB} or {@link Cursor#FIELD_TYPE_NULL}.
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if the field has type {@link Cursor#FIELD_TYPE_BLOB} or
     * {@link Cursor#FIELD_TYPE_NULL}.
     * @deprecated Use {@link #getType(int, int)} instead.
     */
    //@Deprecated
    virtual CARAPI IsBlob(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* isBlob);

    /**
     * Returns true if the field at the specified row and column index
     * has type {@link Cursor#FIELD_TYPE_INTEGER}.
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if the field has type {@link Cursor#FIELD_TYPE_INTEGER}.
     * @deprecated Use {@link #getType(int, int)} instead.
     */
    //@Deprecated
    virtual CARAPI IsInt64(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* isInt64);

    /**
     * Returns true if the field at the specified row and column index
     * has type {@link Cursor#FIELD_TYPE_FLOAT}.
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if the field has type {@link Cursor#FIELD_TYPE_FLOAT}.
     * @deprecated Use {@link #getType(int, int)} instead.
     */
    //@Deprecated
    virtual CARAPI IsFloat(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* isFloat);

    /**
     * Returns true if the field at the specified row and column index
     * has type {@link Cursor#FIELD_TYPE_STRING} or {@link Cursor#FIELD_TYPE_NULL}.
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if the field has type {@link Cursor#FIELD_TYPE_STRING}
     * or {@link Cursor#FIELD_TYPE_NULL}.
     * @deprecated Use {@link #getType(int, int)} instead.
     */
    //@Deprecated
    virtual CARAPI IsString(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* isString);

    /**
     * Returns the type of the field at the specified row and column index.
     * <p>
     * The returned field types are:
     * <ul>
     * <li>{@link Cursor#FIELD_TYPE_NULL}</li>
     * <li>{@link Cursor#FIELD_TYPE_INTEGER}</li>
     * <li>{@link Cursor#FIELD_TYPE_FLOAT}</li>
     * <li>{@link Cursor#FIELD_TYPE_STRING}</li>
     * <li>{@link Cursor#FIELD_TYPE_BLOB}</li>
     * </ul>
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The field type.
     */
    virtual CARAPI GetType(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [out] */ Int32* type);

    /**
     * Gets the value of the field at the specified row and column index as a byte array.
     * <p>
     * The result is determined as follows:
     * <ul>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_NULL}, then the result
     * is <code>null</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_BLOB}, then the result
     * is the blob value.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_STRING}, then the result
     * is the array of bytes that make up the internal representation of the
     * string value.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_INTEGER} or
     * {@link Cursor#FIELD_TYPE_FLOAT}, then a {@link SQLiteException} is thrown.</li>
     * </ul>
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as a byte array.
     */
    virtual CARAPI GetBlob(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out, callee] */ ArrayOf<Byte>** blob);

    /**
     * Gets the value of the field at the specified row and column index as a string.
     * <p>
     * The result is determined as follows:
     * <ul>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_NULL}, then the result
     * is <code>null</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_STRING}, then the result
     * is the string value.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_INTEGER}, then the result
     * is a string representation of the integer in decimal, obtained by formatting the
     * value with the <code>printf</code> family of functions using
     * format specifier <code>%lld</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_FLOAT}, then the result
     * is a string representation of the floating-point value in decimal, obtained by
     * formatting the value with the <code>printf</code> family of functions using
     * format specifier <code>%g</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_BLOB}, then a
     * {@link SQLiteException} is thrown.</li>
     * </ul>
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as a string.
     */
    virtual CARAPI GetString(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    /**
     * Copies the text of the field at the specified row and column index into
     * a {@link CharArrayBuffer}.
     * <p>
     * The buffer is populated as follows:
     * <ul>
     * <li>If the buffer is too small for the value to be copied, then it is
     * automatically resized.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_NULL}, then the buffer
     * is set to an empty string.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_STRING}, then the buffer
     * is set to the contents of the string.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_INTEGER}, then the buffer
     * is set to a string representation of the integer in decimal, obtained by formatting the
     * value with the <code>printf</code> family of functions using
     * format specifier <code>%lld</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_FLOAT}, then the buffer is
     * set to a string representation of the floating-point value in decimal, obtained by
     * formatting the value with the <code>printf</code> family of functions using
     * format specifier <code>%g</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_BLOB}, then a
     * {@link SQLiteException} is thrown.</li>
     * </ul>
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @param buffer The {@link CharArrayBuffer} to hold the string.  It is automatically
     * resized if the requested string is larger than the buffer's current capacity.
      */
    virtual CARAPI CopyStringToBuffer(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [in] */ ICharArrayBuffer* buffer);

    /**
     * Gets the value of the field at the specified row and column index as a <code>long</code>.
     * <p>
     * The result is determined as follows:
     * <ul>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_NULL}, then the result
     * is <code>0L</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_STRING}, then the result
     * is the value obtained by parsing the string value with <code>strtoll</code>.
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_INTEGER}, then the result
     * is the <code>long</code> value.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_FLOAT}, then the result
     * is the floating-point value converted to a <code>long</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_BLOB}, then a
     * {@link SQLiteException} is thrown.</li>
     * </ul>
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as a <code>long</code>.
     */
    virtual CARAPI GetInt64(

        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Int64* value);

    /**
     * Gets the value of the field at the specified row and column index as a
     * <code>double</code>.
     * <p>
     * The result is determined as follows:
     * <ul>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_NULL}, then the result
     * is <code>0.0</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_STRING}, then the result
     * is the value obtained by parsing the string value with <code>strtod</code>.
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_INTEGER}, then the result
     * is the integer value converted to a <code>double</code>.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_FLOAT}, then the result
     * is the <code>double</code> value.</li>
     * <li>If the field is of type {@link Cursor#FIELD_TYPE_BLOB}, then a
     * {@link SQLiteException} is thrown.</li>
     * </ul>
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as a <code>double</code>.
     */
    virtual CARAPI GetDouble(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Double* value);

    /**
     * Gets the value of the field at the specified row and column index as a
     * <code>short</code>.
     * <p>
     * The result is determined by invoking {@link #getLong} and converting the
     * result to <code>short</code>.
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as a <code>short</code>.
     */
    virtual CARAPI GetInt16(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Int16* value);

    /**
     * Gets the value of the field at the specified row and column index as an
     * <code>int</code>.
     * <p>
     * The result is determined by invoking {@link #getLong} and converting the
     * result to <code>int</code>.
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as an <code>int</code>.
     */
    virtual CARAPI GetInt32(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Int32* value);

    /**
     * Gets the value of the field at the specified row and column index as a
     * <code>float</code>.
     * <p>
     * The result is determined by invoking {@link #getDouble} and converting the
     * result to <code>float</code>.
     * </p>
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return The value of the field as an <code>float</code>.
     */
    virtual CARAPI GetFloat(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Float* value);

    /**
     * Copies a byte array into the field at the specified row and column index.
     *
     * @param value The value to store.
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if successful.
     */
    virtual CARAPI PutBlob(
        /* [in] */ const ArrayOf<Byte>& value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* succeeded);

    /**
     * Copies a string into the field at the specified row and column index.
     *
     * @param value The value to store.
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if successful.
     */
    virtual CARAPI PutString(
        /* [in] */ const String& value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* succeeded);

    /**
     * Puts a long integer into the field at the specified row and column index.
     *
     * @param value The value to store.
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if successful.
     */
    virtual CARAPI PutInt64(
        /* [in] */ Int64 value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* succeeded);

     /**
     * Puts a double-precision floating point value into the field at the
     * specified row and column index.
     *
     * @param value The value to store.
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if successful.
     */
    virtual CARAPI PutDouble(
        /* [in] */ Double value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* succeeded);

    /**
     * Puts a null value into the field at the specified row and column index.
     *
     * @param row The zero-based row index.
     * @param column The zero-based column index.
     * @return True if successful.
     */
    virtual CARAPI PutNull(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* succeeded);

    static CARAPI NewFromParcel(
        /* [in] */ IParcel* p,
        /* [out] */ ICursorWindow** cw);

    virtual CARAPI  ReadFromParcel(
        /* [in] */ IParcel* source);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    virtual CARAPI ToString(
        /* [out] */ String* str);

protected:
    //@Override
    CARAPI_(void) OnAllReferencesReleased();

private:
    CARAPI_(Int64) NativeCreate(
        /* [in] */ const String& name,
        /* [in] */ Int32 cursorWindowSize);

    CARAPI_(Int64) NativeCreateFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI_(void) NativeDispose(
        /* [in] */ Int64 windowPtr);

    CARAPI NativeWriteToParcel(
        /* [in] */ Int64 windowPtr,
        /* [in] */ IParcel* parcel);

    CARAPI_(void) NativeClear(
        /* [in] */ Int64 windowPtr);

    CARAPI_(Int32) NativeGetNumRows(
        /* [in] */ Int64 windowPtr);

    CARAPI_(Boolean) NativeSetNumColumns(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 columnNum);

    CARAPI_(Boolean) NativeAllocRow(
        /* [in] */ Int64 windowPtr);

    CARAPI_(void) NativeFreeLastRow(
        /* [in] */ Int64 windowPtr);

    CARAPI_(Int32) NativeGetType(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 column);

    CARAPI NativeGetBlob(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ ArrayOf<Byte>** blob);

    CARAPI NativeGetString(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    CARAPI NativeGetInt64(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Int64* value);

    CARAPI NativeGetDouble(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Double* value);

    CARAPI NativeCopyStringToBuffer(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [in] */ ICharArrayBuffer* buffer);

    CARAPI_(Boolean) NativePutBlob(
        /* [in] */ Int64 windowPtr,
        /* [in] */ const ArrayOf<Byte>& value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

    CARAPI_(Boolean) NativePutString(
        /* [in] */ Int64 windowPtr,
        /* [in] */ const String& value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

    CARAPI_(Boolean) NativePutInt64(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int64 value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

    CARAPI_(Boolean) NativePutDouble(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Double value,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

    CARAPI_(Boolean) NativePutNull(
        /* [in] */ Int64 windowPtr,
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

    CARAPI_(String) NativeGetName(
        /* [in] */ Int64 windowPtr);

    virtual CARAPI Dispose();

    CARAPI_(void) RecordNewWindow(
        /* [in] */ Int32 pid,
        /* [in] */ Int64 window);

    CARAPI_(void) RecordClosingOfWindow(
        /* [in] */ Int64 window);

    CARAPI_(String) PrintStats();

    CARAPI_(Int32) GetCursorWindowSize();

public:
    /**
     * The native CursorWindow object pointer.  (FOR INTERNAL USE ONLY)
     * @hide
     */
    Int64 mWindowPtr;

private:
    // This static member will be evaluated when first used.
    static Int32 sCursorWindowSize;

    Int32 mStartPos;

    String mName;

    AutoPtr<ICloseGuard> mCloseGuard;

    static HashMap<Int64, Int32> sWindowToPidMap;

    static Object sWindowToPidMapLock;

    friend class CCursorWindow;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CURSORWINDOW_H__
