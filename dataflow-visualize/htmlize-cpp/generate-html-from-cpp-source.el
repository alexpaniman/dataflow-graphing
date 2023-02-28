;; Colors of a Monokai Pro theme: 
(custom-set-faces
 '(default                           ((t (:foreground "#fcfcfa" :background "#2d2a2e"))))
 '(font-lock-comment-face            ((t (:foreground "#727072"                      ))))
 '(font-lock-comment-delimiter-face  ((t (:foreground "#727072"                      ))))
 '(font-lock-constant-face           ((t (:foreground "#AB9DF2"                      ))))
 '(font-lock-function-name-face      ((t (:foreground "#A9DC76"                      ))))
 '(font-lock-keyword-face            ((t (:foreground "#FF6188"                      ))))
 '(font-lock-negation-char-face      ((t (:foreground "#FF6188" :bold t              ))))
 '(font-lock-preprocessor-face       ((t (:foreground "#FF6188" :bold t              ))))
 '(font-lock-string-face             ((t (:foreground "#FFD866"                      ))))
 '(font-lock-type-face               ((t (:foreground "#78DCE8"                      ))))
 '(font-lock-variable-name-face      ((t (:foreground "#FCFCFA"                      )))))

(load-file "htmlize.el")

(setq make-backup-files nil) ;; Prevent emacs from creating backups of result-file

(let* ((source-file (file-truename (pop command-line-args-left)))
       (result-file (file-truename (pop command-line-args-left))))
  (with-current-buffer (find-file-noselect source-file)
    (font-lock-fontify-buffer)
    (with-current-buffer (htmlize-buffer) (write-file result-file))))
