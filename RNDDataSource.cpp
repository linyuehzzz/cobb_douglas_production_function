#include "RNDDataSource.h"

RNDDataSource::RNDDataSource(const char* readFilePath1,const char* readFilePath2,const char* readFilePath3,const char* writeFilePath)
{
    this->readFilePath1 = readFilePath1;
    this->readFilePath2 = readFilePath2;
    this->readFilePath3 = readFilePath3;
    this->writeFilePath = writeFilePath;
    this->num = 0;

}

RNDDataSource::~RNDDataSource()
{
    //析构成员变量dataSet
    Admin* admin;
    for(list<Admin* >::iterator iter = dataSet.begin();
        iter != dataSet.end();iter++)
    {
        admin = *iter;
        if(admin != NULL)
        {
            OGRGeometryFactory::destroyGeometry(admin->poly);
            for(vector<Road* >::iterator it = admin->roads.begin();
                it != admin->roads.end();it++)
            {
                delete *it;
                *it = NULL;
            }
            delete *iter;
            *iter = NULL;
        }
    }
    dataSet.clear();
}


/// <summary>
/// 读取行政区划数据
/// </summary>
void RNDDataSource::ReadAdminFile()
{
    GDALAllRegister();
    GDALDataset* poDS;
    poDS = (GDALDataset*) GDALOpenEx( readFilePath1, GDAL_OF_VECTOR, NULL, NULL, NULL );
    if( poDS == NULL )
    {
        printf( "Admin file open failed.\n" );
        exit( 1 );
    }

    OGRLayer* poLayer;
    poLayer = poDS->GetLayer(0);
    OGRFeature* poFeature;
    poLayer->ResetReading();
    num = poLayer->GetFeatureCount();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon )
        {
            OGRPolygon* poPolygon = new OGRPolygon();
            poPolygon = (OGRPolygon* ) poGeometry;
            double area = poPolygon->get_Area();

            Admin* admin = new Admin();
            admin->poly = poPolygon;
            admin->area = area;

            dataSet.push_back(admin);
        }
        else if( poGeometry != NULL
                 && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon )
        {
            OGRMultiPolygon* poMultiPolygon = new OGRMultiPolygon();
            poMultiPolygon = (OGRMultiPolygon* ) poGeometry;
            double area = poMultiPolygon->get_Area();

            Admin* admin = new Admin();
            admin->poly = poMultiPolygon;
            admin->area = area;

            dataSet.push_back(admin);
        }
        else
        {
            printf( "no polygon geometry\n" );
        }
    }
    GDALClose( poDS );
}


/// <summary>
/// 读取道路数据
/// </summary>
void RNDDataSource::ReadRoadFile()
{
    GDALAllRegister();
    GDALDataset* poDS;
    poDS = (GDALDataset*) GDALOpenEx( readFilePath2, GDAL_OF_VECTOR, NULL, NULL, NULL );
    if( poDS == NULL )
    {
        printf( "Road file open failed.\n" );
        exit( 1 );
    }

    OGRLayer* poLayer;
    poLayer = poDS->GetLayer(0);
    OGRFeature* poFeature;
    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        int iField = poFeature->GetFieldIndex("FID_admin_") ;//替换为行政区编号字段,interger类型
        OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
        if( poFieldDefn->GetType() == OFTInteger )
        {
            int id = poFeature->GetFieldAsInteger(iField);
            list<Admin* >::iterator iter = dataSet.begin();
            for(int i = 0;i < id; i++)
                iter++;
            Admin* admin = (*iter);

            OGRGeometry* poGeometry = poFeature->GetGeometryRef();
            if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )
            {
                Road* road = new Road();
                OGRLineString* poLineString = new OGRLineString();
                poLineString = (OGRLineString* ) poGeometry;

                road->curve = poLineString;
                road->len = poLineString->get_Length();
                admin->roads.push_back(road);
            }
            else if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString )
            {
                Road* road = new Road();
                OGRMultiLineString* poMultiLineString = new OGRMultiLineString();
                poMultiLineString = (OGRMultiLineString* ) poGeometry;

                road->curve = poMultiLineString;
                road->len = poMultiLineString->get_Length();
                admin->roads.push_back(road);
            }
            else
            {
                printf( "no curve geometry.\n" );
            }

        }
        else
            printf("admin id field must be interger!/n");
    }
    GDALClose( poDS );
}


