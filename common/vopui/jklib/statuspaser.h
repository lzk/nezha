#ifndef STATUSPASER_H
#define STATUSPASER_H


class StatusPaser
{
public:
    StatusPaser();

    static bool is_enable_scan(int status);
    static bool is_enable_copy(int status);
    static bool is_enable_setting(int status);
};

#endif // STATUSPASER_H
