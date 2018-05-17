/**************************************
Author:林玥
Date:2017/11/25
Update:2017/12/1
Description:数据源类，读取和输出文件
***************************************/
#ifndef RNDDATASOURCE_H
#define RNDDATASOURCE_H

#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <string>
#include <QFileInfo>
#include "ogrsf_frmts.h"
using namespace std;

/* 存储道路信息 */
class Road
{
public:
    OGRGeometry* curve;
    double len;
};

/* 存储行政区相关信息 */
class Admin
{
public:
   OGRGeometry* poly;  //区域面
   double area;       //面积
   vector<Road* > roads; //区域内道路
   double roadDens;   //道路密度
   double popDens;    //人口密度
   double GDPDens;    //GDP密度
   double eRoadDens;  //估计道路密度
   double DFI;        //道路网依赖-偏好指数
};

class RNDDataSource
{
public:

    RNDDataSource(const char* readFilePath1,const char* readFilePath2,
                  const char* readFilePath3,const char* writeFilePath);
    ~RNDDataSource();

    list<Admin* > dataSet;  //存放行政区内所有数据
    int num;   //行政区域数目

    /* 读取行政区划数据 */
    void ReadAdminFile();

    /* 读取道路数据 */
    void ReadRoadFile();

    /* 读取行政区内人口和GDP数据 */
    void ReadPop_GDPFile();

    /* 写入道路密度统计值、模型估计值和道路网依赖-偏好指数 */
    void WriteFile();

private:
    const char* readFilePath1;//输入行政区划数据路径
    const char* readFilePath2;//输入道路数据路径
    const char* readFilePath3;//输入人口、GDP数据路径
    const char* writeFilePath;//输出路径

    /* 字符串转为单条记录
    str:待转换字符串
    size:字符串长度
    p:人口总数
    g:总GDP */
    void StringToRecord(string str,int size,double &p,double &g);
};

#endif // RNDDATASOURCE_H
