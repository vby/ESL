
#include <gtest/gtest.h>
#include <esl/dynamic_library.hpp>

#define DL_FILENAME ESL_STRING(ESL_TESTS_DL_FILENAME) 

TEST(DynamicLibraryTest, succ_open_close) {
	esl::dynamic_library dl;
	ASSERT_FALSE(dl.is_open());
	ASSERT_TRUE(dl.good());

	dl.open(DL_FILENAME);
	ASSERT_TRUE(dl.is_open());
	ASSERT_TRUE(dl.good());

	dl.close();
	ASSERT_FALSE(dl.is_open());
	ASSERT_TRUE(dl.good());
}

TEST(DynamicLibraryTest, fail_open_close) {
	esl::dynamic_library dl;
	ASSERT_FALSE(dl.is_open());
	ASSERT_TRUE(dl.good());

	dl.open("no_such_file");
	ASSERT_FALSE(dl.is_open());
	ASSERT_FALSE(dl.good());
}

TEST(DynamicLibraryTest, open_on_construct) {
	{
		esl::dynamic_library dl(DL_FILENAME);
		ASSERT_TRUE(dl.is_open());
		ASSERT_TRUE(dl.good());
	}
	{
		esl::dynamic_library dl("no_such_file");
		ASSERT_FALSE(dl.is_open());
		ASSERT_FALSE(dl.good());
	}
}

TEST(DynamicLibraryTest, symbol) {
	esl::dynamic_library dl(DL_FILENAME);
	void* add_ptr = dl.symbol("dynamic_library_tests_dl_func_add");
	ASSERT_TRUE(add_ptr);
	ASSERT_TRUE(dl.good());

	auto add = dl.symbol<int(*)(int, int)>("dynamic_library_tests_dl_func_add");
	ASSERT_TRUE(add);
	ASSERT_TRUE(dl.good());
	ASSERT_EQ(add(1, 2), 3);

	void* sym = dl.symbol("no_such_symbol");
	ASSERT_FALSE(sym);
	ASSERT_TRUE(dl.good());

	std::string error_message;
	void* sym2 = dl.symbol("no_such_symbol2", error_message);
	ASSERT_FALSE(sym);
	ASSERT_TRUE(dl.good());
	ASSERT_FALSE(error_message.empty());
}

