#include "statuspaser.h"
#include "uiconfig.h"
StatusPaser::StatusPaser()
{

}

bool StatusPaser::is_enable_scan(int status)
{
    bool enable = false;
    switch (status) {
    case UIConfig::WarmingUp:
        enable = true;
        break;
    default:
    {
        int status_type = UIConfig::GetStatusTypeForUI(status);
        switch (status_type) {
        case UIConfig::Status_Ready:
        case UIConfig::Status_Sleep:
            enable = true;
            break;
        default:
            break;
        }
    }
        break;
    }
    return enable;
}

bool StatusPaser::is_enable_copy(int status)
{
    bool enable = false;
    switch (status) {
    case UIConfig::WarmingUp:
        enable = true;
        break;
    default:
    {
        int status_type = UIConfig::GetStatusTypeForUI(status);
        switch (status_type) {
        case UIConfig::Status_Ready:
        case UIConfig::Status_Sleep:
            enable = true;
            break;
        default:
            break;
        }
    }
    }
    return enable;
}

bool StatusPaser::is_enable_setting(int status)
{
    bool enable = true;
    switch (status) {
    default:
    {
        int status_type = UIConfig::GetStatusTypeForUI(status);
        switch (status_type) {
        case UIConfig::Status_Offline:
            enable = false;
            break;
        default:
            break;
        }
    }
    }
    return enable;
}
