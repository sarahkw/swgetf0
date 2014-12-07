
(define config '())

;; (make-alist '(a b c d)) => ((c . (eval d)) (a . (eval b)))
(define (make-alist kv-pair)
  (let loop ((kv-pair kv-pair)
             (alist '()))
    (if (null? kv-pair)
        alist
        (loop (cddr kv-pair)
              (cons (cons (car kv-pair) (eval (cadr kv-pair)))
                    alist)))))

(macro (defmacro form-)
  `(macro (,(cadr form-) form)
     `(define config
        (cons (cons ',(car form) ',(make-alist (cdr form)))
              config))))

(defmacro audio-config)
(defmacro ui-config)
(defmacro esps-config)

(macro (ui-marker-lines form))

(define (rgb r g b))
