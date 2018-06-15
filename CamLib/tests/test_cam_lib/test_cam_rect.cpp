/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <CamLib/rect.h>

TEST(test_rect, test_clamp)
{
    CRect rect(10, 10, 100, 100);
    const CRect max_rect(0, 0, 95, 95);

    rect = clamp_rect(rect, max_rect);

    EXPECT_EQ(rect.left, 5);
    EXPECT_EQ(rect.top, 5);
    EXPECT_EQ(rect.right, 95);
    EXPECT_EQ(rect.bottom, 95);
}

TEST(test_rect, test_merge)
{
    CRect a(10, 10, 20, 20);
    CRect b(15, 15, 25, 25);

    const auto rect = merge_rect(a, b);

    EXPECT_EQ(rect.left, 10);
    EXPECT_EQ(rect.top, 10);
    EXPECT_EQ(rect.right, 25);
    EXPECT_EQ(rect.bottom, 25);
}

//merge_rect