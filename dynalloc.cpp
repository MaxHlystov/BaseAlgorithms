/*Попробуйте реализовать динамический аллокатор памяти (интерфейс в шаблоне кода).
Введем несколько обозначений:
    - BufSize - размер участка логической памяти, который ваш аллокатор будет распределять
    - MaxSize - наибольший размер участка памяти, который можно аллоцировать вашим
      аллокатором для данного BufSize (проверяющая система найдет его бинарным поиском)
    - EffectiveSize - максимальное количество памяти, которое может быть выделено пользователю
      для данного BufSize (например, если мы аллоцируем много небольших участков памяти)

Ваш аллокатор памяти должен удовлетворять следующим условиям:
    - MaxSize должен быть не меньше 8/9 BufSize
    - EffectiveSize должен быть не меньше 1/9 BufSize
    - ваш аллокатор должен бороться с фрагментацией, т. е. если от начального состояния
      аллокатора мы смогли успешно аллоцировать какое-то количество памяти, то если мы освободим
      всю эту память и заново попробуем повторить аллокацию, она должна быть успешной
    - если аллокатор не смог аллоцировать участок памяти нужного размера, то он должен вернуть NULL
    - использование динамической аллокации памяти запрещено (malloc/new/new[]/free/delete/delete[]).

Гарантируется
    - что BufSize будет не меньше 100Kb и не больше 1Mb
    - что минимальный аллоцируемый участок памяти будет не меньше 16 байт.
*/


#define MY_TEST 1


#include <iostream>
#include <climits>


#ifdef MY_TEST
  #include <cstdlib>
  #include <iomanip>
  
  #define BUF_SIZE 102400
  
  #define MY_DEBUG 1
#endif


//////////////////////////////////////////////////////////////////////////////
// Basic definitions

// Minimum actual size of a block
#define MIN_BLOCK_SIZE 16

// function to move to the next header (see, getNext() and getPrevious())
typedef struct header* (*HeaderIterator)(struct header*);

// start marker of the block
struct header {
  unsigned char free; // block is free to allocate
  std::size_t actualSize; // size to use. without header and tail
};

// end marker of the block
struct tail {
  unsigned char free; // block is free to allocate
  std::size_t actualSize; // size to use. without header and tail
};


//////////////////////////////////////////////////////////////////////////////
// Globals variables
struct header* _startHeader; // the first header
struct tail* _endTail; // the end tail
std::size_t _size; // size of the entire block
std::size_t minSize; // minimum block size

//////////////////////////////////////////////////////////////////////////////
// Function

// Mark initial bufer with header and tail
struct header* initBlock(void *buf, std::size_t size) {
  #ifdef MY_DEBUG
    std::cout << "initBlock(" << buf << ", " << size << ')' << std::endl;
  #endif
  
  struct header* ph = (struct header*)buf;
  ph->free = 1;
  ph->actualSize = size - sizeof(struct header) - sizeof(struct tail);
  #ifdef MY_DEBUG
    std::cout << "   ph addr =" << ph << "; free =" << (int)(ph->free) << "; actualSize = " << ph->actualSize << std::endl;
  #endif
  
  struct tail* pt = (struct tail*)((unsigned char*)buf + size - sizeof(struct tail));
  pt->free = 1;
  pt->actualSize = ph->actualSize;
  #ifdef MY_DEBUG
    std::cout << "   pt addr =" << pt << "; free =" << (int)(pt->free) << "; actualSize = " << pt->actualSize << std::endl;
  #endif
  
  return ph;
}

// get tail corresponds to the header
struct tail* getTail(struct header* phead) {
  unsigned char* base = (unsigned char*)phead;
  base += sizeof(struct header) + phead->actualSize;
  return (struct tail*)base;
}

// get header corresponds to the tail
struct header* getHeader(struct tail* ptail) {
  unsigned char* base = (unsigned char*)ptail;
  base -= sizeof(struct header) + ptail->actualSize;
  return (struct header*)base;
}

// return pointer to the next block.
// if the current block is the very last, then return NULL
struct header* getNext(struct header* phead) {
  #ifdef MY_DEBUG
    std::cout << "getNext(" << phead << ')' << std::endl;
  #endif
  
