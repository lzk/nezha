#ifndef PRO_CONFIG_H
#define PRO_CONFIG_H

#define APP_VERSION "1.0.11"

#define APP_PATH "/usr/share/lntgr2z/"

#define EXE_TRANS_NAME "lntgr2zsm"
#define EXE_UI_NAME "lntgr2zsmui"
#define EXE_FILTERLIB_NAME "lntgr2zfilterlib"
#define EXE_VOP_NAME "lntgr2zvop"
#define EXE_FILTERUI_NAME "lntgr2zfilterui"

#ifdef DEBUG_DEBUG
#define LOG_TRANS_FILE "/tmp/lntgr2zsm.log"
#define LOG_UI_FILE "/tmp/lntgr2zsmui.log"
#define LOG_FILTERLIB_FILE "/tmp/lntgr2zfilterlib.log"
#define LOG_VOP_FILE "/tmp/lntgr2zvop.log"
#else
#define LOG_TRANS_FILE "/usr/share/lntgr2z/sm.log"
#define LOG_UI_FILE "/usr/share/lntgr2z/ui.log"
#define LOG_FILTERLIB_FILE "/var/spool/cups/tmp/lntgr2zfilterlib.log"
#define LOG_VOP_FILE "/usr/share/lntgr2z/vop.log"
#endif


#ifdef DEBUG_DEBUG
#define FILTER_TRANS_PATH "/tmp/lntgr2zsm.domain"
#else
#define FILTER_TRANS_PATH "/var/run/cups/lntgr2zsm.domain"
#endif

#define LOCKER_TRANS "/tmp/.lntgr2zsm_locker"
#define LOCKER_UI "/tmp/.lntgr2zsmui_locker"
#define LOCKER_VOP "/tmp/.lntgr2zvop_locker"

#define DOMAIN_UIEXE "/tmp/lntgr2zsmui.domain"

#define STATUS_FILE "/tmp/.lntgr2zstatus"
#define STATUS_LOCKER_FILE "/tmp/.lntgr2zstatus_lock"
#define TEST_STATUS_FILE "/tmp/.status"
#define TEST_STATUS_LOCKER_FILE "/tmp/.status_lock"

#ifdef DEBUG_DEBUG
#define CONFIG_FILE "/tmp/app.conf"
#else
#define CONFIG_FILE "/usr/share/lntgr2z/config/app.conf"
#endif
#define CONFIG_KEY_isLoginWhenPrinting "is_login_when_printing"


#ifndef DEBUG_DEBUG
#define Trans_Conf_file "/usr/share/lntgr2z/config/sm.conf"
//#define Trans_Conf_file "/tmp/.lntgr2zsm.conf"
#define JOBHISTORYFILE "/usr/share/lntgr2z/job_history.xml"
#else
#define Trans_Conf_file "/tmp/lntgr2zsm.conf"
#define JOBHISTORYFILE "/tmp/lntgr2zjob_history.xml"
#endif

#define LOCKER_SANE "/tmp/.lntgr2zscan_lock"
#define SANE_INFO_FILE "/tmp/.lntgr2zscan_used"
#endif // PRO_CONFIG_H
