#include "watcher.h"
Watcher::Watcher(QObject *parent)
    : CurrentStatusReader(parent)
{

}

Watcher::~Watcher()
{
}

void Watcher::watcher_job()
{
    CurrentStatusReader::watcher_job();
}
