
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_DATETIMESUGGESTIONLISTADAPTER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_DATETIMESUGGESTIONLISTADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
// TODO #include "elastos/droid/widget/ArrayAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
// TODO using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ITextView;

// import org.chromium.content.R;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Date/time suggestion adapter for the suggestion dialog.
 */
class DateTimeSuggestionListAdapter : public Object// TODO : public ArrayAdapter
{
public:
    DateTimeSuggestionListAdapter(
        /* [in] */ IContext* context,
        /* [in] */ IList* objects);

    //@Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** outView);

    //@Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

private:
    const AutoPtr<IContext> mContext;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_DATETIMESUGGESTIONLISTADAPTER_H__
