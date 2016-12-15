; The Metacircular Evaluator

#| basic function |#

(define (tagged-list? exp tag)
  (if (pair? exp)
      (eq? (car exp) tag)
      false))

#| internal definition |#

(define (scan-out-defines procedure)
  (define (scan procs)
    (cond ((null? procs) '())
          ((definition? (car procs))
           (let ((var (definition-variable (car procs)))
                 (val (definition-value (car procs))))
             (set-car! procs (make-assignment var val))
             (cons (list var (make-quote '*unassigned*))
                   (scan (cdr procs)))))
          (else (scan (cdr procs)))))
  (let ((vars (scan procedure)))
    (if (null? vars)
        procedure
        (list (make-let vars procedure)))))

#| procedure |#
(define apply-in-underlying-scheme apply)
(define (make-procedure parameters body env)
  (list 'procedure parameters (scan-out-defines body) env))
(define (compound-procedure? p)
  (tagged-list? p 'procedure))
(define (procedure-parameters p) (car (cdr p)))
(define (procedure-body p) (car (cdr (cdr p))))
(define (procedure-environment p) (car (cdr (cdr (cdr p)))))
(define (primitive-procedure? proc)
  (tagged-list? proc 'primitive))
(define (primitive-implementation proc) (car (cdr proc)))
(define primitive-procedures
  (list (list 'car car)
        (list 'cdr cdr)
        (list 'cons cons)
        (list 'list list)
        (list 'null? null?)
        (list 'random random)
        (list 'even? even?)
        (list 'remainder remainder)
        (list '+ +)
        (list '- -)
        (list '* *)
        (list '/ /)
        (list '= =)
        (list '< <)
        (list '> >)
        (list 'not not)
        ))
(define (primitive-procedure-names)
  (map car
       primitive-procedures))
(define (primitive-procedure-objects)
  (map (lambda (proc) (list 'primitive (car (cdr proc))))
       primitive-procedures))
(define (apply-primitive-procedure proc args)
  (apply-in-underlying-scheme
   (primitive-implementation proc) args))

#| apply |#

(define (sicp-apply procedure arguments)
  (cond ((primitive-procedure? procedure)
         (apply-primitive-procedure procedure arguments))
        ((compound-procedure? procedure)
         (eval-sequence
          (procedure-body procedure)
          (extend-environment
           (procedure-parameters procedure)
           arguments
           (procedure-environment procedure))))
        (else
         (error
          "Unkown procedure type -- APPLY" procedure))))

#| environment |#

; frame operation
(define (make-frame variables values)
  (cons variables values))
(define (frame-variables frame) (car frame))
(define (frame-values frame) (cdr frame))
(define (add-binding-to-frame! var val frame)
  (set-car! frame (cons var (car frame)))
  (set-cdr! frame (cons val (cdr frame))))
; environment operation
(define (enclosing-environment env) (cdr env))
(define (first-frame env) (car env))
(define the-empty-environment '())
(define (extend-environment vars vals base-env)
  (if (= (length vars) (length vals))
      (cons (make-frame vars vals) base-env)
      (if (< (length vars) (length vals))
          (error "Too many argumnets supplied" vars vals)
          (error "Too few arguments supplied" vars vals))))
(define (lookup-variable-value var env)
  (define (env-loop env)
    (define (scan vars vals)
      (cond ((null? vars)
             (env-loop (enclosing-environment env)))
            ((eq? var (car vars))
             (car vals))
            (else (scan (cdr vars) (cdr vals)))))
    (if (eq? env the-empty-environment)
        (error "Unkown variable" var)
        (let ((frame (first-frame env)))
          (scan (frame-variables frame)
                (frame-values frame)))))
  (let ((value (env-loop env)))
    (if (eq? value '*unassigned*)
        (error "Cannot use before initialization" var)
        value)))
(define (set-variable-value! var val env)
  (define (env-loop env)
    (define (scan vars vals)
      (cond ((null? vars)
             (env-loop (enclosing-environment env)))
            ((eq? var (car vars))
             (set-car! vals val))
            (else (scan (cdr vars) (cdr vals)))))
    (if (eq? env the-empty-environment)
        (error "Unkown variable" var)
        (let ((frame (first-frame env)))
          (scan (frame-variables frame)
                (frame-values frame)))))
  (env-loop env))
(define (define-variable! var val env)
  (let ((frame (first-frame env)))
    (define (scan vars vals)
      (cond ((null? vars)
             (add-binding-to-frame! var val frame))
            ((eq? var (car vars))
             (set-car! vals val))
            (else (scan (cdr vars) (cdr vals)))))
    (scan (frame-variables frame)
          (frame-values frame))))

; setup

(define (setup-environment)
  (let ((initial-env
         (extend-environment (primitive-procedure-names)
                             (primitive-procedure-objects)
                             the-empty-environment)))
    (define-variable! 'true true initial-env)
    (define-variable! 'false false initial-env)
    initial-env))
(define the-global-environment (setup-environment))

#| lambda |#

(define (lambda? exp) (tagged-list? exp 'lambda))
(define (lambda-parameters exp) (car (cdr exp)))
(define (lambda-body exp) (cdr (cdr exp)))
(define (make-lambda parameters body)
  (cons 'lambda (cons parameters body)))

#| self-evaluting |#

(define (self-evaluting? exp)
  (cond ((number? exp) true)
        ((string? exp) true)
        (else false)))

#| variable |#

(define (variable? exp) (symbol? exp))

#| quoted |#

(define (quoted? exp)
  (tagged-list? exp 'quote))
(define (text-of-quotation exp) (car (cdr exp)))
(define (make-quote exp) (list 'quote exp))

#| assignment |#

(define (assignment? exp)
  (tagged-list? exp 'set!))
(define (assignment-variable exp) (car (cdr exp)))
(define (assignment-value exp) (car (cdr (cdr exp))))
(define (eval-assignment exp env)
  (set-variable-value! (assignment-variable exp)
                       (sicp-eval (assignment-value exp) env)
                       env)
  'ok)
(define (make-assignment var exp)
  (list 'set! var exp))

#| definition |#

(define (definition? exp)
  (tagged-list? exp 'define))
(define (definition-variable exp)
  (if (symbol? (car (cdr exp)))
      (car (cdr exp))
      (car (car (cdr exp)))))
(define (definition-value exp)
  (if (symbol? (car (cdr exp)))
      (car (cdr (cdr exp)))
      (make-lambda (cdr (car (cdr exp))) ; formal parameters
                   (cdr (cdr exp))))); body
(define (eval-definition exp env)
  (define-variable! (definition-variable exp)
    (sicp-eval (definition-value exp) env)
    env)
  'ok)
(define (make-definition var exp)
  (cons 'define (cons var exp)))

#| if |#

(define (true? x)
  (not (eq? x false)))
(define (false? x)
  (eq? x false))
(define (if? exp) (tagged-list? exp 'if))
(define (if-predicate exp) (car (cdr exp)))
(define (if-consequence exp) (car (cdr (cdr exp))))
(define (if-alternative exp)
  (if (not (null? (cdr (cdr (cdr exp)))))
      (car (cdr (cdr (cdr exp))))
      'false))
(define (eval-if exp env)
  (if (true? (sicp-eval (if-predicate exp) env))
      (sicp-eval (if-consequence exp) env)
      (sicp-eval (if-alternative exp) env)))
(define (make-if predicate consequence alternative)
  (list 'if predicate consequence alternative))

#| sequence |#

(define (begin? exp) (tagged-list? exp 'begin))
(define (begin-actions exp) (cdr exp))
(define (last-exp? seq) (null? (cdr seq)))
(define (first-exp seq) (car seq))
(define (rest-exps seq) (cdr seq))
(define (make-begin seq) (cons 'begin seq))
(define (sequence->exp seq)
  (cond ((null? seq) seq)
        ((last-exp? seq) (first-exp seq))
        (else (make-begin seq))))
(define (eval-sequence exps env)
  (cond ((last-exp? exps) (sicp-eval (first-exp exps) env))
        (else (sicp-eval (first-exp exps) env)
              (eval-sequence (rest-exps exps) env))))

#| cond |#

(define (cond? exp) (tagged-list? exp 'cond))
(define (cond-clauses exp) (cdr exp))
(define (cond-predicate clause) (car clause))
(define (cond-actions clause) (cdr clause))
(define (cond-have-recipient? clause)
  (eq? (car (cdr clause)) '=>))
(define (cond-recipient clause) (car (cdr (cdr clause))))
(define (cond-else-clause? clause)
  (eq? (cond-predicate clause) 'else))
(define (expand-clauses clauses)
  (if (null? clauses)
      'false
      (let ((first (car clauses))
            (rest (cdr clauses)))
        (if (cond-else-clause? first)
            (if (null? rest)
                (sequence->exp (cond-actions first))
                (error "ELSE clause isn't last -- COND->IF"
                       clauses))
            (make-if (cond-predicate first)
                     (if (cond-have-recipient? first)
                         (make-application (cond-recipient first)
                                           (list (cond-predicate first)))
                         (sequence->exp (cond-actions first)))
                     (expand-clauses rest))))))
(define (cond->if exp)
  (expand-clauses (cond-clauses exp)))

#| application |#

(define (application? exp) (pair? exp))
(define (operator exp) (car exp))
(define (operands exp) (cdr exp))
(define (no-operands? ops) (null? ops))
(define (first-operand ops) (car ops))
(define (rest-operands ops) (cdr ops))
(define (list-of-values exps env)
  (if (no-operands? exps)
      '()
      (cons (sicp-eval (first-operand exps) env)
            (list-of-values (rest-operands exps) env))))
(define (make-application operator operands)
  (cons operator operands))

#| and |#

(define (and? exp) (tagged-list? exp 'and))
(define (and-exps exp) (cdr exp))
(define (and->if exp)
  (define (expand-exp exps)
    (if (null? exps)
        'true
        (make-if (car exps)
                 (expand-exp (cdr exps))
                 'false)))
  (expand-exp (and-exps exp)))

#| or |#

(define (or? exp) (tagged-list? exp 'or))
(define (or-exps exp) (cdr exp))
(define (or->if exp)
  (define (expand-exp exps)
    (if (null? exps)
        'false
        (make-if (car exps)
                 'true
                 (expand-exp (cdr exps)))))
  (expand-exp (or-exps exp)))

#| let |#

(define (let? exp) (tagged-list? exp 'let))
(define (let-have-var? exp) (symbol? (car (cdr exp))))
(define (let-var exp)
  (if (let-have-var? exp)
      (car (cdr exp))
      false))
(define (let-bindings exp)
  (if (let-have-var? exp)
      (car (cdr (cdr exp)))
      (car (cdr exp))))
(define (let-body exp)
  (if (let-have-var? exp)
      (cdr (cdr (cdr exp)))
      (cdr (cdr exp))))
(define (first-binding bindings) (car bindings))
(define empty-bindings '())
(define (rest-bindings bindings) (cdr bindings))
(define (last-binding? binding) (null? (cdr binding)))
(define (binding-var binding) (car binding))
(define (binding-val binding) (car (cdr binding)))
(define (let->combination exp)
  (define (binding-vars bindings)
    (if (null? bindings)
        null
        (cons (binding-var (first-binding bindings))
              (binding-vars (rest-bindings bindings)))))
  (define (binding-vals bindings)
    (if (null? bindings)
        null
        (cons (binding-val (first-binding bindings))
              (binding-vals (rest-bindings bindings)))))
  (let ((bindings (let-bindings exp)))
    (if (let-have-var? exp)
        (sequence->exp (list (make-definition (cons (let-var exp)
                                                    (binding-vars bindings))
                                              (let-body exp))
                             (make-application (let-var exp)
                                               (binding-vals bindings))))
        (make-application (make-lambda (binding-vars bindings)
                                       (let-body exp))
                          (binding-vals bindings)))))
(define (make-let bindings body)
  (cons 'let (cons bindings body)))

#| let* |#

(define (let*? exp) (tagged-list? exp 'let*))
(define (let*->nested-let exp)
  (define (expand-exp bindings)
    (if (null? bindings)
        null
        (make-let (list (car bindings))
                  (if (last-binding? bindings)
                      (let-body exp)
                      (list (expand-exp (rest-bindings bindings)))))))
  (expand-exp (let-bindings exp)))

#| while |#

(define (while? exp) (tagged-list? exp 'while))
(define (while-condition exp) (car (cdr exp)))
(define (while-body exp) (cdr (cdr exp)))
(define (while->combination exp)
  (sequence->exp
   (list
    (make-definition
     '(iter)
     (list
      (make-if
       (while-condition exp)
       (sequence->exp
        (append (while-body exp)
                (list (make-application 'iter '()))))
       "ok")))
    (make-application 'iter '()))))

#| for |#

(define (for? exp) (tagged-list? exp 'for))
(define (for-init exp) (car (cdr exp)))
(define (for-condition exp) (car (cdr (cdr exp))))
(define (for-increment exp) (car (cdr (cdr (cdr exp)))))
(define (for-body exp) (cdr (cdr (cdr (cdr exp)))))
(define (for->combination exp)
  (sequence->exp
   (list
    (for-init exp)
    (make-definition
     '(iter)
     (list
      (make-if
       (for-condition exp)
       (sequence->exp
        (append
         (append (for-body exp)
                 (list (for-increment exp)))
         (make-application '(iter) '())))
       "ok")))
    (make-application 'iter '()))))

#| for-each |#

(define (for-each? exp) (tagged-list? exp 'for-each))
(define (for-each-list exp) (car (cdr exp)))
(define (for-each-lambda exp) (car (cdr (cdr exp))))
(define (for-each->combination exp)
  (sequence->exp
   (list
    (make-definition
     '(iter node)
     (list
      (make-if
       '(not (null? node))
       (sequence->exp
        (list
         (make-application
          (for-each-lambda exp)
          (list (make-application 'car '(node))))
         (make-application
          'iter
          (list (make-application 'cdr '(node))))))
       "ok")))
    (make-application 'iter (list (for-each-list exp))))))

#| letrec |#

(define (letrec? exp) (tagged-list? exp 'letrec))
(define (letrec-binding exp) (car (cdr exp)))
(define (letrec-body exp) (cdr (cdr exp)))
(define (letrec->let exp)
  (define (scan-bindings bindings)
    (if (null? bindings) '()
        (let* ((binding (car bindings))
              (var (binding-var binding))
              (val (binding-val binding)))
          (set-car! (cdr binding) (make-quote '*unassigned*))
          (cons (make-assignment var val)
                (scan-bindings (cdr bindings))))))
  (let* ((bindings (letrec-binding exp))
        (assignment (scan-bindings bindings))
        (body (letrec-body exp)))
    (make-let bindings (append assignment body))))

#| evalute |#

(define (sicp-eval exp env)
  (cond ((self-evaluting? exp) exp)
        ((variable? exp) (lookup-variable-value exp env))
        ((quoted? exp) (text-of-quotation exp))
        ((assignment? exp) (eval-assignment exp env))
        ((definition? exp) (eval-definition exp env))
        ((if? exp) (eval-if exp env))
        ((lambda? exp)
         (make-procedure (lambda-parameters exp)
                         (lambda-body exp)
                         env))
        ((begin? exp)
         (eval-sequence (begin-actions exp) env))
        ((cond? exp) (sicp-eval (cond->if exp) env))
        ((and? exp) (sicp-eval (and->if exp) env))
        ((or? exp) (sicp-eval (or->if exp) env))
        ((let? exp) (sicp-eval (let->combination exp) env))
        ((let*? exp) (sicp-eval (let*->nested-let exp) env))
        ((while? exp) (sicp-eval (while->combination exp) env))
        ((for? exp) (sicp-eval (for->combination exp) env))
        ((for-each? exp) (sicp-eval (for-each->combination exp) env))
        ((letrec? exp) (sicp-eval (letrec->let exp) env))
        ((application? exp)
         (sicp-apply (sicp-eval (operator exp) env)
                     (list-of-values (operands exp) env)))
        (else
         (error "Unkown expression type -- EVAL" exp))))

#| user interface |#

(define input-prompt ";;; M-Eval input:")
(define output-prompt ";;; M-Eval value:")
(define (prompt-for-input string)
  (newline) (newline) (display string) (newline))
(define (announce-for-output string)
  (newline) (display string) (newline))
(define (user-input object)
  (if (compound-procedure? object)
      (display (list 'compund-procedure
                     (procedure-parameters object)
                     (procedure-body object)
                     '<precedure>))
      (display object)))
(define (driver-loop)
  (prompt-for-input input-prompt)
  (let ((input (read)))
    (let ((output (sicp-eval input the-global-environment)))
      (announce-for-output output-prompt)
      (user-input output)))
  (driver-loop))

; start
(driver-loop)
