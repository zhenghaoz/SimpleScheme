; Testing for Primality

(define (square x) (* x x))
(define (expmod base exp m)
  (cond ((= exp 0) 1)
        ((even? exp)
         (remainder 
          (square (expmod base (/ exp 2) m))
          m))
        (else
         (remainder 
          (* base (expmod base (- exp 1) m))
          m))))
(define (fermat-test n)
  (define (try-it a)
    (= (expmod a n n) a))
  (try-it (+ 1 (random (- n 1)))))
(define (fast-prime? n times)
  (cond ((= times 0) true)
        ((fermat-test n) 
         (fast-prime? n (- times 1)))
        (else false)))

(define times 1)
(assert (fast-prime? 1363005552434666078217421284621279933627102780881053358473 times))
(assert (fast-prime? 359334085968622831041960188598043661065388726959079837 times))
(assert (fast-prime? 393050634124102232869567034555427371542904833 times))
(assert (fast-prime? 170141183460469231731687303715884105727 times))
(assert (fast-prime? 56713727820156410577229101238628035243 times))
(assert= (fast-prime? 1363005552434666078217421284621279933627102780881053358477 times) false)
(assert= (fast-prime? 359334085968622831041960188598043661065388726959079833 times) false)
(assert= (fast-prime? 393050634124102232869567034555427371542904837 times) false)
(assert= (fast-prime? 170141183460469231731687303715884105729 times) false)
(assert= (fast-prime? 56713727820156410577229101238628035247 times) false)
