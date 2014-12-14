(load "config.scm")

(load "default-config.scm")

(write config)
(newline)
(write (config/serialize-config))
(newline)
