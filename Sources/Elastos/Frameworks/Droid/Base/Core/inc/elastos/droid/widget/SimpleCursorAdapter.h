
#ifndef __ELASTOS_DROID_WIDGET_SIMPLECURSORADAPTER_H__
#define __ELASTOS_DROID_WIDGET_SIMPLECURSORADAPTER_H__

#include "elastos/droid/widget/ResourceCursorAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An easy adapter to map columns from a cursor to TextViews or ImageViews
 * defined in an XML file. You can specify which columns you want, which
 * views you want to display the columns, and the XML file that defines
 * the appearance of these views.
 *
 * Binding occurs in two phases. First, if a
 * {@link android.widget.SimpleCursorAdapter.ViewBinder} is available,
 * {@link ViewBinder#setViewValue(android.view.View, android.database.Cursor, Int32)}
 * is invoked. If the returned value is true, binding has occured. If the
 * returned value is false and the view to bind is a TextView,
 * {@link #setViewText(TextView, String)} is invoked. If the returned value
 * is false and the view to bind is an ImageView,
 * {@link #setViewImage(ImageView, String)} is invoked. If no appropriate
 * binding can be found, an {@link IllegalStateException} is thrown.
 *
 * If this adapter is used with filtering, for instance in an
 * {@link android.widget.AutoCompleteTextView}, you can use the
 * {@link android.widget.SimpleCursorAdapter.CursorToStringConverter} and the
 * {@link android.widget.FilterQueryProvider} interfaces
 * to get control over the filtering process. You can refer to
 * {@link #convertToString(android.database.Cursor)} and
 * {@link #runQueryOnBackgroundThread(CharSequence)} for more information.
 */
class ECO_PUBLIC SimpleCursorAdapter
    : public ResourceCursorAdapter
    , public ISimpleCursorAdapter
{
public:
    CAR_INTERFACE_DECL()

    SimpleCursorAdapter();

    /**
     * Constructor the enables auto-requery.
     *
     * @deprecated This option is discouraged, as it results in Cursor queries
     * being performed on the application's UI thread and thus can cause poor
     * responsiveness or even Application Not Responding errors.  As an alternative,
     * use {@link android.app.LoaderManager} with a {@link android.content.CursorLoader}.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layout,
        /* [in] */ ICursor* c,
        /* [in] */ ArrayOf<String>* from,
        /* [in] */ ArrayOf<Int32>* to);

    /**
     * Standard constructor.
     *
     * @param context The context where the ListView associated with this
     *            SimpleListItemFactory is running
     * @param layout resource identifier of a layout file that defines the views
     *            for this list item. The layout file should include at least
     *            those named views defined in "to"
     * @param c The database cursor.  Can be null if the cursor is not available yet.
     * @param from A list of column names representing the data to bind to the UI.  Can be null
     *            if the cursor is not available yet.
     * @param to The views that should display column in the "from" parameter.
     *            These should all be TextViews. The first N views in this list
     *            are given the values of the first N columns in the from
     *            parameter.  Can be null if the cursor is not available yet.
     * @param flags Flags used to determine the behavior of the adapter,
     * as per {@link CursorAdapter#CursorAdapter(Context, Cursor, Int32)}.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layout,
        /* [in] */ ICursor* c,
        /* [in] */ ArrayOf<String>* from,
        /* [in] */ ArrayOf<Int32>* to,
        /* [in] */ Int32 flags);

    /**
     * Binds all of the field names passed into the "to" parameter of the
     * constructor with their corresponding cursor columns as specified in the
     * "from" parameter.
     *
     * Binding occurs in two phases. First, if a
     * {@link android.widget.SimpleCursorAdapter.ViewBinder} is available,
     * {@link ViewBinder#setViewValue(android.view.View, android.database.Cursor, Int32)}
     * is invoked. If the returned value is true, binding has occured. If the
     * returned value is false and the view to bind is a TextView,
     * {@link #setViewText(TextView, String)} is invoked. If the returned value is
     * false and the view to bind is an ImageView,
     * {@link #setViewImage(ImageView, String)} is invoked. If no appropriate
     * binding can be found, an {@link IllegalStateException} is thrown.
     *
     * @throws IllegalStateException if binding cannot occur
     *
     * @see android.widget.CursorAdapter#bindView(android.view.View,
     *      android.content.Context, android.database.Cursor)
     * @see #getViewBinder()
     * @see #setViewBinder(android.widget.SimpleCursorAdapter.ViewBinder)
     * @see #setViewImage(ImageView, String)
     * @see #setViewText(TextView, String)
     */
    virtual CARAPI BindView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    /**
     * Returns the {@link ViewBinder} used to bind data to views.
     *
     * @return a ViewBinder or null if the binder does not exist
     *
     * @see #bindView(android.view.View, android.content.Context, android.database.Cursor)
     * @see #setViewBinder(android.widget.SimpleCursorAdapter.ViewBinder)
     */
    virtual CARAPI GetViewBinder(
        /* [out] */ ISimpleCursorAdapterViewBinder** viewBinder);

    /**
     * Sets the binder used to bind data to views.
     *
     * @param viewBinder the binder used to bind data to views, can be null to
     *        remove the existing binder
     *
     * @see #bindView(android.view.View, android.content.Context, android.database.Cursor)
     * @see #getViewBinder()
     */
    virtual CARAPI SetViewBinder(
        /* [in] */ ISimpleCursorAdapterViewBinder* viewBinder);

    /**
     * Called by bindView() to set the image for an ImageView but only if
     * there is no existing ViewBinder or if the existing ViewBinder cannot
     * handle binding to an ImageView.
     *
     * By default, the value will be treated as an image resource. If the
     * value cannot be used as an image resource, the value is used as an
     * image Uri.
     *
     * Intended to be overridden by Adapters that need to filter strings
     * retrieved from the database.
     *
     * @param v ImageView to receive an image
     * @param value the value retrieved from the cursor
     */
    virtual CARAPI SetViewImage(
        /* [in] */ IImageView* v,
        /* [in] */ const String& value);

    /**
     * Called by bindView() to set the text for a TextView but only if
     * there is no existing ViewBinder or if the existing ViewBinder cannot
     * handle binding to a TextView.
     *
     * Intended to be overridden by Adapters that need to filter strings
     * retrieved from the database.
     *
     * @param v TextView to receive text
     * @param text the text to be set for the TextView
     */
    virtual CARAPI SetViewText(
        /* [in] */ ITextView* v,
        /* [in] */ const String& text);

    /**
     * Return the index of the column used to get a String representation
     * of the Cursor.
     *
     * @return a valid index in the current Cursor or -1
     *
     * @see android.widget.CursorAdapter#convertToString(android.database.Cursor)
     * @see #setStringConversionColumn(Int32)
     * @see #setCursorToStringConverter(android.widget.SimpleCursorAdapter.CursorToStringConverter)
     * @see #getCursorToStringConverter()
     */
    virtual CARAPI GetStringConversionColumn(
        /* [in] */ Int32* column);

    /**
     * Defines the index of the column in the Cursor used to get a String
     * representation of that Cursor. The column is used to convert the
     * Cursor to a String only when the current CursorToStringConverter
     * is null.
     *
     * @param stringConversionColumn a valid index in the current Cursor or -1 to use the default
     *        conversion mechanism
     *
     * @see android.widget.CursorAdapter#convertToString(android.database.Cursor)
     * @see #getStringConversionColumn()
     * @see #setCursorToStringConverter(android.widget.SimpleCursorAdapter.CursorToStringConverter)
     * @see #getCursorToStringConverter()
     */
    virtual CARAPI SetStringConversionColumn(
        /* [in] */ Int32 stringConversionColumn);

    /**
     * Returns the converter used to convert the filtering Cursor
     * into a String.
     *
     * @return null if the converter does not exist or an instance of
     *         {@link android.widget.SimpleCursorAdapter.CursorToStringConverter}
     *
     * @see #setCursorToStringConverter(android.widget.SimpleCursorAdapter.CursorToStringConverter)
     * @see #getStringConversionColumn()
     * @see #setStringConversionColumn(Int32)
     * @see android.widget.CursorAdapter#convertToString(android.database.Cursor)
     */
    virtual CARAPI GetCursorToStringConverter(
        /* [out] */ ICursorToStringConverter** converter);

    /**
     * Sets the converter  used to convert the filtering Cursor
     * into a String.
     *
     * @param cursorToStringConverter the Cursor to String converter, or
     *        null to remove the converter
     *
     * @see #setCursorToStringConverter(android.widget.SimpleCursorAdapter.CursorToStringConverter)
     * @see #getStringConversionColumn()
     * @see #setStringConversionColumn(Int32)
     * @see android.widget.CursorAdapter#convertToString(android.database.Cursor)
     */
    virtual CARAPI SetCursorToStringConverter(
        /* [in] */ ICursorToStringConverter* cursorToStringConverter);

    /**
     * Returns a CharSequence representation of the specified Cursor as defined
     * by the current CursorToStringConverter. If no CursorToStringConverter
     * has been set, the String conversion column is used instead. If the
     * conversion column is -1, the returned String is empty if the cursor
     * is null or Cursor.toString().
     *
     * @param cursor the Cursor to convert to a CharSequence
     *
     * @return a non-null CharSequence representing the cursor
     */
    virtual CARAPI ConvertToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ ICharSequence** str);

    virtual CARAPI SwapCursor(
        /* [in] */ ICursor* c,
        /* [out] */ ICursor** cursor);

    /**
     * Change the cursor and change the column-to-view mappings at the same time.
     *
     * @param c The database cursor.  Can be null if the cursor is not available yet.
     * @param from A list of column names representing the data to bind to the UI.  Can be null
     *            if the cursor is not available yet.
     * @param to The views that should display column in the "from" parameter.
     *            These should all be TextViews. The first N views in this list
     *            are given the values of the first N columns in the from
     *            parameter.  Can be null if the cursor is not available yet.
     */
    CARAPI ChangeCursorAndColumns(
        /* [in] */ ICursor* c,
        /* [in] */ ArrayOf<String>* from,
        /* [in] */ ArrayOf<Int32>* to);

protected:
    CARAPI InitImpl(
        /* [in] */ ICursor* c,
        /* [in] */ ArrayOf<String>* from,
        /* [in] */ ArrayOf<Int32>* to);
private:

    /**
     * Create a map from an array of strings to an array of column-id integers in cursor c.
     * If c is null, the array will be discarded.
     *
     * @param c the cursor to find the columns from
     * @param from the Strings naming the columns of interest
     */
    ECO_LOCAL CARAPI FindColumns(
        /* [in] */ ICursor* c,
        /* [in] */ ArrayOf<String>* from);

protected:
    /**
     * A list of View ids representing the views to which the data must be bound.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<ArrayOf<Int32> > mTo;

private:
    /**
     * A list of columns containing the data to bind to the UI.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<ArrayOf<Int32> > mFrom;

    Int32 mStringConversionColumn;// = -1;
    AutoPtr<ICursorToStringConverter> mCursorToStringConverter;
    AutoPtr<ISimpleCursorAdapterViewBinder> mViewBinder;

    AutoPtr<ArrayOf<String> > mOriginalFrom;
};

} // namespace Elastos
} // namespace Droid
} // namespace Widget

#endif // __ELASTOS_DROID_WIDGET_SIMPLECURSORADAPTER_H__
