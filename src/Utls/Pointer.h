/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include <memory>
#include "OLHash.h"
namespace OL
{


template <typename Ty>
class SPtr
{
public:
    inline Ty* Get() const { return InnerPtr.get(); };
    SPtr() {}

    SPtr(Ty* Ptr): InnerPtr(Ptr)
    {}

    template<typename Ty2>
    SPtr(Ty2* Ptr): InnerPtr(Ptr)
    {}

    template<typename Ty2>
    SPtr(const SPtr<Ty2>& Ptr): InnerPtr(std::static_pointer_cast<Ty>(Ptr.InnerPtr))
    {}

    SPtr(const SPtr<Ty>& Src) : InnerPtr(Src.InnerPtr)
    {
    }

    SPtr(const SPtr<Ty>&& Src) : InnerPtr(std::move(Src.InnerPtr))
    {
    }

    SPtr<Ty>& operator =(const SPtr<Ty>& Src)
    {
        InnerPtr = Src.InnerPtr;
        return *this;
    }

    SPtr<Ty>& operator =(const SPtr<Ty>&& Src)
    {
        InnerPtr = std::move(Src.InnerPtr);
        return *this;
    }

    SPtr<Ty>& operator =(const Ty* Src)
    {
        InnerPtr = std::shared_ptr<Ty>(Src);
        return *this;
    }

    SPtr<Ty>& operator =(Ty* Src)
    {
        InnerPtr = std::shared_ptr<Ty>(Src);
        return *this;
    }

    template<typename Ty2> SPtr<Ty>& operator =(const SPtr<Ty2>& Src)
    {
        InnerPtr = std::static_pointer_cast<Ty2>(Src.InnerPtr);
        return *this;
    }

    template<typename Ty2> SPtr<Ty>& operator =(const SPtr<Ty2>&& Src)
    {
        InnerPtr = std::static_pointer_cast<Ty2>(std::move(Src.InnerPtr));
        return *this;
    }

    template<typename Ty2> SPtr<Ty>& operator =(Ty2* Src)
    {
        InnerPtr = std::shared_ptr<Ty>(static_cast<Ty*>(Src));
        return *this;
    }

    template<typename Ty2> SPtr<Ty>& operator =(const Ty2* Src)
    {
        InnerPtr = std::shared_ptr<Ty>(static_cast<Ty*>(Src));
        return *this;
    }

    Ty* operator->() const
    {
        return InnerPtr.get();
    }

    Ty& operator* () const
    {
        return *(InnerPtr.get());
    }

    operator bool() const 
    {
        return InnerPtr.get() != nullptr;
    }


    template<typename Ty2> bool PtrIs() const ;
    // {
    //     static_assert(TIsRTTI<T>::Value && TIsRTTI<T2>::Value, "Cannot call PtrIs<> for non-RTTI types ")
    //     return InnerPtr->Is<T2>();
    // }
    template<typename Ty2> SPtr<Ty2> PtrAs() const ;
    // {
    //     static_assert(TIsRTTI<T>::Value && TIsRTTI<T2>::Value, "Cannot call PtrAs<> for non-RTTI types ");
    //     SPtr<T2> Ret;
        
    //     Ret.InnerPtr = std::static_pointer_cast<T2>(InnerPtr);
    //     return Ret;
    // }

    std::shared_ptr<Ty> InnerPtr;
};

template <typename Ty>
SPtr<Ty> MakeShared(Ty* Ptr)
{
    SPtr<Ty> Ret;
    Ret.InnerPtr = std::make_shared<Ty>(Ptr);
    return Ret;

}




template <typename Ty>
class WPtr
{
public:
    inline Ty* Get() const { return InnerPtr.lock().get(); };
    WPtr() {}

    WPtr(const WPtr<Ty>& Src) : InnerPtr(Src.InnerPtr)
    {
        
    }

    WPtr(const WPtr<Ty>&& Src) : InnerPtr(std::move(Src.InnerPtr))
    {
    }

    WPtr(const SPtr<Ty>& Src) : InnerPtr(Src.InnerPtr)
    {

    }

    template<typename Ty2>
    WPtr(const SPtr<Ty2>& Src) : InnerPtr(std::static_pointer_cast<Ty>(Src.InnerPtr))
    {}

    WPtr<Ty>& operator =(const WPtr<Ty>& Src)
    {
        InnerPtr = Src.InnerPtr;
        return *this;
    }

