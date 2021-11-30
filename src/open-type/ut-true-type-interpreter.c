#include <assert.h>

#include "ut.h"

// https://developer.apple.com/fonts/TrueType-Reference-Manual/
// https://learn.microsoft.com/en-us/typography/opentype/spec/

typedef struct {
  UtObject object;

  // Instructions being run.
  UtObject *instructions;

  size_t instruction_pointer;

  bool auto_flip;

  // First error that occured during run.
  UtObject *error;
} UtTrueTypeInterpreter;

static void set_error(UtTrueTypeInterpreter *self, const char *description) {
  if (self->error != NULL) {
    return;
  }

  self->error = ut_open_type_error_new(description);
}

static void set_error_take(UtTrueTypeInterpreter *self, char *description) {
  set_error(self, description);
  free(description);
}

static void clear(UtTrueTypeInterpreter *self) {}

static uint32_t pop(UtTrueTypeInterpreter *self) { return 0; }

static void push(UtTrueTypeInterpreter *self, uint32_t value) {}

static void push_bool(UtTrueTypeInterpreter *self, bool value) {
  push(self, value ? 1 : 0);
}

static void do_svtca(UtTrueTypeInterpreter *self, bool x_axis) {
  // FIXME
}

static void do_spvtca(UtTrueTypeInterpreter *self, bool x_axis) {
  set_error(self, "spvtca not supported");
}

static void do_spvtl(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "spvtl not supported");
}

static void do_sfvtl(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "sfvtl not supported");
}

static void do_spvfs(UtTrueTypeInterpreter *self) {
  set_error(self, "spvfs not supported");
}

static void do_sfvfs(UtTrueTypeInterpreter *self) {
  set_error(self, "sfvfs not supported");
}

static void do_gpv(UtTrueTypeInterpreter *self) {
  set_error(self, "gpv not supported");
}

static void do_gfv(UtTrueTypeInterpreter *self) {
  set_error(self, "gfv not supported");
}

static void do_sfvtpv(UtTrueTypeInterpreter *self) {
  set_error(self, "sfvtpv not supported");
}

static void do_isect(UtTrueTypeInterpreter *self) {
  set_error(self, "isect not supported");
}

static void do_srp0(UtTrueTypeInterpreter *self) {
  /*uint32_t p = */ pop(self);
  // FIXME
}

static void do_srp1(UtTrueTypeInterpreter *self) {
  /*uint32_t p = */ pop(self);
  // FIXME
}

static void do_srp2(UtTrueTypeInterpreter *self) {
  /*uint32_t p = */ pop(self);
  // FIXME
}

static void do_szp0(UtTrueTypeInterpreter *self) {
  set_error(self, "szp0 not supported");
}

static void do_szp1(UtTrueTypeInterpreter *self) {
  set_error(self, "szp1 not supported");
}

static void do_szp2(UtTrueTypeInterpreter *self) {
  set_error(self, "szp2 not supported");
}

static void do_szps(UtTrueTypeInterpreter *self) {
  set_error(self, "szps not supported");
}

static void do_sloop(UtTrueTypeInterpreter *self) {
  set_error(self, "sloop not supported");
}

static void do_rtg(UtTrueTypeInterpreter *self) {
  set_error(self, "rtg not supported");
}

static void do_rthg(UtTrueTypeInterpreter *self) {
  set_error(self, "rthg not supported");
}

static void do_smd(UtTrueTypeInterpreter *self) {
  set_error(self, "SMD not supported");
}

static void do_else(UtTrueTypeInterpreter *self) {
  set_error(self, "ELSE not supported");
}

static void do_jmpr(UtTrueTypeInterpreter *self) {
  set_error(self, "JMPR not supported");
}

static void do_scvtci(UtTrueTypeInterpreter *self) {
  set_error(self, "scvtci not supported");
}

static void do_sswci(UtTrueTypeInterpreter *self) {
  set_error(self, "sswci not supported");
}

static void do_ssw(UtTrueTypeInterpreter *self) {
  set_error(self, "ssw not supported");
}

static void do_dup(UtTrueTypeInterpreter *self) {
  uint32_t e = pop(self);
  push(self, e);
  push(self, e);
}

static void do_pop(UtTrueTypeInterpreter *self) { pop(self); }

static void do_clear(UtTrueTypeInterpreter *self) { clear(self); }

static void do_swap(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push(self, e2);
  push(self, e1);
}

