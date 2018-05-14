#include <type_safe/integer.hpp>
#include <type_safe/flag.hpp>

#include <limits>
#include <iostream>

#include <boost/test/unit_test.hpp>

namespace ts = type_safe;

using namespace std;

BOOST_AUTO_TEST_SUITE(type_safe)

BOOST_AUTO_TEST_CASE(ts_integer)
{
    {
        using ts_uint = ts::integer<unsigned>;

        const ts_uint a{1u};
        const unsigned int b{2};

        const ts_uint c = a + b;
        BOOST_TEST(c == ts::make_unsigned(3));
    }
    {
        using ts_uint = ts::integer<unsigned, ts::checked_arithmetic>;

        const ts_uint a{numeric_limits<ts_uint::integer_type>::max() - 1};
        const auto b{2u};

        BOOST_REQUIRE_THROW(a + b, ts::checked_arithmetic::error);
    }
}

BOOST_AUTO_TEST_CASE(ts_flag)
{
    using flag = ts::flag;
    {
        flag a{true};

        BOOST_TEST(a.toggle());
        BOOST_TEST(!a.toggle());

        a.change(false);
        a.change(true);

        a.set();
        a.reset();

        BOOST_TEST(a.try_set());
        BOOST_TEST(!a.try_set());

        BOOST_TEST(a.try_reset());
        BOOST_TEST(!a.try_reset());
    }
    {
        vector nums = {1, 2, 1, 3, 4, 1, 5, 6, 7};
        flag found_one{false};
        vector<int> nums_after_one{};
        for(const auto& a : nums)
        {
            if(a == 1) found_one.change(true);
            else if(found_one.try_reset()) nums_after_one.push_back(a);
        }
        const vector expected{2, 3, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(
            begin(nums_after_one), end(nums_after_one),
            begin(expected), end(expected));
    }
}

BOOST_AUTO_TEST_SUITE_END()
