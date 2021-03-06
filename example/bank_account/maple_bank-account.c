/** 
 * 
 * Authors - Jie Yu (jieyu@umich.edu)
 *	     Markus J. Kusano (mukusano@vt.edu)
 *
 *  The original code was written by Jie Yu using C++ features not found in C.
 *  Markus Kusano changed the code to be able to be compiled by GCC.
 *
 *  See README.md for more information
 */

#include <stdlib.h>
#include <pthread.h>

/* Comment out to insert calls to assert() */
//#define ASSERT

/* Comment out to disable calls to printf() */
#define DEBUG

#ifdef ASSERT
#include <assert.h>
#endif

#ifdef DEBUG
#include <stdio.h>
#endif

// typedefs
typedef struct {
  int balance;
  pthread_mutex_t lock;
} bank_account_type;

int get_balance(bank_account_type *account) {
  int ret = 0;
  pthread_mutex_lock(&account->lock);
  ret = account->balance;
  pthread_mutex_unlock(&account->lock);
  return ret;
}

void set_balance(bank_account_type *account, int balance) {
  pthread_mutex_lock(&account->lock);
  account->balance = balance;
  pthread_mutex_unlock(&account->lock);
}

void withdraw(bank_account_type *account, int amount) {
  int curr_balance = 0;
  curr_balance = get_balance(account);
  curr_balance -= amount;
  set_balance(account, curr_balance);
}

void deposit(bank_account_type *account, int amount) {
  int curr_balance = 0;
  curr_balance = get_balance(account);
  curr_balance += amount;
  set_balance(account, curr_balance);
}

void init_account(bank_account_type *account) {
  account->balance = 0;
  pthread_mutex_init(&account->lock, NULL);
}

static int amount = 20;

void *t1_main(void *args) {
  bank_account_type *account = (bank_account_type *)args;
  printf("t1 is depositing %d\n", amount);
  deposit(account, amount);
  printf("deposit done\n");
  return NULL;
}

void *t2_main(void *args) {
  bank_account_type *account = (bank_account_type *)args;
  printf("t2 is withdrawing %d\n", amount);
  withdraw(account, 20);
  printf("withdraw done\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  // allocate bank account
  bank_account_type *account = (bank_account_type *) malloc(sizeof(bank_account_type));
  init_account(account);

  pthread_t *tids = (pthread_t *) malloc(sizeof(pthread_t) * 2);
  pthread_create(&tids[0], NULL, t1_main, account);
  pthread_create(&tids[1], NULL, t2_main, account);
  pthread_join(tids[0], NULL);
  pthread_join(tids[1], NULL);

  // print results
  printf("balance = %d\n", account->balance);

#ifdef ASSERT
  assert(account->balance == 0);
#endif

  return 0;
}

