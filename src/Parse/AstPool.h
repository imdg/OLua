/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "CompileCommon.h"

namespace OL
{

class AstPool
{
public:
    static AstPool* GetInst();



    template<typename AstType, typename... ConstructParam>
    AstType* NewAst(CodeLineInfo Line, ConstructParam... Params)
    {
        AstType* Ret = new AstType(Params...);
        Ret->Line = Line;
        return Ret;
    };

    template<typename AstType>
    void DeleteAst(AstType* Node)
    {
        delete Node;
    };

    template<typename AstType, typename... ConstructParam>
    static AstType* New(CodeLineInfo Line, ConstructParam... Params)
    {
        return GetInst()->NewAst<AstType>(Line, Params...);
    }

    template<typename AstType>
    static void Delete(AstType* Node)
    {
        return GetInst()->DeleteAst(Node);
    }

private:
    static AstPool* Inst;
};


}