  struct tail* ptail = getTail(phead);
  if(ptail == _endTail)
    return NULL;
  
  unsigned char* base = (unsigned char*)ptail;
  base += sizeof(struct tail);
  return (struct header*)base;
}

// return pointer to the previous block.
// if the current block is the very first, then return NULL
struct header* getPrevious(struct header* phead) {
  #ifdef MY_DEBUG
    std::cout << "getPrevious(" << phead << ')' << std::endl;
  #endif
  
  if(phead == _startHeader)
    return NULL;
  
  unsigned char* base = (unsigned char*)phead;
  base -= sizeof(struct tail);
  return getHeader((struct tail*)base);
}

// return size nedeed to allocate block with header and tail
std::size_t getAllSize(std::size_t size) {
  return size + sizeof(struct header) + sizeof(struct tail);
}

// return actual size of the block between header and tail.
std::size_t getActualSize(struct header* ph, struct tail* pt) {
  if((unsigned char*)ph >= (unsigned char*)pt)
    return 0;
  return (unsigned char*)pt - (unsigned char*)ph - sizeof(struct header);
}

// set phStart header and phEnd tail actualSize
//  == (phStart->actualSize + phEnd->actualSize).
// So phStart and ptEnd make new big block.
// Do not check any limits.
void joinBlocks(struct header* phStart, struct header* phEnd) {
  if(phStart == phEnd)
    return;
  
  #ifdef MY_DEBUG
    std::cout << "joinBlocks(" << phStart << ", " << phEnd << ')' << std::endl;
  #endif
  
  if(phStart > phEnd) {
    
    // phStart has to precede to phEnd
    struct header* tmp = phStart;
    phStart = phEnd;
    phEnd = tmp;
    #ifdef MY_DEBUG
      std::cout << "  swap block. phtStart " << phStart << "; phEnd " << phEnd << std::endl;
    #endif
  }
  
  struct tail* ptEnd = getTail(phEnd);
  #ifdef MY_DEBUG
    std::cout << "   ptEnd = " << ptEnd << std::endl;
  #endif
  
  phStart->actualSize = getActualSize(phStart, ptEnd);
  ptEnd->actualSize = phStart->actualSize;
  #ifdef MY_DEBUG
    std::cout << "   actualSize = " << phStart->actualSize << std::endl;
  #endif
}

// Split free block on two parts. First block is free.
// Second is used with size (size).
// Return pointer to the header of second block.
// If there was an error or size of old block too small,
// or the old block is not free return NULL.
struct header* utilizeBlock(struct header* ph, std::size_t size) {
  if(!ph->free || ph->actualSize < size)
    return NULL;
  
  #ifdef MY_DEBUG
    std::cout << "utilizeBlock(" << ph << ", " << size << ")" << std::endl;
  #endif
  
  // size with header and tail
  std::size_t allSize = getAllSize(size);
  
  struct tail* ptEnd = getTail(ph);
  
  if(ph->actualSize <= allSize) {
    ph->free = 0;
    ptEnd->free = 0;
    return ph;
  }
  
  std::size_t newSize = ph->actualSize - allSize;
  
  if(newSize < MIN_BLOCK_SIZE) {
    // it does not make sens to divide the block
    ph->free = 0;
    ptEnd->free = 0;
    return ph;
  }
  
  // split block on two: Start and End.
  // Because of adjusting, we may take anoter size of the blocks.
  // Compute it.
  unsigned char* base = (unsigned char*)ptEnd;
  base -= allSize;
  
  // tail of the start block
  struct tail* ptStart = (struct tail*)base;
  ptStart->free = 1;
  // find new size of the Start block in memory with adjusting
  ptStart->actualSize = getActualSize(ph, ptStart);
  ph->actualSize = ptStart->actualSize;
  
  // Create header of the second block next the tail of the first block
  struct header* phEnd = (struct header*)(base + sizeof(struct tail));
  phEnd->free = 0;
  phEnd->actualSize = getActualSize(phEnd, ptEnd);
  
