
#include "esl/declspec.hpp"

extern "C" {

ESL_EXPORT int shared_library_tests_dl_func_add(int a, int b) {
	return a + b;
}

}