static void do_depth(UtTrueTypeInterpreter *self) {
  push(self, 0); // FIXME: Depth of stack
}

static void do_cindex(UtTrueTypeInterpreter *self) {
  set_error(self, "cindex not supported");
}

static void do_mindex(UtTrueTypeInterpreter *self) {
  set_error(self, "mindex not supported");
}

static void do_alignpts(UtTrueTypeInterpreter *self) {
  set_error(self, "alignpts not supported");
}

static void do_utp(UtTrueTypeInterpreter *self) {
  set_error(self, "utp not supported");
}

static void do_loopcall(UtTrueTypeInterpreter *self) {
  set_error(self, "LOOPCALL not supported");
}

static void do_call(UtTrueTypeInterpreter *self) {
  set_error(self, "CALL not supported");
}

static void do_fdef(UtTrueTypeInterpreter *self) {
  set_error(self, "FDEF not supported");
}

static void do_endf(UtTrueTypeInterpreter *self) {
  set_error(self, "ENDF not supported");
}

static void do_mdap(UtTrueTypeInterpreter *self, bool round) {
  /*uint32_t p = */ pop(self);
  // FIXME
}

static void do_iup(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "iup not supported");
}

static void do_shp(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "shp not supported");
}

static void do_shc(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "shc not supported");
}

static void do_shz(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "shz not supported");
}

static void do_shpix(UtTrueTypeInterpreter *self) {
  set_error(self, "shpix not supported");
}

static void do_ip(UtTrueTypeInterpreter *self) {
  set_error(self, "ip not supported");
}

static void do_msirp(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "msirp not supported");
}

static void do_aligngrp(UtTrueTypeInterpreter *self) {
  set_error(self, "aligngrp not supported");
}

static void do_rtdg(UtTrueTypeInterpreter *self) {
  set_error(self, "rtdg not supported");
}

static void do_miap(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "miap not supported");
}

static void do_npushb(UtTrueTypeInterpreter *self) {
  size_t count =
      ut_uint8_list_get_element(self->instructions, self->instruction_pointer);
  self->instruction_pointer++;

  self->instruction_pointer += count;
}

static void do_npushw(UtTrueTypeInterpreter *self) {
  size_t count =
      ut_uint8_list_get_element(self->instructions, self->instruction_pointer);
  self->instruction_pointer++;

  self->instruction_pointer += count * 2;
}

static void do_ws(UtTrueTypeInterpreter *self) {
  set_error(self, "ws not supported");
}

static void do_rs(UtTrueTypeInterpreter *self) {
  set_error(self, "rs not supported");
}

static void do_wcvtp(UtTrueTypeInterpreter *self) {
  set_error(self, "wcvtp not supported");
}

static void do_rcvt(UtTrueTypeInterpreter *self) {
  set_error(self, "rcvt not supported");
}

static void do_gc(UtTrueTypeInterpreter *self, bool use_original) {
  set_error(self, "gc not supported");
}

static void do_scfs(UtTrueTypeInterpreter *self) {
  set_error(self, "scfs not supported");
}

static void do_md(UtTrueTypeInterpreter *self, bool use_original) {
  set_error(self, "md not supported");
}

static void do_mppem(UtTrueTypeInterpreter *self) {
  set_error(self, "mppem not supported");
}

static void do_mps(UtTrueTypeInterpreter *self) {
  set_error(self, "mps not supported");
}

static void do_flipon(UtTrueTypeInterpreter *self) { self->auto_flip = true; }

static void do_flipoff(UtTrueTypeInterpreter *self) { self->auto_flip = false; }

static void do_debug(UtTrueTypeInterpreter *self) { pop(self); }

static void do_lt(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push(self, e1 < e2 ? 1 : 0);
}

static void do_lteq(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push(self, e1 <= e2 ? 1 : 0);
}

static void do_gt(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push(self, e1 > e2 ? 1 : 0);
}

static void do_gteq(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push_bool(self, e1 >= e2);
}

static void do_eq(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push_bool(self, e1 == e2);
}

static void do_neq(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push_bool(self, e1 != e2);
}

static void do_odd(UtTrueTypeInterpreter *self) {
  uint32_t e1 = pop(self);
  push_bool(self, e1 % 2 == 1); // FIXME: rounding and 26.6
}

static void do_even(UtTrueTypeInterpreter *self) {
  uint32_t e1 = pop(self);
  push_bool(self, e1 % 2 == 0); // FIXME: rounding and 26.6
}