/// <summary>
/// 读取人口和GDP数据
/// </summary>
void RNDDataSource::ReadPop_GDPFile()
{
    ifstream in(readFilePath3);
    if(!in)
    {
        printf( "Pop_GDP file open failed.\n" );
        exit( 1 );
    }

    string str;
    int size;
    list<Admin* >::iterator iter = dataSet.begin();
    while(getline(in, str)) //每次把一个记录读到str中
    {
        Admin* admin = (*iter);
        double pop,gdp;
        size = str.size();
        StringToRecord(str,size,pop,gdp);
        admin->popDens = pop / admin->area *10000000000; //  人/平方公里
        admin->GDPDens = gdp / admin->area *10000000000;  // 元/平方公里
        iter++;
    }
    in.close();
}


/// <summary>
/// 将字符串转换为数字
/// </summary>
/// <param name="str">待转换字符串</param>
/// <param name="size">字符串长度</param>
/// <param name="p">人口总数</param>
/// <param name="g">总GDP</param>
void RNDDataSource::StringToRecord(string str,int size,double &p,double &g)
{
    string s;
    double a,b;
    int i = 0;
    int pos = 0;//字段第一个字母的位置
    int count = 0;//字段长度

    while(str.at(i)!= ',')
    {
        count++;
        i++;
    }
    s.assign(str, pos, count);
    a = atof(s.c_str());
    p = a;
    pos += count + 1;

    count = size - pos;
    s.assign(str, pos, count);
    b = atof(s.c_str());
    g = b;
}


/// <summary>
/// 写入.shp文件
/// </summary>
void RNDDataSource::WriteFile()
{
//    runStatus->setStatus(0, QString("模型计算完成，开始数据输出……"));
    GDALAllRegister();
    //执行文件采用utf8编码，故设置为yes，支持中文路径，但不支持名称含有中文的shp
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if(poDriver == nullptr)
    {
        printf( "%s driver not available.\n", "ESRI Shapefile" );
        exit( 1 );
//        qCritical()<<(QString("GDAL驱动创建失败！"));
//        runStatus->setStatus(1, QString("GDAL驱动创建失败！"));
    }
    GDALDataset *ds1 = poDriver->Create(writeFilePath, 0, 0, 0, GDT_Unknown, nullptr);

    GDALDataset* poDS;
    poDS = (GDALDataset*) GDALOpenEx(readFilePath1, GDAL_OF_VECTOR, NULL, NULL, NULL );
    if( poDS == NULL )
    {
        printf( "Admin file open failed.\n" );
        exit( 1 );
    }
    OGRLayer* poLayer = poDS->GetLayer(0);

    if(ds1 == nullptr || poLayer == nullptr)
    {
        printf( "Creation of output file failed.\n" );
        exit( 1 );
//        qCritical()<<(QString("创建图层失败！"));
//        runStatus->setStatus(1, QString("创建图层失败！"));
    }



    const char *filePath = writeFilePath;
    char *outName=new char;
    _splitpath(filePath, NULL, NULL, outName, NULL);

    //复制图层
    char **papszOptions = NULL;
    //设置输出的dbf文件languageDriverID为0
    papszOptions = CSLAddNameValue(papszOptions,"ENCODING", "UTF-8");
    ds1->CopyLayer(poLayer,outName,papszOptions);
    CSLDestroy( papszOptions );

    OGRLayer *outputLayer = ds1->GetLayerByName(outName);
    //添加字段
    OGRFieldDefn *fieldDr = new OGRFieldDefn("Dr",OFTReal);//实际道路密度
    OGRFieldDefn *fieldeDr = new OGRFieldDefn("eDr",OFTReal);//估计道路密度
    OGRFieldDefn *fieldDFI = new OGRFieldDefn("DFI",OFTReal);//DFIndex

    outputLayer->CreateField(fieldDr,1);
    outputLayer->CreateField(fieldeDr,1);
    outputLayer->CreateField(fieldDFI,1);

    outputLayer->ResetReading();
//    int i = 0 , count = outputLayer->GetFeatureCount(1);
    OGRFeature *feature;
    while((feature = outputLayer->GetNextFeature())!= NULL)
    {
        int id = feature->GetFID();
        list<Admin* >::iterator iter = dataSet.begin();
        for(int i = 0;i < id; i++)
            iter++;
        Admin* admin = *iter;
        feature->SetField("Dr", admin->roadDens);
        cout<<admin->roadDens<<' ';
        feature->SetField("eDr", admin->eRoadDens);
        feature->SetField("DFI", admin->DFI);
        outputLayer->SetFeature(feature);
//        runStatus->setProgress(int(i * 5 / count) + 95);
    }

    OGRFeature::DestroyFeature( feature );
//    runStatus->setStatus(0, QString("模型计算成功"));
    GDALClose( poDS );
}
