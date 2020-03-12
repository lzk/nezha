#ifndef FILTERCOMMON_H
#define FILTERCOMMON_H

int filter_get_status(int jobid);
void filter_update_status(int jobid ,int status);
void filter_update_result(int jobid ,int result);
int filter_get_result(int jobid);

#endif // FILTERCOMMON_H
