/*
 * This material contains unpublished, proprietary software of 
 * Entropic Research Laboratory, Inc. Any reproduction, distribution, 
 * or publication of this work must be authorized in writing by Entropic 
 * Research Laboratory, Inc., and must bear the notice: 
 *
 *    "Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. 
 *                   All rights reserved"
 *
 * The copyright notice above does not evidence any actual or intended 
 * publication of this source code.     
 *
 * Written by:  Derek Lin
 * Checked by:
 * Revised by:  David Talkin
 *
 * Brief description:  Estimates F0 using normalized cross correlation and
 *   dynamic programming.
 *
 */

static char *sccs_id = "@(#)get_f0.c	1.14	10/21/96	ERL";

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <esps/esps.h>
#include <esps/fea.h>
#include <esps/feasd.h>

#include "f0.h"

#define SYNTAX USAGE("get_f0 [-P param_file][-{pr} range][-s range][-S frame_step_samples]\n     [-i frame_step_seconds][-x debug_level] infile outfile")

char	    *ProgName = "get_f0";
static char *Version = "1.14";
static char *Date = "10/21/96";

int	    debug_level = 0;
extern void fea_skiprec();


static int check_f0_params(F0_params *par, double sample_freq);

int main_sw_tmp(ac, av)
    int     ac;
    char    **av;
{
  extern char *optarg;
  extern int optind, getopt();
  char *get_cmd_line();
  float *fdata;
  char c, *ifname, *ofname, *range = NULL;
  FILE *ifile, *ofile;
  struct header *ihd, *ohd;
  struct feasd *sd_rec;
  struct fea_data *fea_rec;
  int done;
  long buff_size, actsize;
  double sf, output_starts, frame_rate;
  F0_params *par, *read_f0_params();
  char *param_file = NULL;
  float *f0p, *vuvp, *rms_speech, *acpkp;
  double *rec_F0, *rec_pv, *rec_rms, *rec_acp;
  int i, vecsize;
  int init_dp_f0(), dp_f0(), check_f0_params();
  long sdstep = 0;

  par = (F0_params *) malloc(sizeof(F0_params));
  par->cand_thresh = 0.3;
  par->lag_weight = 0.3;
  par->freq_weight = 0.02;
  par->trans_cost = 0.005;
  par->trans_amp = 0.5;
  par->trans_spec = 0.5;
  par->voice_bias = 0.0;
  par->double_cost = 0.35;
  par->min_f0 = 50;
  par->max_f0 = 550;
  par->frame_step = 0.01;
  par->wind_dur = 0.0075;
  par->n_cands = 20;
  par->mean_f0 = 200;     /* unused */
  par->mean_f0_weight = 0.0;  /* unused */
  par->conditioning = 0;    /*unused */

  while((c = getopt(ac,av,"x:P:p:r:s:S:i:")) != EOF){
    switch(c){
    case 'P':
      param_file = optarg;
      break;
    case 'x':
      debug_level = atoi(optarg);
      break;
    default:
      SYNTAX;
      exit(1);
    }
  }
  
  if((ac - optind) != 2){
    SYNTAX;
    exit(1);
  }
  
  (void) read_params(param_file, SC_NOCOMMON, (char *)NULL);
  
  if(symtype("frame_step") != ST_UNDEF)
    par->frame_step = getsym_d("frame_step");

  if( symtype("cand_thresh") != ST_UNDEF)
    par->cand_thresh = getsym_d("cand_thresh");
  if( symtype("lag_weight") != ST_UNDEF)
    par->lag_weight = getsym_d("lag_weight");
  if( symtype("freq_weight") != ST_UNDEF)
    par->freq_weight = getsym_d("freq_weight");
  if( symtype("trans_cost") != ST_UNDEF)
    par->trans_cost = getsym_d("trans_cost");
  if( symtype("trans_amp") != ST_UNDEF)
    par->trans_amp = getsym_d("trans_amp");
  if( symtype("trans_spec") != ST_UNDEF)
    par->trans_spec = getsym_d("trans_spec");
  if( symtype("voice_bias") != ST_UNDEF)
    par->voice_bias = getsym_d("voice_bias");
  if( symtype("double_cost") != ST_UNDEF)
    par->double_cost = getsym_d("double_cost");
  if( symtype("min_f0") != ST_UNDEF)
    par->min_f0 = getsym_d("min_f0");
  if( symtype("max_f0") != ST_UNDEF)
    par->max_f0 = getsym_d("max_f0");
  if( symtype("wind_dur") != ST_UNDEF)
    par->wind_dur = getsym_d("wind_dur");
  if( symtype("n_cands") != ST_UNDEF)
    par->n_cands = getsym_i("n_cands");

  ifname = eopen(ProgName, av[optind], "r", FT_FEA, FEA_SD, &ihd, &ifile);
  ofname = eopen(ProgName, av[optind+1], "w", NONE, NONE, &ohd, &ofile);
  sf = get_genhd_val("record_freq", ihd, 0.0);
  if (sf == 0.0) {
    Fprintf(stderr, "%s: no sampling frequency---exiting.\n", ProgName);
    exit(1);
  }
  if(check_f0_params(par, sf)){
    Fprintf(stderr, "%s: invalid/inconsistent parameters -- exiting.\n",
	    ProgName);
    exit(1);
  }

  /*SW: Removed range restricter, but this may be interesting: 
    if (total_samps < ((par->frame_step * 2.0) + par->wind_dur) * sf), then
      input range too small*/

  ohd = new_header(FT_FEA);
  if (ohd == NULL) {
    Fprintf(stderr, "%s: failed to create output header---exiting.\n",
	    ProgName);
    exit(1);
  }
  (void) strcpy (ohd->common.prog, ProgName);
  (void) strcpy (ohd->common.vers, Version);
  (void) strcpy (ohd->common.progdate, Date);
  ohd->common.tag = NO;
  add_source_file(ohd,ifname,ihd);
  add_comment(ohd,get_cmd_line(ac,av));

  add_fea_fld("F0", 1L, 0, (long *) NULL, DOUBLE, (char **) NULL, ohd);
  add_fea_fld("prob_voice", 1L, 0, (long *) NULL, DOUBLE, (char **) NULL, ohd);
  add_fea_fld("rms", 1L, 0, (long *) NULL, DOUBLE, (char **) NULL, ohd);
  add_fea_fld("ac_peak", 1L, 0, (long *) NULL, DOUBLE, (char **) NULL, ohd);
  fea_rec = allo_fea_rec(ohd);
  rec_F0 = (double *) get_fea_ptr(fea_rec,"F0", ohd);
  rec_pv = (double *) get_fea_ptr(fea_rec,"prob_voice", ohd);
  rec_rms = (double *) get_fea_ptr(fea_rec,"rms", ohd);
  rec_acp = (double *) get_fea_ptr(fea_rec,"ac_peak", ohd);

  output_starts = par->wind_dur/2.0; 
  /* Average delay due to loc. of ref. window center. */
  frame_rate = 1.0 / par->frame_step;

  write_header(ohd, ofile);

  /* Initialize variables in get_f0.c; allocate data structures;
   * determine length and overlap of input frames to read.
   */
  if (init_dp_f0(sf, par, &buff_size, &sdstep)
      || buff_size > INT_MAX || sdstep > INT_MAX)
  {
    Fprintf(stderr, "%s: problem in init_dp_f0().\n", ProgName);
    exit(1);
  }

  if (debug_level)
    Fprintf(stderr, "%s: init_dp_f0 returned buff_size %ld, sdstep %ld.\n",
	    ProgName, buff_size, sdstep);

  sd_rec = allo_feasd_recs(ihd, FLOAT, buff_size, NULL, NO);
  fdata = (float *) sd_rec->data;
  
  fea_skiprec(ifile, 0, ihd);

  actsize = get_feasd_recs(sd_rec, 0L, buff_size, ihd, ifile);

  while (TRUE) {

    done = (actsize < buff_size);

    if (dp_f0(fdata, (int) actsize, (int) sdstep, sf, par,
	      &f0p, &vuvp, &rms_speech, &acpkp, &vecsize, done)) {
      Fprintf(stderr, "%s: problem in dp_f0().\n", ProgName);
      exit(1);
    }

    for (i = vecsize - 1; i >= 0; i--) {
      *rec_F0 = f0p[i];
      *rec_pv = vuvp[i];
      *rec_rms = rms_speech[i];
      *rec_acp = acpkp[i];
      put_fea_rec(fea_rec, ohd, ofile);
    }

    if (done)
      break;
    
    actsize = get_feasd_orecs( sd_rec, 0L, buff_size, sdstep, ihd, ifile);

  }

  exit(0);
}


