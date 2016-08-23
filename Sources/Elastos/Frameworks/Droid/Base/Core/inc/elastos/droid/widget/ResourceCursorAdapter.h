
#ifndef __ELASTOS_DROID_WIDGET_RESOURCECURSORADAPTER_H__
#define __ELASTOS_DROID_WIDGET_RESOURCECURSORADAPTER_H__

#include "elastos/droid/widget/CursorAdapter.h"

using Elastos::Droid::View::ILayoutInflater;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An easy adapter that creates views defined in an XML file. You can specify
 * the XML file that defines the appearance of the views.
 */
class ECO_PUBLIC ResourceCursorAdapter
    : public CursorAdapter
    , public IResourceCursorAdapter
{
public:
    CAR_INTERFACE_DECL()

    ResourceCursorAdapter();

    /**
     * Constructor the enables auto-requery.
     *
     * @deprecated This option is discouraged, as it results in Cursor queries
     * being performed on the application's UI thread and thus can cause poor
     * responsiveness or even Application Not Responding errors.  As an alternative,
     * use {@link android.app.LoaderManager} with a {@link android.content.CursorLoader}.
     *
     * @param context The context where the ListView associated with this adapter is running
     * @param layout resource identifier of a layout file that defines the views
     *            for this list item.  Unless you override them later, this will
     *            define both the item views and the drop down views.
     * @Deprecated
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layout,
        /* [in] */ ICursor* c);

    /**
     * Constructor with default behavior as per
     * {@link CursorAdapter#CursorAdapter(Context, Cursor, boolean)}; it is recommended
     * you not use this, but instead {@link #ResourceCursorAdapter(Context, int, Cursor, int)}.
     * When using this constructor, {@link #FLAG_REGISTER_CONTENT_OBSERVER}
     * will always be set.
     *
     * @param context The context where the ListView associated with this adapter is running
     * @param layout resource identifier of a layout file that defines the views
     *            for this list item.  Unless you override them later, this will
     *            define both the item views and the drop down views.
     * @param c The cursor from which to get the data.
     * @param autoRequery If true the adapter will call requery() on the
     *                    cursor whenever it changes so the most recent
     *                    data is always displayed.  Using true here is discouraged.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layout,
        /* [in] */ ICursor* c,
        /* [in] */ Boolean autoRequery);

    /**
     * Standard constructor.
     *
     * @param context The context where the ListView associated with this adapter is running
     * @param layout Resource identifier of a layout file that defines the views
     *            for this list item.  Unless you override them later, this will
     *            define both the item views and the drop down views.
     * @param c The cursor from which to get the data.
     * @param flags Flags used to determine the behavior of the adapter,
     * as per {@link CursorAdapter#CursorAdapter(Context, Cursor, int)}.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layout,
        /* [in] */ ICursor* c,
        /* [in] */ Int32 flags);

    /**
     * Inflates view(s) from the specified XML file.
     *
     * @see android.widget.CursorAdapter#newView(android.content.Context,
     *      android.database.Cursor, ViewGroup)
     */
    virtual CARAPI NewView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI NewDropDownView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);
    /**
     * <p>Sets the layout resource of the item views.</p>
     *
     * @param layout the layout resources used to create item views
     */
    CARAPI SetViewResource(
        /* [in] */ Int32 layout);

    /**
     * <p>Sets the layout resource of the drop down views.</p>
     *
     * @param dropDownLayout the layout resources used to create drop down views
     */
    CARAPI SetDropDownViewResource(
        /* [in] */ Int32 dropDownLayout);

protected:
    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layout);

private:
    Int32 mLayout;
    Int32 mDropDownLayout;
    AutoPtr<ILayoutInflater> mInflater;
};

} // namespace Elastos
} // namespace Droid
} // namespace Widget

#endif //__ELASTOS_DROID_WIDGET_RESOURCECURSORADAPTER_H__
