#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Global variables
int proc_index ;      
int num_levels = 0;       
int total_locks = 0;      
int *locks = 0;           

// int sum_of_powers_of_two(int number) ;
int is_power_of_two(int x) {

  return x > 0 && (x & (x - 1)) == 0;
}

int tournament_create(int processes) {

  if (!is_power_of_two(processes) || processes > 16) {
    return -1;
  }
  proc_index = -1;
  // Calculate tree height
  int L = 0;
  int n = processes;
  while (n >>= 1) {
    L++;
  }

  num_levels = L;
  total_locks = processes - 1;
  locks = malloc(sizeof(int) * total_locks);
  if (!locks) return -1;

  // Create all Peterson locks
  for (int i = 0; i < total_locks; i++) {
    locks[i] = peterson_create();
    if (locks[i] < 0) return -1;
  }

  // Fork (processes - 1) times to create N processes
  for (int i = 0; i < processes - 1; i++) {
    
    int pid = fork();
    if (pid < 0) {
      return -1;
    }

    if (pid == 0) {
      // Child process
      proc_index = i + 1;
      break;
    } else {
      // Parent continues
      continue;
    }
  }

  // Parent has index 0
  if (proc_index == -1) proc_index = 0;
  return proc_index;
}

int tournament_acquire(void) {
  if (proc_index < 0 || !locks || num_levels <= 0)
    return -1;
  for (int l = num_levels - 1; l >= 0; l--) {
    // Compute role at level l
    int bit_pos = num_levels - l - 1;
    int role = (proc_index & (1 << bit_pos)) >> bit_pos;
    // Compute lock index in locks[] (BFS layout)
    int lock_l = proc_index >> (num_levels - l);
    int lock_index = lock_l + (1<<l)-1; // lock[l] + sum_{k=0}^{l-1} 2^k
    if (peterson_acquire(locks[lock_index], role) < 0){
      return -1;
    }
  }

  return 0;
}
int tournament_release(void) {
  if (proc_index < 0 || !locks || num_levels <= 0)
    return -1;

  for (int l = 0; l < num_levels; l++) {
    int bit_pos = num_levels - l - 1;
    int role = (proc_index & (1 << bit_pos)) >> bit_pos;

    int lock_l = proc_index >> (num_levels - l);
    int lock_index = lock_l + ((1 << l) - 1);

    if (peterson_release(locks[lock_index], role) < 0)
      return -1;
  }

  return 0;
}
// int sum_of_powers_of_two(int number) {
//   if (number <= 0) return 1;

//   int sum = 0;
//   for (int k = 0; k < number; k++) {
//     sum += (1 << k); // 2^k is equivalent to 1 << k
//   }

//   return sum;
// }