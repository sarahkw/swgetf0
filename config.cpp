#include "config.h"

using namespace config;

Config::Config(const char* configData)
{
  schemeinterface::SchemeInterface si;
  si.loadResource(":/scheme/config.scm");
  si.load_string(configData);

  auto p = si.read_eval(
       "(let* ((get-key (lambda (key alist) (cdr (assv key alist))))           \n"
       "       (get-items (lambda (keys alist)                                 \n"
       "                    (let ((getter (lambda (key) (get-key key alist)))) \n"
       "                      (map getter keys)))))                            \n"
       "  (list                                                                \n"
       "   (get-items '(:sample-rate)                                          \n"
       "              (get-key 'audio-config config))                          \n"
       "   (get-items '(:width :height :note-width :min-note :max-note)        \n"
       "              (get-key 'ui-config config))                             \n"
       "   (get-key 'ui-marker-lines config)                                   \n"
       "   (get-items '(:cand-thresh :lag-weight :freq-weight :trans-cost      \n"
       "                             :trans-amp :trans-spec :voice-bias        \n"
       "                             :double-cost :min-f0 :max-f0 :frame-step  \n"
       "                             :wind-dur :n-cands)                       \n"
       "              (get-key 'esps-config config))))                         \n");

  schemeinterface::loadValues(p, audioConfig, uiConfig, uiMarkerLines,
                              espsConfig);
}
