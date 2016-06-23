
#include "elastos/droid/internal/widget/Smileys.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Internal::Widget::EIID_ISmileys;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const AutoPtr< ArrayOf<Int32> > Smileys::sIconIds = Smileys::InnerInitIconds();

CAR_INTERFACE_IMPL(Smileys, Object, ISmileys)

Smileys::Smileys()
{
}

ECode Smileys::GetSmileyResource(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    *result = (*sIconIds)[which];
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > Smileys::InnerInitIconds()
{
    const Int32 iconIds[] = {
        R::drawable::emo_im_happy,
        R::drawable::emo_im_sad,
        R::drawable::emo_im_winking,
        R::drawable::emo_im_tongue_sticking_out,
        R::drawable::emo_im_surprised,
        R::drawable::emo_im_kissing,
        R::drawable::emo_im_yelling,
        R::drawable::emo_im_cool,
        R::drawable::emo_im_money_mouth,
        R::drawable::emo_im_foot_in_mouth,
        R::drawable::emo_im_embarrassed,
        R::drawable::emo_im_angel,
        R::drawable::emo_im_undecided,
        R::drawable::emo_im_crying,
        R::drawable::emo_im_lips_are_sealed,
        R::drawable::emo_im_laughing,
        R::drawable::emo_im_wtf
    };

    AutoPtr< ArrayOf<Int32> > result = TO_ATTRS_ARRAYOF(iconIds);
    return result;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
