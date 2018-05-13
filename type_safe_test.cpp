#include <type_safe/integer.hpp>
#include <limits>

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

BOOST_AUTO_TEST_SUITE_END()
