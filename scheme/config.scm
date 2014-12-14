(define config '())

;; (make-eval-alist '(a b c d)) => ((c . (eval d)) (a . (eval b)))
(define (make-eval-alist kv-pair)
  (let loop ((kv-pair kv-pair)
             (alist '()))
    (if (null? kv-pair)
        alist
        (loop (cddr kv-pair)
              (cons (cons (car kv-pair) (eval (cadr kv-pair)))
                    alist)))))

;; consecutive key value pairs in a list
(macro (defconfig form-)
  `(macro (,(cadr form-) form)
     `(define config
        (cons (cons ',(car form) ',(make-eval-alist (cdr form)))
              config))))

(defconfig audio-config)
(defconfig ui-config)
(defconfig ui-marker-lines)
(defconfig esps-config)

(define (rgb r g b)
  (list r g b))

