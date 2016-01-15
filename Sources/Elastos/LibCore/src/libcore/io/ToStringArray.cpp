
#include "ToStringArray.h"

namespace Libcore {
namespace IO {
struct VectorCounter
{
    const Vector<String>& strings;
    VectorCounter(const Vector<String>& strings) : strings(strings) {}
    size_t operator()()
    {
        return strings.GetSize();
    }
};

struct VectorGetter
{
    const Vector<String>& strings;
    VectorGetter(const Vector<String>& strings) : strings(strings) {}
    const char* operator()(size_t i)
    {
        return strings[i].string();
    }
};

AutoPtr< ArrayOf<String> > ToStringArray(const Vector<String>& strings)
{
    VectorCounter counter(strings);
    VectorGetter getter(strings);
    return ToStringArray<VectorCounter, VectorGetter>(&counter, &getter);
}

struct ArrayCounter
{
    const char* const* strings;
    ArrayCounter(const char* const* strings) : strings(strings) {}
    size_t operator()()
    {
        size_t count = 0;
        while (strings[count] != NULL) {
            ++count;
        }
        return count;
    }
};

struct ArrayGetter
{
    const char* const* strings;
    ArrayGetter(const char* const* strings) : strings(strings) {}
    const char* operator()(size_t i)
    {
        return strings[i];
    }
};

AutoPtr< ArrayOf<String> > ToStringArray(const char* const* strings)
{
    ArrayCounter counter(strings);
    ArrayGetter getter(strings);
    return ToStringArray(&counter, &getter);
}
}
}
