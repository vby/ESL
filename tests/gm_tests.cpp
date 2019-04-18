
#include <gtest/gtest.h>
#include <esl/gm.hpp>

namespace gm = esl::gm;

TEST(GMTest, vec4) {
    // From scalar
    {
        gm::Vec4 v(1.0);
        ASSERT_EQ(v.x, 1);
        ASSERT_EQ(v.y, 1);
        ASSERT_EQ(v.z, 1);
        ASSERT_EQ(v.w, 1);
    }

    {
        gm::Vec4 v1(1.0, 2.0, 3.0, 4.0);
        gm::Vec4 v2(1.0, 2.0, v1.zw);
        gm::Vec3 zxx(3.0, 1.0, 1.0);
        ASSERT_EQ(v1, v2);
        ASSERT_EQ(v1.ww, gm::Vec2(4, 4));
        ASSERT_EQ(v1, v1.xyzw);
        ASSERT_EQ(v1.xyzw, v1);
        ASSERT_EQ(zxx, v1.zxx);

        v1.zyw = v2.xzz;
        ASSERT_EQ(v1, gm::Vec4(1.0, 3.0, 1.0, 3.0));
    }

    {
        gm::Vec4 v1(1, 2, 3, 4);
        gm::Vec4 v2(5, 6, 7, 8);
        gm::Vec4 v = v1 + v2;
        ASSERT_EQ(v, gm::Vec4(6, 8, 10, 12));
        v.xyzw -= v2;
        ASSERT_EQ(v, v1);

        v *= 2;
        ASSERT_EQ(v, gm::Vec4(2, 4, 6, 8));
    }
}
