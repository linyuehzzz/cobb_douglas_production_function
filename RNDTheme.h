/**************************************
Author:林玥
Date:2017/11/26
Update:2017/12/1
Description:主题类，数据源和模型的运行环境
***************************************/
#ifndef RNDTHEME_H
#define RNDTHEME_H

#include "RNDDataSource.h"
#include "RNDModel.h"

class RNDTheme
{
public:

    RNDTheme();
    ~RNDTheme();

    /* 运行模型 */
    void RunModel();

private:
    RNDDataSource* d;//数据源
    RNDModel* m;//模型
};

#endif // RNDTHEME_H
