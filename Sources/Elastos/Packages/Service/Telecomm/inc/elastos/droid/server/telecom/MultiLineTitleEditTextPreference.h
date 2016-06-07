
#ifndef __ELASTOS_DROID_SERVER_TELECOM_MULTILINETITLEEDITTEXTPREFERENCE_H__
#define __ELASTOS_DROID_SERVER_TELECOM_MULTILINETITLEEDITTEXTPREFERENCE_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/preference/EditTextPreference.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::EditTextPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Ultra-simple subclass of EditTextPreference that allows the "title" to wrap
 * onto multiple lines.
 *
 * (By default, the title of an EditTextPreference is singleLine="true"; see
 * preference_holo.xml under frameworks/base.  But in the "Respond via SMS"
 * settings UI we want titles to be multi-line, since the customized messages
 * might be fairly long, and should be able to wrap.)
 *
 * TODO: This is pretty cumbersome; it would be nicer for the framework to
 * either allow modifying the title's attributes in XML, or at least provide
 * some way from Java (given an EditTextPreference) to reach inside and get a
 * handle to the "title" TextView.
 *
 * TODO: Also, it would reduce clutter if this could be an inner class in
 * RespondViaSmsManager.java, but then there would be no way to reference the
 * class from XML.  That's because
 *    <com.android.server.telecom.MultiLineTitleEditTextPreference ... />
 * isn't valid XML syntax due to the "$" character.  And Preference
 * elements don't have a "class" attribute, so you can't do something like
 * <view class="com.android.server.telecom.Foo$Bar"> as you can with regular views.
 */
class MultiLineTitleEditTextPreference
    : public EditTextPreference
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    // setSingleLine(false) on it.
    // @Override
    CARAPI OnBindView(
        /* [in] */ IView* view);
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_MULTILINETITLEEDITTEXTPREFERENCE_H__
