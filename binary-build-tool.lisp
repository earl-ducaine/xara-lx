


(defun list-docroot (&key (node *current-host-node*))
  (run/lines `(ls "-l" ,(get-docroot))
	     :host
	     (get-ssh-connection-string :node node)))


(with-output-to-string (stream)
  (uiop:run-program
   "objdump -t /home/rett/dev/xoamorph/xara-lx/libs/x86_64/libCDraw_la-bshadow2-new.o"
   :output stream) stream)

;; objdump -t /home/rett/dev/xoamorph/xara-lx/libs/x86_64/libCDraw_la-bshadow2-new.o | cut -c 26-
