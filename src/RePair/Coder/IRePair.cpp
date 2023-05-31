
/*

IRePair -- an implementation of Larsson and Moffat's compression and
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

#include "IRePair.h"

#include <stdlib.h>

int IRePair::compress(int *t, uint64_t length, size_t *csymbols,
                      size_t *crules, Tdiccarray **rules, int aMB) {
    text = t;
    MB = aMB;
    prepare0(length);
    length = repair0(length);
    prepare(length);
    repair();

    // DOES NOT OBTAIN THE COMPRESSED SEQUENCE!!!!!!!!!!!!!!!!!!!
    // Returns the original sequence...

    free(L);
    Heap::destroyHeap(&Heap);
    HashRP::destroyHash(&Hash);

    // Linking results with function parameters
    *csymbols = alph;
    *crules = n - alph;
    *rules = &Dicc;
   /* t = (int*) malloc(u * sizeof(int));
    relong i = 0;
    while(i < u){
        t[i] = (int) C[i];
        ++i;
    }*/

    return 0;
}

relong IRePair::repair0(relong len) {

    int oid,id;
    relong cpos,pos;
    Trecord *rec,*orec;
    Tpair pair;
    int left,right;

    while (n+1 > 0)
    {
        if ((len /1024/1024) * 3 * sizeof(relong) <= MB) return len;
        else if (PRNP) printf ("Avoiding to use %lli MB\n",
                               (len /1024/1024) * 3 * sizeof(relong));
        if (PRNR) prnRec();
        oid = Heap::extractMax(&Heap);
        if (oid == -1) break; // the end!!
        orec = &Rec.records[oid];

        // Adding a new rule to the dictionary
        int lrule = 0;

        if (orec->pair.left < alph)
            lrule++;
        else
            lrule += Dicc.rules[orec->pair.left - alph].l;

        if (orec->pair.right < alph)
            lrule++;
        else
            lrule += Dicc.rules[orec->pair.right - alph].l;

        Trule nrule = {orec->pair, lrule};
        Dictionary::insertRule(&Dicc, nrule);

        if (PRNP)
        { printf("Chosen pair %i = (",n);
            prnSym(orec->pair.left);
            printf(",");
            prnSym(orec->pair.right);
            printf(") (%lli occs)\n",orec->freq);
        }
        pos = 0;
        for (cpos=0;cpos<len-1;cpos++)
        { if ((text[cpos] != left) || (text[cpos+1] != right))
                text[pos] = text[cpos];
            else // occurrence of the pair to modify
            {   // decr freqs of pairs that disappear
                if (pos > 0)
                { pair.left = text[pos-1]; pair.right = text[cpos];
                    id = HashRP::searchHash(Hash,pair);
                    if (id != -1) // may not exist if purgeHeap'd
                    { if (id != oid) Heap::decFreq(&Heap,id); //not to my pair!
                    }
                }
                if (cpos < len-2)
                { pair.left = text[cpos+1]; pair.right = text[cpos+2];
                    id = HashRP::searchHash(Hash,pair);
                    if (id != -1) // may not exist if purgeHeap'd
                    { if (id != oid) Heap::decFreq(&Heap,id); //not to my pair!
                    }
                }
                // create or incr freqs of pairs that appear
                if (pos > 0)
                { pair.left = text[pos-1];
                    pair.right = n;
                    id = HashRP::searchHash(Hash,pair);
                    if (id == -1) // new pair, insert
                    { id = Records::insertRecord (&Rec,pair);
                    }
                    else
                    { Heap::incFreq (&Heap,id);
                    }
                }
                if (cpos < len-2)
                { pair.left = n; pair.right = text[cpos+2];
                    id = HashRP::searchHash(Hash,pair);
                    if (id == -1) // new pair, insert
                    { id = Records::insertRecord (&Rec,pair);
                    }
                    else
                    { Heap::incFreq (&Heap,id);
                    }
                }
                // actually do the substitution
                text[pos] = n; cpos++;
            }
            pos++;
        }
        if (cpos == len-1) text[pos++] = text[cpos];
        len = pos;
        Records::removeRecord (&Rec,oid);
        n++;
        Heap::purgeHeap(&Heap); // remove freq 1 from heap
        text = (int*)realloc ((int*)text,len*sizeof(int)); // should not move it
    }
    Heap::purgeHeap(&Heap); // remove freq 1 from heap, if it exited for oid=-1
    return len;
}

