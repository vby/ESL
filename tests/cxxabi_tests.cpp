
#include <gtest/gtest.h>
#include <esl/cxxabi.hpp>

struct CxxabiTestClassBase {
	virtual ~CxxabiTestClassBase() = default;
};
struct CxxabiTestClassA : CxxabiTestClassBase {
	struct ClassB {};
	using int_type = int;
	bool func(int, const ClassB*) {}
};

#ifdef ESL_HAS_DEMANGLE

TEST(CxxabiTest, demangle) {
	{
		auto name = esl::demangle(typeid(CxxabiTestClassA).name());
		ASSERT_STREQ(name.get(), "CxxabiTestClassA");
	}
	{
		auto name = esl::demangle(typeid(CxxabiTestClassA::int_type).name());
		ASSERT_STREQ(name.get(), "int");
	}
	{
		auto name = esl::demangle(typeid(CxxabiTestClassA::ClassB).name());
		ASSERT_STREQ(name.get(), "CxxabiTestClassA::ClassB");
	}
	{
		auto name = esl::demangle(typeid(&CxxabiTestClassA::func).name());
		ASSERT_STREQ(name.get(), "bool (CxxabiTestClassA::*)(int, CxxabiTestClassA::ClassB const*)");
	}
}

TEST(CxxabiTest, object_name) {
	CxxabiTestClassA a;
	CxxabiTestClassBase& b = a;
	{
		auto name = esl::object_name(a); 
		ASSERT_EQ(name, "CxxabiTestClassA");
	}
	{
		auto name = esl::object_name(b); 
		ASSERT_EQ(name, "CxxabiTestClassA");
	}
}

TEST(CxxabiTest, type_name) {
	auto name = esl::type_name<CxxabiTestClassA>(); 
	ASSERT_EQ(name, "CxxabiTestClassA");
}

#endif // ESL_HAS_DEMANGLE


