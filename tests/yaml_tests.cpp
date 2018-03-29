
#include <gtest/gtest.h>
#include <esl/yaml.hpp>

TEST(YamlTest, parse) {
	std::string_view s = R"(
k1: v1
k2: v2
kmap:
  mk1: mv1
  mkseq:
    - sv1
    - sv2
  mk2: mv2
  mk3: mv3
)";
	auto docs = esl::yaml::parse(s.data(), s.size());
	ASSERT_EQ(docs.size(), 1);

	auto& doc = docs[0];
	ASSERT_EQ(doc.index(), esl::yaml::map_index);

	auto& m = std::get<esl::yaml::map>(doc);
	ASSERT_EQ(m.size(), 3);
	ASSERT_EQ(std::get<esl::yaml::str>(m["k1"]), "v1");
	ASSERT_EQ(std::get<esl::yaml::str>(m["k2"]), "v2");

	auto& submap = m["kmap"];
	ASSERT_EQ(submap.index(), esl::yaml::map_index);

	auto& sm = std::get<esl::yaml::map>(submap);
	ASSERT_EQ(sm.size(), 4);
	ASSERT_EQ(std::get<esl::yaml::str>(sm["mk1"]), "mv1");
	ASSERT_EQ(std::get<esl::yaml::str>(sm["mk2"]), "mv2");
	ASSERT_EQ(std::get<esl::yaml::str>(sm["mk3"]), "mv3");

	auto& subseq = sm["mkseq"];
	ASSERT_EQ(subseq.index(), esl::yaml::seq_index);

	auto& ss = std::get<esl::yaml::seq>(subseq);
	ASSERT_EQ(ss.size(), 2);
	ASSERT_EQ(std::get<esl::yaml::str>(ss[0]), "sv1");
	ASSERT_EQ(std::get<esl::yaml::str>(ss[1]), "sv2");
}

