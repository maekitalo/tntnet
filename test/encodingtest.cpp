#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <tnt/encoding.h>

class EncodingTest : public cxxtools::unit::TestSuite
{
  public:
    EncodingTest()
      : cxxtools::unit::TestSuite("encoding")
    {
      registerMethod("default", *this, &EncodingTest::def);
      registerMethod("whitespace", *this, &EncodingTest::whitespace);
      registerMethod("wildcard", *this, &EncodingTest::wildcard);
      registerMethod("parse", *this, &EncodingTest::parse);
      registerMethod("parseIdentity", *this, &EncodingTest::parseIdentity);
    }

    void def()
    {
        tnt::Encoding enc;
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("identity"), 1001u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("gzip"), 0u);
    }

    void whitespace()
    {
        tnt::Encoding enc(" blah ; q = 0.764 , * ; q \t = 0.67 ");
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("blah"), 764u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("identity"), 670u);
    }

    void wildcard()
    {
        tnt::Encoding enc("*;q=0.7");
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("identity"), 700u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("gzip"), 700u);
    }

    void parse()
    {
        tnt::Encoding enc("deflate, gzip;q=1.0, *;q=0.5");
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("deflate"), 1000u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("gzip"), 1000u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("blah"), 500u);
    }

    void parseIdentity()
    {
        tnt::Encoding enc("identity;q=1, *;q=0");
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("identity"), 1000u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("deflate"), 0u);
        CXXTOOLS_UNIT_ASSERT_EQUALS(enc.accept("gzip"), 0u);
    }

};

cxxtools::unit::RegisterTest<EncodingTest> register_EncodingTest;
