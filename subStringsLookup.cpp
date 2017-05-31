/* Karp-Rabin's algoritm to search substrings in the string.
 It has time complexity O(n) in the best case, and  O(n+m) in the worst case.
 Use findPatterns() function to search.
*/

#include <iostream>
#include <cstring>
#include <list>


//#define MY_DEBUG 1


#define PRIME_NUM (1000000007ULL)

// Type of a list of a positions in string.
// Is used as a result of the findPatterns function.
typedef std::list<size_t> PositionsList;
// The type of numbers to use.
typedef long long Number;


// Residue of a number x to the modulus of the magic prime number.
// We use it to avoid arithmetic overflowing.
Number norm(Number x) {
  return (x + PRIME_NUM) % PRIME_NUM;
}

// Hash of the substring
Number getHash(const std::string& str, size_t pos, size_t len, Number xMult, Number* xP) {
  #ifdef MY_DEBUG
    std::cout << "getHash(" << str << ", " << pos << ", " << len << ", " << xMult << ", " << *xP << ") = ";
  #endif
  
  Number res = 0;
  Number xi = 1;
  *xP = xi;
  for(size_t i=0; i<len; ++i) {
    res = (res + (str[i+pos] * xi) % PRIME_NUM) % PRIME_NUM;
    if(i == len-1)
      *xP = xi;
    xi = (xi * xMult) % PRIME_NUM;
  }
  
  #ifdef MY_DEBUG
    std::cout << (res%PRIME_NUM) << "; xP " << (*xP) << std::endl;
  #endif
  
  return res % PRIME_NUM;
}


// Find a pattern in the text, and fill result with the positions
// of the entries found.
void findPatterns(const std::string& pattern, const std::string& text, PositionsList& result) {
  #ifdef MY_DEBUG
    std::cout << "findPatterns(" << pattern << ", " << text << ")" << std::endl;
  #endif
  
  size_t pLen = pattern.size();
  size_t tLen = text.size();
  
  if(pLen > tLen)
    return;
  
  Number xMult = 263;
  Number xP = 1;
  Number hashP = getHash(pattern, 0, pLen, xMult, &xP);
  Number hashT = getHash(text, tLen-pLen, pLen, xMult, &xP);
  
  if(hashP == hashT) {
    #ifdef MY_DEBUG
      std::cout << "   hashes are equal, compare pattern and substring \"" << text.substr(tLen-pLen, pLen) << "\"" << std::endl;
    #endif
    if(!text.compare(tLen-pLen, pLen, pattern)) {
      #ifdef MY_DEBUG
        std::cout << "   strings are equal. Add to the list " << (tLen-pLen) << std::endl;
      #endif
      result.push_back(tLen-pLen);
    }
  }
  
  for(std::ptrdiff_t i=tLen-pLen-1; i>=0; --i) {
    #ifdef MY_DEBUG
      std::cout << "   Substring: " << text.substr(i, pLen) << "; [i]= " << text[i] << ":" << (int)(text[i]) << "; [i+len]= " << text[i+pLen] << ":" << (int)(text[i+pLen]) << "; ";
    #endif
    hashT = ((norm(hashT - (text[i+pLen] * xP) % PRIME_NUM) * xMult) % PRIME_NUM + text[i]) % PRIME_NUM;
    #ifdef MY_DEBUG
      std::cout << "hash = " << hashT << std::endl;
    #endif
    if(hashP == hashT) {
      #ifdef MY_DEBUG
        std::cout << "   hashes are equal, compare pattern and substring \"" << text.substr(i, pLen) << "\"" << std::endl;
      #endif
      if(!text.compare(i, pLen, pattern)) {
        #ifdef MY_DEBUG
          std::cout << "   strings are equal. Add to the list " << i << std::endl;
        #endif
        result.push_back(i);
      }
    }
  }
}

  
int main() {
  std::string pattern;
  std::string text;
  PositionsList list;
  
  std::cin >> pattern >> text;
 
  findPatterns(pattern, text, list);

  #ifdef MY_DEBUG
    std::cout << "Result size is " << list.size() << std::endl;
  #endif
    
  for(auto it = list.crbegin(); it != list.crend(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;

  
  return 0;
}
