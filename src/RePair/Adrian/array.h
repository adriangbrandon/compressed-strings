
/*

Repair -- an implementation of Larsson and Moffat's compression and
decompression algorithms.
Copyright (C) 2010-current_year Gonzalo Navarro

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Gonzalo Navarro, Dept. of Computer Science, University of
Chile. Blanco Encalada 2120, Santiago, Chile. gnavarro@dcc.uchile.cl

*/

	// extendible array for pairs

#ifndef ARRAYINCLUDED
#define ARRAYINCLUDED

#include "basics.h"

namespace repair_gn{

    typedef struct
    {   int64_t *pairs; // identifiers
        int64_t maxsize;
        int64_t size;
        int64_t fst; // first of circular array
        float factor;
        int64_t minsize;
        void *Rec; // records
    } Tarray;

// contents can be accessed as A.pairs[0..A.size-1]

    int64_t insertArray (Tarray *A, int64_t pair); // inserts pair in A, returns pos

    void deleteArray (Tarray *A); // deletes last cell in A

    Tarray createArray (void *Rec, float factor, int64_t minsize); // creates empty array

    void destroyArray (Tarray *A); // destroys A
}

#include "records.h"
#endif
