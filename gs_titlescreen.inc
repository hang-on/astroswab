; gs_titlescreen.inc
;
prepare_titlescreen:
  ld a,GS_RUN_TITLESCREEN
  ld (game_state),a
jp main_loop
;
run_titlescreen:
  ;
  nop
jp main_loop
