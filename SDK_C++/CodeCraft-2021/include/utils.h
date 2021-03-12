//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_UTILS_H
#define HUAWEI_CODECRAFT_UTILS_H
#include "iostream"

#define LOGD(args...) fprintf(stdout,"D:");fprintf(stdout, ##args); putchar('\n')
#define LOGI(args...) fprintf(stdout,"I:");fprintf(stdout, ##args); putchar('\n')
#define LOGE(args...) fprintf(stdout,"E:");fprintf(stdout, ##args); putchar('\n')

#endif //HUAWEI_CODECRAFT_UTILS_H
