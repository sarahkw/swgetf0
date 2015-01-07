;; Functions needed for schemeinterface

(define (si/read-eval script)
  (eval (read (open-input-string script))))

; Let schemeinterface know about the error, and then also let
; tinyscheme handle it.
(define *error-hook* (lambda args
                       (apply si/report-error args)
                       (apply error args)))
