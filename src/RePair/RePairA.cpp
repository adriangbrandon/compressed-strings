/* RePair.cpp
 * Copyright (C) 2014, Francisco Claude & Rodrigo Canovas & Miguel A.
 * Martinez-Prieto all rights reserved.
 *
 * This class comprises some utilities for RePair compression and decompression.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Francisco Claude:  	fclaude@recoded.cl
 *   Rodrigo Canovas:  		rcanovas@student.unimelb.edu.au
 *   Miguel A. Martinez-Prieto:	migumar2@infor.uva.es
 */

#include "RePairA.h"

using namespace std;

RePairA::RePairA() {
    this->G = NULL;
    this->Cls = NULL;
    this->Cdac = NULL;
    this->maxchar = 0;
}

RePairA::RePairA(int *sequence, uint64_t length, uchar maxchar) {
    this->G = NULL;
    this->Cls = NULL;
    this->Cdac = NULL;
    this->maxchar = maxchar;

    std::string repair_file = "repair_" + std::to_string(getpid());
    std::string fileR = repair_file + ".R";

    repair_gn::build_grammar_irepair(sequence, length, 250*1024, repair_file.c_str());

    FILE *Tf;
    struct stat s;

    if (stat(fileR.c_str(), &s) != 0) {
        fprintf(stderr, "Error: cannot stat file %s\n", fileR.c_str());
        exit(1);
    }
    uint64_t n = s.st_size;
    Tf = fopen(fileR.c_str(), "r");
    if (Tf == NULL) {
        fprintf(stderr, "Error: cannot open file %s for reading\n", fileR.c_str());
        exit(1);
    }

    if (fread(&terminals, sizeof(int), 1, Tf) != 1) {
        fprintf(stderr, "Error: cannot read file %s\n", fileR.c_str());
        exit(1);
    }
    rules = (n - sizeof(int)) / sizeof(repair_gn::Tpair);
    auto res = (repair_gn::Tpair *) malloc(rules * sizeof(repair_gn::Tpair));
    if (fread(res, sizeof(repair_gn::Tpair), rules, Tf) != rules) {
        fprintf(stderr, "Error: cannot read file %s\n", fileR.c_str());
        exit(1);
    }
    fclose(Tf);


    // Building the array for the dictionary
    G = new LogSequence(bits(rules + terminals), 2 * rules);

    for (uint64_t i = 0; i < rules; i++) {
        G->setField(2 * i, res[i].left);
        G->setField((2 * i) + 1, res[i].right);
    }
}

uint RePairA::expandRule(uint rule, uchar *str) {
    uint pos = 0;
    uint left = G->getField(2 * rule);
    uint right = G->getField((2 * rule) + 1);

    if (left >= terminals)
        pos += expandRule(left - terminals, str + pos);
    else {
        str[pos] = (char)left;
        pos++;
    }

    if (right >= terminals)
        pos += expandRule(right - terminals, str + pos);
    else {
        str[pos] = (char)right;
        pos++;
    }

    return pos;
}

int RePairA::expandRuleAndCompareString(uint rule, uchar *str, uint *pos) {
    int cmp = 0;

    uint left = G->getField(2 * rule);
    if (left >= terminals) {
        cmp = expandRuleAndCompareString(left - terminals, str, pos);
        if (cmp != 0) return cmp;
    } else {
        if ((uchar)left != str[*pos]) return (int)((uchar)left - str[*pos]);
        (*pos)++;
    }

    uint right = G->getField((2 * rule) + 1);
    if (right >= terminals) {
        cmp = expandRuleAndCompareString(right - terminals, str, pos);
        if (cmp != 0) return cmp;
    } else {
        if ((uchar)right != str[*pos]) return (int)((uchar)right - str[*pos]);
        (*pos)++;
    }

    return cmp;
}

int RePairA::extractStringAndCompareRP(uint id, uchar *str, uint strLen) {
    str[strLen] = maxchar;

    uint l = 0, pos = 0, next;
    int cmp = 0;

    while (pos <= strLen) {
        next = Cls->getField(id + l);

        if (next >= terminals) {
            cmp = expandRuleAndCompareString(next - terminals, str, &pos);
            if (cmp != 0) break;
        } else {
            if ((uchar)next != str[pos]) return (int)((uchar)next - str[pos]);
            pos++;
        }

        l++;
    }

    str[strLen] = 0;

    return cmp;
}

