;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; File name: ` ~/.emacs '
;;; ---------------------
;;;
;;; If you need your own personal ~/.emacs
;;; please make a copy of this file
;;; an placein your changes and/or extension.
;;;
;;; Copyright (c) 1997-2002 SuSE Gmbh Nuernberg, Germany.
;;;
;;; Author: Werner Fink, <feedback@suse.de> 1997,98,99,2002
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Test of Emacs derivates
;;; -----------------------
(if (string-match "XEmacs\\|Lucid" emacs-version)
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;; XEmacs
  ;;; ------
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  (progn
     (if (file-readable-p "~/.xemacs/init.el")
        (load "~/.xemacs/init.el" nil t))
  )
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;; GNU-Emacs
  ;;; ---------
  ;;; load ~/.gnu-emacs or, if not exists /etc/skel/.gnu-emacs
  ;;; For a description and the settings see /etc/skel/.gnu-emacs
  ;;;   ... for your private ~/.gnu-emacs your are on your one.
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  (if (file-readable-p "~/.gnu-emacs")
      (load "~/.gnu-emacs" nil t)
    (if (file-readable-p "/etc/skel/.gnu-emacs")
	(load "/etc/skel/.gnu-emacs" nil t)))

  ;; Custum Settings
  ;; ===============
  ;; To avoid any trouble with the customization system of GNU emacs
  ;; we set the default file ~/.gnu-emacs-custom
  (setq custom-file "~/.gnu-emacs-custom")
  (load "~/.gnu-emacs-custom" t t)
;;;
)
;;;

;;load files from ~/.emacs.d
(setq load-path (append load-path (list "~/.emacs.d")))

;;load the php-mode LISP
(require 'php-mode)
(add-hook 'php-mode-user-hook 'turn-on-font-lock)

;;load python-mode
(autoload 'python-mode "python-mode" "Python Mode." t)
(add-to-list 'auto-mode-alist '("\\.py\\'" . python-mode))
(add-to-list 'interpreter-mode-alist '("python" . python-mode))

;; COMPILATION settings

(add-hook 'java-mode-hook
	  (lambda()
	    (set (make-local-variable 'compile-command) (concat "javac " (buffer-name)))))

;;default major mode is text-mode
(setq default-major-mode 'text-mode)

;; define users email
(setq user-mail-address "jason@jasonantman.com")
;; define user's full name
(setq user-full-name "Jason Antman")
;; show line numbering
(line-number-mode 1)

;; setup automatic time-stamping
(if (not (memq 'time-stamp write-file-hooks))
    (setq write-file-hooks
          (cons 'time-stamp write-file-hooks)))

;; DEFINE the FUNCTION KEYS
(global-set-key [f1] 'other-window)
(global-set-key [f2] 'list-buffers)
(global-set-key [f3] 'split-window-vertically)
(global-set-key [f4] 'undo)
(global-set-key [f5] 'goto-line)
(global-set-key [f6] 'shell)
(global-set-key [f7] 'compile)
(global-set-key [f8] 'next-error)
(global-set-key [f9] 'vc-next-action)
(global-set-key [f10] 'diff-mode)
(global-set-key [f11] 'delete-window)
(global-set-key [f12] 'kill-this-buffer)

;; disable C-x, C-z
(global-unset-key [(control x) (control z)])   ; normally suspends or minimizes emacs (!)
;; enter the debugger every time an error is found in .emacs on load
(setq debug-on-error t)

;; do not display a splash screen on startup
(setq inhibit-splash-screen t)

;; make files starting with shebang +x automatically - added 2008-01-26
(add-hook 'after-save-hook' executable-make-buffer-file-executable-if-script-p)