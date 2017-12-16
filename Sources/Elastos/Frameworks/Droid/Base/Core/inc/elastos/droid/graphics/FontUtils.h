
#include <minikin/FontFamily.h>
#include <memory>

namespace Elastos {
namespace Droid {
namespace Graphics {

struct FontFamilyWrapper
{
    FontFamilyWrapper(std::shared_ptr<minikin::FontFamily>&& family) : family(family) {}
    std::shared_ptr<minikin::FontFamily> family;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
