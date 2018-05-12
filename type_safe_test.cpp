#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(type_safe)

BOOST_AUTO_TEST_CASE(test_a)
{
    int a{1};
    int b{2};

    BOOST_TEST(a + b == 0);
}

BOOST_AUTO_TEST_SUITE_END()
