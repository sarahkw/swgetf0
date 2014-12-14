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


(define (config/serialize-config)
  (let* ((get-key (lambda (key alist) (cdr (assv key alist))))
         (get-items (lambda (keys alist)
                      (let ((getter (lambda (key) (get-key key alist))))
                        (map getter keys)))))
    (list
     (get-items '(:sample-rate)
                (get-key 'audio-config config))
     (get-items '(:width :height :note-width :min-note :max-note)
                (get-key 'ui-config config))
     (get-key 'ui-marker-lines config)
     (get-items '(:cand-thresh :lag-weight :freq-weight :trans-cost
                               :trans-amp :trans-spec :voice-bias
                               :double-cost :min-f0 :max-f0 :frame-step
                               :wind-dur :n-cands)
                (get-key 'esps-config config)))))