relong IRePair::repair() {
    int oid, id;
    relong cpos;
    Trecord *rec, *orec;
    Tpair pair;

    if (PRNC) prnC();
    while (n + 1 > 0) {
        if (PRNR) prnRec();
        oid = Heap::extractMax(&Heap);
        if (oid == -1) break;  // the end!!
        orec = &Rec.records[oid];
        cpos = orec->cpos;

        // Adding a new rule to the dictionary
        int lrule = 0;

        if (orec->pair.left < alph)
            lrule++;
        else
            lrule += Dicc.rules[orec->pair.left - alph].l;

        if (orec->pair.right < alph)
            lrule++;
        else
            lrule += Dicc.rules[orec->pair.right - alph].l;

        Trule nrule = {orec->pair, lrule};
        Dictionary::insertRule(&Dicc, nrule);

        if (PRNP) {
            printf("Chosen pair %i = (", n);
            prnSym(orec->pair.left);
            printf(",");
            prnSym(orec->pair.right);
            printf(") (%lu occs)\n", orec->freq);
        }
        while (cpos != -1) {
            relong ant, sgte, ssgte;
            // replacing bc->e in abcd, b = cpos, c = sgte, d = ssgte
            if (C[cpos + 1] < 0)
                sgte = -C[cpos + 1] - 1;
            else
                sgte = cpos + 1;
            if ((sgte + 1 < static_cast<int64_t>(u)) && (C[sgte + 1] < 0))
                ssgte = -C[sgte + 1] - 1;
            else
                ssgte = sgte + 1;
            // remove bc from L
            if (L[cpos].next != -1) L[L[cpos].next].prev = -oid - 1;
            orec->cpos = L[cpos].next;
            if (ssgte != static_cast<int64_t>(u))  // there is ssgte
            {                                      // remove occ of cd
                pair.left = C[sgte];
                pair.right = C[ssgte];
                id = HashRP::searchHash(Hash, pair);
                if (id != -1)  // may not exist if purgeHeap'd
                {
                    if (id != oid) Heap::decFreq(&Heap, id);  // not to my pair!
                    if (L[sgte].prev != NullFreq)  // still exists(not removed)
                    {
                        rec = &Rec.records[id];
                        if (L[sgte].prev < 0)  // this cd is head of its list
                            rec->cpos = L[sgte].next;
                        else
                            L[L[sgte].prev].next = L[sgte].next;
                        if (L[sgte].next != -1)  // not tail of its list
                            L[L[sgte].next].prev = L[sgte].prev;
                    }
                }
                // create occ of ed
                pair.left = n;
                id = HashRP::searchHash(Hash, pair);
                if (id == -1)  // new pair, insert
                {
                    id = Records::insertRecord(&Rec, pair);
                    rec = &Rec.records[id];
                    L[cpos].next = -1;
                } else {
                    Heap::incFreq(&Heap, id);
                    rec = &Rec.records[id];
                    L[cpos].next = rec->cpos;
                    L[L[cpos].next].prev = cpos;
                }
                L[cpos].prev = -id - 1;
                rec->cpos = cpos;
            }
            if (cpos != 0)  // there is ant
            {               // remove occ of ab
                if (C[cpos - 1] < 0) {
                    ant = -C[cpos - 1] - 1;
                    if (ant == cpos)  // sgte and ant clashed -> 1 hole
                        ant = cpos - 2;
                } else
                    ant = cpos - 1;
                pair.left = C[ant];
                pair.right = C[cpos];
                id = HashRP::searchHash(Hash, pair);
                if (id != -1)  // may not exist if purgeHeap'd
                {
                    if (id != oid) Heap::decFreq(&Heap, id);  // not to my pair!
                    if (L[ant].prev != NullFreq)  // still exists (not removed)
                    {
                        rec = &Rec.records[id];
                        if (L[ant].prev < 0)  // this ab is head of its list
                            rec->cpos = L[ant].next;
                        else
                            L[L[ant].prev].next = L[ant].next;
                        if (L[ant].next != -1)  // it is not tail of its list
                            L[L[ant].next].prev = L[ant].prev;
                    }
                }
                // create occ of ae
                pair.right = n;
                id = HashRP::searchHash(Hash, pair);
                if (id == -1)  // new pair, insert
                {
                    id = Records::insertRecord(&Rec, pair);
                    rec = &Rec.records[id];
                    L[ant].next = -1;
                } else {
                    Heap::incFreq(&Heap, id);
                    rec = &Rec.records[id];
                    L[ant].next = rec->cpos;
                    L[L[ant].next].prev = ant;
                }
                L[ant].prev = -id - 1;
                rec->cpos = ant;
            }
            C[cpos] = n;
            if (ssgte != u) C[ssgte - 1] = -cpos - 1;
            C[cpos + 1] = -ssgte - 1;
            c--;
            orec = &Rec.records[oid];  // just in case of Rec.records realloc'd
            cpos = orec->cpos;
        }
        if (PRNC) prnC();
        Records::removeRecord(&Rec, oid);
        n++;
        Heap::purgeHeap(&Heap);  // remove freq 1 from heap

         if (c < factor * u) // compact C
            { relong i,ni;
              i = 0;
              for (ni=0;ni<c-1;ni++)
                { C[ni] = C[i];
                  L[ni] = L[i];
                  if (L[ni].prev < 0)
                     { if (L[ni].prev != NullFreq) // real ptr
                          Rec.records[-L[ni].prev-1].cpos = ni;
                     }
                  else L[L[ni].prev].next = ni;
                  if (L[ni].next != -1) L[L[ni].next].prev = ni;
                  i++; if (C[i] < 0) i = -C[i]-1;
                }
              C[ni] = C[i];
              u = c;
              C = (relong*) realloc (C, c * sizeof(relong));
              L = (Tlist*)realloc (L, c * sizeof(Tlist));
          Records::assocRecords (&Rec,&Hash,&Heap,L);
            }
    }
    return 0;
}
void IRePair::prepare0 (relong len)