  // The end of the start block became end of the end block
  ptEnd->free = 0;
  ptEnd->actualSize = phEnd->actualSize;
  
  return phEnd;
}

// Find all consequent free blocks and join it.
// ph - start block header.
// iterator - function to get next block.
// Return pointer to the header of new start block.
struct header* joinNearestFreeBlocks(struct header* ph, HeaderIterator iterator) {
  if(ph == NULL || !ph->free)
    return ph;
  
  #ifdef MY_DEBUG
    std::cout << "joinNearestFreeBlocks(" << ph << ')' << std::endl;
  #endif
  
  struct header* next = iterator(ph);
  #ifdef MY_DEBUG
    std::cout << "   next header = " << next << (next == NULL ? '=': '!') << "= NULL";
  #endif
  
  while(next != NULL && next->free) {
    #ifdef MY_DEBUG
      std::cout << "; free = " << (unsigned)(next->free) << std::endl;
    #endif
    joinBlocks(ph, next); // it does not need to ph precede next
    ph = next;
    next = iterator(ph);
    #ifdef MY_DEBUG
      std::cout << "   next header = " << next << (next == NULL ? '=': '!') << "= NULL";
    #endif
  }
  #ifdef MY_DEBUG
    std::cout << std::endl;
  #endif
  
  return ph;
}

    // Эта функция будет вызвана перед тем как вызывать myalloc и myfree
    // используйте ее чтобы инициализировать ваш аллокатор перед началом
    // работы.
    //
    // buf - указатель на участок логической памяти, который ваш аллокатор
    //       должен распределять, все возвращаемые указатели должны быть
    //       либо равны NULL, либо быть из этого участка памяти
    // size - размер участка памяти, на который указывает buf
	void mysetup(void *buf, std::size_t size) {
    _size = size;
    _startHeader = initBlock(buf, size);
    _endTail = getTail(_startHeader);
    // minimum size of the block
    minSize = getAllSize(MIN_BLOCK_SIZE);
  }

    // Функция аллокации
	void *myalloc(std::size_t size) {
    // look up for free block with apropriate size
    std::size_t allSize = getAllSize(size);
    struct header* ph = _startHeader;
    while(ph != NULL) {
      if(ph->free && (ph->actualSize >= allSize || ph->actualSize >= size)) {
        // we find free block with needed size
        unsigned char *base = (unsigned char*)utilizeBlock(ph, size);
        base += sizeof(struct header);
        return (void*)base;
      }
      ph = getNext(ph);
    }
    // there is not any free block with needed size
    return NULL;
  }

    // Функция освобождения
	void myfree(void *p) {
    #ifdef MY_DEBUG
      std::cout << "myfree(" << p << ')' << std::endl;
    #endif
    
    if(p == NULL)
      return;
    
    struct header* ph = (struct header*)((unsigned char*)p - sizeof(struct header));
    ph->free = 1;
    
    #ifdef MY_DEBUG
      std::cout << "   ph addr =" << ph << "; free =" << (unsigned int)(ph->free) << "; actualSize = " << ph->actualSize << std::endl;
    #endif
  
    struct tail* pt = getTail(ph);
    pt->free = 1;
    
    #ifdef MY_DEBUG
      std::cout << "   pt addr =" << pt << "; free =" << (unsigned int)(pt->free) << "; actualSize = " << pt->actualSize << std::endl;
    #endif
  
    // join blocks prevous to the current
    ph = joinNearestFreeBlocks(ph, getPrevious);
    // join blocks next to the current
    joinNearestFreeBlocks(ph, getNext);
  }
  
