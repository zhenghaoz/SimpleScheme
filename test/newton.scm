; Square Roots by Newton’s Method

(define (square x) (* x x))
(define (abs x) (if (< x 0) (- x) x))
(define (sqrt-iter guess x)
  (if (good-enough? guess x)
      guess
      (sqrt-iter (improve guess x) x)))
(define (improve guess x)
  (average guess (/ x guess)))
(define (average x y) 
  (/ (+ x y) 2))
(define (good-enough? guess x)
  (< (abs (- (square guess) x)) 0.001))
(define (sqrt x)
  (sqrt-iter 1.0 x))

(assert= (number->string (sqrt 9)) "3.00009")
(assert= (number->string (sqrt (+ 100 37))) "11.7047")
(assert= (number->string (sqrt (+ (sqrt 2) (sqrt 3)))) "1.77393")
(assert= (number->string (square (sqrt 1000))) "1000")
