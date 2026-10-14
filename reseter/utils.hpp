#pragma once

#include <JaszczurHAL.h>
#include <Credentials.h>


const char *getMyMAC(void);
const char *getMyHostname(void);
void clearAndFreeSecret(char *secret);
