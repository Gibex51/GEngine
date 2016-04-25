#pragma once

#include "GDevice.h"
#include "GInput.h"
//#include "G_BulletPhysics.h"

//Game Mode
#define G_GAMEMODE_START          0
#define G_GAMEMODE_MAIN_MENU      1
#define G_GAMEMODE_GAME_LOAD      2
#define G_GAMEMODE_GAME_PLAY      3
#define G_GAMEMODE_EDITOR_CUBE   10
#define G_GAMEMODE_EDITOR_NPC    11
#define G_GAMEMODE_EDITOR_OBJECT 12

//Max Player Speed
#define G_GM_MAX_SPEED     1000

//Mouse ScrollScreen Frame Width
#define G_GM_SCROLL_FRAME_WIDTH   3

//Max Unit Name Length
#define G_MAXUNITNAMELEN   250
#define G_MAXMODELNAMELEN  250

//Map metrics
#define CUBE_SECTOR_SIZE 160    //Ширина/длинна/высота сектора
#define NCUBESXZ 40             //Кол-во кубов по оси X и Z
#define NCUBESY 40              //Кол-во кубов по оси Y

const g_ivec3 cubes_in_sec = g_ivec3(NCUBESXZ,NCUBESY,NCUBESXZ);

const g_vec3 csize = g_vec3(g_float(CUBE_SECTOR_SIZE)/g_float(NCUBESXZ), //Размер одного куба
	                        g_float(CUBE_SECTOR_SIZE)/g_float(NCUBESY),
					        g_float(CUBE_SECTOR_SIZE)/g_float(NCUBESXZ));

const g_int32 cubescount = NCUBESXZ*NCUBESXZ*NCUBESY;

//Map setings
#define MAPDEEP 6 // Кол-во секторов по Y
#define SECTORSXZ_IN_REGION 9 // Сектор для сохранения SECTORSXZ_IN_REGION x SECTORSXZ_IN_REGION x MAPDEEP
#define SEALEVEL NCUBESY*2 // Уровень моря

//Центральный сектор 
const g_ivec2 center_sector_xy = g_ivec2(-10,-10);

//Центральная точка мира
const g_vec3 world_center = g_vec3(center_sector_xy.x*(CUBE_SECTOR_SIZE/2.0f),
							       -10*(CUBE_SECTOR_SIZE/2.0f),
							       center_sector_xy.y*(CUBE_SECTOR_SIZE/2.0f));

//Расстояние до солнца
const g_float sun_dist = 16*CUBE_SECTOR_SIZE;

//Высота по умолчанию для камеры
const g_float default_height = -200;

/* Неогр. карта */
#ifdef _DEBUG
  #define MAPRADIUS 1  // Радиус прорисовки секторов (Debug)
#else
  #define MAPRADIUS 7  // Радиус прорисовки секторов 
#endif

const g_uint16 mapsq = MAPRADIUS*MAPRADIUS*4 - MAPRADIUS*4 + 1;
const g_uint16 maxsectors = 5400;
//const g_uint32 allcubes = mapsq*MAPDEEP*NCUBESXZ*NCUBESXZ*NCUBESY;

/* Огр. карта */
#define MAP_BORDER_SECTORS  2   // Дополнительные сектора по краям
#define MAX_MAP_SIZE        30  // Максимальное количество секторов на сторону

const g_uint16 max_fullmap_size = MAX_MAP_SIZE*NCUBESXZ;

//Sector Preview Status
#define G_PGS_STATUS_NONGEN    0
#define G_PGS_STATUS_PARTGEN   1
#define G_PGS_STATUS_FULLGEN   100

//Draw Map Settings
#define DRAWENDFACES false
#define MAXFACES 4000000

//NULL cube
#define G_INVALID_CUBE         0xFFFF

