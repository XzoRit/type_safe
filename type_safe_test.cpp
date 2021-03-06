#include <type_safe/constrained_type.hpp>
#include <type_safe/flag.hpp>
#include <type_safe/integer.hpp>
#include <type_safe/optional.hpp>

#include <boost/core/demangle.hpp>

#include <iostream>
#include <limits>
#include <type_traits>

#include <boost/test/unit_test.hpp>

namespace ts = type_safe;

using namespace std;

using boost::core::demangle;

BOOST_AUTO_TEST_SUITE(type_safe)

BOOST_AUTO_TEST_CASE(ts_integer)
{
    {
        using ts_uint = ts::integer<unsigned>;

        const ts_uint      a{1u};
        const unsigned int b{2};

        const ts_uint c = a + b;
        BOOST_TEST(c == ts::make_unsigned(3));
    }
    {
        using ts_uint = ts::integer<unsigned, ts::checked_arithmetic>;

        const ts_uint a{numeric_limits<ts_uint::integer_type>::max() - 1};
        const auto    b{2u};

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
        vector      nums = {1, 2, 1, 3, 4, 1, 5, 6, 7};
        flag        found_one{false};
        vector<int> nums_after_one{};
        for(const auto& a : nums) {
            if(a == 1)
                found_one.change(true);
            else if(found_one.try_reset())
                nums_after_one.push_back(a);
        }
        const vector expected{2, 3, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(begin(nums_after_one), end(nums_after_one), begin(expected),
                                        end(expected));
    }
}

struct boost_test_verifier
{
    template <class Value, class Constrain>
    static constexpr Value verify(Value a, const Constrain& c)
    {
        BOOST_TEST(c(a), "constraint: \"" << demangle(typeid(c).name())
                                          << "\" not satisfied with value = " << a);
        return a;
    }
};

struct is_odd
{
    template <class Value>
    struct is_valid : is_unsigned<Value>
    {
    };

    template <class Value>
    constexpr bool operator()(Value a) const noexcept(noexcept(a % 2))
    {
        return a % 2;
    }
};

using always_odd = ts::constrained_type<unsigned, is_odd, boost_test_verifier>;

ostream& operator<<(ostream& s, const always_odd& v)
{
    s << "always_odd(" << v.get_value() << ")";
    return s;
}

BOOST_AUTO_TEST_CASE(ts_constrained_type)
{
    static_assert(!is_constructible<always_odd, signed>::value);
    always_odd a{3u};
    BOOST_TEST(is_odd{}(a.get_value()));

    const auto b = ts::constrain<boost_test_verifier>(3u, is_odd{});
    static_assert(is_same_v<decay_t<decltype(b)>, always_odd>);
    BOOST_TEST(a == b);

    auto a_mod = a.modify();
    a_mod.get() += 2;
    ts::with(a, [](auto& a) { a -= 2; });
    BOOST_TEST(a == b);
}

BOOST_AUTO_TEST_CASE(ts_optional)
{
    ts::optional<int> a;
    BOOST_TEST(!a.has_value());
    BOOST_TEST(a.value_or(11) == 11);

    a = 11;
    ts::optional<string> b = a.map([](const auto& a) { return to_string(a); })
                                 .map([](const auto& a) { return stoi(a); })
                                 .map([](const auto& a) { return to_string(a); });
    BOOST_TEST(b.has_value());
    BOOST_TEST(b.value() == "11");

    int i;
    ts::with(b, [](const auto& a, auto& b) { b = stoi(a); }, i);
    BOOST_TEST(i == 11);
}

BOOST_AUTO_TEST_SUITE_END()
