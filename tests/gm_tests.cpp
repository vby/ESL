
#include <gtest/gtest.h>
#include <esl/gm.hpp>

namespace gm = esl::gm;

TEST(GMTest, vec4) {
    {
        gm::Vec4 v(1);
        ASSERT_EQ(v.x, 1);
        ASSERT_EQ(v.y, 1);
        ASSERT_EQ(v.z, 1);
        ASSERT_EQ(v.w, 1);

        ASSERT_EQ(v[0], 1);
        ASSERT_EQ(v[1], 1);
        ASSERT_EQ(v[2], 1);
        ASSERT_EQ(v[3], 1);

        gm::Vec4 v1(1, 1, 1, 1);
        gm::Vec4 v2(1, 1, 1, 2);
        ASSERT_EQ(v, v1);
        ASSERT_NE(v1, v2);
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

        v *= 2.0;
        ASSERT_EQ(v, gm::Vec4(2, 4, 6, 8));
    }

    {
        gm::Vec4 v(1, 2, 3, 4);
        gm::Vec4 v0 = v;
        ASSERT_EQ(+v, v0);
        ASSERT_EQ(v, v0);
    }

    {
        gm::Vec4 v(1, 2, 3, 4);
        gm::Vec4 v0 = v;
        gm::Vec4 v1(-1, -2, -3, -4);
        ASSERT_NE(-v, v0);
        ASSERT_EQ(-v, v1);
        ASSERT_EQ(v, v0);
    }

    {
        gm::Vec4 v(1, 2, 3, 4);
        gm::Vec4 v0 = v;
        gm::Vec4 v1(2, 3, 4, 5);
        gm::Vec4 v2(3, 4, 5, 6);
        ASSERT_EQ(v++, v0);
        ASSERT_EQ(v, v1);
        ASSERT_EQ(++v, v2);
        ASSERT_EQ(v, v2);
    }
}

TEST(GMTest, mat4) {
    {
        gm::Vec4 vx(1, 2, 3, 4);
        gm::Vec4 vy(5, 6, 7, 8);
        gm::Vec4 vz(9, 10, 11, 12);
        gm::Vec4 vw(13, 14, 15, 16);
        gm::Mat4 m(vx, vy, vz, vw);

        ASSERT_EQ(m.x, vx);
        ASSERT_EQ(m.y, vy);
        ASSERT_EQ(m.z, vz);
        ASSERT_EQ(m.w, vw);
        ASSERT_EQ(m.x.xyzw, vx);

        ASSERT_EQ(m[0], vx);
        ASSERT_EQ(m[1], vy);
        ASSERT_EQ(m[2], vz);
        ASSERT_EQ(m[3], vw);
    }

    {
        gm::Mat4 m(1);
        ASSERT_EQ(m.x.x, 1);
        ASSERT_EQ(m.y.y, 1);
        ASSERT_EQ(m.z.z, 1);
        ASSERT_EQ(m.w.w, 1);

        ASSERT_EQ(m, gm::Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
    }

    {
        gm::Mat4 m(gm::Mat2(5));
        ASSERT_EQ(m, gm::Mat4(5, 0, 0, 0, 0, 5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

        gm::Mat4 m2(gm::Mat3(5));
        ASSERT_EQ(m2, gm::Mat4(5, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5, 0, 0, 0, 0, 1));
    }
}