//#define SAVE_PATH_TEMPLATE "..\\Saves\\%s.sec"
#define SAVE_OBJ_PATH_TEMPLATE "..\\Data\\Objects\\%s.%s"
#define SAVE_MAP_PATH_TEMPLATE "..\\Data\\Maps\\%s\\%d_%d.%s"
#define SAVE_MAP_PATH "..\\Data\\Maps\\"
#define SAVE_OBJ_PATH "..\\Data\\Objects\\"
#define SAVE_TMP_PATH "..\\Data\\Temp\\"

#define SAVE_TMP_TEMPLATE "_$GENG$\\%s\\%s_%d.%s"

//ID массива треугольников террэйна для физики
#define PHYS_IVA_UID 10001

//Константы поворота
#define G_CUBE_ROT_0     0
#define G_CUBE_ROT_90    1
#define G_CUBE_ROT_180   2
#define G_CUBE_ROT_270   3

//Флаги создания карты высот
#define G_HMF_GEN_CLEAR  0
#define G_HMF_GEN_TYPE1  1
#define G_HMF_GEN_TYPE2  2

//Типы объектов из кубиков
#define G_COT_GRASS      0
#define G_COT_WATERGRASS 1
#define G_COT_STONE      2
#define G_COT_TREE       3

//Типы объектов на карте
#define G_MOT_EMPTY         0
#define G_MOT_UNIT_UNKNOWN  1
#define G_MOT_UNIT_HERO     2
#define G_MOT_UNIT_SOLDIER  3
#define G_MOT_UNIT_NPC      4
//#define G_MOT_DECOR_      100 //Декорации
//#define G_MOT_TRAP_       200 //Ловушки
//#define G_MOT_            300 //Еще что нибудь

//Target Cube Calculation
const g_vec3  tcSegmSize = csize * 2;
const g_int32 tcSegmMaxNum = 600;

//PathFinder Constants
const g_uint16 pfMaxPathPoints = 200;

//Game Keys Actions
#define GM_ACT_MAPMOVEUP       1
#define GM_ACT_MAPMOVEDOWN     2
#define GM_ACT_MAPMOVELEFT     3
#define GM_ACT_MAPMOVERIGHT    4
#define GM_ACT_MAPROTENABLE    5

#define GM_ACT_LMTOOL_DOWN     101
#define GM_ACT_RMTOOL_DOWN     102
#define GM_ACT_LMTOOL_UP       103
#define GM_ACT_RMTOOL_UP       104
#define GM_ACT_LMTOOL_PRS      105
#define GM_ACT_RMTOOL_PRS      106

#define GM_ACT_SETNPC          201
#define GM_ACT_MOVENPC         202

#define GM_ACT_UNDO            301
#define GM_ACT_COPY            302
#define GM_ACT_PASTE           303

#define GM_ACT_WIREFRAME       401

//CubeTerrain Editor Fill Area Type
#define G_CT_AREATYPE_CUBE       0
#define G_CT_AREATYPE_SPHERE     1
#define G_CT_AREATYPE_CYLINDER_X 2
#define G_CT_AREATYPE_CYLINDER_Y 3
#define G_CT_AREATYPE_CYLINDER_Z 4

//Максимальная длинна выделения по осям
#define G_MAX_SEL_LENGTH         250
#define G_MAX_SEL_LENGTH_COPY    120

//Возможное кол-во отмен
#define G_MAX_UNDO               15

//Разное
const g_ivec3 max_ivec3 = g_ivec3(G_MAX_INT,G_MAX_INT,G_MAX_INT);
const g_ivec3 min_ivec3 = g_ivec3(G_MIN_INT,G_MIN_INT,G_MIN_INT);

//Карты теней
#define G_SHDMAP_RES   2048
#define G_SHDMAP_CASC  3

//Хэш типа объекта
#define G_OT_INVALID_HASH 65535

//Хэш полного фэйса
const g_uint16 hash_fulltype[4] = {20100, 20200, 40100, 40200};

