
#include <gtest/gtest.h>
#include <esl/yaml.hpp>

namespace yaml = esl::yaml;

std::string_view test_yaml_empty = R"(

)";

std::string_view test_yaml1 = R"(
k1: v1
k2: v2
kmap:
  mk1: mv1
  mkseq: &seq1
    - sv1
    - sv2
  mk2: mv2
  mk3: mv3
kseq: *seq1
)";

void test_yaml1_test(std::vector<yaml::node>& docs) {
	auto& doc = docs[0];
	ASSERT_EQ(doc.index(), yaml::map_index);

	auto& m = std::get<yaml::map>(doc);
	ASSERT_EQ(m.size(), 4);
	char k1[] = "k1";
	ASSERT_EQ(std::get<yaml::str>(m[k1]), "v1");
	ASSERT_EQ(std::get<yaml::str>(m["k2"]), "v2");

	auto& submap = m["kmap"];
	ASSERT_EQ(submap.index(), yaml::map_index);

	auto& sm = std::get<yaml::map>(submap);
	ASSERT_EQ(sm.size(), 4);
	ASSERT_EQ(std::get<yaml::str>(sm["mk1"]), "mv1");
	ASSERT_EQ(std::get<yaml::str>(sm["mk2"]), "mv2");
	ASSERT_EQ(std::get<yaml::str>(sm["mk3"]), "mv3");

	auto& subseq = sm["mkseq"];
	ASSERT_EQ(subseq.index(), yaml::seq_index);

	auto& ss = std::get<yaml::seq>(subseq);
	ASSERT_EQ(ss.size(), 2);
	ASSERT_EQ(std::get<yaml::str>(ss[0]), "sv1");
	ASSERT_EQ(std::get<yaml::str>(ss[1]), "sv2");

	auto& anchor_subseq = m["kseq"];
	ASSERT_EQ(anchor_subseq.index(), yaml::seq_index);

	auto& ass = std::get<yaml::seq>(anchor_subseq);
	ASSERT_EQ(ass.size(), 2);
	ASSERT_EQ(ass, ss);
}

TEST(YamlTest, load) {
	auto docs = yaml::load(test_yaml_empty);
	ASSERT_EQ(docs.size(), 0);
}

TEST(YamlTest, load_and_dump) {
	auto docs = yaml::load(test_yaml1);
	ASSERT_EQ(docs.size(), 1);

	test_yaml1_test(docs);

	auto s = yaml::dump(docs);
	ASSERT_TRUE(!s.empty());

	docs = yaml::load(s);
	ASSERT_EQ(docs.size(), 1);

	test_yaml1_test(docs);
}

TEST(YamlTest, error_yaml_load) {
	{
		const char* y = R"(
k1: v1
- v2
)";
		ASSERT_THROW(yaml::load(y), yaml::load_error);

		try {
			yaml::load(y);
		} catch (const yaml::load_error& e) {
			ASSERT_TRUE(e.what()[0] != '\0');
			ASSERT_GT(e.position(), 0u);
			ASSERT_EQ(e.line(), 2);
			ASSERT_EQ(e.column(), 0); // '-'
		}
	}
	{
		const char* y = R"(
k1: &m1
  kk1: v1
  kk2: v2
k2: *m2
k3: v3
)";
		ASSERT_THROW(yaml::load(y), yaml::load_error);

		try {
			yaml::load(y);
		} catch (const yaml::load_error& e) {
			ASSERT_TRUE(e.what()[0] != '\0');
			ASSERT_GT(e.position(), 0u);
			ASSERT_EQ(e.line(), 4);
			ASSERT_EQ(e.column(), 4); // '*'
		}
	}
}

