#ifndef MY_MEMORY_POOL_H
#define MY_MEMORY_POOL_H

#include <cstdint>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <iostream>

int pair(int x, int y) {
  return x > y ? x * x + x + y : y * y + x;
}

template<int ObjectSize, int ObjectNumber>
class MyMemoryPool {
  union node {
    int offset;
    char data[ObjectSize];
  };
  struct shared_use_struct {
    int first;
    node data[ObjectNumber];
  };
  int IPCKEY, shmid;
  void* shmaddr;
  shared_use_struct* shared;
  void init();
 public:
  MyMemoryPool();
  ~MyMemoryPool();
  void* malloc();
  void free(void*);
  void print();
};

template<int ObjectSize, int ObjectNumber>
void MyMemoryPool<ObjectSize, ObjectNumber>::init() {
  shared->first = 4;
  for (int i = 0; i < ObjectNumber; ++i) {
    if (i + 1 != ObjectNumber) 
      shared->data[i].offset = sizeof(node);
    else
      shared->data[i].offset = 0;
  }
}

template<int ObjectSize, int ObjectNumber>
MyMemoryPool<ObjectSize, ObjectNumber>::MyMemoryPool() {
  IPCKEY = pair(ObjectSize, ObjectNumber);
  shmid = shmget(IPCKEY, sizeof(shared_use_struct), 0666);
  if (shmid == -1) {
    shmid = shmget(IPCKEY, sizeof(shared_use_struct), 0666 | IPC_CREAT);
    shmaddr = shmat(shmid, NULL, 0);
    shared = (shared_use_struct*)shmaddr;
    init();
  }
  else {
    shmaddr = shmat(shmid, NULL, 0);
    shared = (shared_use_struct*)shmaddr;
  } 
}

template<int ObjectSize, int ObjectNumber>
MyMemoryPool<ObjectSize, ObjectNumber>::~MyMemoryPool() {
  shmdt(shmaddr);
}

//Allocates ObjectSize bytes of uninitialized storage from shared memory
//Returns NULL if no memory is available else a pointer to the allocated memory
template<int ObjectSize, int ObjectNumber>
void* MyMemoryPool<ObjectSize, ObjectNumber>::malloc() {
  if (shared->first == 0) {
    return NULL;
  }
  void* ans = (uint8_t*)shmaddr + shared->first;
  int offset = ((node*)(ans))->offset;
  if (offset == 0)
    shared->first = 0;
  else
    shared->first = shared->first + ((node*)(ans))->offset;
  return ans;
}

//Deallocates the space previously allocated by malloc
//If ptr is a null pointer, the function does nothing.
template<int ObjectSize, int ObjectNumber>
void MyMemoryPool<ObjectSize, ObjectNumber>::free(void* ptr) {
  if (ptr == NULL) return;
  int delta = (uint8_t*)ptr - (uint8_t*)shmaddr;
  if (shared->first == 0)
    ((node*)ptr)->offset = 0;
  else
    ((node*)ptr)->offset = shared->first - delta;
  shared->first = delta;
}

//Print the shared memory
template<int ObjectSize, int ObjectNumber>
void MyMemoryPool<ObjectSize, ObjectNumber>::print() {
  std::cout << "first = " << shared->first << std::endl;
  for (int i = 0; i < ObjectNumber; ++i) {
    std::cout << "offset = " << shared->data[i].offset << std::endl;
  }
}

#endif /* MY_MEMORY_POOL_H */
