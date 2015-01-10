;; Functions needed for schemeinterface

(define (si/read-eval script)
  (eval (read (open-input-string script))))

; Let schemeinterface know about the error, and then also let
; tinyscheme handle it.
(define *error-hook* (lambda args
                                        ; tinyscheme breaks the error
                                        ; message down into multiple
                                        ; components. XXX for now,
                                        ; report it as a string
                                        ; because that's all
                                        ; schemeinterface supports.
                       (let ((os (open-output-string)))
                         (write args os)
                         (si/report-error (get-output-string os)))
                       (apply error args)))
