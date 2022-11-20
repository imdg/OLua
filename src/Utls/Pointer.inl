
/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Pointer.h"
namespace OL
{
template<typename Ty>
template<typename Ty2> 
bool SPtr<Ty>::PtrIs() const 
{
    static_assert(TIsRTTI<Ty>::Value && TIsRTTI<Ty2>::Value, "Cannot call PtrIs<> for non-RTTI types ");
    return InnerPtr-> template Is<Ty2>();
}

template<typename Ty>
template<typename Ty2> 
SPtr<Ty2> SPtr<Ty>::PtrAs() const 
{
    static_assert(TIsRTTI<Ty>::Value && TIsRTTI<Ty2>::Value, "Cannot call PtrAs<> for non-RTTI types ");
    SPtr<Ty2> Ret;
    
    Ret.InnerPtr = std::static_pointer_cast<Ty2>(InnerPtr);
    return Ret;
}
}