/*
petersonlock.h
*/
# include "types.h"
struct petersonlock
{
  int id;
  int flag[2];
  int turn;
  int role;
  uint acquired;
};
void petersonlock_init(void);
int peterson_create(void);
int peterson_acquire(int lock_id, int role);
int peterson_release(int lock_id, int role);
int peterson_destroy(int lock_id);
