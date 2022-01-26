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

#include "arrayg.h"

#include <stdlib.h>

#include "records.h"

int64_t ArrayG::insertArray(Tarray *A, int64_t pair) {
    int64_t *npairs;
    int64_t max, size, i, pos, id, fst;
    Trecord *rec = ((Trarray *)A->Rec)->records;
    if (A->size == A->maxsize) {
        if (A->maxsize == 0) {
            A->maxsize = A->minsize;
            A->pairs = (int64_t *)malloc(A->maxsize * sizeof(int64_t));
            A->fst = 0;
        } else {
            max = A->maxsize;
            A->maxsize /= A->factor;
            npairs = (int64_t *)malloc(A->maxsize * sizeof(int64_t));
            size = A->size;
            fst = A->fst;
            for (i = 0; i < size; i++) {
                id = A->pairs[fst];
                npairs[i] = id;
                rec[id].hpos = i;
                fst = (fst + 1) % max;
            }
            free(A->pairs);
            A->pairs = npairs;
            A->fst = 0;
        }
    }
    pos = (A->fst + A->size) % A->maxsize;
    A->pairs[pos] = pair;
    A->size++;
    return pos;
}

void ArrayG::deleteArray(Tarray *A) {
    int64_t *npairs;
    int64_t size, i, id, max, fst;
    Trecord *rec = ((Trarray *)A->Rec)->records;
    A->fst = (A->fst + 1) % A->maxsize;
    A->size--;
    if (A->size == 0) {
        A->maxsize = 0;
        free(A->pairs);
        A->pairs = NULL;
        A->fst = 0;
    } else if ((A->size < A->maxsize * A->factor * A->factor) &&
               (A->maxsize * A->factor >= A->minsize)) {
        max = A->maxsize;
        A->maxsize *= A->factor;
        npairs = (int64_t *)malloc(A->maxsize * sizeof(int64_t));
        size = A->size;
        fst = A->fst;
        for (i = 0; i < size; i++) {
            id = A->pairs[fst];
            npairs[i] = id;
            rec[id].hpos = i;
            fst = (fst + 1) % max;
        }
        free(A->pairs);
        A->pairs = npairs;
        A->fst = 0;
    }
}

Tarray ArrayG::createArray(void *Rec, float factor, int64_t minsize) {
    Tarray A;
    A.Rec = Rec;
    A.pairs = NULL;
    A.maxsize = 0;
    A.size = 0;
    A.fst = 0;
    A.factor = factor;
    A.minsize = minsize;
    return A;
}

void ArrayG::destroyArray(Tarray *A) {
    if (A->maxsize == 0) return;
    free(A->pairs);
    A->pairs = NULL;
    A->maxsize = 0;
    A->size = 0;
    A->fst = 0;
}