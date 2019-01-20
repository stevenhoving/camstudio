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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <CamEncoder/av_dict.h>
#include <CamEncoder/av_ffmpeg.h>
#include <fmt/printf.h>

TEST(test_dict, test_assignment)
{
    av_dict dict;
    dict["test"] = "42";
    auto key_value = dict.at("test");
    ASSERT_STREQ(key_value->value, "42");
}

TEST(test_dict, test_access)
{
    av_dict dict;
    dict["test"] = "42";
    const auto value = dict["test"];
    ASSERT_STREQ(value, "42");
}

TEST(test_dict, test_size)
{
    av_dict dict;
    dict["test"] = "42";
    EXPECT_EQ(dict.size(), 1);
}

TEST(test_dict, test_clear)
{
    av_dict dict;
    dict["test"] = "42";
    dict.clear();
    EXPECT_TRUE(dict.empty());
}

TEST(test_dict, test_empty_access)
{
    av_dict dict;
    ASSERT_THROW({ dict.at("test"); }, std::out_of_range);
}

TEST(test_dict, test_copy_assignment)
{
    av_dict dict;
    dict["test"] = "42";

    av_dict dict_copy = dict;
    dict.clear();

    EXPECT_TRUE(dict.empty());
    const auto value = dict_copy["test"];
    ASSERT_STREQ(value, "42");
}

TEST(test_dict, test_copy_constructor)
{
    av_dict dict;
    dict["test"] = "42";

    av_dict dict_copy(dict);
    dict.clear();

    EXPECT_TRUE(dict.empty());
    const auto value = dict_copy["test"];
    ASSERT_STREQ(value, "42");
}

TEST(test_dict, test_make_av_dict)
{
    auto dict = make_av_dict({
        {"test", "42"}
    });

    EXPECT_FALSE(dict.empty());
    for (auto itr : dict)
        fmt::print("{} {}\n", itr.key, itr.value);

    const auto value = dict.at("test");
    ASSERT_STREQ(value->value, "42");
}