static void do_if(UtTrueTypeInterpreter *self) {
  set_error(self, "IF not supported");
}

static void do_eif(UtTrueTypeInterpreter *self) {
  set_error(self, "EIF not supported");
}

static void do_and(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push_bool(self, e1 != 0 && e2 != 0);
}

static void do_or(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push_bool(self, e1 != 0 || e2 != 0);
}

static void do_not(UtTrueTypeInterpreter *self) {
  uint32_t e = pop(self);
  push_bool(self, e == 0);
}

static void do_deltap1(UtTrueTypeInterpreter *self) {
  set_error(self, "deltap1 not supported");
}

static void do_sdb(UtTrueTypeInterpreter *self) {
  set_error(self, "sdb not supported");
}

static void do_sds(UtTrueTypeInterpreter *self) {
  set_error(self, "sds not supported");
}

static void do_add(UtTrueTypeInterpreter *self) {
  uint32_t n2 = pop(self);
  uint32_t n1 = pop(self);
  push(self, n1 + n2); // FIXME: 26.6
}

static void do_sub(UtTrueTypeInterpreter *self) {
  uint32_t n2 = pop(self);
  uint32_t n1 = pop(self);
  push(self, n2 - n1); // FIXME: 26.6
}

static void do_div(UtTrueTypeInterpreter *self) {
  uint32_t n2 = pop(self);
  uint32_t n1 = pop(self);
  push(self, (n1 * 64) / n2); // FIXME: 26.6
}

static void do_mul(UtTrueTypeInterpreter *self) {
  uint32_t n2 = pop(self);
  uint32_t n1 = pop(self);
  push(self, (n2 * n1) / 64); // FIXME: 26.6
}

static void do_abs(UtTrueTypeInterpreter *self) {
  uint32_t n = pop(self);
  push(self, n < 0 ? -n : n); // FIXME: 26.6
}

static void do_neg(UtTrueTypeInterpreter *self) {
  uint32_t n = pop(self);
  push(self, -n); // FIXME: 26.6
}

static void do_floor(UtTrueTypeInterpreter *self) {
  uint32_t n = pop(self);
  // FIXME
  push(self, n); // FIXME: 26.6
}

static void do_ceiling(UtTrueTypeInterpreter *self) {
  uint32_t n = pop(self);
  // FIXME
  push(self, n); // FIXME: 26.6
}

static void do_round(UtTrueTypeInterpreter *self, uint8_t distance_type) {
  set_error(self, "round not supported");
}

static void do_nround(UtTrueTypeInterpreter *self, uint8_t distance_type) {
  set_error(self, "nround not supported");
}

static void do_wcvtf(UtTrueTypeInterpreter *self) {
  set_error(self, "wcvtf not supported");
}

static void do_deltap2(UtTrueTypeInterpreter *self) {
  set_error(self, "deltap2 not supported");
}

static void do_deltap3(UtTrueTypeInterpreter *self) {
  set_error(self, "deltap3 not supported");
}

static void do_deltac1(UtTrueTypeInterpreter *self) {
  set_error(self, "deltac1 not supported");
}

static void do_deltac2(UtTrueTypeInterpreter *self) {
  set_error(self, "deltac2 not supported");
}

static void do_deltac3(UtTrueTypeInterpreter *self) {
  set_error(self, "deltac3 not supported");
}

static void do_sround(UtTrueTypeInterpreter *self) {
  set_error(self, "sround not supported");
}

static void do_s45round(UtTrueTypeInterpreter *self) {
  set_error(self, "s45round not supported");
}

static void do_jrot(UtTrueTypeInterpreter *self) {
  set_error(self, "jrot not supported");
}

static void do_jrof(UtTrueTypeInterpreter *self) {
  set_error(self, "jrof not supported");
}

static void do_roff(UtTrueTypeInterpreter *self) {
  set_error(self, "roff not supported");
}

static void do_rutg(UtTrueTypeInterpreter *self) {
  set_error(self, "rutg not supported");
}

static void do_rdtg(UtTrueTypeInterpreter *self) {
  set_error(self, "rdtg not supported");
}

static void do_sangw(UtTrueTypeInterpreter *self) {
  set_error(self, "sangw not supported");
}

static void do_aa(UtTrueTypeInterpreter *self) {
  set_error(self, "aa not supported");
}

static void do_flippt(UtTrueTypeInterpreter *self) {
  set_error(self, "flippt not supported");
}

