;; AUDIO -----------------------------------------------------------------------

(audio-config
 :sample-rate 44100
 )

;; UI --------------------------------------------------------------------------

(ui-config
 :width 1024
 :height 800
 :note-width 1

 :min-note 100
 :max-note 400
 )

(define color-normal (rgb 100 0 0))
(define color-sub    (rgb 50 0 0))  ; Less attention
(define color-target (rgb 255 0 0)) ; More attention


; http://www.reddit.com/r/asktransgender/comments/1wi7uk/violets_voice_tips/
(define (violet stage)
  (map (lambda (freq) (cons freq (rgb 0 80 0)))
       (case stage
         ((1) '(165 170))
         ((2) '(175 185))
         ((3) '(185 200))
         ((4) '(205 220)))))

(ui-marker-lines
 220 color-target ; A3
 247 color-normal ; B3
 262 color-normal ; C4
 277 color-sub    ; C#4
 294 color-normal ; D4
 330 color-normal ; E4
 131 color-normal ; C3

 (violet 1)
 (violet 4)
 )

;; ESPS ------------------------------------------------------------------------

(esps-config
 :cand-thresh 0.3    ; only correlation peaks above this are considered
 :lag-weight  0.3    ; degree to which shorter lags are weighted
 :freq-weight 0.02   ; weighting given to F0 trajectory smoothness
 :trans-cost  0.005  ; fixed cost for a voicing-state transition
 :trans-amp   0.5    ; amplitude-change-modulated VUV trans. cost
 :trans-spec  0.5    ; spectral-change-modulated VUV trans. cost
 :voice-bias  0.0    ; fixed bias towards the voiced hypothesis
 :double-cost 0.35   ; cost for octave F0 jumps
 :min-f0      50     ; min. F0 to search for (Hz)
 :max-f0      550    ; max. F0 to search for (Hz)
 :frame-step  0.01   ; inter-frame-interval (sec)
 :wind-dur    0.0075 ; duration of correlation window (sec)
 :n-cands     20     ; max. # of F0 cands. to consider at each frame
 )
