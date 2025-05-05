/*
petersonlock.c
*/
#include "petersonlock.h"
#include "param.h"
#include "types.h"
struct petersonlock plock[NPLOCK];
void petersonlock_init(void){

    struct petersonlock *p;
    
    for (p = plock; p < &plock[NPLOCK]; p++) {
        p->id = -1;
        p->flag[0] = 0;
        p->flag[1] = 0;
        p->turn = -1;
        p->role = 0;
        p->acquired = 0;
    }
}
int peterson_create(void){
    for(int j =0 ; j < NPLOCK; j++){
        if(plock[j].id == -1){
            plock[j].id = j;
            plock[j].flag[0] = 0;
            plock[j].flag[1] = 0;
            plock[j].turn = -1;
            plock[j].role = 0;
            return j;
        }
    }
    return -1;
}
int peterson_acquire(int lock_id, int role) {
    if (role != 0 && role != 1)
        return -1;
    if (lock_id < 0 || lock_id >= NPLOCK)
        return -1;

    struct petersonlock *lock = &plock[lock_id];

    if (lock->id == -1)
        return -1;

    __sync_lock_test_and_set(&lock->flag[role], 1);

    __sync_lock_test_and_set(&lock->turn, 1 - role);

    __sync_lock_test_and_set(&lock->acquired, 1);

    __sync_synchronize();

    while (lock->flag[1 - role] && lock->turn == 1 - role) {
        yield();

        __sync_synchronize();
    }

    return 0;
}
int peterson_release(int lock_id, int role){
    if (role != 0 && role != 1)
        return -1;
    if (lock_id < 0 || lock_id >= NPLOCK)
        return -1;

    struct petersonlock *lock = &plock[lock_id];

    if (lock->id == -1)
        return -1;

    __sync_lock_test_and_set(&lock->flag[role], 0);

    __sync_lock_test_and_set(&lock->acquired, 0);


    __sync_synchronize();

    return 0;
}
int peterson_destroy(int lock_id){

    if (lock_id < 0 || lock_id >= NPLOCK)
        return -1;

    struct petersonlock *lock = &plock[lock_id];

    if (lock->id == -1)
        return -1; // Already inactive

    // Clear the lock state
    lock->id = -1;
    lock->flag[0] = 0;
    lock->flag[1] = 0;
    lock->turn = -1;

    return 0;
}