static void do_fliprgon(UtTrueTypeInterpreter *self) {
  set_error(self, "fliprgon not supported");
}

static void do_fliprgoff(UtTrueTypeInterpreter *self) {
  set_error(self, "fliprgoff not supported");
}

static void do_scanctrl(UtTrueTypeInterpreter *self) {
  set_error(self, "scanctrl not supported");
}

static void do_sdpvtl(UtTrueTypeInterpreter *self, bool) {
  set_error(self, "sdpvtl not supported");
}

static void do_getinfo(UtTrueTypeInterpreter *self) {
  set_error(self, "getinfo not supported");
}

static void do_idef(UtTrueTypeInterpreter *self) {
  set_error(self, "idef not supported");
}

static void do_roll(UtTrueTypeInterpreter *self) {
  set_error(self, "roll not supported");
}

static void do_max(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push(self, e1 > e2 ? e1 : e2);
}

static void do_min(UtTrueTypeInterpreter *self) {
  uint32_t e2 = pop(self);
  uint32_t e1 = pop(self);
  push(self, e1 < e2 ? e1 : e2);
}

static void do_scantype(UtTrueTypeInterpreter *self) {
  set_error(self, "scantype not supported");
}

static void do_instctrl(UtTrueTypeInterpreter *self) {
  set_error(self, "instctrl not supported");
}

static void do_pushb(UtTrueTypeInterpreter *self, size_t count) {
  // FIXME: Check valid
  self->instruction_pointer += count;
}

static void do_pushw(UtTrueTypeInterpreter *self, size_t count) {
  // FIXME: Check valid
  self->instruction_pointer += count * 2;
}

static void do_mdrp(UtTrueTypeInterpreter *self, bool set_rp0,
                    bool min_distance, bool round_distance,
                    uint8_t distance_type) {
  /*uint32_t p = */ pop(self);
  // FIXME
}

static void do_mirp(UtTrueTypeInterpreter *self, bool set_rp0,
                    bool min_distance, bool round_distance,
                    uint8_t distance_type) {
  /*uint32_t n = */ pop(self);
  /*uint32_t p = */ pop(self);
  // FIXME
}

