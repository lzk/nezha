#ifndef FILTER_CHECK_FINGER_H
#define FILTER_CHECK_FINGER_H

#ifdef __cplusplus
extern "C" {
#endif

enum{
    Filterlib_Status_OK,
    Filterlib_Status_Fail,
    Filterlib_Status_Cancel,
    Filterlib_Status_invalidJobid,
    Filterlib_Status_timeout,
    Filterlib_Status_checking,
    Filterlib_Status_Abort_Print,
    Filterlib_Status_authorize_fail,
    Filterlib_Status_nologin_model,
};

enum{
    Filterlib_Model_LJ2320DN,
};

typedef
struct filterlib_struct
{
    int jobid;
    int status;
    int model;
    char username[256];
    char filename[256];
    char printername[256];
}
    filterlib_struct;
int filterlib(filterlib_struct* para);
int filterlib_exit();
int filterlib_abort();
#ifdef __cplusplus
}
#endif
#endif // FILTER_CHECK_FINGER_H
