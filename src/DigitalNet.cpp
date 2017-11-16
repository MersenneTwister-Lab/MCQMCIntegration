/**
 * @file DigitalNet.cpp
 *
 * @brief DigitalNet class for Quasi Monte-Carlo Method.
 *
 * @note Currently only 64-bit DigitalNet is tested.
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
#include "config.h"
#include "digital.h"
#include "bit_operator.h"
#include "sobolpoint.h"
#include <MCQMCIntegration/DigitalNet.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <sqlite3.h>

using namespace std;

/*
 * Unnamed NameSpace for file scope things.
 */
namespace {
    using namespace MCQMCIntegration;

    const int N = 64;

    const string digital_net_path = "DIGITAL_NET_PATH";
    struct digital_net_name {
        std::string name;
        std::string abb;
        std::string construction;
    };

    const digital_net_name digital_net_name_data[] = {
        {"NX", "nx", "Niederreiter-Xing"},
        {"Sobol", "sobolbase", "Sobol"},
        {"Old_Sobol", "oldso", "Old Sobol"},
        {"NX_LowWAFOM", "nxlw", "NX+LowWAFOM, CV = (max(CV) + min(CV))/2"},
        {"Sobol_LowWAFOM", "solw", "Sobol+LowWAFOM, CV = (max(CV) + min(CV))/2"},
        {"ISobol2", "sobol_alpha2", "Interlaced Sobol Alpha 2"},
        {"ISobol3", "sobol_alpha3", "Interlaced Sobol Alpha 3"},
        {"ISobol4", "sobol_alpha4", "Interlaced Sobol Alpha 4"},
        {"ISobol5", "sobol_alpha5", "Interlaced Sobol Alpha 5"},
        {"ISobol2LW", "soa2lw", "Interlaced Sobol Alpha 2 Low WAFOM"},
        {"ISobol3LW", "soa3lw", "Interlaced Sobol Alpha 3 Low WAFOM"},
        {"ISobol4LW", "soa4lw", "Interlaced Sobol Alpha 4 Low WAFOM"},
        {"ISobol5LW", "soa5lw", "Interlaced Sobol Alpha 5 Low WAFOM"},
    };

    const uint32_t digital_net_name_data_size = 9; // to be 13

    const char * getDataPath()
    {
        return getenv(digital_net_path.c_str());
    }

    const string makePath(const string& name, const string& ext)
    {
        string path;
        const char * cpath = getDataPath();
        if (cpath == NULL) {
            path = "../data";
        } else {
            path = cpath;
        }
#if __cplusplus >= 201103L
        if (path.back() != '/') {
            path += "/";
        }
#else
        path += "/";
#endif
        path += name;
        path += ext;
        return path;
    }

    template<typename U>
    int readSobolBase(const string& path, uint32_t s, uint32_t m, U base[])
    {
        ifstream ifs(path, ios::in | ios::binary);
        if (!ifs) {
            cerr << "can't open:" << path << endl;
            return -1;
        }
        uint64_t data[s * m];
        bool r = get_sobol_base(ifs, s, m, data);
        if (!r) {
            return -1;
        }
        if (sizeof(U) * 8  == 32) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                         & UINT32_C(0xffffffff));
            }
        } else {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i];
            }
        }
        return 0;
    }

    template<typename U>
    int readInterlacedSobolBase(const string& path, uint32_t s, uint32_t m,
                                U base[])
    {
        ifstream ifs(path, ios::in | ios::binary);
        if (!ifs) {
            cerr << "can't open:" << path << endl;
            return -1;
        }
        uint64_t data[s * m];
        bool r = get_interlaced_sobol_base(ifs, s, m, data);
        if (!r) {
            return -1;
        }
        if (sizeof(U) * 8  == 32) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                         & UINT32_C(0xffffffff));
            }
        } else {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i];
            }
        }
        return 0;
    }

#if 0
    template<typename U>
    int selectSobolBase(const string& path, uint32_t s, uint32_t m, U base[])
    {
        uint64_t data[s * m];
        //int bitsize = sizeof(U) * 8;
        //bool r = select_sobol_base(path, bitsize, s, m, data);
        bool r = select_sobol_base(path, s, m, data);
        if (!r) {
            return -1;
        }
        if (sizeof(U) * 8  == 32) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                         & UINT32_C(0xffffffff));
            }
        } else {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i];
            }
        }
        return 0;
    }
