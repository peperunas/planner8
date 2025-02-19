/***
 * cpddl
 * --------
 * Copyright (c)2014 Daniel Fiser <danfis@danfis.cz>
 *
 *  This file is part of cpddl.
 *
 *  Distributed under the OSI-approved BSD License (the "License");
 *  see accompanying file BDS-LICENSE for details or see
 *  <http://www.opensource.org/licenses/bsd-license.php>.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the License for more information.
 */

#include "internal.h"
#include "pddl/fifo.h"

/** Just resets all pointers to NULL */
_pddl_inline void fifoReset(pddl_fifo_t *fifo);
/** Initializes pointer to the first element in block and the end of the
 *  block. */
_pddl_inline void fifoInitElPtrs(pddl_fifo_t *fifo, pddl_fifo_segm_t *segm,
                                 char **el, char **end);
/** Creates and initializes a new segment */
_pddl_inline pddl_fifo_segm_t *segmNew(pddl_fifo_t *fifo);

pddl_fifo_t *pddlFifoNew(size_t el_size)
{
    return pddlFifoNewSize(el_size, PDDL_FIFO_BUF_SIZE);
}

pddl_fifo_t *pddlFifoNewSize(size_t el_size, size_t buf_size)
{
    pddl_fifo_t *fifo;

    fifo = ALLOC(pddl_fifo_t);
    pddlFifoInitSize(fifo, el_size, buf_size);

    return fifo;
}

void pddlFifoDel(pddl_fifo_t *fifo)
{
    pddlFifoFree(fifo);
    FREE(fifo);
}

void pddlFifoInit(pddl_fifo_t *fifo, size_t el_size)
{
    pddlFifoInitSize(fifo, el_size, PDDL_FIFO_BUF_SIZE);
}

void pddlFifoInitSize(pddl_fifo_t *fifo, size_t el_size, size_t buf_size)
{
    fifo->segm_size = buf_size;
    fifo->el_size   = el_size;
    fifoReset(fifo);
}

void pddlFifoFree(pddl_fifo_t *fifo)
{
    pddlFifoClear(fifo);
}

void pddlFifoClear(pddl_fifo_t *fifo)
{
    pddl_fifo_segm_t *next;

    while (fifo->front){
        next = fifo->front->next;
        FREE(fifo->front);
        fifo->front = next;
    }
    fifoReset(fifo);
}

void pddlFifoPush(pddl_fifo_t *fifo, void *el_data)
{
    if (fifo->front == NULL){
        // FIFO is empty, so initialize everything
        fifo->front = segmNew(fifo);
        fifoInitElPtrs(fifo, fifo->front, &fifo->front_el, &fifo->front_end);

        fifo->back = fifo->front;
        fifo->back_el = fifo->front_el;
        fifo->back_end = fifo->front_end;

    }else if (fifo->back_el + fifo->el_size > fifo->back_end){
        // A next segment must be allocated
        fifo->back->next = segmNew(fifo);
        fifo->back = fifo->back->next;
        fifoInitElPtrs(fifo, fifo->back, &fifo->back_el, &fifo->back_end);
    }

    // Copy data to the buffer
    memcpy(fifo->back_el, el_data, fifo->el_size);
    fifo->back_el += fifo->el_size;
}

void pddlFifoPop(pddl_fifo_t *fifo)
{
    pddl_fifo_segm_t *next;

    if (!fifo->front)
        return;

    // Notice that we are not really removing an element we just move the
    // pointer and remove only the whole segment if necessary
    fifo->front_el += fifo->el_size;

    // Remove the front segment if no more elements can fit it or if have
    // pop'ed the last element.
    if (fifo->front_el + fifo->el_size > fifo->front_end
            || (fifo->front == fifo->back && fifo->front_el >= fifo->back_el)){

        // Remember the next segment
        next = fifo->front->next;

        // free the whole segment
        FREE(fifo->front);

        // Use the remembered segment and initialize front_el and front_end
        fifo->front = next;
        if (fifo->front != NULL){
            fifoInitElPtrs(fifo, fifo->front,
                           &fifo->front_el, &fifo->front_end);

        }else{
            fifoReset(fifo);
        }
    }
}

_pddl_inline void fifoReset(pddl_fifo_t *fifo)
{
    fifo->front = fifo->back = NULL;
    fifo->front_el = fifo->front_end = NULL;
    fifo->back_el = fifo->back_end = NULL;
}

_pddl_inline void fifoInitElPtrs(pddl_fifo_t *fifo, pddl_fifo_segm_t *segm,
                                 char **el, char **end)
{
    *el  = ((char *)segm) + sizeof(pddl_fifo_segm_t);
    *end = ((char *)segm) + fifo->segm_size;
}

_pddl_inline pddl_fifo_segm_t *segmNew(pddl_fifo_t *fifo)
{
    pddl_fifo_segm_t *segm;
    segm = (pddl_fifo_segm_t *)MALLOC(fifo->segm_size);
    segm->next = NULL;
    return segm;
}
