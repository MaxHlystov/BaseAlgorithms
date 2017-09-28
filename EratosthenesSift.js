// Create an array filled with prime numbers not less than fromVal,
// and not greater than toVal.
function EratosthenesSift(fromVal, toVal) {
    function newSift (size) {
        let arr = [];
        arr.length = size;
        arr.fill(true);
        return arr;
    }
    
    function mark_sieve(arr, first, last, factor) {
        //  assert (first  !=  last)
        arr[first]  =  false;
        while (last - first > factor) {
            first = first + factor;
            arr[first] = false;
        }
    }
    
    function sift (n) {
        let arr = newSift(Math.floor((n+1) / 2)); // Don't store even numbers.
        let i = 0; // First index is value 3.
        let index_square = 3; // Used to make computation faster.
        let factor = 3; // Value
        
        while (index_square < n) {
            // Invariant: index_square = 2*i^2 + 6*i + 3,
            // factor = 2*i + 3
            if (arr[i]) {
                mark_sieve(arr, index_square, n, factor);
            }
            ++i;
            index_square += factor;
            factor += 2; 
            index_square += factor;
        }
        
        return arr;
    }
    
    let primes = sift(toVal);
    let res = [];
    
    if(fromVal <= 2) {
        fromVal = 3;
        res.push(2);
    }
    else if((fromVal & 1) === 0) {
        ++fromVal;
    }

    let i = Math.floor((fromVal - 3) / 2);
    for(let v = fromVal; v <= toVal; v += 2, ++i) {
        if(primes[i])
            res.push(v);
    }
    return res;
}


console.log("Fuction answer (99, 140): " + EratosthenesSift(99, 140).join(" "));
console.log("Correct answer (99, 140): 101 103 107 109 113 127 131 137 139")

console.log("Fuction answer (18, 104): " + EratosthenesSift(18, 104).join(" "));
