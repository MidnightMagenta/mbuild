#ifndef MBUILD_EMIT_H
#define MBUILD_EMIT_H

#include <string>
#include <types.hpp>
#include <vector>

namespace mb {
class Emitter {
public:
    Emitter() {}
    virtual ~Emitter() {}

    virtual void begin()                                                       = 0;
    virtual void global_var(const std::string &name, const std::string &value) = 0;
    virtual void rule(const Rule &rule)                                        = 0;
    virtual void edge(const std::string              &rule,
                      const std::vector<std::string> &out,
                      const std::vector<std::string> &in)                      = 0;
    virtual void end()                                                         = 0;

private:
};
}// namespace mb

#endif
