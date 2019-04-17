
#include <gtest/gtest.h>
#include <esl/gm.hpp>

namespace gm = esl::gm;

TEST(GMTest, vec) {
    gm::Vec4 v1(1.0, 2.0, 3.0, 4.0);
    gm::Vec4 v2(1.0, 2.0, 3.0, 4.0);
    gm::Vec3 zxx(3.0, 1.0, 1.0);
    ASSERT_EQ(v1, v2);
    ASSERT_EQ(v1, v1.xyzw);
    ASSERT_EQ(v1.xyzw, v1);
    ASSERT_EQ(zxx, v1.zxx);
    
    v1.zyw = v2.xzz;
    ASSERT_EQ(v1, gm::Vec4(1.0, 3.0, 1.0, 3.0));
}