int RePairA::extractStringAndCompareDAC(uint id, uchar *str, uint strLen) {
    uint l = 0, pos = 0, next;
    int cmp = 0;

    while (id != (uint)-1) {
        next = Cdac->access_next(l, &id);

        if (next >= terminals) {
            cmp = expandRuleAndCompareString(next - terminals, str, &pos);
            if (cmp != 0) return cmp;
        } else {
            if ((uchar)next != str[pos]) return (int)((uchar)next - str[pos]);
            pos++;
        }

        l++;
    }

    if (pos == strLen)
        return cmp;
    else
        return -str[pos];
}

int RePairA::expandRuleAndComparePrefixDAC(uint rule, uchar *str, uint *pos) {
    int cmp = 0;

    uint left = G->getField(2 * rule);
    if (left >= terminals) {
        cmp = expandRuleAndComparePrefixDAC(left - terminals, str, pos);
        if (cmp != 0) return cmp;
    } else {
        if ((uchar)left != str[*pos]) return (int)((uchar)left - str[*pos]);
        (*pos)++;
    }

    if (str[*pos] == '\0') return cmp;

    uint right = G->getField((2 * rule) + 1);
    if (right >= terminals) {
        cmp = expandRuleAndComparePrefixDAC(right - terminals, str, pos);
        if (cmp != 0) return cmp;
    } else {
        if ((uchar)right != str[*pos]) return (int)((uchar)right - str[*pos]);
        (*pos)++;
    }

    return cmp;
}

int RePairA::extractPrefixAndCompareDAC(uint id, uchar *prefix, uint prefixLen) {
    uint l = 0, pos = 0, next;
    int cmp = 0;

    while (id != (uint)-1) {
        next = Cdac->access_next(l, &id);

        if (next >= terminals) {
            cmp = expandRuleAndComparePrefixDAC(next - terminals, prefix, &pos);
            if (cmp != 0) return cmp;
        } else {
            if ((uchar)next != prefix[pos])
                return (int)((uchar)next - prefix[pos]);
            pos++;
        }

        if (prefix[pos] == '\0') return 0;

        l++;
    }

    if (pos == prefixLen)
        return cmp;
    else
        return -prefix[pos];
}

void RePairA::save(ofstream &out, uint encoding) {
    saveValue<uchar>(out, maxchar);
    saveValue<uint64_t>(out, terminals);
    saveValue<uint64_t>(out, rules);
    G->save(out);

    saveValue<uint32_t>(out, encoding);

    if ((encoding == HASHRPDAC) || (encoding == RPDAC))
        Cdac->save(out);
    else
        Cls->save(out);
}

void RePairA::save(ofstream &out) {
    saveValue<uchar>(out, maxchar);
    saveValue<uint64_t>(out, terminals);
    saveValue<uint64_t>(out, rules);
    G->save(out);
}

RePairA *RePairA::load(ifstream &in) {
    RePairA *dict = new RePairA();

    dict->maxchar = loadValue<uchar>(in);
    dict->terminals = loadValue<uint64_t>(in);
    dict->rules = loadValue<uint64_t>(in);
    dict->G = new LogSequence(in);

    uint encoding = loadValue<uint32_t>(in);

    if ((encoding == HASHRPDAC) || (encoding == RPDAC))
        dict->Cdac = DAC_VLS::load(in);
    else
        dict->Cls = new LogSequence(in);

    return dict;
}

RePairA *RePairA::loadNoSeq(ifstream &in) {
    RePairA *dict = new RePairA();

    dict->maxchar = loadValue<uchar>(in);
    dict->terminals = loadValue<uint64_t>(in);
    dict->rules = loadValue<uint64_t>(in);
    dict->G = new LogSequence(in);

    return dict;
}

size_t RePairA::getSize() {
    if (Cdac != NULL) return G->getSize() + Cdac->getSize() + sizeof(RePairA);
    if (Cls != NULL) return G->getSize() + Cls->getSize() + sizeof(RePairA);
    return G->getSize() + sizeof(RePairA);
}

RePairA::~RePairA() {
    delete G;
    if (Cls != NULL) delete Cls;
    if (Cdac != NULL) delete Cdac;
}
