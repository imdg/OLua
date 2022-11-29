/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "AstPool.h"
#include "Common.h"

namespace OL
{

AstPool * AstPool::Inst = nullptr;

AstPool* AstPool::GetInst()
{
    if(Inst == nullptr )
        Inst = new AstPool();
    return Inst;
}



}