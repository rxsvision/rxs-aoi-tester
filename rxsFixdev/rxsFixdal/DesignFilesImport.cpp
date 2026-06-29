
#include "dwg_api.h"
#include "DesignFilesImport.h"
#include <string>
#include <time.h>
#include <iostream>


#pragma warning(disable : 4996)



CADImport::CADImport(const char* exepath)
{
    extExe = 0;
    dllHander = 0;
    memset(&dwg, 0, sizeof(Dwg_Data));
    if (exepath != 0) {
        //路径文件存在判断
        extExe = new std::string(exepath);
    }
    else {
        dllHander = LoadLibraryEx(L"libredwg-0.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);       
    }
}

CADImport::~CADImport()
{
    if (dllHander != 0) {
        FreeLibrary(dllHander);
        dllHander = 0;
    }
    if (extExe != 0) {
        delete (std::string*)extExe;
        extExe = 0;
    }
}

void CADImport::Free() {
    if (dllHander == 0) { return; }
    typedef void (*dwgFree)(Dwg_Data* __restrict dwg);
    dwgFree dwgfree = (dwgFree)GetProcAddress(dllHander, ("dwg_free"));
    if (dwgfree == 0 ) { return; }
    dwgfree(&dwg);
}

bool CADImport::isDxf(const char* path) {
    int sl = strlen(path);
    char cmpuv[3] = { 'D','X','F' };
    char cmplv[3] = { 'd','x','f' };
    int i = sl;
    int cmi = 3;
    while (i--) {
        cmi -= 1;
        if (path[i] == cmplv[cmi] || path[i] == cmpuv[cmi]) {
            if (cmi == 0 && path[i - 1] == '.') {
                return true;
            }
            continue;
        }
        cmi = 3;
        if (path[i] == '.') {
            break;
        }
    }
    return false;
}


int CADImport::ReadFile(const char* path) {
    if (dllHander == 0) { return -1; }
    bool isDwg = !isDxf(path);
    if (isDwg) {
        typedef int (*dwgReadFile)(const char* __restrict filename, Dwg_Data* __restrict dwg);
        dwgReadFile funs = (dwgReadFile)GetProcAddress(dllHander, ("dwg_read_file"));
        if (funs == 0) { return -2; }
        return funs(path, &dwg);
    }
    else {
        typedef int (*dxfReadFile)(const char* __restrict filename, Dwg_Data* __restrict dwg);
        dxfReadFile funs = (dxfReadFile)GetProcAddress(dllHander, ("dxf_read_file"));
        if (funs == 0) { return -2; }
        return funs(path, &dwg);
    }
}

void CADImport::Close() {
    Free();
    memset(&dwg, 0, sizeof(Dwg_Data));
}

bool CADImport::isExternalApp() {
    return (extExe != 0);
}

bool CADImport::openExe() {
    (std::string*)extExe;

    return true;
}

bool CADImport::Load(const char* filename, unsigned int opts) {
    if (extExe != 0) {
        return openExe();
    }
    BITCODE_BL i;
    int success;   
    dwg_point_3d pt;
    memset(&dwg, 0, sizeof(Dwg_Data));
    dwg.opts = opts;
    success = ReadFile(filename);
    // get the insertion point for our fingerprint
    pt.x = dwg.header_vars.LIMMAX.x;
    pt.y = dwg.header_vars.LIMMAX.y;
    pt.z = 0.0;

    for (i = 0; i < dwg.num_objects; i++)
    {
        std::cout << dwg.object[i].fixedtype << "\n";
        if (dwg.object[i].fixedtype == DWG_TYPE_TEXT)
        {
            Dwg_Entity_TEXT* _obj = dwg.object[i].tio.entity->tio.TEXT;
            if (pt.x == _obj->ins_pt.x && pt.y == _obj->ins_pt.y && _obj->horiz_alignment == HORIZ_ALIGNMENT_RIGHT)
            {
                //change_fingerprint(&dwg, _obj);
                //fprintf(stderr, "  at (%f, %f)\n", pt.x, pt.y);
            }
        }
        else if (dwg.object[i].fixedtype == DWG_TYPE_ARC)
        {
            Dwg_Entity_ARC* arc_obj = dwg.object[i].tio.entity->tio.ARC;
        }
        else if (dwg.object[i].fixedtype == DWG_TYPE_CIRCLE)
        {
            Dwg_Entity_CIRCLE* cir_obj = dwg.object[i].tio.entity->tio.CIRCLE;
        }
        else if (dwg.object[i].fixedtype == DWG_TYPE_LINE)
        {
            Dwg_Entity_LINE* line_obj = dwg.object[i].tio.entity->tio.LINE;
        }
        else if (dwg.object[i].fixedtype == DWG_TYPE_ELLIPSE)
        {
            Dwg_Entity_ELLIPSE *ellipse_obj = dwg.object[i].tio.entity->tio.ELLIPSE;
        }
        else if (dwg.object[i].fixedtype == DWG_TYPE_REGION)
        {
            Dwg_Entity_REGION* region_obj = dwg.object[i].tio.entity->tio.REGION;
        }
        else if (dwg.object[i].fixedtype == DWG_TYPE_POLYLINE_2D)
        {
            Dwg_Entity_POLYLINE_2D* polyline = dwg.object[i].tio.entity->tio.POLYLINE_2D;
        }
    }
    return success;
}