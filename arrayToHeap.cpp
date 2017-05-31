/* Rearrange an array so it meets requirements of a heap data structure.
The algorithm has time complexity O(nlog n) in the worst case,
is not so speed as qsort, but it can be used for very big data,
when it can not be placed in a memory.


Построение кучи
Переставить элементы заданного массива чисел так, чтобы он удовлетворял
 свойству мин-кучи.
Вход. Массив чисел A[0...n − 1].
Выход. Переставить элементы массива так, чтобы выпол-
нялись неравенства A[i] ≤ A[2i + 1] и A[i] ≤ A[2i + 2] для
всех i.

Построение кучи — ключевой шаг ал-
горитма сортировки кучей. Данный алго-
ритм имеет время работы O(nlogn) в худ-
шем случае в отличие от алгоритма быст-
рой сортировки, который гарантирует та-
кую оценку только в среднем случае. Алго-
ритм быстрой сортировки чаще использу-
ют на практике, поскольку в большинстве
случаев он работает быстрее, но алгоритм
сортировки кучей используется для внеш-
ней сортировки данных, когда необходимо
отсортировать данные огромного размера,
не помещающиеся в память компьютера.
Чтобы превратить данный массив в кучу, необходимо произвести
несколько обменов его элементов. Обменом мы называем базовую
операцию, которая меняет местами элементы A[i] и A[j]. Ваша цель
в данной задаче — преобразовать заданный массив в кучу за линей-
ное количество обменов.
Формат входа. Первая строка содержит число n. Следующая строка
задаёт массив чисел A[0], . . . , A[n − 1].
Формат выхода. Первая строка выхода должна содержать число об-
менов m, которое должно удовлетворять неравенству 0 ≤ m ≤
4n.Каждая из последующих m строк должна задавать обмен двух
элементов массива A. Каждый обмен задаётся парой различных
индексов 0 ≤ i != j ≤ n − 1. После применения всех обменов в
указанном порядке массив должен превратиться в мин-кучу, то
есть для всех 0 ≤ i ≤ n − 1 должны выполняться следующие два
условия:
• если 2i + 1 ≤ n − 1, то A[i] < A[2i + 1].
• если 2i + 2 ≤ n − 1, то A[i] < A[2i + 2].
Ограничения. 1 ≤ n ≤ 10^5; 0 ≤ A[i] ≤ 10^9 для всех 0 ≤ i ≤ n − 1; все
A[i] попарно различны; i != j.
Пример.
Вход:
5
5 4 3 2 1
Выход:
3
1 4
0 1
1 3
Пример.
Вход:
5
1 2 3 4 5
Выход:
0
*/

#include <iostream>
#include <vector>
#include <utility>
#include <climits>

#ifndef INIT_HEAP_SIZE
  #define INIT_HEAP_SIZE 255
#endif

#define __DEBUG_MY 1


class PermutationsLog {
private:
    typedef std::pair<int, int> PermutationRecord;
    typedef std::vector<PermutationRecord> PermutationsArray;
    
    // every pair of items defines pair to permutate
    PermutationsArray perm;
    
public:

    PermutationsLog(): perm() {};
    
    void insert(int first, int second) {
        perm.emplace_back(first, second);
    };
    
    int size() { return perm.size(); }
    
    void clear() {
        if(!perm.empty()) {
            perm.clear();
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, PermutationsLog& plog);
    
};

std::ostream& operator<<(std::ostream& os, PermutationsLog& plog) {
    #ifdef __DEBUG_MY
        std::cout << "Permutations count is ";
    #endif
    
    std::cout << plog.size() << std::endl;
    for(auto it = plog.perm.cbegin(); it != plog.perm.cend(); ++it) {
        std::cout << it->first << ' ' << it->second << std::endl;
    }
    
    return os;
}

class Heap {
private:
    int* arr;
    int maxSize; // size of the array arr
    int _size;// size of the heap
    
    // to store permutations made by the time of heap construct
    PermutationsLog perms;
    
    void swap(int i, int j) {
        // save info about permutation
            perms.insert(i, j);
            
            int tmp = arr[j];
            arr[j] = arr[i];
            arr[i] = tmp;
    };
    
