/* wt_node_leaf.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * wt_node_leaf
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
 */

#include "libcds-static/sequence/wt_node_leaf.h"

namespace cds_static {

wt_node_leaf::wt_node_leaf(uint symbol, size_t count) {
    this->symbol = symbol;
    this->count = count;
}

wt_node_leaf::wt_node_leaf() {}

wt_node_leaf::~wt_node_leaf() {}

size_t wt_node_leaf::rank(uint symbol, size_t pos, uint /* l */,
                          wt_coder * /* c */) const {
    if (symbol != this->symbol) {
        return 0;
    }
    pos++;
    return pos;
}

size_t wt_node_leaf::select(uint symbol, size_t pos, uint /* l */,
                            wt_coder * /* c */) const {
    if (symbol != this->symbol) return (size_t)-1;
    if (pos == 0 || pos > count) return (size_t)-1;
    return pos;
}

uint wt_node_leaf::access(size_t /* pos */) const { return symbol; }

uint wt_node_leaf::access(size_t pos, size_t &rank) const {
    rank = pos + 1;
    return symbol;
}

size_t wt_node_leaf::getSize() const { return sizeof(wt_node_leaf); }

void wt_node_leaf::save(ofstream &fp) const {
    uint wr = WT_NODE_LEAF_HDR;
    saveValue(fp, wr);
    saveValue(fp, count);
    saveValue(fp, symbol);
}

wt_node_leaf *wt_node_leaf::load(ifstream &fp) {
    uint rd = loadValue<uint>(fp);
    if (rd != WT_NODE_LEAF_HDR) return NULL;
    wt_node_leaf *ret = new wt_node_leaf();
    ret->count = loadValue<size_t>(fp);
    ret->symbol = loadValue<uint>(fp);
    return ret;
}
}  // namespace cds_static
