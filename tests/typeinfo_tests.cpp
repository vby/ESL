
#include <gtest/gtest.h>
#include <esl/typeinfo.hpp>

struct TypeinfoTestClassBase {
	virtual ~TypeinfoTestClassBase() = default;
};
struct TypeinfoTestClassA : TypeinfoTestClassBase {
	struct ClassB {};
	using int_type = int;
	bool func(int, const ClassB*) { return false; }
};

#ifdef ESL_HAS_DEMANGLE

TEST(TypeinfoTest, demangle) {
	{
		auto name = esl::demangle(typeid(TypeinfoTestClassA).name());
		ASSERT_STREQ(name.get(), "TypeinfoTestClassA");
	}
	{
		auto name = esl::demangle(typeid(TypeinfoTestClassA::int_type).name());
		ASSERT_STREQ(name.get(), "int");
	}
	{
		auto name = esl::demangle(typeid(TypeinfoTestClassA::ClassB).name());
		ASSERT_STREQ(name.get(), "TypeinfoTestClassA::ClassB");
	}
	{
		auto name = esl::demangle(typeid(&TypeinfoTestClassA::func).name());
		ASSERT_STREQ(name.get(), "bool (TypeinfoTestClassA::*)(int, TypeinfoTestClassA::ClassB const*)");
	}
}

TEST(TypeinfoTest, typeid_name) {
	TypeinfoTestClassA a;
	TypeinfoTestClassBase& b = a;
	{
		auto name = esl::typeid_name(typeid(a)); 
		ASSERT_EQ(name, "TypeinfoTestClassA");
	}
	{
		auto name = esl::typeid_name(a); 
		ASSERT_EQ(name, "TypeinfoTestClassA");
	}
	{
		auto name = esl::typeid_name(b); 
		ASSERT_EQ(name, "TypeinfoTestClassA");
	}
	{
		auto name = esl::typeid_name<TypeinfoTestClassA>(); 
		ASSERT_EQ(name, "TypeinfoTestClassA");
	}
}

#endif // ESL_HAS_DEMANGLE


