/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"

namespace OL
{

// A helper to manage stack during visitor running
// The stack looks like this
//  (base) x  x  x  x  x  x  x
//         ~~~~~~~  ^   ~~~~~~~
//           |      |      |
//           |   Current   |
//        Parent          Children returned
//
//  A Node pushed a place holder when its visiting begin, turnning the active node to itself
//  When EndVisit, it has all return values from its children
//  Usually it uses these values to do its logic, update the "Current" value, 
//        then move the "Current" cursor to the parent, leaving itself as a return value
//  (base) x  x  x  x  (x  x  x)
//         ~~~~~ ^  ^   ~~~~~~~
//               |  |    Popped
//      New Current |
//             Previous "Current", left as return value
//  Use VisitStackPop to work this way
//  
//  Other usage
//  VisitStackConsume: Only pop child return values, does not move the "Current" cursor to the parent
//         This is used to do some local analysis
//  VisitStackClearPop: This pops itself and its children, leaving no return values
template <typename T>
class VisitStackHelper;




template <typename T>
class VisitStackItBase
{
public:
    VisitStackHelper<T>& Owner;
    int                 HoldingIndex;
    VisitStackItBase(VisitStackHelper<T>& InOwner)
        : Owner(InOwner)
    {
       
    };

    inline int GetElemCount()
    {
        return Owner.ElementStack.Count() - 1 - HoldingIndex;
    };
    
    inline T& GetElem(int Index)
    {
        return Owner.ElementStack[HoldingIndex + 1 + Index];
    };

    inline T& GetHoldingElem()
    {
        return Owner.ElementStack[HoldingIndex];
    };

    virtual ~VisitStackItBase()
    {
        Owner.ElementStack.PopTo(HoldingIndex);
    };
};

template <typename T>
class VisitStackPop : public VisitStackItBase<T>
{
public:
    VisitStackPop(VisitStackHelper<T>& InOwner )
        : VisitStackItBase<T>(InOwner)
    {
        this->HoldingIndex = InOwner.IndexStack.PickPop();
    };
};

template <typename T>
class VisitStackConsume : public VisitStackItBase<T>
{
public:
    VisitStackConsume(VisitStackHelper<T>& InOwner )
        : VisitStackItBase<T>(InOwner)
    {
        this->HoldingIndex = InOwner.IndexStack.Top();
    };
};

template <typename T>
class VisitStackClearPop : public VisitStackItBase<T>
{
public:
    VisitStackClearPop(VisitStackHelper<T>& InOwner )
        : VisitStackItBase<T>(InOwner)
    {
        this->HoldingIndex = InOwner.IndexStack.PickPop() - 1;
    };
};


template <typename T>
class VisitStackHelper
{
public:
    OLList<int> IndexStack;
    OLList<T>   ElementStack;

    void Push(T& Elem )
    {
        ElementStack.Add(Elem);
        IndexStack.Add(ElementStack.Count() - 1);
    };

    void Push(T&& Elem )
    {
        ElementStack.Add(Elem);
        IndexStack.Add(ElementStack.Count() - 1);
    };

    T& Top()
    {
        return ElementStack[ElementStack.Count() - 1];
    };

};

}