    WPtr<Ty>& operator =(const WPtr<Ty>&& Src)
    {
        InnerPtr = std::move(Src.InnerPtr);
        return *this;
    }

    WPtr<Ty>& operator =(const SPtr<Ty>& Src)
    {
        InnerPtr = Src.InnerPtr;
        return *this;
    }


    WPtr<Ty>& operator =(const Ty* Src)
    {
        InnerPtr = std::weak_ptr<Ty>(Src);
        return *this;
    }

    WPtr<Ty>& operator =(Ty* Src)
    {
        InnerPtr = std::weak_ptr<Ty>(Src);
        return *this;
    }

    WPtr<Ty>& operator = (decltype(nullptr))
    {
        Reset();
        return *this;
    }

    template<typename Ty2> WPtr<Ty>& operator =(const WPtr<Ty2>& Src)
    {
        InnerPtr = Src.InnerPtr;
        return *this;
    }

    template<typename Ty2> WPtr<Ty>& operator =(const WPtr<Ty2>&& Src)
    {
        InnerPtr = std::move(Src.InnerPtr);
        return *this;
    }

    template<typename Ty2> WPtr<Ty>& operator =(const SPtr<Ty2>& Src)
    {
        InnerPtr = Src.InnerPtr;
        return *this;
    }

    template<typename Ty2> WPtr<Ty>& operator =(Ty2* Src)
    {
        InnerPtr = std::weak_ptr<Ty>(static_cast<Ty*>(Src));
        return *this;
    }

    template<typename Ty2> WPtr<Ty>& operator =(const Ty2* Src)
    {
        InnerPtr = std::weak_ptr<Ty>(static_cast<Ty*>(Src));
        return *this;
    }

    Ty* operator->() const
    {
        return InnerPtr.lock().get();
    }

    //Ty operator* () const
    //{
    //    return *(InnerPtr.lock());
    //}

    operator bool() const 
    {
        return !InnerPtr.expired();
    }

    SPtr<Ty> Lock() const
    {
        SPtr<Ty> Ret;
        Ret.InnerPtr = InnerPtr.lock();
        return Ret;
    }

    void Reset() { InnerPtr.reset();}

    std::weak_ptr<Ty> InnerPtr;
};

template <typename Ty>
bool operator==(const SPtr<Ty>& Op1, std::nullptr_t) 
{
    return Op1.Get() == nullptr;
}

template <typename Ty>
bool operator==(const WPtr<Ty>& Op1, std::nullptr_t) 
{
    return Op1.Get() == nullptr;
}

template <typename Ty1, typename Ty2>
bool operator==(const SPtr<Ty1>& Op1, const SPtr<Ty2>& Op2) 
{
    return Op1.Get() == Op2.Get();
}

template <typename Ty1, typename Ty2>
bool operator==(const WPtr<Ty1>& Op1, const WPtr<Ty2>& Op2) 
{
    return Op1.Get() == Op2.Get();
}


template <typename Ty>
bool operator!=(const SPtr<Ty>& Op1, std::nullptr_t) 
{
    return Op1.Get() != nullptr;
}

template <typename Ty>
bool operator!=(const WPtr<Ty>& Op1, std::nullptr_t) 
{
    return Op1.Get() != nullptr;
}

template <typename Ty1, typename Ty2>
bool operator!=(const SPtr<Ty1>& Op1, const SPtr<Ty2>& Op2) 
{
    return Op1.Get()!= Op2.Get();
}

template <typename Ty1, typename Ty2>
bool operator!=(const WPtr<Ty1>& Op1, const WPtr<Ty2>& Op2) 
{
    return Op1.Get() != Op2.Get();
}

template<typename Ty>
class SThisOwner : public std::enable_shared_from_this<Ty>
{
public:
    template<typename Ty2>
    SPtr<Ty2> ThisPtr()
    {
        SPtr<Ty2> Ret;
        Ret.InnerPtr = std::static_pointer_cast<Ty2>(this->shared_from_this());
        return Ret;
    };


};

template<typename Ty>
struct RealType
{
    using Type = Ty;
};

template<typename Ty>
struct RealType<Ty*>
{
    using Type = Ty;
};

#define SThis (ThisPtr<RealType<decltype(this)>::Type>())


template < typename Ty >
struct THasher<SPtr<Ty>>
{
    size_t operator()(const SPtr<Ty> _Keyval) const noexcept {
        return std::hash<Ty*>{}(_Keyval.Get());
    };
    typedef THasher<SPtr<Ty>> Type;
};

}