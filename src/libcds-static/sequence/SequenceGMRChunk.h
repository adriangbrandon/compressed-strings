/* SequenceGMRChunk.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * gmr_chunk
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

#ifndef SEQUENCEGMRCHUNK_H
#define SEQUENCEGMRCHUNK_H

#include <cassert>
#include <iostream>

#include "libcds-static/bitsequence/BitSequence.h"
#include "libcds-static/bitsequence/BitSequenceBuilder.h"
#include "libcds-static/permutation/Permutation.h"
#include "libcds-static/permutation/PermutationBuilder.h"
#include "libcds-static/sequence/Sequence.h"
#include "libcds-utils/libcdsBasics.h"

namespace cds_static {
// using namespace std;

/** Implementation of the Chunk of Golynski et al's rank/select
 * data structure [1].
 *
 * [1] A. Golynski and I. Munro and S. Rao.
 * Rank/select operations on large alphabets: a tool for text indexing.
 * SODA 06.
 *
 * @author Francisco Claude
 */
class SequenceGMRChunk : public Sequence {
   public:
    /** Builds the structures needed for the chunk
     * @param sequence sequence as array of unsigned integers.
     * @param chunk_length length of the sequence.
     * @param bmb bitmap builder for X (as named in the paper).
     * @param pmb permutation builder for Pi (as named in the paper).
     * */
    SequenceGMRChunk(uint *sequence, uint chunk_length, BitSequenceBuilder *bmb,
                     PermutationBuilder *pmb);
    SequenceGMRChunk(const Array &sequence, BitSequenceBuilder *bmb,
                     PermutationBuilder *pmb);

    /** Destroy the chunk */
    ~SequenceGMRChunk();

    virtual uint access(size_t j) const;
    virtual size_t select(uint i, size_t j) const;
    virtual size_t rank(uint i, size_t j) const;
    virtual size_t getSize() const;
    virtual void save(ofstream &fp) const;
    static SequenceGMRChunk *load(ifstream &fp);

   protected:
    /** Bitmap */
    BitSequence *X;
    /** Permutation */
    Permutation *permutation;
    /** Size of the alphabet */
    uint sigma;

    SequenceGMRChunk();
};
}  // namespace cds_static
#endif
