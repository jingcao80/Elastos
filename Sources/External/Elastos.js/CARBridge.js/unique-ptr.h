#ifndef __CAR_BRIDGE_UNIQUE_PTR_H
# define __CAR_BRIDGE_UNIQUE_PTR_H

# include "macros.h"

# include "type-traits.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class T>
struct Delete {
    typedef void (*Type)(T *p);

    static void DefaultValue(T *p)
    {
        delete p;
    }
};

template<class T>
struct Delete<T []> {
    typedef void (*Type)(T a[]);

    static void DefaultValue(T a[])
    {
        delete [] a;
    }
};

template<>
struct Delete<void> {
    typedef void (*Type)(void *p);

    static void DefaultValue(void *p) noexcept
    {
        operator delete(p);
    }
};

template<>
struct Delete<void const> {
    typedef void (*Type)(void const *p);

    static void DefaultValue(void const *p) noexcept
    {
        operator delete((void *)p);
    }
};

template<class T, typename Delete<T>::Type const delete_ = &Delete<T>::DefaultValue>
class UniquePtr {
private:
    typedef typename RemoveExtent<T>::Type Type;

    typedef typename AddLeftValueReference<Type>::Type LeftValueReference;

public:
    UniquePtr(Type *p = nullptr) noexcept
    {
        _p = p;
    }

    template<class _T>
    UniquePtr(_T *p = nullptr) noexcept
    {
        _p = (Type *)p;
    }

    UniquePtr(UniquePtr &uniquePtr) noexcept
    {
        MoveConstruct(uniquePtr);
    }

    UniquePtr(UniquePtr &&uniquePtr) noexcept: UniquePtr(uniquePtr)
    {}

    template<class _T>
    UniquePtr(UniquePtr<_T, (typename Delete<_T>::Type)delete_> &uniquePtr) noexcept
    {
        MoveConstruct(uniquePtr);
    }

    template<class _T>
    UniquePtr(UniquePtr<_T, (typename Delete<_T>::Type)delete_> &&uniquePtr) noexcept: UniquePtr(uniquePtr)
    {}

    ~UniquePtr()
    {
        delete_(_p);
    }

    UniquePtr &operator=(Type *p)
    {
        if (_p != p) {
            delete_(_p);

            _p = p;
        }

        return *this;
    }

    template<class _T>
    UniquePtr &operator=(_T *p)
    {
        if (_p != (Type *)p) {
            delete_(_p);

            _p = (Type *)p;
        }

        return *this;
    }

    UniquePtr &operator=(UniquePtr &uniquePtr)
    {
        if (_p != uniquePtr._p) {
            delete_(_p);

            MoveConstruct(uniquePtr);
        }

        return *this;
    }

    UniquePtr &operator=(UniquePtr &&uniquePtr)
    {
        *this = uniquePtr;
    }

    template<class _T>
    UniquePtr &operator=(UniquePtr<_T, (typename Delete<_T>::Type)delete_> &uniquePtr)
    {
        if (_p != (Type *)uniquePtr._p) {
            delete_(_p);

            MoveConstruct(uniquePtr);
        }

        return *this;
    }

    template<class _T>
    UniquePtr &operator=(UniquePtr<_T, (typename Delete<_T>::Type)delete_> &&uniquePtr)
    {
        *this = uniquePtr;
    }

    operator bool() const noexcept
    {
        return !!_p;
    }

    operator Type *() const noexcept
    {
        return _p;
    }

    template<class _T>
    explicit operator _T *() const noexcept
    {
        return (_T *)_p;
    }

    LeftValueReference operator*() const noexcept
    {
        return *_p;
    }

    Type *operator->() const noexcept
    {
        return _p;
    }

    LeftValueReference operator[](int index) const noexcept
    {
        return _p[index];
    }

    bool operator==(Type const *p) const noexcept
    {
        return _p == p ? true : false;
    }

    template<class _T>
    bool operator==(_T const *p) const noexcept
    {
        return _p == (Type const *)p ? true : false;
    }

