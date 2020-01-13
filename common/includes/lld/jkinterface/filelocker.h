#ifndef FILELOCKER_H
#define FILELOCKER_H

#include <stdio.h>
//not thread safe
class FileLocker
{
public:
    FileLocker();
    int lock(const char* path);
    int trylock(const char* path);
    int unlock();

protected:
    FILE* fp;
    char lock_file[256];
};

#endif // FILELOCKER_H
