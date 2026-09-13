#ifndef SAVE_H
#define SAVE_H

#include "ocr.h"

void save_ocr(const char *path, Ocr *ocr);
Ocr *load_ocr(const char *path);

#endif