#endif
    template<typename U>
    int read_digital_net_data(std::istream& is, int n,
                              uint32_t s, uint32_t m,
                              U base[],
                              int64_t * tvalue, double * wafom)
    {
        uint64_t data[s * m];
        uint64_t tmp;
        uint32_t i = 0;
        uint32_t j = 0;
        for (i = 0; i < m; i++) {
            for (j = 0; j < s; j++) {
                if (!is) {
                    cerr << "too less data i = " << dec << i
                         << " j = " << j
                         << " s = " << s << " m = " << m
                         << endl;
                    return -1;
                }
                is >> tmp;
                data[i * s + j] = tmp;
            }
        }
        if (i * s + j < s * m) {
            cerr << "too less data i = " << dec << i
                 << " s = " << s << " m = " << m
                 << endl;
            return -1;
        }
        if (is) {
            is >> *wafom;
        }
        if (is) {
            is >> *tvalue;
        }
        if (n == sizeof(U) * 8) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>(data[i]);
            }
        } else if (n == 32) { // U is uint64_t
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i] << 32;
            }
        } else { // n == 64 U is uint32_t
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                        & UINT32_C(0xffffffff));
            }
        }
        return 0;
    }

#if 0
    template<typename U>
    int read_digital_net_data(DigitalNetID id, uint32_t s, uint32_t m,
                              U base[],
                              int * tvalue, double * wafom) {
#if defined(DEBUG)
        cout << "in read_digital_net_data" << endl;
#endif
        string name = digital_net_name_data[id].abb;
        string path = makePath(name, ".dat");
        if (id == SOBOL) {
            return readSobolBase(path, s, m, base);
        }
#if defined(DEBUG)
        cout << "fname = " << path << endl;
#endif
        const char * mode = "rb";
        size_t count;
        errno = 0;
        FILE *fp = fopen(path.c_str(), mode);
        if (fp == NULL) {
            cerr << path.c_str() << ":" << strerror(errno) << endl;
            //throw runtime_error("can't open");
            throw "can't open";
        }
        uint64_t dmy;
        count = fread(&dmy, sizeof(uint64_t), 1, fp);
        if (count != 1) {
            cerr << "fail to read magic number" << endl;
            //throw runtime_error("fail to read magic number");
            throw "fail to read magic number";
        }
        if (dmy != DIGITAL_MAGIC) {
            cerr << "magic number mismatch" << endl;
            //throw runtime_error("magic number mismatch");
            throw "magic number mismatch";
        }
        digital_net_header_t header;
        for (;;) {
            count = fread(&header, sizeof(digital_net_header_t), 1, fp);
            if (count != 1) {
                cerr << "fail to read header s = "
                     << dec << s << " m = " << m << endl;
                //throw runtime_error("can't read header");
                throw "can't read header";
            }
            if (header.s > s && header.m > m) {
                cerr << "header s = " << dec << header.s << " m = " << header.m
                     << endl;
                //throw runtime_error("can't find s and m in header");
                throw "can't find s and m in header";
            }
            if (header.s == s && header.m == m) {
                break;
            }
        }
        fseek(fp, header.pos, SEEK_SET);
        size_t size = s * m * sizeof(uint64_t);
        digital_net_data_t dn;
        count = fread(&dn, sizeof(digital_net_data_t), 1, fp);
        if (count != 1) {
            cerr << "fail to read digital net data" << endl;
            //throw runtime_error("fail to read digital net data");
            throw "fail to read digital net data";
        }
        uint64_t data[s * m];
        count = fread(data, size, 1, fp);
        if (count != 1) {
            cout << "fail to read digtal net array" << endl;
            //throw runtime_error("fail to read digtal net array");
            throw "fail to read digtal net array";
        }
#if defined(DEBUG)
        cout << "read_digital_net_data after read" << endl;
        cout << "data:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
#endif
        *wafom = dn.wafom;
        *tvalue = dn.tvalue;
        //uint64_t * data = dn.data;
        if (sizeof(U) * 8  == 32) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                         & UINT32_C(0xffffffff));
            }
        } else {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i];
            }
        }
#if defined(DEBUG)
        cout << "out read_digital_net_data" << endl;
        cout << "base:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << base[i] << " ";
        }
        cout << endl;
#endif
        return 0;
    }
