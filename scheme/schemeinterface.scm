;; Functions needed for schemeinterface

(define (si/read-eval script)
  (eval (read (open-input-string script))))
