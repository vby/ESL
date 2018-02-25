
#include <gtest/gtest.h>
#include <esl/shared_library.hpp>

#define DL_FILENAME ESL_STRINGIFY(ESL_TESTS_DL_FILENAME)

TEST(SharedLibraryTest, succ_open_close) {
	esl::shared_library dl;
	ASSERT_FALSE(dl.is_open());
	ASSERT_FALSE(dl);

	dl.open(DL_FILENAME);
	ASSERT_TRUE(dl.is_open());

	dl.close();
	ASSERT_FALSE(dl.is_open());
}

TEST(SharedLibraryTest, fail_open_close) {
	esl::shared_library dl;
	ASSERT_FALSE(dl.is_open());

	ASSERT_THROW(dl.open(nullptr), std::system_error);

	ASSERT_THROW(dl.open("no_such_file"), std::system_error);
}

TEST(SharedLibraryTest, open_on_construct) {
	{
		esl::shared_library dl(DL_FILENAME);
		ASSERT_TRUE(dl.is_open());
	}
	{
		std::system_error err(std::error_code{});
		try {
			esl::shared_library dl("no_such_file");
		} catch(const std::system_error& e) {
			err = e;
		}
		ASSERT_TRUE(err.code());
	}
}

TEST(SharedLibraryTest, get_symbol) {
	esl::shared_library dl(DL_FILENAME);
	void* add_ptr = dl.get_symbol("shared_library_tests_dl_func_add");
	ASSERT_TRUE(add_ptr);

	auto add = dl.get_symbol<int(*)(int, int)>("shared_library_tests_dl_func_add");
	ASSERT_TRUE(add);
	ASSERT_EQ(add(1, 2), 3);

	ASSERT_THROW(dl.get_symbol("no_such_symbol"), std::system_error);

	std::string err_msg;
	void* sym = dl.get_symbol("no_such_symbol2", err_msg);
	ASSERT_FALSE(sym);
	ASSERT_FALSE(err_msg.empty());
}

