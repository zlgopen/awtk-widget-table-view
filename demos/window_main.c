#include "awtk.h"
#include "window_main_common.inc"

/**
 * 初始化
 */
ret_t application_init(void) {
  open_main_window("main");

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
