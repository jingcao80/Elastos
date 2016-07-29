#ifndef __CAR_BRIDGE_VECTOR_H
# define __CAR_BRIDGE_VECTOR_H

# ifndef __USE_MALLOC
#  define __USE_MALLOC
# endif
# include <elastos/utility/etl/Vector.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class Element, class Alloc = _ETL Allocator<Element>>
class Vector: public _ETL Vector<Element, Alloc> {
public:
    using typename _ETL Vector<Element, Alloc>::AllocatorType;

    explicit Vector(AllocatorType const &allocator = AllocatorType()): _ETL Vector<Element, Alloc>(allocator)
    {}

    using typename _ETL Vector<Element, Alloc>::SizeType;

    Vector(SizeType size, Element const &value, AllocatorType const &allocator = AllocatorType()):
        _ETL Vector<Element, Alloc>(size, value, allocator)
    {}

    explicit Vector(SizeType size): _ETL Vector<Element, Alloc>(size)
    {}

    Vector(Vector const &vector): _ETL Vector<Element, Alloc>(vector)
    {}

    template<class InputIterator>
    Vector(InputIterator first, InputIterator last, AllocatorType const &allocator = AllocatorType()):
        _ETL Vector<Element, Alloc>(first, last)
    {}

    SizeType Size() const noexcept
    {
        return GetSize();
    }

    using typename _ETL Vector<Element, Alloc>::Pointer;

    Pointer Data() noexcept
    {
        if (GetSize() == 0)
            return nullptr;

        return &GetFront();
    }

    using typename _ETL Vector<Element, Alloc>::ConstPointer;

    ConstPointer Data() const noexcept
    {
        if (GetSize() == 0)
            return nullptr;

        return &GetFront();
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
