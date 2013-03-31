// myrtti.cpp : 定义控制台应用程序的入口点。
//

#include "pw_object.h"

using namespace pwutils;

class B1 : public PWOBJECT
{
	RTTI(B1,Object);
};


class B2 : public PWOBJECT
{
	RTTI(B2,Object);
};

class S1 : public B1
{
	RTTI(S1,B1);
};

class C1 : public S1
{
	RTTI(C1,S1);
};

int _tmain(int argc, char* argv[])
{
	Object* o = new Object();
	S1* s = new S1();
	const Class* c1 = s->GetClass();

	S1* s2 = c1->CreateObject()->QueryObject<S1>();

	s->QueryObject<Object>();
 	s->QueryObject<B1>();
 	s->QueryObject<C1>();
	
	c1 = o->GetClass();
	B1* b = new B1();
	c1 = b->GetClass();
	

	g_objClassMap.add<Object>();
	g_objClassMap.add<B1>();
	g_objClassMap.add<B2>();
	g_objClassMap.add<S1>();
	g_objClassMap.add<C1>();
//	B1* bb = (B1*)g_objClassMap.get<B1>()->CreateObject();

	return 0;
}

