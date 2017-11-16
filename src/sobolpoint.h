#pragma once
#ifndef SOBOL_POINT_H
#define SOBOL_POINT_H

#include <iostream>
#include <inttypes.h>

namespace MCQMCIntegration {
    bool get_sobol_base(std::istream& is,
                        uint32_t s, uint32_t m,  uint64_t base[]);
    bool get_interlaced_sobol_base(std::istream& is,
                                   uint32_t s, uint32_t m,  uint64_t base[]);
    int get_sobol_s_max();
    int get_sobol_s_min();
    int get_sobol_m_max();
    int get_sobol_m_min();
    int get_interlaced_sobol_s_max(const std::string& path, int alpha);
    int get_interlaced_sobol_s_min(const std::string& path, int alpha);
    int get_interlaced_sobol_m_max(const std::string& path, int alpha, int s);
    int get_interlaced_sobol_m_min(const std::string& path, int alpha, int s);
}
#endif //SOBOL_POINT_H
