#ifndef PRO_CONFIG_H
#define PRO_CONFIG_H

#define APP_VERSION "1.0.15"

#define APP_PATH "/usr/share/lntgr2z/"

#define EXE_SERVICE_NAME "lntgr2zservice"
#define EXE_TRANS_NAME "lntgr2zsm"
#define EXE_UI_NAME "lntgr2zsmui"
#define EXE_FILTERLIB_NAME "lntgr2zfilterlib"
#define EXE_VOP_NAME "lntgr2zvop"
#define EXE_FILTERUI_NAME "lntgr2zfilterui"

//#ifdef DEBUG_DEBUG
#define LOG_SERVICE_FILE "/tmp/lntgr2zservice.log"
#define LOG_TRANS_FILE "/tmp/lntgr2zsm.log"
#define LOG_UI_FILE "/tmp/lntgr2zsmui.log"
#define LOG_FILTERLIB_FILE "/tmp/lntgr2zfilterlib.log"
#define LOG_VOP_FILE "/tmp/lntgr2zvop.log"
//#else
//#define LOG_SERVICE_FILE "/usr/share/lntgr2z/service.log"
//#define LOG_TRANS_FILE "/usr/share/lntgr2z/sm.log"
//#define LOG_UI_FILE "/usr/share/lntgr2z/ui.log"
//#define LOG_FILTERLIB_FILE "/var/spool/cups/tmp/lntgr2zfilterlib.log"
//#define LOG_VOP_FILE "/usr/share/lntgr2z/vop.log"
//#endif

#define SERVICE_TRANS_PATH "/tmp/lntgr2ztransrw"
#ifdef DEBUG_DEBUG
#define FILTER_TRANS_PATH "/tmp/lntgr2ztransrw"
#else
#define FILTER_TRANS_PATH "/var/spool/cups/tmp/lntgr2ztransrw"
#endif

#define LOCKER_SERVICE "/tmp/.lntgr2zservice_locker"
#define LOCKER_TRANS "/tmp/.lntgr2zsm_locker"
#define LOCKER_UI "/tmp/.lntgr2zsmui_locker"
#define LOCKER_VOP "/tmp/.lntgr2zvop_locker"

#define DOMAIN_UIEXE "/tmp/lntgr2zsmui.domain"
#define DOMAIN_VOPEXE "/tmp/lntgr2zvop.domain"

#define STATUS_FILE "/tmp/.lntgr2zstatus"
#define STATUS_LOCKER_FILE "/tmp/.lntgr2zstatus_lock"
#define TEST_STATUS_FILE "/tmp/.status"
#define TEST_STATUS_LOCKER_FILE "/tmp/.status_lock"

#ifdef DEBUG_DEBUG
#define CONFIG_FILE "/tmp/lntgr2z_app.conf"
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

#define TMP_SCAN_DIR "/tmp/lntgr2zvop_scan"
#define LOCKER_SANE "/tmp/.lntgr2zscan_lock"
#define SANE_INFO_FILE "/tmp/.lntgr2zscan_used"
#endif // PRO_CONFIG_H
