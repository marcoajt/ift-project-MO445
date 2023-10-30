//
// Created by Samuel Martins on 2019-01-04.
//

#ifndef IFT_LIFO_H
#define IFT_LIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "/home/marco/Documentos/Unicamp/Disciplinas/1-Semestre/Image_Analysis/Codigo/libmo445/include/BasicDataTypes.h"

typedef struct ift_lifo {
    int  *LIFO;
    char *color;
    int   last;
    int   n;
} iftLIFO;

iftLIFO *iftCreateLIFO(int n);
void     iftDestroyLIFO(iftLIFO **L);
char     iftInsertLIFO(iftLIFO *L, int node);
int      iftRemoveLIFO(iftLIFO *L);
char     iftFullLIFO(iftLIFO *L);
char     iftEmptyLIFO(iftLIFO *L);
void     iftResetLIFO(iftLIFO *L);

#ifdef __cplusplus
}
#endif

#endif //IFT_LIFO_H
