#pragma once
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/**
 * $Id: 7d9be91afefb78027a96ae438fbe71ef25e7ed85 $
 *
 * @file atomic_queue.h
 * @brief Thread-safe queues.
 *
 * @copyright 2016 Alan DeKok <aland@freeradius.org>
 */
RCSIDH(atomic_queue_h, "$Id: 7d9be91afefb78027a96ae438fbe71ef25e7ed85 $")

#ifdef HAVE_WDOCUMENTATION
DIAG_OFF(documentation)
#endif
#include <talloc.h>
#ifdef HAVE_WDOCUMENTATION
DIAG_ON(documentation)
#endif
#include <stdbool.h>

#ifdef HAVE_STDATOMIC_H
#  include <stdatomic.h>
#else
#  include <freeradius-devel/stdatomic.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	Some macros to make our life easier.
 */
#define atomic_uint32_t _Atomic(uint32_t)

#define cas_incr(_store, _var)    atomic_compare_exchange_strong_explicit(&_store, &_var, _var + 1, memory_order_release, memory_order_relaxed)
#define cas_decr(_store, _var)    atomic_compare_exchange_strong_explicit(&_store, &_var, _var - 1, memory_order_release, memory_order_relaxed)
#define load(_var)           atomic_load_explicit(&_var, memory_order_relaxed)
#define aquire(_var)         atomic_load_explicit(&_var, memory_order_acquire)
#define store(_store, _var)  atomic_store_explicit(&_store, _var, memory_order_release);

typedef struct fr_atomic_queue_t fr_atomic_queue_t;

fr_atomic_queue_t	*fr_atomic_queue_create(TALLOC_CTX *ctx, int size);
bool			fr_atomic_queue_push(fr_atomic_queue_t *aq, void *data);
bool			fr_atomic_queue_pop(fr_atomic_queue_t *aq, void **p_data);

#ifndef NDEBUG
void			fr_atomic_queue_debug(fr_atomic_queue_t *aq, FILE *fp);
#endif


#ifdef __cplusplus
}
#endif
