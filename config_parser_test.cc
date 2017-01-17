#include <sstream>
#include <string>
#include <fstream>
#include "gtest/gtest.h"
#include "config_parser.h"

//Checks to see that toString works correctly
TEST(NginxConfigTest, ToString) {
	NginxConfigStatement statement;
	statement.tokens_.push_back("foo");
	statement.tokens_.push_back("bar");
	EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

//Creates test fixture for Parse (string input)
class NginxStringConfigTest:public testing::Test {
	protected:
		bool ParseString(const std::string config_string) {
			std::stringstream config_stream(config_string);
			return parser.Parse(&config_stream, &out_config);
		}
	NginxConfigParser parser;
	NginxConfig out_config;
};

//Checks to see that ParseString works correctly
TEST_F(NginxStringConfigTest, SimpleParserTest) {
	EXPECT_TRUE(ParseString("foo bar;"));
	EXPECT_EQ(1, out_config.statements_.size());
	EXPECT_EQ("foo", out_config.statements_[0]->tokens_[0]);
	EXPECT_EQ("bar", out_config.statements_[0]->tokens_[1]);
}

//Checks to see that ParseString returns errors
TEST_F(NginxStringConfigTest, InvalidConfig) {
	EXPECT_FALSE(ParseString("foo bar"));
}

//Checks to see that ParseString handles brackets correctly
TEST_F(NginxStringConfigTest, NestedConfig) {
	EXPECT_TRUE(ParseString("server { listen 80; }"));
}

//Checks to see that ParseString handles brackets correctly
//This passed although false was expected
//Changed config_parser to correct the bug
TEST_F(NginxStringConfigTest, UnbalancedConfig) {
	EXPECT_FALSE(ParseString("server { listen 80;"));
	EXPECT_FALSE(ParseString("server listen 80; }"));
}

//Creates text fixture for Parse (file name input)
class NginxConfigParserTest:public testing::Test {
	protected:
		bool ParseFile(const char* file_name) {
			std::ifstream config_file;
			config_file.open(file_name);
			bool ret = parser.Parse(dynamic_cast<std::istream*>(&config_file), &out_config);
			config_file.close();
			return ret;
		}
	NginxConfigParser parser;
	NginxConfig out_config;
};

//Checks that example_config is parsed correctly
TEST_F(NginxConfigParserTest, SimpleConfig) {
	EXPECT_TRUE(ParseFile("example_config"));
}

//Checks to see that Parse handles examples where there are multiple brackets
//Snippet of code taken from https://www.nginx.com/resources/wiki/start/topics/examples/full/
//This did not pass although true was expected
//Changed config_parser to fix the bug
TEST_F(NginxConfigParserTest, DoubleNestedConfig) {
	EXPECT_TRUE(ParseFile("snip_config"));
}

//Code taken from https://www.nginx.com/resources/wiki/start/topics/examples/full/
TEST_F(NginxConfigParserTest, FullExampleConfig) {
	EXPECT_TRUE(ParseFile("fullex_config"));
}
