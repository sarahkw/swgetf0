;; Functions needed for schemeinterface

(define (read-eval script)
  (eval (read (open-input-string script))))
