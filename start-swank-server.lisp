
;;; -*- mode: lisp ; syntax: ansi-common-lisp -*-

;; standard quicklisp init file, since with be launching ecl without ~/.eclrc
(let ((quicklisp-init (merge-pathnames "quicklisp/setup.lisp"
                                       (user-homedir-pathname))))
  (when (probe-file quicklisp-init)
    (load quicklisp-init)))

(when (probe-file  "/tmp/slime.2565")
  (delete-file "/tmp/slime.2565"))

(ql:quickload :swank)
;; (load
;;  "~/quicklisp/dists/quicklisp/software/slime-2.14/swank-loader.lisp"
;;  :verbose t)

(ASDF:LOAD-SYSTEM  :swank :force t)
(funcall (read-from-string "swank-loader:init"))
(funcall (read-from-string "swank:start-server")
         "/tmp/slime.2565"))
