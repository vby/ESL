
#include "esl/macros.hpp"

extern "C" {

ESL_DECL_EXPORT int shared_library_tests_dl_func_add(int a, int b) {
	return a + b;
}

}