static bool run(UtTrueTypeInterpreter *self) {
  size_t instructions_length = ut_list_get_length(self->instructions);

  while (self->instruction_pointer < instructions_length) {
    uint8_t instruction = ut_uint8_list_get_element(self->instructions,
                                                    self->instruction_pointer);
    self->instruction_pointer++;

    switch (instruction) {
    case 0x00:
    case 0x01:
      do_svtca(self, instruction != 0);
      break;
    case 0x02:
    case 0x03:
      do_spvtca(self, instruction != 0);
      break;
    case 0x06:
    case 0x07:
      do_spvtl(self, instruction != 0);
      break;
    case 0x08:
    case 0x09:
      do_sfvtl(self, instruction != 0);
      break;
    case 0x0a:
      do_spvfs(self);
      break;
    case 0x0b:
      do_sfvfs(self);
      break;
    case 0x0c:
      do_gpv(self);
      break;
    case 0x0d:
      do_gfv(self);
      break;
    case 0x0e:
      do_sfvtpv(self);
      break;
    case 0x0f:
      do_isect(self);
      break;
    case 0x10:
      do_srp0(self);
      break;
    case 0x11:
      do_srp1(self);
      break;
    case 0x12:
      do_srp2(self);
      break;
    case 0x13:
      do_szp0(self);
      break;
    case 0x14:
      do_szp1(self);
      break;
    case 0x15:
      do_szp2(self);
      break;
    case 0x16:
      do_szps(self);
      break;
    case 0x17:
      do_sloop(self);
      break;
    case 0x18:
      do_rtg(self);
      break;
    case 0x19:
      do_rthg(self);
      break;
    case 0x1a:
      do_smd(self);
      break;
    case 0x1b:
      do_else(self);
      break;
    case 0x1c:
      do_jmpr(self);
      break;
    case 0x1d:
      do_scvtci(self);
      break;
    case 0x1e:
      do_sswci(self);
      break;
    case 0x1f:
      do_ssw(self);
      break;
    case 0x20:
      do_dup(self);
      break;
    case 0x21:
      do_pop(self);
      break;
    case 0x22:
      do_clear(self);
      break;
    case 0x23:
      do_swap(self);
      break;
    case 0x24:
      do_depth(self);
      break;
    case 0x25:
      do_cindex(self);
      break;
    case 0x26:
      do_mindex(self);
      break;
    case 0x27:
      do_alignpts(self);
      break;
    case 0x29:
      do_utp(self);
      break;
    case 0x2a:
      do_loopcall(self);
      break;
    case 0x2b:
      do_call(self);
      break;
    case 0x2c:
      do_fdef(self);
      break;
    case 0x2d:
      do_endf(self);
      break;
    case 0x2e:
      do_mdap(self, false);
      break;
    case 0x2f:
      do_mdap(self, true);
      break;
    case 0x30:
    case 0x31:
      do_iup(self, (instruction & 0x1) != 0);
      break;
    case 0x32:
    case 0x33:
      do_shp(self, (instruction & 0x1) != 0);
      break;
    case 0x34:
    case 0x35:
      do_shc(self, (instruction & 0x1) != 0);
      break;
    case 0x36:
    case 0x37:
      do_shz(self, (instruction & 0x1) != 0);
      break;
    case 0x38:
      do_shpix(self);
      break;
    case 0x39:
      do_ip(self);
      break;
    case 0x3a:
    case 0x3b:
      do_msirp(self, (instruction & 0x1) != 0);
      break;
    case 0x3c:
      do_aligngrp(self);
      break;
    case 0x3d:
      do_rtdg(self);
      break;
    case 0x3e:
    case 0x3f:
      do_miap(self, (instruction & 0x1) != 0);
      break;
    case 0x40:
      do_npushb(self);
      break;
    case 0x41:
      do_npushw(self);
      break;
    case 0x42:
      do_ws(self);
      break;
    case 0x43:
      do_rs(self);
      break;
    case 0x44:
      do_wcvtp(self);
      break;
    case 0x45:
      do_rcvt(self);
      break;
    case 0x46:
    case 0x47:
      do_gc(self, (instruction & 0x1) != 0);
      break;
    case 0x48:
      do_scfs(self);
      break;
    case 0x49:
    case 0x4a:
      do_md(self, (instruction & 0x1) != 0);
      break;
    case 0x4b:
      do_mppem(self);
      break;
    case 0x4c:
      do_mps(self);
      break;
    case 0x4d:
      do_flipon(self);
      break;
    case 0x4e:
      do_flipoff(self);
      break;
    case 0x4f:
      do_debug(self);
      break;
    case 0x50:
      do_lt(self);
      break;
    case 0x51:
      do_lteq(self);
      break;
    case 0x52:
      do_gt(self);
      break;
    case 0x53:
      do_gteq(self);
      break;
    case 0x54:
      do_eq(self);
      break;
    case 0x55:
      do_neq(self);
      break;
    case 0x56:
      do_odd(self);
      break;
    case 0x57:
      do_even(self);
      break;
    case 0x58:
      do_if(self);
      break;
    case 0x59:
      do_eif(self);
      break;
    case 0x5a:
      do_and(self);
      break;
    case 0x5b:
      do_or(self);
      break;
    case 0x5c:
      do_not(self);
      break;
    case 0x5d:
      do_deltap1(self);
      break;
    case 0x5e:
      do_sdb(self);
      break;
    case 0x5f:
      do_sds(self);
      break;
    case 0x60:
      do_add(self);
      break;
    case 0x61:
      do_sub(self);
      break;
    case 0x62:
      do_div(self);
      break;
    case 0x63:
      do_mul(self);
      break;
    case 0x64:
      do_abs(self);
      break;
    case 0x65:
      do_neg(self);
      break;
    case 0x66:
      do_floor(self);
      break;
    case 0x67:
      do_ceiling(self);
      break;
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
      do_round(self, instruction & 0x3);
      break;
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x6f:
      do_nround(self, instruction & 0x3);
      break;
    case 0x70:
      do_wcvtf(self);
      break;
    case 0x71:
      do_deltap2(self);
      break;
    case 0x72:
      do_deltap3(self);
      break;
    case 0x73:
      do_deltac1(self);
      break;
    case 0x74:
      do_deltac2(self);
      break;
    case 0x75:
      do_deltac3(self);
      break;
    case 0x76:
      do_sround(self);
      break;
    case 0x77:
      do_s45round(self);
      break;
    case 0x78:
      do_jrot(self);
      break;
    case 0x79:
      do_jrof(self);
      break;
    case 0x7a:
      do_roff(self);
      break;
    case 0x7c:
      do_rutg(self);
      break;
    case 0x7d:
      do_rdtg(self);
      break;
    case 0x7e:
      do_sangw(self);
      break;
    case 0x7f:
      do_aa(self);
      break;
    case 0x80:
      do_flippt(self);
      break;
    case 0x81:
      do_fliprgon(self);
      break;
    case 0x82:
      do_fliprgoff(self);
      break;
    case 0x85:
      do_scanctrl(self);
      break;
    case 0x86:
    case 0x87:
      do_sdpvtl(self, (instruction & 0x1) != 0);
      break;
    case 0x88:
      do_getinfo(self);
      break;
    case 0x89:
      do_idef(self);
      break;
    case 0x8a:
      do_roll(self);
      break;
    case 0x8b:
      do_max(self);
      break;
    case 0x8c:
      do_min(self);
      break;
    case 0x8d:
      do_scantype(self);
      break;
    case 0x8e:
      do_instctrl(self);
      break;
    case 0xb0:
    case 0xb1:
    case 0xb2:
    case 0xb3:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
      do_pushb(self, (instruction & 0x7) + 1);
      break;
    case 0xb8:
    case 0xb9:
    case 0xba:
    case 0xbb:
    case 0xbc:
    case 0xbd:
    case 0xbe:
    case 0xbf:
      do_pushw(self, (instruction & 0x7) + 1);
      break;
    case 0xc0:
    case 0xc1:
    case 0xc2:
    case 0xc3:
    case 0xc4:
    case 0xc5:
    case 0xc6:
    case 0xc7:
    case 0xc8:
    case 0xc9:
    case 0xca:
    case 0xcb:
    case 0xcc:
    case 0xcd:
    case 0xce:
    case 0xcf:
    case 0xd0:
    case 0xd1:
    case 0xd2:
    case 0xd3:
    case 0xd4:
    case 0xd5:
    case 0xd6:
    case 0xd7:
    case 0xd8:
    case 0xd9:
    case 0xda:
    case 0xdb:
    case 0xdc:
    case 0xdd:
    case 0xde:
    case 0xdf:
      do_mdrp(self, (instruction & 0x10) != 0, (instruction & 0x8) != 0,
              (instruction & 0x4) != 0, instruction & 0x3);
      break;
    case 0xe0:
    case 0xe1:
    case 0xe2:
    case 0xe3:
    case 0xe4:
    case 0xe5:
    case 0xe6:
    case 0xe7:
    case 0xe8:
    case 0xe9:
    case 0xea:
    case 0xeb:
    case 0xec:
    case 0xed:
    case 0xee:
    case 0xef:
    case 0xf0:
    case 0xf1:
    case 0xf2:
    case 0xf3:
    case 0xf4:
    case 0xf5:
    case 0xf6:
    case 0xf7:
    case 0xf8:
    case 0xf9:
    case 0xfa:
    case 0xfb:
    case 0xfc:
    case 0xfd:
    case 0xfe:
    case 0xff:
      do_mirp(self, (instruction & 0x10) != 0, (instruction & 0x8) != 0,
              (instruction & 0x4) != 0, instruction & 0x3);
      break;
    default:
      set_error_take(self, ut_cstring_new_printf("Unknown instruction 0x%02X",
                                                 instruction));
      return false;
    }

    if (self->error != NULL) {
      return false;
    }
  }

  return true;
}

static void ut_true_type_interpreter_cleanup(UtObject *object) {
  UtTrueTypeInterpreter *self = (UtTrueTypeInterpreter *)object;
  ut_object_unref(self->instructions);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTrueTypeInterpreter",
    .cleanup = ut_true_type_interpreter_cleanup};

UtObject *ut_true_type_interpreter_new(UtObject *instructions) {
  UtObject *object =
      ut_object_new(sizeof(UtTrueTypeInterpreter), &object_interface);
  UtTrueTypeInterpreter *self = (UtTrueTypeInterpreter *)object;
  self->instructions = ut_object_ref(instructions);
  return object;
}

void ut_true_type_interpreter_run(UtObject *object) {
  assert(ut_object_is_true_type_interpreter(object));
  UtTrueTypeInterpreter *self = (UtTrueTypeInterpreter *)object;

  if (!run(self)) {
    return;
  }
}

UtObject *ut_true_type_interpreter_get_error(UtObject *object) {
  assert(ut_object_is_true_type_interpreter(object));
  UtTrueTypeInterpreter *self = (UtTrueTypeInterpreter *)object;
  return self->error;
}

bool ut_object_is_true_type_interpreter(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