    bool operator!=(Type const *p) const noexcept
    {
        return _p != p ? true : false;
    }

    template<class _T>
    bool operator!=(_T const *p) const noexcept
    {
        return _p != (Type const *)p ? true : false;
    }

    bool operator<(Type const *p) const noexcept
    {
        return _p < p ? true : false;
    }

    template<class _T>
    bool operator<(_T const *p) const noexcept
    {
        return _p < (Type const *)p ? true : false;
    }

    bool operator<=(Type const *p) const noexcept
    {
        return _p <= p ? true : false;
    }

    template<class _T>
    bool operator<=(_T const *p) const noexcept
    {
        return _p <= (Type const *)p ? true : false;
    }

    bool operator>=(Type const *p) const noexcept
    {
        return _p >= p ? true : false;
    }

    template<class _T>
    bool operator>=(_T const *p) const noexcept
    {
        return _p >= (Type const *)p ? true : false;
    }

    bool operator>(Type const *p) const noexcept
    {
        return _p > p ? true : false;
    }

    template<class _T>
    bool operator>(_T const *p) const noexcept
    {
        return _p > (Type const *)p ? true : false;
    }

    bool operator==(UniquePtr const &uniquePtr) const noexcept
    {
        return _p == uniquePtr._p ? true : false;
    }

    template<class _T>
    bool operator==(UniquePtr<_T, (typename Delete<_T>::Type)delete_> const &uniquePtr) const noexcept
    {
        return _p == (Type const *)uniquePtr._p ? true : false;
    }

    bool operator!=(UniquePtr const &uniquePtr) const noexcept
    {
        return _p != uniquePtr._p ? true : false;
    }

    template<class _T>
    bool operator!=(UniquePtr<_T, (typename Delete<_T>::Type)delete_> const &uniquePtr) const noexcept
    {
        return _p != (Type const *)uniquePtr._p ? true : false;
    }

    bool operator<(UniquePtr const &uniquePtr) const noexcept
    {
        return _p < uniquePtr._p ? true : false;
    }

    template<class _T>
    bool operator<(UniquePtr<_T, (typename Delete<_T>::Type)delete_> const &uniquePtr) const noexcept
    {
        return _p < (Type const *)uniquePtr._p ? true : false;
    }

    bool operator<=(UniquePtr const &uniquePtr) const noexcept
    {
        return _p <= uniquePtr._p ? true : false;
    }

    template<class _T>
    bool operator<=(UniquePtr<_T, (typename Delete<_T>::Type)delete_> const &uniquePtr) const noexcept
    {
        return _p <= (Type const *)uniquePtr._p ? true : false;
    }

    bool operator>=(UniquePtr const &uniquePtr) const noexcept
    {
        return _p >= uniquePtr._p ? true : false;
    }

    template<class _T>
    bool operator>=(UniquePtr<_T, (typename Delete<_T>::Type)delete_> const &uniquePtr) const noexcept
    {
        return _p >= (Type const *)uniquePtr._p ? true : false;
    }

    bool operator>(UniquePtr const &uniquePtr) const noexcept
    {
        return _p > uniquePtr._p ? true : false;
    }

    template<class _T>
    bool operator>(UniquePtr<_T, (typename Delete<_T>::Type)delete_> const &uniquePtr) const noexcept
    {
        return _p > (Type const *)uniquePtr._p ? true : false;
    }

    Type *Release(void) noexcept
    {
        Type *p;

        p = _p;

        _p = nullptr;

        return p;
    }

private:
    Type *_p;

    void MoveConstruct(UniquePtr &uniquePtr) noexcept
    {
        _p = uniquePtr._p, uniquePtr._p = nullptr;
    }

    template<class _T>
    void MoveConstruct(UniquePtr<_T, (typename Delete<_T>::Type)delete_> &uniquePtr) noexcept
    {
        _p = (Type *)uniquePtr._p, uniquePtr._p = nullptr;
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