    void siftUp(int i) {
        #ifdef __DEBUG_MY
            std::cout << "Sift up for " << i << std::endl;
        #endif
        
        if(!i) return;
        
        for(int _parent = parent(i);
            i > 0 && arr[_parent] > arr[i];
            i = _parent, _parent = parent(i)) {
            // swap parent and smaller child
            swap(_parent, i);
        }
    };
    
    void siftDown(int i) {
        #ifdef __DEBUG_MY
            std::cout << "Sift down for " << i << std::endl;
        #endif
        
        int minIdx = i;
        int l = leftChild(i);
        int r = rightChild(i);
        if(l < _size && arr[l] < arr[minIdx])
            minIdx = l;
        if(r < _size && arr[r] < arr[minIdx])
            minIdx = r;
        if(minIdx != i) {
            swap(i, minIdx);
            siftDown(minIdx);
        }
    };
    
    void buildHeap() {
        #ifdef __DEBUG_MY
            std::cout << "Build heap from vertex #" << parent(maxSize-1) << std::endl;
        #endif
        _size = maxSize;

        for(int i=parent(maxSize-1); i>=0; --i) {
            siftDown(i);
        }
    };
    
    void heapSort() {
        #ifdef __DEBUG_MY
            std::cout << "Heap sort" << std::endl;
        #endif
        
        perms.clear();
        buildHeap();
        for(int i=0; i<maxSize-1; ++i) {
            swap(0, _size-1);
            --_size;
            siftDown(0);
        }
        _size = maxSize;
    };
    
public:
    
    Heap(int _maxSize=INIT_HEAP_SIZE, int* a=nullptr): arr(a), maxSize(_maxSize), _size(0) {
        if(a != nullptr)
            heapSort();
        else {
          if(!maxSize) maxSize = INIT_HEAP_SIZE;
          try {
            arr = new int[maxSize];
          }
          catch(...) {
            arr = nullptr;
          }
        }
    };
    
    ~Heap() {
        delete[] arr;
    };
    
    int size() { return _size; };
    
    int parent (int i) {
        return (i-1) / 2;
    };

    int leftChild(int i) {
        return 2*i + 1;
    };

    int rightChild(int i) {
        return 2*i + 2;
    };
    
    void insert(int v) {
      if(_size == maxSize)
        throw "ERROR: maximum size of the heap is exceeded!";
      
      arr[_size] = v;
      ++_size;
      siftUp(_size-1);
      
    };
    
    int min() {
      if(!_size)
        throw "ERROR: there are not any value in the heap!";
      return arr[0];
    };
    
    // removes minimum value
    void removeMin() {
      if(!_size) return;
      if(_size == 1) {
        --_size;
        return;
      }
      
      arr[0] = arr[_size-1];
      --_size;
      siftDown(0);
    };
    
    void remove(int idx) {
      if(idx < 0 || idx >= _size)
        return;
      arr[idx] = INT_MIN;
      siftUp(idx);
      removeMin();
    };
    
    void changePriority(int idx, int p) {
      if(idx < 0 || idx >= _size)
        return;
      
      int old = arr[idx];
      arr[idx] = p;
      if(p > old)
        siftUp(idx);
      else
        siftDown(idx);
    };
    
    void clearLog() { perms.clear(); };
    

    friend std::ostream& operator<<(std::ostream& os, Heap& h);
    
};

std::ostream& operator<<(std::ostream& os, Heap& h) {
    #ifdef __DEBUG_MY
        std::cout << "Heap array size is " << h._size << std::endl;
        std::cout << "Elements:" << std::endl;
        for(int i=0; i<h._size; ++i) {
            std::cout << h.arr[i] << std::endl;
        }
        std::cout << "Permutations log:" << std::endl;
    #endif
    
    std::cout << h.perms;
    
    return os;
}

int main() {
    int n = 0; // array size to sort
    int* arr = nullptr;
    
    std::cin >> n;
    arr = new int[n];
    for(int i=0; i<n; ++i) {
        std::cin >> arr[i];
    }
    
    Heap h(n, arr);
    
    std::cout << h;
    
    return 0;
}
