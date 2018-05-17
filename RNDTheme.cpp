#include "RNDTheme.h"
RNDTheme::RNDTheme()
{
    d = new RNDDataSource("D:/Lydia/c++/RND/data/admin_Project.shp","D:/Lydia/c++/RND/data/road_Identity.shp",
                          "D:/Lydia/c++/RND/data/population_gdp.csv","D:/Lydia/c++/RND/data/resultt.shp");
    m = new RNDModel();
}

RNDTheme::~RNDTheme()
{
    if(d != NULL){
        delete d;
        d = NULL;
    }

    if(m != NULL){
        delete m;
        m = NULL;
    }
}


/// <summary>
/// 运行模型
/// </summary>
void RNDTheme::RunModel()
{
    d->ReadAdminFile();
    d->ReadRoadFile();
    d->ReadPop_GDPFile();
    cout << "Finish reading." << endl;

    m->GetRoadDensity(d->dataSet);
    m->RNDProcess(d->dataSet,d->num);

    d->WriteFile();
    cout << "Finish writing." << endl;

}
