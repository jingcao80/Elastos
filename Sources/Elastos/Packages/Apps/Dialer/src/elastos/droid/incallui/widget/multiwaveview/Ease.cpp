
#include "elastos/droid/incallui/widget/multiwaveview/Ease.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

//=====================================================================
//               Ease::Cubic
//=====================================================================
AutoPtr<Ease::Cubic::TimeInterpolatorIn> Ease::Cubic::sEaseIn;
AutoPtr<Ease::Cubic::TimeInterpolatorOut> Ease::Cubic::sEaseOut;
AutoPtr<Ease::Cubic::TimeInterpolatorInOut> Ease::Cubic::sEaseInOut;


//=====================================================================
//               Ease::Quad
//=====================================================================
AutoPtr<Ease::Quad::TimeInterpolatorIn> Ease::Quad::sEaseIn;
AutoPtr<Ease::Quad::TimeInterpolatorOut> Ease::Quad::sEaseOut;
AutoPtr<Ease::Quad::TimeInterpolatorInOut> Ease::Quad::sEaseInOut;


//=====================================================================
//               Ease::Quart
//=====================================================================
AutoPtr<Ease::Quart::TimeInterpolatorIn> Ease::Quart::sEaseIn;
AutoPtr<Ease::Quart::TimeInterpolatorOut> Ease::Quart::sEaseOut;
AutoPtr<Ease::Quart::TimeInterpolatorInOut> Ease::Quart::sEaseInOut;

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
