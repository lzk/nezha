#ifndef PRO_CONFIG_H
#define PRO_CONFIG_H

#define APP_VERSION "1.0.18"

#define APP_PATH "/usr/share/lntgr3z/"

#define EXE_SERVICE_NAME "lntgr3zservice"
#define EXE_TRANS_NAME "lntgr3zsm"
#define EXE_UI_NAME "lntgr3zsmui"
#define EXE_FILTERLIB_NAME "lntgr3zfilterlib"
#define EXE_VOP_NAME "lntgr3zvop"
#define EXE_FILTERUI_NAME "lntgr3zfilterui"

//#ifdef DEBUG_DEBUG
#define LOG_SERVICE_FILE "/tmp/lntgr3zservice.log"
#define LOG_TRANS_FILE "/tmp/lntgr3zsm.log"
#define LOG_UI_FILE "/tmp/lntgr3zsmui.log"
#define LOG_FILTERLIB_FILE "/tmp/lntgr3zfilterlib.log"
#define LOG_VOP_FILE "/tmp/lntgr3zvop.log"
//#else
//#define LOG_SERVICE_FILE "/usr/share/lntgr3z/service.log"
//#define LOG_TRANS_FILE "/usr/share/lntgr3z/sm.log"
//#define LOG_UI_FILE "/usr/share/lntgr3z/ui.log"
//#define LOG_FILTERLIB_FILE "/var/spool/cups/tmp/lntgr3zfilterlib.log"
//#define LOG_VOP_FILE "/usr/share/lntgr3z/vop.log"
//#endif

#define SERVICE_TRANS_PATH "/tmp/lntgr3ztransrw"
#ifdef DEBUG_DEBUG
#define FILTER_TRANS_PATH "/tmp/lntgr3ztransrw"
#else
#define FILTER_TRANS_PATH "/var/spool/cups/tmp/lntgr3ztransrw"
#endif

#define LOCKER_SERVICE "/tmp/.lntgr3zservice_locker"
#define LOCKER_TRANS "/tmp/.lntgr3zsm_locker"
#define LOCKER_UI "/tmp/.lntgr3zsmui_locker"
#define LOCKER_VOP "/tmp/.lntgr3zvop_locker"

#define DOMAIN_UIEXE "/tmp/lntgr3zsmui.domain"
#define DOMAIN_VOPEXE "/tmp/lntgr3zvop.domain"

#define STATUS_FILE "/tmp/.lntgr3zstatus"
#define STATUS_LOCKER_FILE "/tmp/.lntgr3zstatus_lock"
#define TEST_STATUS_FILE "/tmp/.status"
#define TEST_STATUS_LOCKER_FILE "/tmp/.status_lock"

#ifdef DEBUG_DEBUG
#define CONFIG_FILE "/tmp/lntgr3z_app.conf"
#else
#define CONFIG_FILE "/usr/share/lntgr3z/config/app.conf"
#endif
#define CONFIG_KEY_isLoginWhenPrinting "is_login_when_printing"


#ifndef DEBUG_DEBUG
#define Trans_Conf_file "/usr/share/lntgr3z/config/sm.conf"
//#define Trans_Conf_file "/tmp/.lntgr3zsm.conf"
#define JOBHISTORYFILE "/usr/share/lntgr3z/job_history.xml"
#else
#define Trans_Conf_file "/tmp/lntgr3zsm.conf"
#define JOBHISTORYFILE "/tmp/lntgr3zjob_history.xml"
#endif

#define TMP_SCAN_DIR "/tmp/lntgr3zvop_scan"
#define LOCKER_SANE "/tmp/.lntgr3zscan_lock"
#define SANE_INFO_FILE "/tmp/.lntgr3zscan_used"
#endif // PRO_CONFIG_H
