#ifndef __TYPE_TRAITS_H__
# define __TYPE_TRAITS_H__



template<class T>
struct RemoveConst {
    typedef T Type;
};

template<class T>
struct RemoveConst<T const> {
    typedef T Type;
};


template<class T>
struct RemoveReference {
    typedef T Type;
};

template<class T>
struct RemoveReference<T &> {
    typedef T Type;
};

# if __cplusplus >= 201103L
template<class T>
struct RemoveReference<T &&> {
    typedef T Type;
};

# endif
#endif // __TYPE_TRAITS_H__
