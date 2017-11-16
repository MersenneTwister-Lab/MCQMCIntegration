#pragma once
#ifndef MCQMC_INTEGRATION_DIGITAL_NET_H
#define MCQMC_INTEGRATION_DIGITAL_NET_H
/**
 * @file DigitalNet.h
 *
 * @brief DigitalNet class for Quasi Monte-Carlo Method.
 *
 * @note Currently only 64-bit DigitalNet is implemented.
 *
 * @author Shinsuke Mori (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 * @author Mutsuo Saito
 *
 * Copyright (C) 2017 Shinsuke Mori, Makoto Matsumoto, Mutsuo Saito
 * and Hiroshima University.
 * All rights reserved.
 *
 * The GPL ver.3 is applied to this software, see
 * COPYING
 */

#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <cerrno>
#include <random>

namespace MCQMCIntegration {
    /**
     * ID of pre-defined Digital Net.
     */
    enum DigitalNetID {
        /** Niederreiter-Xing Point Set. */
        NX = 0,
        /** Sobol Point Set up to dimension 21201. */
        SOBOL = 1,
        /** Niederreiter-Xing point set of Low WAFOM. */
        NXLW = 3,
        /** Sobol point set of Low WAFOM. */
        SOLW = 4,
        /** Interlaced Sobol Alpha 2 */
        ISOBOL_A2 = 5,
        /** Interlaced Sobol Alpha 3 */
        ISOBOL_A3 = 6,
        /** Interlaced Sobol Alpha 4 */
        ISOBOL_A4 = 7,
        /** Interlaced Sobol Alpha 5 */
        ISOBOL_A5 = 8,
        /** Interlaced Sobol Alpha 2 of Low WAFOM */
        ISOBOL_A2_LW = 9,
        /** Interlaced Sobol Alpha 3 of Low WAFOM */
        ISOBOL_A3_LW = 10,
        /** Interlaced Sobol Alpha 4 of Low WAFOM */
        ISOBOL_A4_LW = 11,
        /** Interlaced Sobol Alpha 5 of Low WAFOM */
        ISOBOL_A5_LW = 12
    };

    /**
     * get maximum number of pre-defined digital net dimension @b s.
     * @return maximum number of pre-defined digital net dimension @b s.
     */
    uint32_t getSMax(DigitalNetID id);

    /**
     * get minimum number of pre-defined digital net dimension @b s.
     * @return minimum number of pre-defined digital net dimension @b s.
     */
    uint32_t getSMin(DigitalNetID id);

    /**
     * get maximum number of pre-defined digital net F2 dimension @b m.
     * @return maximum number of pre-defined digital net F2 dimension @b m.
     */
    uint32_t getMMax(DigitalNetID id, uint32_t s);

    /**
     * get minimum number of pre-defined digital net F2 dimension @b m.
     * @return minimum number of pre-defined digital net F2 dimension @b m.
     */
    uint32_t getMMin(DigitalNetID id, uint32_t s);

    const std::string getDigitalNetName(uint32_t index);
    /**
     * Digital Net class for Quasi Mote-Carlo Method.
     * This class is almost dummy.
     *
     * @tparam T uint32_t of uint64_t, currently uint64_t is specialized.
     */
    template<typename T>
        class DigitalNet {
    public:
        static const char * getDataPath();
        static uint32_t getParameterSize();
        static const std::string getDigitalNetConstruction(uint32_t index);
    private:
        ~DigitalNet();
        DigitalNet(int s, int m);
        T dmy;
    };

    /**
     * Digital Net class for Quasi Mote-Carlo Method.
     *
     * Explicit specialization for 64-bit unsigned integer.
     */
    template<>
        class DigitalNet<uint64_t> {
    private:
        // First of all, forbid copy and assign.
        DigitalNet(const DigitalNet<uint64_t>& that);
        DigitalNet<uint64_t>& operator=(const DigitalNet<uint64_t>&);
        class GrayIndex {
        public:
            GrayIndex();
            void clear();
            void next();
            int index();
        private:
            uint64_t count;
        };

    public:
        /**
         * constructor from stream.
         *
         * FORMAT:
         * @li separators are white spaces or newlines.
         * @li 1st element : 64 fixed.
         * @li 2nd element : @b s, dimension of point set.
         * @li 3rd element : @b m, F2 dimension of element of point set.
         * @li 4th -       : @b s * @b m elements of 64-bit integers.
         * @li last but one: WAFOM value, optional.
         * @li last        : t-value, optional.
         * @throw runtime_error when can't read from stream.
         */
        DigitalNet(std::istream& is);

        /**
         * constructor from pre-defined data.
         *
         * DigitalNetID:
         * @li NXLW : Niederreiter-Xing low WAFOM up to dimension 10.
         * @li SOBOL: Sobol Point Set up to dimension 21201.
         * @li SOLW : Sobol low WAFOM up to dimension 10.
         * @param[in] id ID of pre-defined digital net.
         * @param[in] s dimension of point set, s should be 4 <= s
         * @param[in] m F2 dimension of element of point set, m should be
         * 10 <= m <= 18.
         */
        DigitalNet(DigitalNetID id, uint32_t s, uint32_t m);

        /**
         * destructor.
         */
        ~DigitalNet();

        /**
         * get an element of base matrix of generating point set.
         * @param[in] i row
         * @param[in] j column
         * @return an element of base matrix of generating point set.
         */
        uint64_t getBase(int i, int j) const {
            return base[i * s + j];
        }

        /**
         * get a component of a point vector.
         * @param[in] i get i-th component.
         * @return a component of a point vector.
         */
        double getPoint(int i) const {
            return point[i];
        }

        /**
         * get a point vector.
         * @return a point vector.
         */
        const double * getPoint() const {
            return point;
        }

        /**
         * get dimension of digital net.
         * @return dimension of digital net.
         */
        uint32_t getS() const {
            return s;
        }

        /**
         * get F2 dimension of element of digital net.
         * @return F2 dimension of element of digital net.
         */
        uint32_t getM() const {
            return m;
        }

        /**
         * show internal status.
         * @param[in,out] os output stream
         */
        void showStatus(std::ostream& os);

        /**
         * (re-)initialize point.
         */
        void pointInitialize();

        /**
         * state transition to next point.
         */
        void nextPoint();

        void setDigitalShift(bool value) {
            digitalShift = value;
        }
        /**
         * set seed for random number generator for scramble.
         */
        void setSeed(uint64_t seed);

        /**
         * get WAFOM value if exist.
         * @return WAFOM value.
         */
        double getWAFOM() {
            return wafom;
        }

        /**
         * get t-value if exist.
         * @return t-value
         */
        int64_t getTvalue() {
            return tvalue;
        }
        void linearScramble();
    private:
        void setBase(int i, int j, uint64_t value) {
            base[i * s + j] = value;
        }
        void convertPoint();
        uint32_t s;
        uint32_t m;
        uint64_t *shift;
        uint64_t count;
        bool digitalShift;
        int get_max;
        double factor;
        double eps;
        double wafom;
        int64_t tvalue;
        GrayIndex grayindex;
        std::mt19937_64 mt;
        uint64_t * base;
        uint64_t * point_base;
        double * point;
    };
}
#endif // MCQMC_INTEGRATION_DIGITAL_NET_H
