// Copyright 2011 The University of Michigan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors - Jie Yu (jieyu@umich.edu)
//	     Markus Kusano (mukusano@vt.edu) (added comments and error handling)

/**
 * \file
 * \author Jie Yu (jieyu@umich.edu)
 * \author Markus Kusano (mukusano@vt.edu)
 * 
 * Program exemplifying a data race/atomicity violation. A global counter is
 * used by multiple threads through a temporary variable that is not protected
 * by a lock (see thread()).
 *
 * Source: https://github.com/jieyu/maple
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
 
/**
 * The number of threads to spawn in thread(). Modified via argc == 1 when
 * executing the process. 
 */
unsigned NUM_THREADS = 1;

/**
 * Counter that is manipulated in thread(). The counter is updated by first
 * copying it into a temporary local variable, then incrementing the temp
 * variable and then assigning the value of temp to global_count. These actions
 * are assumed to be atomic which is where the bug arises
 */
unsigned global_count = 0;

/**
 * Thread function that increments global_count.
 * The counter is updated by first copying it into a temporary local variable,
 * then incrementing the temp variable and then assigning the value of temp to
 * global_count. These actions are assumed to be atomic which is where the bug
 * arises
 */
void *thread(void *);
               
/**
 * Extracts the number of threads to spawn by looking at argv[1]. That many
 * threads are spawned into thread() and then main call pthread_join() on all
 * the threads.
 *
 * Since each thread simply increments global_count by 1, and global count
 * starts at 0, it is a valid assertion that global_count == NUM_THREADS.
 *
 * \param argc Standard argument count
 * \param argv[] argv[1] is the number of threads to spawn (assigned to
 * NUM_THREADS). This must be provided, all other arguments are ignored
 * \return Returns 0 upon completion
 */
int main(int argc, char *argv[]) {
  long i;
  pthread_t pthread_id[200];

  if (argc < 2) {
      fprintf(stderr, "Error: Argument 2 must be provided\n"
	              "Usage: %s <NUM_THREADS>\n", argv[0]);
      return 1;
  }

  NUM_THREADS = atoi(argv[1]);

  for(i = 0; i < NUM_THREADS; i++)
    pthread_create(&pthread_id[i], NULL, thread, (void *) i);
  for(i = 0; i < NUM_THREADS; i++)
    pthread_join(pthread_id[i], NULL);

  assert(global_count==NUM_THREADS);
  return 0;
}
 
void *thread(void * num) {
  unsigned temp = global_count;
  temp++;
  global_count = temp;
  return NULL;
}

