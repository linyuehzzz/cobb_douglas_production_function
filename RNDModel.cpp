#include "RNDModel.h"

RNDModel::RNDModel()
{

}

RNDModel::~RNDModel()
{

}

/// <summary>
/// 计算道路密度
/// </summary>
void RNDModel::GetRoadDensity(list<Admin* > dataSet)
{
    Admin* admin;
    Road* road;
    double len;
    for(list<Admin* >::iterator iter = dataSet.begin(); iter != dataSet.end(); iter++)
    {
        admin = *iter;
        for(vector<Road* > ::iterator it = admin->roads.begin(); it != admin->roads.end(); it++)
        {
            road = *it;
            len += road->len;
        }
        admin->roadDens = len / admin->area *1000000; // 千米/平方公里
        len = 0;
    }
}

/// <summary>
/// 获得回归原始矩阵
/// </summary>
void RNDModel::GetRawData(list<Admin* > dataSet)
{
    Admin* admin;
    vector<double> vec;
    for(list<Admin* >::iterator iter = dataSet.begin(); iter != dataSet.end(); iter++)
    {
        admin = *iter;
        vec.push_back(log(admin->popDens));
        vec.push_back(log(admin->GDPDens));
        X.push_back(vec);
        vec.clear();

        Y.push_back(log(admin->roadDens));
    }
}


/// <summary>
/// 模型计算
/// </summary>
void RNDModel::RNDProcess(list<Admin* > dataSet, int num)
{

    GetRawData(dataSet);

    //多元线性回归，估计弹性系数
    LRVector lr(X, Y, num);
    double alpha = 0.001;
    int iter = 100;
    lr.train(alpha, iter);

    //弹性系数赋值
    double k = exp(lr.t.at(0));
    double a = lr.t.at(1);
    double b = lr.t.at(2);
    for(list<Admin* >::iterator iter = dataSet.begin(); iter != dataSet.end(); iter++)
    {
        Admin* admin = *iter;

        //人口密度、GDP密度赋值
        double dp = admin->popDens;
        double dg = admin->GDPDens;

        //计算道路密度理论值
        double dr = k*pow(dp,a)*pow(dg,b);
        admin->eRoadDens = dr;

        // 计算道路网依赖-偏好指数
        double dfi = admin->roadDens/dr;
        admin->DFI = dfi;
    }

}

