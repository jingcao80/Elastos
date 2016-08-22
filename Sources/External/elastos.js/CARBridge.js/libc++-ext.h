#ifndef __CAR_BRIDGE_LIBCPP_EXT_H
# define __CAR_BRIDGE_LIBCPP_EXT_H

# include <memory>
# include <new>



namespace std {

template<>
struct default_delete<void> {
    void operator()(void *p) const
    {
        operator delete(p);
    }
};

template<>
struct default_delete<void const> {
    void operator()(void const *p) const
    {
        operator delete(const_cast<void *>(p));
    }
};

}

#endif
