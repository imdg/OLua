#pragma once
#include "Common.h"

namespace OL
{

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

    ~VisitStackItBase()
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