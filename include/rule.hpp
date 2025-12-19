#ifndef MBUILD_RULE_HPP
#define MBUILD_RULE_HPP

#include <string>
#include <unordered_map>

namespace mb {
struct Rule {
    std::string                                  name;
    std::string                                  command;
    std::string                                  description;
    std::string                                  depFile;
    std::unordered_map<std::string, std::string> vars;
};
}// namespace mb

#endif