#endif
    template<typename U>
    int select_digital_net_data(DigitalNetID id, uint32_t s, uint32_t m,
                                U base[],
                                int64_t * tvalue, double * wafom) {
#if defined(DEBUG)
        cout << "in select_digital_net_data" << endl;
#endif
        string name = digital_net_name_data[id].abb;
        string path = makePath("digitalnet", ".sqlite3");
#if 0
        if (id == SOBOL) {
            return selectSobolBase(path, s, m, base);
        }
#endif
#if defined(DEBUG)
        cout << "dbname = " << path << endl;
#endif
        int bit = sizeof(U) * 8;
        // prepare sql
        int r = 0;
        sqlite3 *db;
        r = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
        if (r != SQLITE_OK) {
            cout << "sqlite3_open error code = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return -1;
        }
        string strsql = "select wafom, tvalue, data";
        strsql += " from digitalnet ";
        strsql += " where netname = ? "; // 1
        strsql += "and bitsize = ? ";    // 2
        strsql += "and dimr = ? ";       // 3
        strsql += "and dimf2 = (select min(dimf2) from digitalnet ";
        strsql += "where netname = ? "; // 4
        strsql += "and bitsize = ? ";   // 5
        strsql += "and dimr = ? ";      // 6
        strsql += "and dimf2 >= ?);";   // 7
        sqlite3_stmt* select_sql = NULL;
        stringstream ssbase;
        r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
        if (r != SQLITE_OK) {
            cout << "sqlite3_prepare error 1 code = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            r = sqlite3_close_v2(db);
            return -2;
        }
        if (select_sql == NULL) {
            cout << "sqlite3_prepare null statement" << endl;
            r = sqlite3_close_v2(db);
            return -3;
        }
        do {
            r = sqlite3_bind_text(select_sql, 1, name.c_str(),
                                  -1, SQLITE_STATIC);
            if (r != SQLITE_OK) {
                cout << "error bind netname r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_bind_int(select_sql, 2, bit);
            if (r != SQLITE_OK) {
                cout << "error bind bitsize r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_bind_int(select_sql, 3, s);
            if (r != SQLITE_OK) {
                cout << "error bind dimr r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_bind_text(select_sql, 4, name.c_str(),
                                  -1, SQLITE_STATIC);
            if (r != SQLITE_OK) {
                cout << "error bind netname r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_bind_int(select_sql, 5, bit);
            if (r != SQLITE_OK) {
                cout << "error bind bitsize r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_bind_int(select_sql, 6, s);
            if (r != SQLITE_OK) {
                cout << "error bind dimr r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_bind_int(select_sql, 7, m);
            if (r != SQLITE_OK) {
                cout << "error bind dimf2 r = " << dec << r << endl;
                cout << sqlite3_errmsg(db) << endl;
                return r;
            }
            r = sqlite3_step(select_sql);
            if (r != SQLITE_ROW) {
                cout << "not found" << endl;
                cout << "netname = " << name << endl;
                return r;
            }
            if (sqlite3_column_type(select_sql, 0) == SQLITE_NULL) {
                *wafom = NAN;
            } else {
                *wafom = sqlite3_column_double(select_sql, 0);
            }
            if (sqlite3_column_type(select_sql, 1) == SQLITE_NULL) {
                *tvalue = -1;
            } else {
                *tvalue = sqlite3_column_int(select_sql, 1);
            }
            char * tmp = (char *)sqlite3_column_text(select_sql, 2);
            ssbase << tmp;
        } while (false);
        // release sql
        r = sqlite3_finalize(select_sql);
        if (r != SQLITE_OK) {
            cout << "error finalize r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
        }
        sqlite3_close_v2(db);
        if (r != SQLITE_OK) {
            return r;
        }
        for (size_t i = 0; i < s * m; i++) {
            ssbase >> base[i];
        }
#if defined(DEBUG)
        cout << "out select_digital_net_data" << endl;
        cout << "base:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << base[i] << " ";
        }
        cout << endl;
#endif
        return 0;
    }

    int get_s_minmax(const string& path, DigitalNetID id, int * min, int * max)
    {
        // db open
        string name = digital_net_name_data[id].abb;
        sqlite3 *db;
        int r = 0;
        r = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
        if (r != SQLITE_OK) {
            cout << "sqlite3_open error code = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return -1;
        }
        sqlite3_stmt *select_sql = NULL;
        //r = select_bind(db, &select_sql);
        string strsql = "select min(dimr), max(dimr) from digitalnet ";
        strsql += "where netname = ?;";
        stringstream ssbase;
        r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
        if (r != SQLITE_OK) {
            cout << "sqlite3_prepare error 2 code = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            cout << "strsql =|" << strsql << "|" << endl;
            cout << "path = " << path << endl;
            r = sqlite3_close_v2(db);
            return -2;
        }
        if (select_sql == NULL) {
            cout << "sqlite3_prepare null statement" << endl;
            r = sqlite3_close_v2(db);
            return -3;
        }
        r = sqlite3_bind_text(select_sql, 1, name.c_str(),
                              -1, SQLITE_STATIC);
        if (r != SQLITE_OK) {
            cout << "error bind netname r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            r = sqlite3_close_v2(db);
            return -4;
        }
        r = sqlite3_step(select_sql);
        if (r != SQLITE_ROW) {
            cout << "not found" << endl;
            cout << "netname = " << name << endl;
            r = sqlite3_close_v2(db);
            return -5;
        }
        int s_min = sqlite3_column_int(select_sql, 0);
        int s_max = sqlite3_column_int(select_sql, 1);
        r = sqlite3_finalize(select_sql);
        if (r != SQLITE_OK) {
            cout << "error finalize r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
        }
        sqlite3_close_v2(db);
        *min = s_min;
        *max = s_max;
        return s_min;
    }

    int get_m_minmax(const string& path, DigitalNetID id,
                     int s, int * min, int * max)
    {
        // db open
        string name = digital_net_name_data[id].abb;
        sqlite3 *db;
        int r = 0;
        r = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
        if (r != SQLITE_OK) {
            cout << "sqlite3_open error code = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return -1;
        }
        sqlite3_stmt *select_sql = NULL;
        //r = select_bind(db, &select_sql);
        string strsql = "select min(dimf2), max(dimf2) from digitalnet ";
        strsql += "where netname = ? and dimr = ?;";
        stringstream ssbase;
        r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
        if (r != SQLITE_OK) {
            cout << "sqlite3_prepare error 3 code = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            r = sqlite3_close_v2(db);
            return -2;
        }
        if (select_sql == NULL) {
            cout << "sqlite3_prepare null statement" << endl;
            r = sqlite3_close_v2(db);
            return -3;
        }
        r = sqlite3_bind_text(select_sql, 1, name.c_str(),
                              -1, SQLITE_STATIC);
        if (r != SQLITE_OK) {
            cout << "error bind netname r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            r = sqlite3_close_v2(db);
            return -4;
        }
        r = sqlite3_bind_int(select_sql, 2, s);
        if (r != SQLITE_OK) {
            cout << "error bind netname r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            r = sqlite3_close_v2(db);
            return -5;
        }
        r = sqlite3_step(select_sql);
        if (r != SQLITE_ROW) {
            cout << "not found" << endl;
            cout << "netname = " << name << endl;
            r = sqlite3_close_v2(db);
            return -6;
        }
        int m_min = sqlite3_column_int(select_sql, 0);
        int m_max = sqlite3_column_int(select_sql, 1);
        r = sqlite3_finalize(select_sql);
        if (r != SQLITE_OK) {
            cout << "error finalize r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
        }
        sqlite3_close_v2(db);
        *min = m_min;
        *max = m_max;
        return m_max;
    }
}


namespace MCQMCIntegration {
    uint32_t getSMax(DigitalNetID id)
    {
        string path = makePath("digitalnet", ".sqlite3");
        switch (id) {
        case SOBOL:
            return get_sobol_s_max();
        case ISOBOL_A2:
            return get_interlaced_sobol_s_max(path, 2);
        case ISOBOL_A3:
            return get_interlaced_sobol_s_max(path, 3);
        case ISOBOL_A4:
            return get_interlaced_sobol_s_max(path, 4);
        case ISOBOL_A5:
            return get_interlaced_sobol_s_max(path, 5);
        default:
            int min = 0;
            int max = 0;
            int r = get_s_minmax(path, id, &min, &max);
            if (r < 0) {
                return r;
            } else {
                return max;
            }
        }
    }

    uint32_t getSMin(DigitalNetID id)
    {
        string path = makePath("digitalnet", ".sqlite3");
        switch (id) {
        case SOBOL:
            return get_sobol_s_min();
        case ISOBOL_A2:
            return get_interlaced_sobol_s_min(path, 2);
        case ISOBOL_A3:
            return get_interlaced_sobol_s_min(path, 3);
        case ISOBOL_A4:
            return get_interlaced_sobol_s_min(path, 4);
        case ISOBOL_A5:
            return get_interlaced_sobol_s_min(path, 5);
        default:
            int min = 0;
            int max = 0;
            int r = get_s_minmax(path, id, &min, &max);
            if (r < 0) {
                return r;
            } else {
                return min;
            }
        }
    }

    uint32_t getMMax(DigitalNetID id, uint32_t s)
    {
        string path = makePath("digitalnet", ".sqlite3");
        switch (id) {
        case SOBOL:
            return get_sobol_m_max();
        case ISOBOL_A2:
            return get_interlaced_sobol_m_max(path, 2, s);
        case ISOBOL_A3:
            return get_interlaced_sobol_m_max(path, 3, s);
        case ISOBOL_A4:
            return get_interlaced_sobol_m_max(path, 4, s);
        case ISOBOL_A5:
            return get_interlaced_sobol_m_max(path, 5, s);
        default:
            int min = 0;
            int max = 0;
            int r = get_m_minmax(path, id, s, &min, &max);
            if (r < 0) {
                return r;
            } else {
                return max;
            }
        }
    }

    uint32_t getMMin(DigitalNetID id, uint32_t s)
    {
        string path = makePath("digitalnet", ".sqlite3");
        switch (id) {
        case SOBOL:
            return get_sobol_m_min();
        case ISOBOL_A2:
            return get_interlaced_sobol_m_min(path, 2, s);
        case ISOBOL_A3:
            return get_interlaced_sobol_m_min(path, 3, s);
        case ISOBOL_A4:
            return get_interlaced_sobol_m_min(path, 4, s);
        case ISOBOL_A5:
            return get_interlaced_sobol_m_min(path, 5, s);
        default:
            int min = 0;
            int max = 0;
            int r = get_m_minmax(path, id, s, &min, &max);
            if (r < 0) {
                return r;
            } else {
                return min;
            }
        }
    }

    const string getDigitalNetName(uint32_t index)
    {
        if (index < digital_net_name_data_size) {
            return digital_net_name_data[index].name;
        } else {
            return "";
        }
    }

    int readDigitalNetHeader(std::istream& is, int * n,
                             uint32_t * s, uint32_t * m)
    {
        is >> *n;
        is >> *s;
        is >> *m;
        if (is.good()) {
            return 0;
        } else {
            return -1;
        }
    }

/**
 * Constructor from input stream
 *
 * File Format:
 * separator: white space, blank char, tab char, cr, lf, etc.
 * the first item: bit size, integer fixed to 64, currently.
 * the second item: s, unsigned integer.
 * the third item: m, unsigned integer.
 * from fourth: s * m number of 64-bit unsigned integers.
 * the last but one: wafom double precision number, optional.
 * the last: t-value, integer, optional.
 * @param is input stream, from where digital net data are read.
 * @exception runtime_error, when can't read data from is.
 */

    int readDigitalNetData(std::istream& is, int n,
                           uint32_t s, uint32_t m,
                           uint64_t base[],
                           int64_t * tvalue, double * wafom)
    {
        return read_digital_net_data(is, n, s, m, base, tvalue, wafom);
    }

    int readDigitalNetData(std::istream& is, int n,
                           uint32_t s, uint32_t m,
                           uint32_t base[],
                           int64_t * tvalue, double * wafom)
    {
        return read_digital_net_data(is, n, s, m, base, tvalue, wafom);
    }

/**
 * Constructor from reserved data
 *
 * file are searched from environment variable DIGITAL_NET_PATH
 *
 * @param name name of digital net
 * @param s s value
 * @param m m value
 * @exception runtime_error, when can't read data from is.
 */
    int readDigitalNetData(DigitalNetID id, uint32_t s, uint32_t m,
                           uint64_t base[],
                           int64_t * tvalue, double * wafom)
    {
        //return read_digital_net_data(id, s, m, base, tvalue, wafom);
        string name = digital_net_name_data[id].abb;
        string path = makePath(name, ".dat");
        switch (id) {
        case SOBOL:
            return readSobolBase(path, s, m, base);
        case ISOBOL_A2:
        case ISOBOL_A3:
        case ISOBOL_A4:
        case ISOBOL_A5:
            path = makePath(name, "_Bs53.col");
            return readInterlacedSobolBase(path, s, m, base);
        default:
            return select_digital_net_data(id, s, m, base, tvalue, wafom);
        }
    }

    int readDigitalNetData(DigitalNetID id, uint32_t s, uint32_t m,
                           uint32_t base[],
                           int64_t * tvalue, double * wafom)
    {
        //return read_digital_net_data(id, s, m, base, tvalue, wafom);
        string name = digital_net_name_data[id].abb;
        string path = makePath(name, ".dat");
        switch (id) {
        case SOBOL:
            return readSobolBase(path, s, m, base);
        case ISOBOL_A2:
        case ISOBOL_A3:
        case ISOBOL_A4:
        case ISOBOL_A5:
            path = makePath(name, "_Bs53.col");
            return readInterlacedSobolBase(path, s, m, base);
        default:
            return select_digital_net_data(id, s, m, base, tvalue, wafom);
        }
    }

    void DigitalNet<uint64_t>::pointInitialize() {
#if defined(DEBUG)
        using namespace std;
        cout << "in pointInitialize" << endl;
#endif
#if 0
        if (sizeof(U) * 8 == 64) {
            get_max = 64 - 53;
            factor = exp2(-53);
            eps = exp2(-64);
        } else {
            get_max = 0;
            factor = exp2(-32);
            eps = exp2(-33);
        }
#endif
        get_max = 64 - 53;
        factor = exp2(-53);
        eps = exp2(-64);
        if (shift == NULL) {
            shift = new uint64_t[s]();
        }
        if (point_base == NULL) {
            point_base = new uint64_t[s]();
        }
        if (point == NULL) {
            point = new double[s]();
        }
        for (uint32_t i = 0; i < s; ++i) {
            point_base[i] = 0;
        }
        if (digitalShift) {
            for (uint32_t i = 0; i < s; ++i) {
                shift[i] = mt();
            }
        } else {
            for (uint32_t i = 0; i < s; ++i) {
                shift[i] = 0;
            }
        }
        grayindex.clear();
        count = 0;
        count++;
        convertPoint();
#if defined(DEBUG)
        cout << "out pointInitialize" << endl;
#endif
    }

    void DigitalNet<uint64_t>::nextPoint() {
#if defined(DEBUG)
        using namespace std;
        cout << "in nextPoint" << endl;
#endif
        if (count == (UINT64_C(1) << m)) {
            pointInitialize();
            //return;
        }
        int bit = grayindex.index();
#if defined(DEBUG)
        cout << "bit = " << bit << endl;
        cout << "before boint_base:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << point_base[i] << " ";
        }
        cout << endl;
#endif
        for (uint32_t i = 0; i < s; ++i) {
            point_base[i] ^= getBase(bit, i);
        }
        convertPoint();
        if (count == (UINT64_C(1) << m)) {
            count = 0;
            grayindex.clear();
        } else {
            grayindex.next();
            count++;
        }
#if defined(DEBUG)
        cout << "after boint_base:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << point_base[i] << " ";
        }
        cout << endl;
        cout << "out nextPoint" << endl;
#endif
    }

    void DigitalNet<uint64_t>::convertPoint() {
        for (uint32_t i = 0; i < s; i++) {
            // shift して1を立てている
            uint64_t tmp = (point_base[i] ^ shift[i]) >> get_max;
            point[i] = static_cast<double>(tmp) * factor + eps;
        }
    }

    void DigitalNet<uint64_t>::linearScramble() {
        const size_t N = 64;
        uint64_t LowTriMat[N];
        uint64_t tmp;
        const uint64_t one = 1;
        for (size_t i = 0; i < s; i++) {
            // 正則な下三角行列を作る
            for (size_t j = 0; j < N; j++) {
                uint64_t p2 = one << (N - j - 1);
                LowTriMat[j] = (mt() << (N - j - 1)) | p2;
            }
            for (size_t k = 0; k < m; k++) {
                tmp = UINT64_C(0);
                for (size_t j = 0; j < N; j++) {
                    uint64_t bit = innerProduct(LowTriMat[j], getBase(k, i));
                    tmp ^= bit << (N - j - 1);
                }
                setBase(k, i, tmp);
            }
        }
    }

    DigitalNet<uint64_t>::GrayIndex::GrayIndex() {
        count = 1;
    }
    void DigitalNet<uint64_t>::GrayIndex::clear() {
        count = 1;
    }
    void DigitalNet<uint64_t>::GrayIndex::next() {
        count++;
    }
    int DigitalNet<uint64_t>::GrayIndex::index() {
        return tailingZeroBit(count);
    }

/**
 * Constructor from input stream
 *
 * File Format:
 * separator: white space, blank char, tab char, cr, lf, etc.
 * the first item: bit size, integer fixed to 64, currently.
 * the second item: s, unsigned integer.
 * the third item: m, unsigned integer.
 * from fourth: s * m number of 64-bit unsigned integers.
 * the last but one: wafom double precision number, optional.
 * the last: t-value, integer, optional.
 * @param is input stream, from where digital net data are read.
 * @exception runtime_error, when can't read data from is.
 */
    DigitalNet<uint64_t>::DigitalNet(std::istream& is) {
        //using namespace std;
        int n;
        //id = -100;
        int r = readDigitalNetHeader(is, &n, &s, &m);
        if (r != 0) {
            //throw std::runtime_error("data type mismatch!");
            throw "data type mismatch!";
        }
        base = new uint64_t[s * m]();
        r = readDigitalNetData(is, n, s, m, base,
                               &tvalue, &wafom);
        if (r != 0) {
            //throw std::runtime_error("data type mismatch!");
            throw "data type mismatch!";
        }
        shift = NULL;
        point_base = NULL;
        point = NULL;
        count = 0;
        digitalShift = false;
        pointInitialize();
    }

/**
 * Constructor from reserved data
 *
 * file are searched from environment variable DIGITAL_NET_PATH
 *
 * @param name name of digital net
 * @param s s value
 * @param m m value
 * @exception runtime_error, when can't read data from is.
 */
    DigitalNet<uint64_t>::DigitalNet(DigitalNetID id,
                                     uint32_t s, uint32_t m)
    {
        this->s = s;
        this->m = m;
        //this->id = static_cast<int>(id);
        base = new uint64_t[s * m]();
        int r = readDigitalNetData(id, s, m, base,
                                   &tvalue, &wafom);
        if (r != 0) {
            //throw runtime_error("data type mismatch!");
            throw "data type mismatch!";
        }
        shift = NULL;
        point_base = NULL;
        point = NULL;
        count = 0;
        digitalShift = false;
        pointInitialize();
    }

    DigitalNet<uint64_t>::~DigitalNet()
    {
#if defined(DEBUG)
        cout << "DigitalNet DEBUG: before delete[] base" << endl;
#endif
        delete[] base;
        delete[] shift;
        if (point_base != NULL) {
#if defined(DEBUG)
            cout << "DigitalNet DEBUG: before delete[] point_base" << endl;
#endif
            delete[] point_base;
        }
        if (point != NULL) {
#if defined(DEBUG)
            cout << "DigitalNet DEBUG: before delete[] point" << endl;
#endif
            delete[] point;
        }
#if defined(DEBUG)
        cout << "DigitalNet DEBUG: after deconstructor" << endl;
#endif
    }

    void DigitalNet<uint64_t>::showStatus(std::ostream& os)
    {
        os << "n = " << N << endl;
        os << "s = " << s << endl;
        os << "m = " << m << endl;
        for (uint32_t k = 0; k < m; ++k) {
            for (uint32_t i = 0; i < s; ++i) {
                os << "base[" << k << "][" << i << "] = "
                   << getBase(k, i) << ' ';
                os << ' ';
            }
            os << endl;
        }
        os << "WAFOM-value = " << wafom << endl;
        os << "t-value = " << tvalue << endl;
    }

    void DigitalNet<uint64_t>::setSeed(uint64_t seed)
    {
        mt.seed(seed);
    }

#if 0
    const char * DigitalNet<uint64_t>::getDataPath()
    {
        return getenv(digital_net_path.c_str());
    }

    uint32_t DigitalNet<uint64_t>::getParameterSize()
    {
        return digital_net_name_data_size;
    }

    const string DigitalNet<uint64_t>::getDigitalNetName(uint32_t index)
    {
        if (index < digital_net_name_data_size) {
            return digital_net_name_data[index].name;
        } else {
            return "";
        }
    }
    const string DigitalNet<uint64_t>::getDigitalNetConstruction(uint32_t index)
    {
        if (index < digital_net_name_data_size) {
            return digital_net_name_data[index].construction;
        } else {
            return "";
        }
    }
#endif
}
