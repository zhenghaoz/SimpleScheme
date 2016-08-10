(define a (list 1 2 3 4 5 6 7 8 9))
(define (foo x)
	(if (null? x)
		0
		(+ (car x) (foo (cdr x)))))
(foo a)