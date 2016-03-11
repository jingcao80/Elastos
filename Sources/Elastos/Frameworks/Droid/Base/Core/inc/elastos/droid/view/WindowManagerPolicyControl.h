#ifndef __ELASTOS_DROID_VIEW_WINDOWMANAGERPOLICYCONTROL_H__
#define __ELASTOS_DROID_VIEW_WINDOWMANAGERPOLICYCONTROL_H__

namespace Elastos {
namespace Droid {
namespace View {

class WindowManagerPolicyControl
{
public:
    class ImmersiveDefaultStyles {
        static const Int32 IMMERSIVE_FULL = 0;
        static const Int32 IMMERSIVE_STATUS = 1;
        static const Int32 IMMERSIVE_NAVIGATION = 2;
    };

private:
    class Filter {

        private:
            static const String ALL;
            static const String APPS;

            Auto
    };
};

} // namespace View
} // namespace Droid
} // namespace Elastos
#endif