/* Resources Custom UIDS */
#define UID_MESH_SEL         RES_UID_USERDEFINE
#define UID_MESH_ALLMAP      RES_UID_USERDEFINE + 1
#define UID_MESH_OBJPREV     RES_UID_USERDEFINE + 2
#define UID_MESH_PATH        RES_UID_USERDEFINE + 3
#define UID_MESH_SKYSPHERE   RES_UID_USERDEFINE + 4

#define UID_MAT_SEL          RES_UID_USERDEFINE + 10000
#define UID_MAT_OBJPREV      RES_UID_USERDEFINE + 10001
#define UID_MAT_DEFNPC       RES_UID_USERDEFINE + 10002
#define UID_MAT_CUBETERR     RES_UID_USERDEFINE + 10003
#define UID_MAT_SKY_SPHERE   RES_UID_USERDEFINE + 10004

#define UID_TEX_RAD          RES_UID_USERDEFINE + 20000
#define UID_TEX_MM           RES_UID_USERDEFINE + 20001
#define UID_TEX_OBJPREV      RES_UID_USERDEFINE + 20002
#define UID_TEX_LIGHTCAM     RES_UID_USERDEFINE + 20003
#define UID_TEX_SSAO_DS      RES_UID_USERDEFINE + 20004
#define UID_TEX_SSAO_NORM    RES_UID_USERDEFINE + 20005
#define UID_TEX_TOOLS        RES_UID_USERDEFINE + 20006
#define UID_TEX_DEFNPC1      RES_UID_USERDEFINE + 20007
#define UID_TEX_DEFNPC2      RES_UID_USERDEFINE + 20008
#define UID_TEX_MENU         RES_UID_USERDEFINE + 20009
#define UID_TEX_SKY          RES_UID_USERDEFINE + 20010

#define UID_TEX_CT_FIRST     RES_UID_USERDEFINE + 21000 // Cube Terrain Textures 21000 - 21499
#define UID_TEX_CT_LAST      RES_UID_USERDEFINE + 21499

#define UID_CAM_PLAYER       RES_UID_USERDEFINE + 30000
#define UID_CAM_OBJPREV      RES_UID_USERDEFINE + 30001
#define UID_CAM_LIGHTCAM     RES_UID_USERDEFINE + 30002
#define UID_CAM_MENU         RES_UID_USERDEFINE + 30003

#define UID_EP_DEFAULT       RES_UID_USERDEFINE + 40000
#define UID_EP_CUBETERR      RES_UID_USERDEFINE + 40001

#define UID_FONT_FPS         RES_UID_USERDEFINE + 50000

#define UID_FX_DEFAULT       RES_UID_USERDEFINE + 60000
#define UID_FX_OBJPREV       RES_UID_USERDEFINE + 60001
#define UID_FX_SKY           RES_UID_USERDEFINE + 60002
#define UID_FX_CUBETERR      RES_UID_USERDEFINE + 60003


#define UID_MESH_BASENPC     RES_UID_USERDEFINE + 1000000
#define UID_FX_BASENPC       RES_UID_USERDEFINE + 1010000

#define UID_USERDEFINE_MAX   RES_UID_USERDEFINE + 10000000

/* Input Layouts UID */
#define UID_IL_NPC1          IL_UID_USERDEFINE + 200
#define UID_IL_CUBETERR      IL_UID_USERDEFINE + 300

#define UID_IL_SEL_CUBE      IL_UID_USERDEFINE + 1000
#define UID_IL_OBJ_PREV      IL_UID_USERDEFINE + 1001
#define UID_IL_SKY_SPHERE    IL_UID_USERDEFINE + 1002

/* Animations IDs */
//Animation Variations: AID_NPC_... + N ; N = 0..9
#define AID_NPC_STAY_DEF         00  
#define AID_NPC_MOVE_DEF         10
#define AID_NPC_ATTACK_DEF       20
#define AID_NPC_DIE_DEF          30
#define AID_NPC_TAKEDAMAGE_DEF   40
#define AID_NPC_DODGE_DEF        50
#define AID_NPC_JUMP_DEF         60

