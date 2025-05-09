#ifndef _PETERSONLOCK_H_
#define _PETERSONLOCK_H_

struct petersonlock {
  int id;
  int flag[2];
  int turn;
  int role;
};

void petersonlock_init(void);
int peterson_create(void);
int peterson_acquire(int lock_id, int role);
int peterson_release(int lock_id, int role);
int peterson_destroy(int lock_id);

#endif // _PETERSONLOCK_H_
