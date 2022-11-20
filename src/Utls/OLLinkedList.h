/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once

namespace OL
{

class OLLinkedListBase
{
public:
    struct BaseIterator
    {
        virtual void* GetCurrPtr() = 0;
        virtual bool  Next() = 0;
        virtual bool Prev() = 0;
        virtual bool IsValid() = 0;
        virtual ~BaseIterator(){};
    };

    virtual BaseIterator* BeginBaseIter() = 0;
    virtual void FinishBaseIter(BaseIterator* Iter) = 0;
};


template <typename Ty>
class OLLinkedList : public OLLinkedListBase
{
public:
    struct LinkNode
    {
        Ty* Value;
        LinkNode* Prev;
        LinkNode* Next;
    };
    LinkNode* Head;
    LinkNode* Tail;
    int NodeCount;

    struct Iterator : public OLLinkedListBase::BaseIterator
    {
        virtual bool Next()
        {
            if(CurrNode == nullptr)
                return false;
            CurrNode = CurrNode->Next;
            if(CurrNode == nullptr)
                return false;
            return true;
        };

        virtual bool Prev()
        {
            if(CurrNode == nullptr)
                return false;
            CurrNode = CurrNode->Prev;
            if(CurrNode == nullptr)
                return false;
            return true;

        };

        virtual bool IsValid()
        {
           if(CurrNode == nullptr)
                return false;
            return true;
        };

        Ty&   Value()
        {
            return *(CurrNode->Value);
        };

        virtual void* GetCurrPtr()
        {
            return (void*)(CurrNode->Value);
        };

        Iterator(OLLinkedList<Ty>* InOwner)
            : Owner(InOwner)
        {
            CurrNode = Owner->Head;
        };

        OLLinkedList<Ty>* Owner;
        LinkNode* CurrNode;
    };
    

    OLLinkedList()
    {
        Head = nullptr;
        Tail = nullptr;
        NodeCount = 0;
    };

    int Count()
    {
        return NodeCount;
    };
    void AddTail(Ty& Value)
    {
        LinkNode* NewNode = new LinkNode();
        NewNode->Value = new Ty(Value);
        NewNode->Next = nullptr;

        if(Head == nullptr)
        {
            Head = NewNode;
            Tail = NewNode;
            NewNode->Prev = nullptr;
        }
        else
        {
            NewNode->Prev = Tail;
            Tail->Next = NewNode;
            Tail = NewNode;
        }
        NodeCount++;
    };

    void AddHead(Ty& Value)
    {
        LinkNode* NewNode = new LinkNode();
        NewNode->Value = new Ty(Value);
        NewNode->Prev = nullptr;

        if(Tail == nullptr)
        {
            Head = NewNode;
            Tail = NewNode;
            NewNode->Next = nullptr;
        }
        else
        {
            NewNode->Next = Head;
            Head->Prev = NewNode;
            Head = NewNode;
        }
        NodeCount++;
    };

    Ty PopHead()
    {
        LinkNode* Ret = Head;
        OL_ASSERT(Ret != nullptr);

        Head = Head->Next;
        if(Head != nullptr)
            Head->Prev = nullptr; 

        Ty RetVal = *(Ret->Value);
        delete Ret;
        NodeCount--;
        return RetVal;
    };

    Ty PopTail()
    {
        LinkNode* Ret = Tail;
        OL_ASSERT(Ret != nullptr);

        Tail = Tail->Prev;
        if(Tail != nullptr)
            Tail->Next = nullptr; 

        Ty RetVal = *(Ret->Value);
        delete Ret;
        NodeCount--;
        return RetVal;
    };

    void Remove(Iterator It)
    {
        LinkNode* Node = It->CurrNode;

        if(Node->Prev != nullptr)
        {
            Node->Prev->Next = Node->Next;
        }
        else
        {
            Head = Node->Next;
        }

        if(Node->Next != nullptr)
        {
            Node->Next->Prev = Node->Prev;
        }
        else
        {
            Tail = Node->Prev;
        }

        It = Node->Next();
        NodeCount--;
    }

    void InsertAfter(Iterator It, Ty& Value)
    {
        LinkNode* NewNode = new LinkNode();
        NewNode->Value = new Ty(Value);
        
        NewNode->Next = It->CurrNode->Next;
        It->CurrNode->Next = NewNode;

        if(NewNode->Next != nullptr)
            NewNode->Next->Prev = NewNode;

        NewNode->Prev = It->CurrNode;
        NodeCount++;
    }

    void InsertBefore(Iterator It, Ty& Value)
    {
        LinkNode* NewNode = new LinkNode();
        NewNode->Value = new Ty(Value);
        
        NewNode->Prev = It->CurrNode->Prev;
        It->CurrNode->Prev = NewNode;

        if(NewNode->Prev != nullptr)
            NewNode->Prev->Next = NewNode;

        NewNode->Next = It->CurrNode;
        NodeCount++;
    }


    virtual BaseIterator* BeginBaseIter()
    {
        return new Iterator(this);
    };

    virtual void FinishBaseIter(BaseIterator* Iter)
    {
        delete Iter;
    };

    Iterator BeginIterator()
    {
        return Iterator(this);
    };

};



}