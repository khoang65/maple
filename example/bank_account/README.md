## README

## SOURCE
https://github.com/jieyu/maple

## DESCRIPTION
Example buggy program included with maple.

Maple ``is a dynamic analysis framework for concurrent programs (x86 binaries).
It is shipped with a tool for testing concurrent programs built using the
framework.''

This example was converted from C++ to C. This was done to make the example
compatible with inspect.

Note: In the original program, right before main() returns (and the process
exits) there is a call `assert(account->balance == 0)`. This can be included in
the binary if the preprocessor directive `ASSERT` is included. It was chosen to
allow it to be disabled to allow inspect to run freely.

## BUGGY INTERLEAVING
~~~~~
Initially, account->balance == 0
Thread 1                                |               Thread 2
----------------------------------------|--------------------------------------
deposit(...) {                          |withdraw(...) {
                                        |   int curr_balance = 0;
                                        |   curr_balanace = get_balance(...);
                                        |   /* curr_balance == 0 */
                                        |
    int curr_balance = 0;               |
    curr_balance = get_balance(...)     |
    /* curr_balance == 0 */             |
    set_balance(account, curr_balance); |
    /* account->balance == 20 */        |
                                        |   setbalance(account, curr_balance);
}                                       |}
~~~~~

The program asserts right before completion (end of main()) that
account->balance == 0. This is  accurate since the account balance starts at 0
dollars and then deposits 20 dollars and withdraws 20dollars (the ordering of
the withdrawl and deposit is not guaranteed but this does not effect the end
result).

The program has assumed atomicity in withdraw() between getting the current
balance and then modifying the balance.

~~~~~
int get_balance(bank_account_type *account) {
  int ret = 0;
  pthread_mutex_lock(&account->lock);
  ret = account->balance;
  pthread_mutex_unlock(&account->lock);
  return ret;
}
~~~~~

The mutex causes threads to "line up" when accessing `account->balance`, but
does not guarantee atomicity in reading the balance and writing the balance.