{ relong i;
    int id;
    Tpair pair;
    alph = 0;
    for (i=0;i<len;i++)
    { if (text[i] > alph) alph = text[i];
    }
    n = ++alph;
    Rec = Records::createRecords(factor,minsize);
    Heap = Heap::createHeap(len,&Rec,factor,minsize);
    Hash = HashRP::createHash(256*256,&Rec);
    Records::assocRecords (&Rec,&Hash,&Heap,NULL);
    if ((len /1024/1024) * 3 * sizeof(relong) <= MB) return;
    did0 = 1;
    for (i=0;i<len-1;i++)
    { pair.left = text[i]; pair.right = text[i+1];
        id = HashRP::searchHash (Hash,pair);
        if (id == -1) // new pair, insert
        { id = Records::insertRecord (&Rec,pair);
        }
        else
        { Heap::incFreq (&Heap,id);
        }
        if (PRNL && (i%1000000 == 0)) printf ("Processed %lli ints\n",i);
    }
    Heap::purgeHeap (&Heap);
}

void IRePair::prepare(relong len) {
    relong i;
    int id;
    Tpair pair;
    c = u = len;
    C = (relong*)malloc(u*sizeof(relong));
    for (i=0;i<u;i++) C[i] = text[i];
    free (text);

    alph = 0;
    for (i = 0; i < u; i++) {
        if (C[i] > alph) alph = C[i];
    }
    n = ++alph;

    Rec = Records::createRecords(factor, minsize);
    Heap = Heap::createHeap(u, &Rec, factor, minsize);
    Hash = HashRP::createHash(256 * 256, &Rec);
    L = (Tlist *)malloc(u * sizeof(Tlist));
    Records::assocRecords(&Rec, &Hash, &Heap, L);
    Dicc = Dictionary::createDicc(factor, minsize);

    for (i = 0; i < c - 1; i++) {
        pair.left = C[i];
        pair.right = C[i + 1];
        id = HashRP::searchHash(Hash, pair);
        if (id == -1)  // new pair, insert
        {
            if (did0) L[i].prev = NullFreq;
            else {
                id = Records::insertRecord (&Rec,pair);
                L[i].prev = -id-1;
                Rec.records[id].cpos = i;
            }
            L[i].next = -1;
        } else {
            if (Rec.records[id].cpos == -1) // first time I see it this pass
            { L[i].next = -1;
            }
            else
            { L[i].next = Rec.records[id].cpos;
                L[L[i].next].prev = i;
                if (!did0) Heap::incFreq (&Heap,id);
            }
            L[i].prev = -id-1;
            Rec.records[id].cpos = i;
        }
    }
    L[i].prev = NullFreq; L[i].next = -1;
    Heap::purgeHeap(&Heap);
}

void IRePair::prnSym(int c) { printf("%i", c); }

void IRePair::prnC(void) {
    uint64_t i = 0;
    printf("C[1..%lu] = ", c);
    while (i < u) {
        prnSym(C[i]);
        printf(" ");
        i++;
        if ((i < u) && (C[i] < 0)) i = -C[i] - 1;
    }
    printf("\n\n");
    // printf ("Physical C[1..%i] = ",u);
    // for (i=0;i<u;i++)
    //       { prnSym(C[i]);
    //  	printf (" ");
    //       }
    //     printf ("\n\n");
}

void IRePair::prnRec(void) {
    int i;
    printf("Active pairs:\n");
    for (i = 0; i < Rec.size; i++) {
        printf("\t(");
        prnSym(Rec.records[i].pair.left);
        printf(",");
        prnSym(Rec.records[i].pair.right);
        printf("), %lu occs\n", Rec.records[i].freq);
    }
    printf("\n");
}
