#include "Common.h"
#include "Action.h"
#include "Rtti.h"
#include "TextSerializer.h"
#include "OLMap.h"
#include "TextExpand.h"
#include "Lexer.h"
namespace OL
{

class TestChild
{
public:
    DECLEAR_RTTI();

    int childa = 0;
    float childb = 1;
    int* pChildCCC = nullptr;

    TestChild(int a, float b, int* c) : childa(a), childb(b), pChildCCC(c) {}

};

RTTI_BEGIN(TestChild)
    RTTI_MEMBER(childa)
    RTTI_MEMBER(childb)
    RTTI_MEMBER(pChildCCC)
RTTI_END(TestChild)


class Test
{
public:
    DECLEAR_RTTI();

    int a;
    float b;
    
    TestChild* pChild;

    OLList<int> lla;
    OLList<TestChild> lchild;
    OLList<TestChild*> lpchild;

    OLMap<OLString, TestChild*> map;
};
RTTI_BEGIN(Test)
    RTTI_MEMBER(a)
    RTTI_MEMBER(b)
    RTTI_MEMBER(lla)
    RTTI_MEMBER(lchild)
    RTTI_MEMBER(lpchild)
    RTTI_MEMBER(map)
RTTI_END(Test)


void TestFunc(const OLString& tmp)
{

};

RTTI_ENUM(ETestEnum,
    TE_AA,
    TE_BB,
    TE_CC
)


#ifdef USE_MSVC
#if !defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL
#pragma message("Traditional preprocessor")
#else
#pragma message("Crossplatform preprocessor")
#endif
#endif

class TestOne : public Action
{
public:
    virtual int Run()
    {
        printf("TestOne Running\n");

        TypeID ValueID = CreateTypeID<OLMap<OLString, TestChild*>>::Do();
        TypeID KeyID = CreateKeyTypeID<OLMap<OLString, TestChild*>>::Do();

        bool IsMap = TIsMap< OLMap<OLString, TestChild*> > ::Value;

        int temp1 = 100;
        int temp2 = 200;

        Test A;
        TestChild B1(1, 1.5, &temp1);
        TestChild B2(2, 2.5, &temp2);
        TestChild B3(3, 3.5, &temp1);
        TestChild B4(3, 3.5, nullptr);

        TestChild c1(10, 10.5, &temp1);
        TestChild c2(20, 20.5, &temp2);
        TestChild c3(30, 30.5, &temp1);
        TestChild c4(30, 30.5, nullptr);

        A.pChild = &B1;
        A.a = 1000;
        A.b = 0.003f;

        A.lchild.Add(B2);
        A.lchild.Add(B3);
        A.lchild.Add(B4);

        A.lpchild.Add(&c1);
        A.lpchild.Add(&c2);
        A.lpchild.Add(&c3);
        A.lpchild.Add(&c4);
        
        TestFunc(T("aaa"));
         A.map.Add(T("First"), &B1);
         A.map.Add(T("Second"), &B1);

         TextSerializer ts;
         ts.WriteRTTI(&A, &A.RTTI);
        return 0;
    };
};

REGISTER_ACTION(TestOne)











}