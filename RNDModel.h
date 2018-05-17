/**************************************
Author:林玥
Date:2017/11/26
Update:2017/11/26
Description:模型类，得到道路网密度估计值和依赖-偏好指数
***************************************/
#ifndef RNDMODEL_H
#define RNDMODEL_H

#include <vector>
#include <list>
#include <algorithm>
#include "LinearRegression.h"
#include "LrVector.h"
#include "RNDDataSource.h"
using namespace std;

class RNDModel
{
public:

    RNDModel();
    ~RNDModel();

    /* 计算行政区内道路密度 */
    void GetRoadDensity(list<Admin* > dataSet);

    /* 获得回归原始矩阵 */
    void GetRawData(list<Admin* > dataSet);

    /* 模型运算 */
    void RNDProcess(list<Admin *> dataSet, int num);


private:
    vector<vector<double> > X;
    vector<double> Y;
};

#endif // RNDMODEL_H
