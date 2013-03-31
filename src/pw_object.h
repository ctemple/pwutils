#ifndef _pw_object_
#define _pw_object_

#include <stdarg.h>
#include <string.h>
#include <typeinfo>
#include <string>
#include <map>

namespace pwutils
{

#define RTTI_BASE(_this) \
	virtual const Class* GetClass() \
	{ \
		return _this::_GetClass(); \
	} 

#define RTTI_1(_this,_base) \
	public: \
	static const Class* _GetClass() \
		{\
		static const ClassN<_this,1> m_class(1,_base::_GetClass());\
		return &m_class;\
		} \
		RTTI_BASE(_this)

#define RTTI_2(_this,_base1,_base2) \
	public: \
		static const Class* _GetClass() \
		{\
			static const ClassN<_this,2> m_class(2,_base1::_GetClass(),_base2::_GetClass());\
			return &m_class;\
		} \
		RTTI_BASE(_this)

#define RTTI_3(_this,_base1,_base2,_base3) \
	public: \
		static const Class* _GetClass() \
		{\
			static const ClassN<_this,3> m_class(3,_base1::_GetClass(),_base2::_GetClass(),_base3::_GetClass());\
			return &m_class;\
		} \
		RTTI_BASE(_this)

#define RTTI_4(_this,_base1,_base2,_base3,_base4) \
	public: \
		static const Class* _GetClass() \
		{\
			static const ClassN<_this,4> m_class(4,_base1::_GetClass(),_base2::_GetClass(),_base3::_GetClass(),_base4::_GetClass());\
			return &m_class;\
		} \
		RTTI_BASE(_this)

#define RTTI_5(_this,_base1,_base2,_base3,_base4,_base5) \
	public: \
		static const Class* _GetClass() \
		{\
			static const ClassN<_this,5> m_class(5,_base1::_GetClass(),_base2::_GetClass(),_base3::_GetClass(),_base4::_GetClass(),_base5::_GetClass());\
			return &m_class;\
		} \
		RTTI_BASE(_this)
	
#define RTTI RTTI_1

	class Object;
	
	class Class
	{
	public:
		Class(const char* name)
		{
			m_name = strdup(name);
			m_level = 0;
		}
	public:
		inline const char* GetName() const
		{
			return m_name;
		}

		inline int GetLevel() const
		{
			return m_level;
		}
	public:
		virtual int GetParentNum() const = 0;
		virtual const Class* GetParent(int i) const = 0;
		virtual Object* CreateObject() const = 0;
	protected:
		char* m_name;
		int   m_level;
	};

	template<class T,int N> class ClassN : public Class
	{
	public:
		ClassN(int n,...)
			: Class(typeid(T).name())
		{
			va_list vl;
			va_start(vl,n);
			{
				for(int c = 0; c < n; ++c)
				{
					const Class* p = va_arg(vl,const Class*);
					m_parents[c] = p;
					if(m_level < p->GetLevel())
					{
						m_level = p->GetLevel();
					}
				}
			}
			va_end(vl);
			m_level = m_level + 1;
			m_parents_count = n;
		}
	public:
		virtual int GetParentNum() const
		{ 
			return m_parents_count;
		}
		virtual const Class* GetParent(int i) const
		{
			return m_parents[i];
		}

		virtual Object* CreateObject() const
		{
			return new T();
		}
	protected:
		int m_parents_count;
		const Class* m_parents[N];
	};

	template<class T> class ClassN<T,0> : public Class
	{
	public:
		ClassN()
			: Class(typeid(T).name())
		{
		}
	public:
		virtual int GetParentNum() const
		{ 
			return 0;
		}

		virtual const Class* GetParent(int i) const
		{
			return 0;
		}

		virtual Object* CreateObject() const
		{
			return new T();
		}
	};

	class Object
	{
	public:
		virtual ~Object() {}
	public:
		static const Class* _GetClass()
		{
			static ClassN<Object,0> m_class;
			return &m_class;
		}

		virtual const Class* GetClass()
		{
			return Object::_GetClass();
		}

		template<class T> bool IsKindOf()
		{
			return this->IsKindOf(this->GetClass(),T::_GetClass());
		}

		template<class T> T* Cast()
		{
			if(IsKindOf<T>())
#ifdef PW_VIRTUAL_OBJECT
				return dynamic_cast<T*>(this);
#else
				return (T*)(this);
#endif
			return NULL;
		}

		inline bool IsKindOf(const Class* cls)
		{
			const Class* thisClass = this->GetClass();
			return IsKindOf(thisClass,cls);
		}

		static bool IsKindOf(const Class* sub,const Class* base)
		{
			if(base->GetLevel() > sub->GetLevel())
				return false;

			if(sub == base)
				return true;

			int count = sub->GetParentNum();
			for(int i = 0; i < count; ++i)
			{
				const Class* b = sub->GetParent(i);
				if(b == base)
					return true;
				if(IsKindOf(b,base))
					return true;
			}
			return false;
		}
		
	};

	class ClassMap : protected std::map<std::string,const Class*>
	{
		typedef std::map<std::string,const Class*> Super;
	public:
		bool add(const Class* cls)
		{
			return this->insert(std::make_pair(cls->GetName(),cls)).second;
		}

		template<class T> bool add()
		{
			return this->add(T::_GetClass());
		}

		const Class* get(const char* name)
		{
			Super::iterator iter = find(name);
			if(iter != end())
				return iter->second;
			return NULL;
		}

		template<class T> const Class* get()
		{
			return T::_GetClass();
		}
	};

#ifdef PW_VIRTUAL_OBJECT
	#define PWOBJECT virtual Object
#else
	#define PWOBJECT Object
#endif

	extern ClassMap g_objClassMap;
}
#endif // _pw_object_