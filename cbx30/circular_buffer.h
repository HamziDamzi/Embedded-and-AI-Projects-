#ifndef __GENERIC_CIRCULAR_BUFFER__
#define __GENERIC_CIRCULAR_BUFFER__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


struct _circ_buff_t;

typedef void (*circ_buff_lock_cb_t)(struct _circ_buff_t *gcb, void *lock);
typedef void (*circ_buff_signal_set_cb_t)(struct _circ_buff_t *gcb, void *signal);
typedef bool (*circ_buff_signal_wait_cb_t)(struct _circ_buff_t *gcb, void *signal, int timeout);

/// circular buffer object
typedef struct _circ_buff_t {
    // circ buff stuff
    uint8_t *mem;           /**< memory block used by the buffer */
    size_t mem_sz;          /**< memory block size */
    size_t rd_ofs;          /**< read offset */
    size_t wr_ofs;          /**< write offset */

    // locking and signaling
    void *lock;             /**< a private value for the locking functions */
    void *signal;           /**< a private value for the signalling functions */
    circ_buff_lock_cb_t global_lock;        /**< lock function */
    circ_buff_lock_cb_t global_unlock;      /**< unlock function */
    circ_buff_signal_set_cb_t signal_set;   /**< signal raising function */
    circ_buff_signal_wait_cb_t signal_wait; /**< signal waiting function */
} circ_buff_t;

void circ_buff_init(circ_buff_t *gcb, void *mem, size_t mem_sz);
void circ_buff_locking_init(circ_buff_t *gcb,
                            circ_buff_lock_cb_t global_lock,
                            circ_buff_lock_cb_t global_unlock,
                            void *lock);
void circ_buff_signaling_init(circ_buff_t *gcb,
                              circ_buff_signal_set_cb_t signal_set,
                              circ_buff_signal_wait_cb_t signal_wait,
                              void *signal);
void circ_buff_clear(circ_buff_t *gcb);
bool circ_buff_push(circ_buff_t *gcb, uint8_t d, int timeout);
int circ_buff_pop(circ_buff_t *gcb, int timeout);
size_t circ_buff_get_used_size(circ_buff_t *gcb);
size_t circ_buff_get_free_size(circ_buff_t *gcb);

#endif