#ifdef MY_TEST
  
  void showBuf(bool showContent = false) {
    std::cout << "   --- Buff description" << std::endl;
      
    struct header* next = _startHeader;
    while(next != NULL) {
      std::cout << "     ---" << std::endl;
      std::cout << "     block header address " << next << "; free " << (int)(next->free) << "; size " << next->actualSize << std::endl;
      
      struct tail* pt = getTail(next);
      std::cout << "     block tail address " << pt << "; free " << (int)(pt->free) << "; size " << pt->actualSize << std::endl;
      
      std::cout << "     ptail - pheader - sizeof(header) " << ((unsigned char*)pt - (unsigned char*)next - sizeof(struct header)) << std::endl;
      std::size_t allSize = sizeof(struct header) + sizeof(struct tail) + next->actualSize;
      std::cout << "     allSize " << allSize << std::endl;
      
      if(showContent) {
        std::cout << "     content:" << std::endl;
        
        unsigned char* base = (unsigned char*)(next);
        for(std::size_t i=0; i<allSize; ++i) {
          if(i % 16 == 0)
            std::cout << "     ";
          std::cout << std::hex  << std::setw(2) << (unsigned int)(base[i]) << ' ';
          if((i+1) % 16 == 0)
            std::cout << std::endl;
        }
        if(allSize % 16 != 0)
            std::cout << std::endl;
      }
      
      next = getNext(next);
    }
    std::cout << "   --- End buffer description" << std::endl;
  }
  
  int main() {
    unsigned char* buf = new unsigned char[BUF_SIZE];
    
    std::size_t littleNum = BUF_SIZE / (MIN_BLOCK_SIZE * sizeof(unsigned char));
    void** blocks = (void**)(new unsigned char*[littleNum]);
          
    #ifdef MY_DEBUG
      std::cout << "UCHAR_MAX 0x" << std::hex << UCHAR_MAX << std::endl;
      std::cout << "Sizeof(unsighed char) is " << std::hex << sizeof(unsigned char) << std::endl;
      std::cout << "Sizeof(unsighed char*) is " << std::hex << sizeof(unsigned char *) << std::endl;
      std::cout << "Size of struct header is " << sizeof(struct header) << std::endl;
      std::cout << "Size of struct tail is " << sizeof(struct tail) << std::endl;
      std::cout << "Count of smallest blocks is " << littleNum << std::endl;
    #endif
    
    try {
      //std::cout << "start buffer setup" << std::endl;
      mysetup(buf, BUF_SIZE);
      //std::cout << "end buffer setup" << std::endl;
      
      #ifdef MY_DEBUG
        std::cout << "_size is " << _size << std::endl;
        std::cout << "Min size is " << minSize << std::endl;
        std::cout << "Start header address:" << _startHeader << std::endl;
        std::cout << "End tail address:" << _endTail << std::endl;
      
        showBuf();
      #endif
      
      /*
      // simple test of allocate/deallocate 20 bites.
      std::cout << "test1: simple allocate/deallocate 20 bites" << std::endl;
      char* p1 = (char*)myalloc(20);
      if(p1 == NULL) {
        throw "Error allocate 20 bites\n";
      }
      for(std::size_t i=0; i<20; ++i)
        p1[i] = i+1;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      myfree((void*)p1);
      std::cout << "test1: pass" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      // test too big to allocate block.
      std::cout << "test2: too big to allocate block" << std::endl;
      void* p2 = myalloc(BUF_SIZE*2);
      if(p2 != NULL)
        throw "Error allocate too big bites\n";
      std::cout << "test2: pass" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      // test to find and allocate one block of maximum size.
      std::cout << "test3.0: find maximum block size" << std::endl;
      std::size_t min = MIN_BLOCK_SIZE+1;
      std::size_t max = 3 * BUF_SIZE / 2;
      
      while(min != max) {
        std::size_t mid = (max + min + 1) / 2;
        //std::cout << "   min " << min << "; max " << max << "; mid " << mid << ';';
        void* p3 = myalloc(mid);
        if(p3 == NULL) {
          //std::cout << " to big" << std::endl;
          max = mid-1;
        }
        else {
          //std::cout << " to small" << std::endl;
          min = mid;
          myfree(p3);
        }
        //char c;
        //std::cin >> c;
      }
      std::cout << "   max found block size " << max << "; min is " << min << std::endl;
      std::cout << "test3.0: pass" << std::endl;
      
      std::size_t maxBlockSize = max;
      
      std::cout << "test3: allocate one block of maximum size " << maxBlockSize << std::endl;
      void* p3 = myalloc(maxBlockSize);
      if(p3 == NULL)
        throw "Error allocate one block of 8/9 BUF_SIZE\n";
      #ifdef MY_DEBUG
        showBuf();
      #endif
      myfree(p3);
      std::cout << "test3: pass" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      // test to allocate two blocks of half maximum size.
      std::size_t halfSize = maxBlockSize / 2;
      std::cout << "test3.1: allocate two blocks of half maximum size " << halfSize << std::endl;
      void* p3_1 = myalloc(halfSize);
      if(p3_1 == NULL)
        throw "Error allocate one block of half maximum size\n";
      #ifdef MY_DEBUG
        showBuf();
      #endif
      myfree(p3_1);
      std::cout << "test3.1: pass" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      // test to allocate max count of smallest blocks.
      // we have to allocate BUF_SIZE / block with MIN_BLOCK_SIZE byte size of each.
      std::cout << "test4: allocate max count of smallest blocks" << std::endl;
      std::cout << "   allocate blocks: ";
      for(std::size_t i=0; i<littleNum; ++i) {
        //std::cout << i << ", ";
        blocks[i] = myalloc(MIN_BLOCK_SIZE);
      }
      std::cout << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      std::cout << std::endl << "   Free blocks" << std::endl;
      for(std::size_t i=0; i<littleNum; ++i) {
        //std::cout << i << ", ";
        myfree(blocks[i]);
      }
      std::cout << std::endl << "test4: pass" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      // Test defragmentation.
      // We allocate maximum count (littleNum) of smallest blocks (MIN_BLOCK_SIZE byte).
      // Then we free first half of them.
      // And try to allocate block with (littleNum*8-1) size.
      std::cout << "test5: defragmentation ";
      for(std::size_t i=0; i<littleNum; ++i) {
        //std::cout << i << ", ";
        blocks[i] = myalloc(MIN_BLOCK_SIZE);
      }
      std::cout << std::endl << "  alloc many MIN_BLOCK_SIZE bytes blocks: ";
      for(std::size_t i=0; i<=littleNum/2; ++i) {
        //std::cout << i << ", ";
        myfree(blocks[i]);
      }
      std::cout << std::endl << "   alloc big block" << std::endl;
      void* p4 = myalloc(littleNum*8-1);
      if(p4 == NULL)
        throw "Error allocate one block of half maximum size above freed space\n";
      myfree(p4);
      std::cout << "test5: pass" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      */
      
      std::size_t maxBlockSize = 102368;
      
      //аллоцирует много маленьких участков памяти, пока аллокатор не вернет NULL, после чего освобождает участки в случаном порядке
      std::cout << "test6: allocate maximum smallest blocks, then random free some of them" << std::endl;
      std::size_t i = 0;
      while(1) {
        blocks[i] = myalloc(MIN_BLOCK_SIZE);
        if(blocks[i] == NULL) break;
        i++;
      }
      std::cout << "   allocated " << (i-1) << " blocks of size " << MIN_BLOCK_SIZE << std::endl;
      for(std::size_t j = 0; j<i; ++j) {
        std::size_t rnd = rand()/(double)RAND_MAX*(i-1);
        void* tmp = blocks[rnd];
        blocks[rnd] = blocks[j];
        blocks[j] = tmp;
      }
      std::cout << "   Free blocks in random order" << std::endl;
      for(std::size_t j = 0; j < i-1; ++j) {
        myfree(blocks[j]);
      }
      
      std::cout << "   Free all blocks except one" << std::endl;
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      std::cout << "   Free the last block" << std::endl;
      myfree(blocks[i-1]);
      
      #ifdef MY_DEBUG
        showBuf();
      #endif
      
      std::cout << "   Allocate maximim block size " << maxBlockSize << std::endl;
      void* p6 = myalloc(maxBlockSize);
      if(p6 == NULL)
        throw "Error allocate a block of maximum size after free blocks in random order\n";
      myfree(p6);
      std::cout << "test6: pass" << std::endl;

      std::cout << "delete[] blocks" << std::endl;
      delete[] blocks;
    }
    catch(char const *s) {
      std::cout << s;
    }
    
    delete[] buf;
    
    return 0;
  }

#endif