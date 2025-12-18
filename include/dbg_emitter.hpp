#ifndef MBUILD_DEBUG_EMIT_H
#define MBUILD_DEBUG_EMIT_H

#include <emit.hpp>
#include <iomanip>
#include <iostream>

namespace mb {
class DebugEmit : public Emitter {
public:
    DebugEmit() {}
    ~DebugEmit() {}

    virtual void begin() {}
    virtual void edge(const std::string &rule, const std::string &out, const std::vector<std::string> &in) {
        std::cout << std::left << std::setw(10) << out << " -" << rule << "-> ";
        for (const auto &i : in) { std::cout << i << " "; }
        std::cout << "\n";
    }
    virtual void end() {}
};
}// namespace mb

#endif