#define AID_CUSTOM_DEF           10000

/* Interface UIDS */
//Editor Desktop
#define  UID_DESKTOP_EDITOR     501
#define  UID_DESKTOP_MAINMENU   502
#define  UID_DESKTOP_LOADSCREEN 503
#define  UID_DESKTOP_GAME       504

//Main Menu Controls
#define  UID_IMAGE_MM_BACKGR    2301
#define  UID_BUTTON_MM_NEWGAME  2305
#define  UID_BUTTON_MM_EDITOR   2302
#define  UID_BUTTON_MM_SETTINGS 2306
#define  UID_BUTTON_MM_EXIT     2303

//Save/Load Window
#define  UID_WINDOW_SAVELOAD   1001
#define  UID_LISTBOX_LISTOBJ   1002
#define  UID_BUTTON_SAVEMAP    1003
#define  UID_BUTTON_LOADMAP    1004
#define  UID_BUTTON_CANCELSL   1005
#define  UID_LABEL_SLINFO      1008
#define  UID_EDIT_OBJNAME      1009

//Objects Window
#define  UID_WINDOW_OBJECTS    1201
#define  UID_LISTBOX_TEXTURE   1202
#define  UID_IMAGE_OBJECT      1203
#define  UID_SCROLLLIST_GROUPS 1204

//NPC Window
#define  UID_WINDOW_NPC        1301
#define  UID_LISTBOX_NPC       1302

//Tools Window
#define  UID_WINDOW_TOOLS      1501
#define  UID_BUTTON_TOOLGRID   1502

//Editor Mode Window
#define  UID_WINDOW_EDITORMODE 2401
#define  UID_RADIOBTN_EDITMAP  2402
#define  UID_RADIOBTN_EDITOBJ  2403

//Settings Window
#define  UID_WINDOW_SETTINGS    1601
#define  UID_TRACKBAR_SPEED     1602
#define  UID_CHECKBOX_HEIGHT    1603
#define  UID_CHECKBOX_CUBEALIGN 1604
#define  UID_CHECKBOX_FIX_Y     1605
#define  UID_CHECKBOX_SHDMAP    1606
#define  UID_TRACKBAR_FOV       1607
#define  UID_TRACKBAR_DIST      1608
#define  UID_CHECKBOX_FOG       1612
#define  UID_SCROLLLIST_RESOLUT 1613
#define  UID_BUTTON_APPLYSET    1614
//Other Settings Window
#define  UID_WINDOW_OTHERSET     2201
#define  UID_BUTTON_LOADEDMAP_WW 2202
#define  UID_BUTTON_UPDATEMAP    2203
#define  UID_BUTTON_LOADEDMAP    2204
#define  UID_BUTTON_CLEARFPS     2205
#define  UID_BUTTON_CONSOLE      2206

//Cube Objects Window
#define  UID_WINDOW_CUBEOBJ     1701
#define  UID_LISTBOX_CUBEOBJ    1702
#define  UID_BUTTON_DELOBJ      1703

//Light Cam Window
#define  UID_WINDOW_LIGHTCAM    1801
#define  UID_IMAGE_LIGHTCAM     1802
#define  UID_TRACKBAR_LCAMX     1803
#define  UID_TRACKBAR_LCAMZ     1804

//Log Window
#define  UID_WINDOW_LOG         1901
#define  UID_LISTBOX_LOG        1902
#define  UID_EDIT_CONSOLE       1903

//Sidebar Panel
#define  UID_PANEL_SIDEBAR      2101
#define  UID_BUTTON_MENUGRID    2102
#define  UID_BUTTON_HIDESIDEBAR 2103

//Other
#define  UID_BUTTON_EXIT         1101
#define  UID_BUTTON_EXITTOMENU   1102
#define  UID_BUTTON_SAVELOAD     1105
#define  UID_BUTTON_TEST         1109
//#define  UID_IMAGE_VIDEOTEST     1116