/*
 * Some consistency checks on parameter values.
 * Return a positive integer if any errors detected, 0 if none.
 */

static int
check_f0_params(par, sample_freq)
    F0_params   *par;
    double      sample_freq;
{
  int	  error = 0;
  double  dstep;

  if((par->cand_thresh < 0.01) || (par->cand_thresh > 0.99)) {
    Fprintf(stderr,
	    "%s: ERROR: cand_thresh parameter must be between [0.01, 0.99].\n",
	    ProgName);
    error++;
  }
  if((par->wind_dur > .1) || (par->wind_dur < .0001)) {
    Fprintf(stderr,
	    "%s: ERROR: wind_dur parameter must be between [0.0001, 0.1].\n",
	    ProgName);
    error++;
  }
  if((par->n_cands > 100) || (par->n_cands < 3)){
    Fprintf(stderr,
	    "%s: ERROR: n_cands parameter must be between [3,100].\n",
	    ProgName); 
    error++;
  }
  if((par->max_f0 <= par->min_f0) || (par->max_f0 >= (sample_freq/2.0)) ||
     (par->min_f0 < (sample_freq/10000.0))){
    Fprintf(stderr,
	    "%s: ERROR: min(max)_f0 parameter inconsistent with sampling frequency.\n",
	    ProgName); 
    error++;
  }
  dstep = ((double)((int)(0.5 + (sample_freq * par->frame_step))))/sample_freq;
  if(dstep != par->frame_step) {
    if(debug_level)
      Fprintf(stderr,
	      "%s: Frame step set to %f to exactly match signal sample rate.\n",
	      ProgName, dstep);
    par->frame_step = dstep;
  }
  if((par->frame_step > 0.1) || (par->frame_step < (1.0/sample_freq))){
    Fprintf(stderr,
	    "%s: ERROR: frame_step parameter must be between [1/sampling rate, 0.1].\n",
	    ProgName); 
    error++;
  }

  return(error);
}
