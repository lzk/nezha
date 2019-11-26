#ifndef PRO_CONFIG_H
#define PRO_CONFIG_H

#define APP_VERSION "1.0.9"

#define APP_PATH "/usr/share/lntgrvop/"

#define EXE_TRANS_NAME "lntgr1zsm"
#define EXE_UI_NAME "lntgr1zsmui"
#define EXE_FILTERLIB_NAME "lntgr1zfilterlib"
#define EXE_VOP_NAME "lntgr1zvop"
#define EXE_FILTERUI_NAME "lntgr1zfilterui"

#ifdef DEBUG_DEBUG
#define LOG_TRANS_FILE "/tmp/lntgr1zsm.log"
#define LOG_UI_FILE "/tmp/lntgr1zsmui.log"
#define LOG_FILTERLIB_FILE "/tmp/lntgr1zfilterlib.log"
#define LOG_VOP_FILE "/tmp/lntgr1zvop.log"
#else
#define LOG_TRANS_FILE "/usr/share/lntgrvop/sm.log"
#define LOG_UI_FILE "/usr/share/lntgrvop/ui.log"
#define LOG_FILTERLIB_FILE "/var/spool/cups/tmp/lntgr1zfilterlib.log"
#define LOG_VOP_FILE "/usr/share/lntgrvop/vop.log"
#endif


#ifdef DEBUG_DEBUG
#define FILTER_TRANS_PATH "/tmp/lntgr1zsm.domain"
#else
#define FILTER_TRANS_PATH "/var/run/cups/lntgr1zsm.domain"
#endif

#define LOCKER_TRANS "/tmp/.lntgr1zsm_locker"
#define LOCKER_UI "/tmp/.lntgr1zsmui_locker"
#define LOCKER_VOP "/tmp/.lntgr1zvop_locker"

#define DOMAIN_UIEXE "/tmp/lntgr1zsmui.domain"

#define STATUS_FILE "/tmp/.lntgr1zstatus"
#define STATUS_LOCKER_FILE "/tmp/.lntgr1zstatus_lock"
#define TEST_STATUS_FILE "/tmp/.status"
#define TEST_STATUS_LOCKER_FILE "/tmp/.status_lock"

#ifdef DEBUG_DEBUG
#define CONFIG_FILE "/tmp/app.conf"
#else
#define CONFIG_FILE "/usr/share/lntgrvop/config/app.conf"
#endif
#define CONFIG_KEY_isLoginWhenPrinting "is_login_when_printing"


#ifndef DEBUG_DEBUG
#define Trans_Conf_file "/usr/share/lntgrvop/config/sm.conf"
//#define Trans_Conf_file "/tmp/.lntgr1zsm.conf"
#define JOBHISTORYFILE "/usr/share/lntgrvop/job_history.xml"
#else
#define Trans_Conf_file "/tmp/lntgr1zsm.conf"
#define JOBHISTORYFILE "/tmp/lntgr1zjob_history.xml"
#endif

#define LOCKER_SANE "/tmp/.lntgr1zscan_lock"
#define SANE_INFO_FILE "/tmp/.lntgr1zscan_used"
#endif // PRO_CONFIG_H
