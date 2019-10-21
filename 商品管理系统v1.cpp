#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include <conio.h>

/*内容物*************************************************************************************/

//记录的时间
struct shiJian_ {
	int yue = -1;//月
	int ri = -1;//日
	int shi = -1;//时
	int fen = -1;//分
};

//进货记录
struct jinHuoJiLu_ {
	int bianHao;//进货记录编号
	struct shiJian_ jinHuoShiJian;//进货时间
	char peiJianMingCheng[20];//配件名称
	char xingHao[20];//型号
	char zhiZaoShang[20];//制造商
	float danJia;//单价
	int shuLiang;//数量
	float zongJia;//总价
	char gongHuoShang[20];//供货商
	struct jinHuoJiLu_* biaoJi=NULL;//进货结构体标记
};

//批发记录
struct piFaJiLu_ {
	int bianHao;//批发记录编号
	struct shiJian_ piFaShiJian;//批发时间
	char peiJianMingCheng[20];//配件名称
	char xingHao[20];//型号
	char zhiZaoShang[20];//制造商
	float danJia;//批发单价
	int shuLiang;//数量
	float zongJia;//总价
	char keHuXinXi[20];//客户信息
	char zengPinXinXi[20] = "empty";//赠品信息
	struct piFaJiLu_* biaoJi=NULL;//批发结构体标记
};

//零售记录
struct lingShouJiLu_ {
	int bianHao;//零售记录编号
	struct shiJian_ lingShouShiJian;//零售时间
	char peiJianMingCheng[20];//配件名称
	char xingHao[20];//型号
	char zhiZaoShang[20];//制造商
	float danJia;//零售单价
	int shuLiang;//数量
	float zongJia;//总价
	char keHuXinXi[20];//客户信息
	struct lingShouJiLu_* biaoJi=NULL;//零售记录标记
};

//统计结构体
struct tongJi_ {
	char peiJianMingCheng[20];//配件的名称
	float jinHuoZongJia=0;//进货总价
	int jinHuoShuLiang=0;//进货数量
	float jinHuoDanJia=0;//进货单价
	float piFaZongJia=0;//批发总价
	int piFaShuLiang=0;//批发数量
	float lingShouZongJia=0;//零售总价
	int lingShouShuLiang=0;//零售数量
	float jingLiRun=0;//净利润
	bool biaoJi = false;//用于判断该项是否已使用
};

/*一些定义**********************************************************************************/

float ziJin = 5000000.0;//资金，初始资金为五百万
struct jinHuoJiLu_* jinHuoTou, * jinHuoWei;//用于标记进货记录
struct piFaJiLu_* piFaTou=NULL, * piFaWei;//标记批发记录
struct lingShouJiLu_* lingShouTou,* lingShouWei;//标记零售记录
int yue, ri, shi, fen;//转化成数字的时间
#define zengPinQiBuShu  10//当批发量达到此大小时可以获得赠品，赠品起步数
#define pipeidu 0.5//匹配度达到此值时可以输出
int bianHao = 1;//全局编号
int bianHao_panDuan[100];
FILE* daoRu; FILE* daoChu;
//const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//定义显示器句柄变量
COORD pos;
char guanJianCi_[20];//由于实在没办法把查询的关键词保留到几个函数里，所以宏定义一个
int dingWeiDian = 17;//打印数据的定位点
int jiLuXiuGaiBianHao[100];


/*声明函数*********************************************************************************/

void dingWei(int x, int y);//定位光标
struct jinHuoJiLu_* jiLuJinHuoJiLu();//记录进货数据
struct lingShouJiLu_* jiLuLingShouJiLu();//记录零售数据
struct piFaJiLu_* jiLuPiFaJiLu();//记录批发数据
bool lianJie_jinHuo(struct jinHuoJiLu_* p);//整合进货记录
bool lianJie_lingShou(struct lingShouJiLu_* p);//整合零售记录
bool lianJie_piFa(struct piFaJiLu_* p);//整合批发记录
bool jieShiRiQi(char* a);//处理读入的时间，解释日期
bool jieShiShiJian(char* a);//处理读入的时间，解释小时和分
bool tongJiShuJu(bool biaoJi);//统计利润
char* piPeiZengPin(int a);//匹配赠品
bool zengPinGuanLi(struct piFaJiLu_* p);//赠品管理
int daYin(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//打印数据本体
void daYin_huaKuang();//打印的时候的画框
void quanBuDaYin();//全部打印
bool genJuShiJianDuanChaXun();//根据时间段打印
int xiuGaiLianBiao(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//修改链表数据的目录
int xiuGaiLianBiao_riQi(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//修改链表内时间
int xiuGaiLianBiao_peiJian(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//修改链表内配件
int xiuGaiLianBiao_danJia(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//修改链表内的单价
int xiuGaiLianBiao_shuLiang(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//修改链表内的数量
int xiuGaiLianBiao_gongHuoShang(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c);//修改链表里的供货商
int xiuGaiChaXun(int xiuGaiBiaoJi, int type);//修改面板
int wenJianDaoRu(int aa,char* filename);//从文件导入一条信息
struct jinHuoJiLu_* daoRu_jinHuo(int bianHao, char peiJianMingCheng[20], char xingHao[20], char zhiZaoShang[20], char riQi[20], char shiJian[20], float danJia, int shuLiang, float zongJia, char xinXi[20]);
struct piFaJiLu_* daoRu_piFa(int bianHao, char peiJianMingCheng[20], char xingHao[20], char zhiZaoShang[20], char riQi[20], char shiJian[20], float danJia, int shuLiang, float zongJia, char xinXi[20], char zengPinXinXi[20]);
struct lingShouJiLu_* daoRu_lingShou(int bianHao, char peiJianMingCheng[20], char xingHao[20], char zhiZaoShang[20], char riQi[20], char shiJian[20], float danJia, int shuLiang, float zongJia, char xinXi[20]);
bool shanChuShuJu(int type, int bianHao);//删除记录
bool baoCunJinHuoJiLu_(char* fileName);//保存进货记录到文件
bool baoCunPiFaJiLu_(char* fileName);//保存批发记录到文件
bool baoCunLingShouJiLu_(char* fileName);//保存零售记录到文件
int chaXunXinXi(char* guanJianCi, int leiXin, bool xiugai);//查询总函数
int* chaXun_baoLi_jinHuo(char* guanJianCi, int leiXin, bool xiugai);//在进货记录中查询
int* chaXun_baoLi_lingShou(char* guanJianCi, int leiXin, bool xiugai);//在零售记录中查询
int* chaXun_baoLi_piFa(char* guanJianCi, int leiXin, bool xiugai);//在批发记录中查询
int chaXu_moHu_jinHuo(char* guanJianCi);//模糊查询进货数据
int chaXu_moHu_lingShou(char* guanJianCi);//模糊查询零售数据
int chaXu_moHu_piFa(char* guanJianCi);//模糊查询批发数据
bool jiaMiWenJian(char* muBiaoWenJian, char* filename, char* miMa);//加密文件
bool jieMiWenJian(char* muBiaoWenJian, char* filename, char* miMa);//解密文件
char* zhuCaiDan();//主菜单
char* fenCaiDan_jiLuShuJu();//分菜单，记录数据
char* fenCaiDan_xiuGaiShuJu();//分菜单修改数据
char* fenCaiDan_daYinShuJu();//分菜单：打印数据
char* fenCaiDan_chaXunJiLu();//分菜单：查询数据
char* fenCaiDan_chaXunJiLu_jinHuo();//分菜单：查询->进货
char* fenCaiDan_chaXunJiLu_lingShou();//分菜单：查询->零售
char* fenCaiDan_chaXunJiLu_piFa();//分菜单：查询->批发
char* chaXun_jieMian();//查询界面，输入关键词
char* souSuo_jieMian();//搜索界面
int xiuGaiChaXun(int xiuGaiBiaoJi, int type);//修改的时候查询链表
char* fenCaiDan_fuJiaGongNeng();//分菜单：附加功能
char* fenCaiDan_fuJiaGongNeng_moHuChaXun();//分菜单：附加功能->模糊查询
char* fenCaiDan_fuJiaGongNeng_jiaMiWenJian();//分菜单：附加功能->加密文件
char* fenCaiDan_fuJiaGongNeng_jieMiWenJian();//分菜单：附加功能->解密文件



/*配件列举**********************************************************************************/
enum peiJian_ {
	传感器,处理器,磁盘驱动器,储存控制器,电池,显示适配器,音频设备,网络适配器,照相机,监视器
};
const char mingZi[10][20] = { {"传感器"},{"处理器"},{"磁盘驱动器"},{"储存控制器"},{"电池"},{"显示适配器"},{"音频设备"},{"网络适配器"},{"照相机"},{"监视器"} };
//存有配件名字的数组

/*函数们************************************************************************************/

//定位光标
void dingWei(int x, int y)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = x;//横坐标
	pos.Y = y;//纵坐标
	SetConsoleCursorPosition(hOut, pos);
}//定位光标

//在系统界面记录进货数据 
struct jinHuoJiLu_* jiLuJinHuoJiLu(){
	struct jinHuoJiLu_ *p;
	char riQi[10], shiJian[10];//临时存储日期数据
	p = (struct jinHuoJiLu_*)malloc(sizeof(struct jinHuoJiLu_));
	p->bianHao = bianHao; bianHao += 1;
	printf("\n                     "); printf("╭ ───────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│         请输入日期：                                          │"); Sleep(20);
	printf("\n                     "); printf("│         请输入时刻：                                          │"); Sleep(20);
	printf("\n                     "); printf("│         请输入进货的配件名称：                                │"); Sleep(20);
	printf("\n                     "); printf("│         请输入配件的型号：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         请输入配件制造商：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         请输入商品的单价：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         输入商品的数量：                                      │"); Sleep(20);
	printf("\n                     "); printf("│         请输入供货商：                                        │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("╰ ───────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(43, 4); gets_s(riQi);
	while (!(jieShiRiQi(riQi)))
	{ 
		dingWei(31, 4);
		printf("请再试一遍：");
		gets_s(riQi);
	}
	dingWei(31, 4); printf("日期已录入：");
	p->jinHuoShiJian.yue = yue; p->jinHuoShiJian.ri = ri;
	dingWei(43, 5); gets_s(shiJian);
	while(!(jieShiShiJian(shiJian))) { dingWei(31, 5); printf("请再试一遍："); gets_s(shiJian); }
	dingWei(31, 5); printf("时间已录入：");
	p->jinHuoShiJian.shi = shi; p->jinHuoShiJian.fen = fen;
	dingWei(53, 6); gets_s(p->peiJianMingCheng);
	dingWei(49, 7); gets_s(p->xingHao);
	dingWei(49, 8); gets_s(p->zhiZaoShang);
	dingWei(49, 9); scanf_s("%f", &p->danJia);
	dingWei(47, 10); scanf_s("%d", &p->shuLiang);getchar();
	p->zongJia = p->danJia * (float)p->shuLiang;
	dingWei(45, 11); gets_s(p->gongHuoShang);
	return p;
}
//整合进货数据
bool lianJie_jinHuo(struct jinHuoJiLu_ *p){
	if (jinHuoWei == NULL) {
		jinHuoTou = p;
		jinHuoWei = p;
		p->biaoJi = NULL;
	}
	jinHuoWei->biaoJi = p;
	jinHuoWei = p;
	p->biaoJi = NULL;
	return true;
}


//在系统界面记录零售数据
struct lingShouJiLu_* jiLuLingShouJiLu() {
	struct lingShouJiLu_* p;
	char riQi[10], shiJian[10];
	p = (struct lingShouJiLu_*)malloc(sizeof(struct lingShouJiLu_));
	p->bianHao = bianHao; bianHao += 1;
	printf("\n                     "); printf("╭ ───────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│         请输入日期：                                          │"); Sleep(20);
	printf("\n                     "); printf("│         请输入时刻：                                          │"); Sleep(20);
	printf("\n                     "); printf("│         请输入零售的配件名称：                                │"); Sleep(20);
	printf("\n                     "); printf("│         请输入配件的型号：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         请输入配件制造商：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         请输入商品的单价：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         输入商品的数量：                                      │"); Sleep(20);
	printf("\n                     "); printf("│         请输入客户名字：                                      │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("╰ ───────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(43, 4); gets_s(riQi);
	while (!(jieShiRiQi(riQi))) { dingWei(31, 4); printf("请再试一遍："); gets_s(riQi); }
	p->lingShouShiJian.yue = yue; p->lingShouShiJian.ri = ri;
	dingWei(43, 5); gets_s(shiJian);
	while (!(jieShiShiJian(shiJian))) { dingWei(31, 5); printf("请再试一遍："); gets_s(shiJian); }
	p->lingShouShiJian.shi = shi; p->lingShouShiJian.fen = fen;
	dingWei(53, 6); gets_s(p->peiJianMingCheng);
	dingWei(49, 7); gets_s(p->xingHao);
	dingWei(49, 8); gets_s(p->zhiZaoShang);
	dingWei(49, 9); scanf_s("%f", &p->danJia);
	dingWei(47, 10); scanf_s("%d", &p->shuLiang); getchar();
	p->zongJia = p->danJia * (float)p->shuLiang;
	dingWei(47, 11); gets_s(p->keHuXinXi);
	return p;
}
//整合零售数据
bool lianJie_lingShou(struct lingShouJiLu_* p) {
	if (lingShouWei == NULL) {
		lingShouTou = p;
		lingShouWei = p;
		p->biaoJi = NULL;
	}
	lingShouWei->biaoJi = p;
	lingShouWei = p;
	p->biaoJi = NULL;
	return true;
}


//在系统界面记录批发数据 
struct piFaJiLu_* jiLuPiFaJiLu() {
	struct piFaJiLu_* p;
	char riQi[10], shiJian[10];
	p = (struct piFaJiLu_*)malloc(sizeof(struct piFaJiLu_));
	p->bianHao = bianHao; bianHao += 1;
	printf("\n                     "); printf("╭ ───────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│         请输入日期：                                          │"); Sleep(20);
	printf("\n                     "); printf("│         请输入时刻：                                          │"); Sleep(20);
	printf("\n                     "); printf("│         请输入零售的配件名称：                                │"); Sleep(20);
	printf("\n                     "); printf("│         请输入配件的型号：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         请输入配件制造商：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         请输入商品的单价：                                    │"); Sleep(20);
	printf("\n                     "); printf("│         输入商品的数量：                                      │"); Sleep(20);
	printf("\n                     "); printf("│         请输入客户名字：                                      │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("│                                                               │"); Sleep(20);
	printf("\n                     "); printf("╰ ───────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(43, 4); gets_s(riQi);
	while (!(jieShiRiQi(riQi))) { dingWei(31, 4); printf("请再试一遍："); gets_s(riQi); }
	p->piFaShiJian.yue = yue; p->piFaShiJian.ri = ri;
	dingWei(43, 5); gets_s(shiJian);
	while (!(jieShiShiJian(shiJian))) { dingWei(31, 5); printf("请再试一遍："); gets_s(shiJian); }
	p->piFaShiJian.shi = shi; p->piFaShiJian.fen = fen;
	dingWei(53, 6); gets_s(p->peiJianMingCheng);
	dingWei(49, 7); gets_s(p->xingHao);
	dingWei(49, 8); gets_s(p->zhiZaoShang);
	dingWei(49, 9); scanf_s("%f", &p->danJia);
	dingWei(47, 10); scanf_s("%d", &p->shuLiang); getchar();
	p->zongJia = p->danJia * (float)p->shuLiang;
	dingWei(47, 11); gets_s(p->keHuXinXi);
	strcpy_s(p->zengPinXinXi, "$empty$");
	p->zengPinXinXi[7] = '\0';
	zengPinGuanLi(p);
	return p;
}
//整合批发数据
bool lianJie_piFa(struct piFaJiLu_*p) {
	if (piFaWei == NULL) {
		piFaTou = p;
		piFaWei = p;
		p->biaoJi = NULL;
	}
	piFaWei->biaoJi = p;
	piFaWei = p;
	p->biaoJi = NULL;
	return true;
}


//输入一个含有日期的字符串，长度为6，包括'\0'，输出存在yue、ri里
//a为读入的月、日字符串
bool jieShiRiQi(char* a) {
	if (a[0] <= '9' && a[0] >= '0')
	{
		if (a[1] <= '9' && a[1] >= '0')
		{
			if (a[2] == '.' || a[2] == '。' || a[2] == ',')
			{
				yue = (a[0] - '0') * 10 + (a[1] - '0');
				if (a[3] <= '9' && a[3] >= '0')
				{
					if (a[4] <= '9' && a[4] >= '0') {
						ri = (a[3] - '0') * 10 + (a[4] - '0');
						if (a[5] != '\0') {
							dingWei(0, 13);
							printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
							printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的结尾\033[0m                    │");
							printf("\n                        "); printf("│                 请在输入日期后按下回车                  │");
							printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
							Sleep(1000);
							return false;
						}
					}
					else
					{
						if (a[4] == '\0')
						{
							ri = a[3] - '0';
						}
						else
						{
							dingWei(0, 13);
							printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
							printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的结尾\033[0m                    │");
							printf("\n                        "); printf("│                 请在输入日期后按下回车                  │");
							printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
							Sleep(1000);
							return false;
						}
					}
				}
				else
				{
					yue = -1;
					if (a[3] == '\0')
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                      \033[40;31m-!-日子未读入\033[0m                      │");
						printf("\n                        "); printf("│                     请检查输入内容                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
					else
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的内容\033[0m                    │");
						printf("\n                        "); printf("│                     请检查输入内容                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
				}
			}
			else
			{
				dingWei(0, 13);
				printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
				printf("\n                        "); printf("│               \033[40;31m-!-输入时间格式错误，请确认\033[0m               │");
				printf("\n                        "); printf("│                  提示：时间输入格式为                   │");
				printf("\n                        "); printf("│                           6.19                          │");
				printf("\n                        "); printf("│                           13:46                         │");
				printf("\n                        "); printf("│                                                         │");
				printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
				Sleep(1000);
				return false;
			}
		}
		else
		{
			if (a[1] == '.' || a[1] == '。' || a[1] == ',')
			{
				yue = a[0] - '0';
				if (a[2] <= '9' && a[2] >= '0')
				{
					if (a[3] <= '9' && a[3] >= '0')
					{
						ri = (a[2] - '0') * 10 + (a[3] - '0');
					}
					else
					{
						if (a[3] == '\0')
						{
							ri = a[2] - '0';
						}
						else
						{
							dingWei(0, 13);
							printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
							printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的结尾\033[0m                    │");
							printf("\n                        "); printf("│            请在输入日期后按下回车再输入时间             │");
							printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
							Sleep(1000);
							return false;
						}
					}
				}
				else
				{
					if (a[2] == '\0')
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                     \033[40;31m-!-输入内容不全\033[0m                     │");
						printf("\n                        "); printf("│                     请检查输入数据                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
					else
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                    \033[40;31m-!-出乎意料的内容\033[0m                    │");
						printf("\n                        "); printf("│                     请检查输入数据                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
				}
			}
			else
			{
				if (a[1] == '\0')
				{
					dingWei(0, 13);
					printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                        "); printf("│                     \033[40;31m-!-输入内容不全\033[0m                     │");
					printf("\n                        "); printf("│                     请检查输入数据                      │");
					printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					return false;
				}
				else
				{
					dingWei(0, 13);
					printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                        "); printf("│            \033[40;31m-!-出乎意外的内容，请检查输入数据\033[0m            │");
					printf("\n                        "); printf("│                  提示：时间输入格式为                   │");
					printf("\n                        "); printf("│                           6.19                          │");
					printf("\n                        "); printf("│                           13:46                         │");
					printf("\n                        "); printf("│                                                         │");
					printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					return false;
				}
			}
		}
	}
	else
	{
		dingWei(0, 13);
		printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                        "); printf("│                     \033[40;31m-!-输入内容错误\033[0m                     │");
		printf("\n                        "); printf("│                     请检查输入数据                      │");
		printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		return false;
	}
	if (yue <= 0 || yue > 12) {
		yue = -1;
		dingWei(0, 13);
		printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                        "); printf("│                     \033[40;31m-!-月份数据异常\033[0m                     │");
		printf("\n                        "); printf("│                     请检查输入数据                      │");
		printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		return false;
	}
	if ((ri < 1) || (yue == 2 && ri > 29) || ((yue == 1 || yue == 3 || yue == 5 || yue == 7 || yue == 8 || yue == 10 || yue == 12) && ri > 31) || ((yue == 4 || yue == 6 || yue == 9 || yue == 11) && ri > 30) || ri > 31)
	{
		ri = -1;
		dingWei(0, 13);
		printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                        "); printf("│                     \033[40;31m-!-日子数据异常\033[0m                     │");
		printf("\n                        "); printf("│                     请检查输入数据                      │");
		printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		return false;
	}
	return true;
}


//输入一个含有时间的字符串，长度为6，包括'\0'，输出存在shi、fen里
//a为读入的时、分字符串
bool jieShiShiJian(char* a) {
	//a为读入的时、分字符串
	if (a[0] <= '9' && a[0] >= '0')
	{
		if (a[1] <= '9' && a[1] >= '0')
		{
			if (a[2] == ':' || a[2] == '：' || a[2] == '；' || a[2] == ';')
			{
				shi = (a[0] - '0') * 10 + (a[1] - '0');
				if (a[3] <= '9' && a[3] >= '0')
				{
					if (a[4] <= '9' && a[4] >= '0') {
						if (a[5] != '\0') {
							dingWei(0, 13);
							printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
							printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的结尾\033[0m                    │");
							printf("\n                        "); printf("│                 请在输入日期后按下回车                  │");
							printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
							Sleep(1000);
							return false;
						}
						fen = (a[3] - '0') * 10 + (a[4] - '0');
					}
					else
					{
						if (a[4] == '\0')
						{
							fen = a[3] - '0';
						}
						else
						{
							dingWei(0, 13);
							printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
							printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的结尾\033[0m                    │");
							printf("\n                        "); printf("│                 请在输入时间后按下回车                  │");
							printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
							Sleep(1000);
							return false;
						}
					}
				}
				else
				{
					shi = -1;
					if (a[3] == '\0')
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                      \033[40;31m-!-分钟未读入\033[0m                      │");
						printf("\n                        "); printf("│                     请检查输入内容                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
					else
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的内容\033[0m                    │");
						printf("\n                        "); printf("│                     请检查输入内容                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
				}
			}
			else
			{
				printf(" -!-输入时间格式错误，请确认\n  提示：时间输入格式为\n    13:46\n"); return false;
			}
		}
		else
		{
			if (a[1] == ':' || a[1] == '：' || a[1] == '；' || a[1] == ';')
			{
				shi = a[0] - '0';
				if (a[2] <= '9' && a[2] >= '0')
				{
					if (a[3] <= '9' && a[3] >= '0')
					{
						fen = (a[2] - '0') * 10 + (a[3] - '0');
					}
					else
					{
						if (a[3] == '\0')
						{
							fen = a[2] - '0';
						}
						else
						{
							dingWei(0, 13);
							printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
							printf("\n                        "); printf("│                    \033[40;31m-!-出乎意外的结尾\033[0m                    │");
							printf("\n                        "); printf("│            请在输入日期后按下回车再输入时间             │");
							printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
							Sleep(1000);
							return false;
						}
					}
				}
				else
				{
					if (a[2] == '\0')
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│                     \033[40;31m-!-输入内容不全\033[0m                     │");
						printf("\n                        "); printf("│                     请检查输入数据                      │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
					else
					{
						dingWei(0, 13);
						printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                        "); printf("│            \033[40;31m-!-出乎意外的内容，请检查输入数据\033[0m            │");
						printf("\n                        "); printf("│                  提示：时间输入格式为                   │");
						printf("\n                        "); printf("│                           6.19                          │");
						printf("\n                        "); printf("│                           13:46                         │");
						printf("\n                        "); printf("│                                                         │");
						printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						return false;
					}
				}
			}
			else
			{
				if (a[1] == '\0')
				{
					dingWei(0, 13);
					printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                        "); printf("│                     \033[40;31m-!-输入内容不全\033[0m                     │");
					printf("\n                        "); printf("│                     请检查输入数据                      │");
					printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					return false;
				}
				else
				{
					dingWei(0, 13);
					printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                        "); printf("│            \033[40;31m-!-出乎意外的内容，请检查输入数据\033[0m            │");
					printf("\n                        "); printf("│                  提示：时间输入格式为                   │");
					printf("\n                        "); printf("│                           6.19                          │");
					printf("\n                        "); printf("│                           13:46                         │");
					printf("\n                        "); printf("│                                                         │");
					printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					return false;
				}
			}
		}
	}
	else
	{
		dingWei(0, 13);
		printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                        "); printf("│                     \033[40;31m-!-输入内容错误\033[0m                     │");
		printf("\n                        "); printf("│                     请检查输入数据                      │");
		printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000); 
		return false;
	}
	if (shi == 24) shi = 0;
	if (shi <= 0 || shi > 24) {
		yue = -1;
		dingWei(0, 13);
		printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                        "); printf("│                     \033[40;31m-!-小时数据异常\033[0m                     │");
		printf("\n                        "); printf("│                     请检查输入数据                      │");
		printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯");
		Sleep(1000);
		return false;
	}
	if (fen < 0 || fen>60)
	{
		fen = -1;
		dingWei(0, 13);
		printf("\n\n                        "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                        "); printf("│                     \033[40;31m-!-分钟数据异常\033[0m                     │");
		printf("\n                        "); printf("│                     请检查输入数据                      │");
		printf("\n                        "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		return false;
	}
	return true;
}


//统计数据
bool tongJiShuJu(bool biaoJi) {
	struct tongJi_ tongJi[100];
	/*进货*/
	struct jinHuoJiLu_ *p;
	int i = 0;
	bool a = false;
	p = jinHuoTou;
	while (p != NULL) 
	{
		while (tongJi[i].biaoJi) //如果这个统计数组已经使用……
		{
			if (p == NULL)break;
			if ((strcmp(p->peiJianMingCheng, tongJi[i].peiJianMingCheng)) == 0) //判断当前链表的名字是否和当前数组匹配
			{
				//如果匹配……
				tongJi[i].jinHuoShuLiang += p->shuLiang;//加上数量
				tongJi[i].jinHuoDanJia = p->danJia;
				tongJi[i].jinHuoZongJia += (p->shuLiang * p->danJia);//记上价格
				a = true;//阻止后面的赋值运算
				i = 0;//
				p = p->biaoJi;
				break;
			}
			else i++;
		}
		if (!a)
		{
			strcpy_s(tongJi[i].peiJianMingCheng, p->peiJianMingCheng);
			tongJi[i].biaoJi = true;
			i = 0;
		}
		a = false;
	}
	/*批发*/
	struct piFaJiLu_ *pp;
	int n = 0;
	a = false;
	pp = piFaTou ;
	while (pp != NULL)
	{
		while (tongJi[n].biaoJi)
		{
			if (pp == NULL)break;
			if ((strcmp(pp->peiJianMingCheng, tongJi[n].peiJianMingCheng)) == 0)//如果链表内配件名字与统计内名字匹配
			{
				tongJi[n].piFaShuLiang += pp->shuLiang;//统计内加上数据
				tongJi[n].piFaZongJia += (pp->shuLiang * pp->danJia);
				a = true;//标记，该配件有数据
				n = 0;
				pp = pp->biaoJi;
			}
			else n++;
		}
	
		if (!a)//该配件尚未被记录过
		{
			strcpy_s(tongJi[n].peiJianMingCheng, pp->peiJianMingCheng);//在这个位置创建该配件统计
			tongJi[n].biaoJi = true;//该统计单元已被使用
			n = 0;//回到开头
		}
		a = false;
	}
	/*零售*/
	struct lingShouJiLu_* ppp;
	int m = 0;
	a = false;
	ppp = lingShouTou;
	while (ppp != NULL)
	{
		while (tongJi[m].biaoJi)
		{
			if (ppp == NULL)break;
			if ((strcmp(ppp->peiJianMingCheng, tongJi[m].peiJianMingCheng)) == 0)
			{
				tongJi[m].lingShouShuLiang += ppp->shuLiang;
				tongJi[m].lingShouZongJia += (ppp->shuLiang * ppp->danJia);
				a = true;
				m = 0;
				ppp = ppp->biaoJi;
				break;
			}
			else m++;
		}
		if (!a)
		{
			strcpy_s(tongJi[m].peiJianMingCheng, ppp->peiJianMingCheng);
			tongJi[m].biaoJi = true;
			m = 0;
		}
		a = false;
	}
	/*赠品*/
	struct piFaJiLu_* pppp;
	int v = 0;
	a = false;
	pppp = piFaTou;
	while (pppp != NULL)
	{
		while (tongJi[v].biaoJi)
		{
			if (strcmp(pppp->zengPinXinXi, "$empty$") == 0)break;
			if (pppp == NULL)break;
			if (strcmp(pppp->zengPinXinXi, "$empty$") != 0)
			{
				if ((strcmp(pppp->zengPinXinXi, tongJi[v].peiJianMingCheng)) == 0)
				{
					tongJi[v].jinHuoShuLiang--;
					if (tongJi[v].jinHuoDanJia != 0)
						tongJi[v].jinHuoZongJia += tongJi[v].jinHuoDanJia;
					else
						tongJi[v].jinHuoZongJia += 30;
					a = true;
					v = 0;
					pppp = pppp->biaoJi;
					break;
				}
				else v++;
			}
			else v++;
		}
		if (!a)
		{
			if (strcmp(pppp->zengPinXinXi, "$empty$") != 0) {
				strcpy_s(tongJi[v].peiJianMingCheng, pppp->zengPinXinXi);
				tongJi[v].biaoJi = true;
				v = 0;
			}
			else pppp = pppp->biaoJi;
		}
		a = false;
	}
	i = 0;
	while (tongJi[i].biaoJi  == true) {
		tongJi[i].jingLiRun = -(tongJi[i].jinHuoZongJia - tongJi[i].lingShouZongJia - tongJi[i].piFaZongJia);
		i++;
	}
	if (biaoJi) {
		/*输出*/
		system("cls");
		dingWeiDian = 4;
		i = 0;
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │\n"); Sleep(20);
		if (tongJi[0].biaoJi == false) {
			printf("                   "); printf("│                  没有任何数据                                       │\n"); Sleep(20);
			dingWeiDian++;
		}
		while (tongJi[i].biaoJi == true) {
			printf("                   "); printf("│                                                                     │"); Sleep(20);
			dingWei(30, dingWeiDian++);
			printf("  >%s的库存为%d；净利润为 %g\n", tongJi[i].peiJianMingCheng,(tongJi[i].jinHuoShuLiang- tongJi[i].lingShouShuLiang - tongJi[i].piFaShuLiang),tongJi[i].jingLiRun);
			i++;
		}
		printf("                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		/*总情况*/
		float sum = 0.0;
		i = 0;
		while (tongJi[i].biaoJi == true) {
			sum += tongJi[i].jingLiRun;
			i++;
		}
		dingWeiDian += 5;
		printf("\n\n                       "); printf("╭ ──────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                       "); printf("│                                                              │"); Sleep(20);
		printf("\n                       "); printf("│                                                              │"); Sleep(20);
		printf("\n                       "); printf("│                                                              │"); Sleep(20);
		printf("\n                       "); printf("│                                                              │"); Sleep(20);
		printf("\n                       "); printf("╰ ──────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(32, dingWeiDian++);
		printf(" >总情况为");
		if (sum < 0) printf("亏损%g元。", -sum);
		if (sum > 0) printf("盈利%g元。", sum);
		dingWei(32, dingWeiDian++);
		printf(" >目前资金%.2f元。", ziJin + sum);
		dingWei(28, dingWeiDian+3);
	}
	dingWeiDian = 17;
	return true;
}


//匹配赠品
char* piPeiZengPin(int a) {
	char b[20];
	switch (a)
	{
	case 0:strcpy_s(b, "传感器"); break;
	case 1:strcpy_s(b, "处理器"); break;
	case 2:strcpy_s(b, "磁盘驱动器"); break;
	case 3:strcpy_s(b, "储存控制器"); break;
	case 4:strcpy_s(b, "电池"); break;
	case 5:strcpy_s(b, "显示适配器"); break;
	case 6:strcpy_s(b, "音频设备"); break;
	case 7:strcpy_s(b, "网络适配器"); break;
	case 8:strcpy_s(b, "照相机"); break;
	case 9:strcpy_s(b, "监视器"); break;
	default:printf(" -!-在匹配赠品的时候出现了BUG。/710\n");
		break;
	}
	return b;
}
//赠品管理
bool zengPinGuanLi(struct piFaJiLu_* p)
{
	srand(time(NULL));
	int xuanZePeiJian=-1;
	int b[3];
	bool qwe=true;
	char* zengPin;
	if (p->shuLiang > zengPinQiBuShu)
	{
		dingWei(0, 16);
		printf("\n                     "); printf("╭ ───────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                     "); printf("│                                                               │"); Sleep(20);
		printf("\n                     "); printf("│                                                               │"); Sleep(20);
		printf("\n                     "); printf("│                                                               │"); Sleep(20);
		printf("\n                     "); printf("│                                                               │"); Sleep(20);
		printf("\n                     "); printf("│                                                               │"); Sleep(20);
		printf("\n                     "); printf("╰ ───────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(28, 19);
		printf("  >尊敬的土豪，请从以下赠品中选择一个您喜欢的：");
		dingWei(28, 20);
		for (int i = 0; i < 3; i++)
		{
			switch (rand() % 10)
			{
			case 0:printf("    传感器"); b[i] = 0; break;
			case 1:printf("    处理器"); b[i] = 1; break;
			case 2:printf("    磁盘驱动器"); b[i] = 2; break;
			case 3:printf("    储存控制器"); b[i] = 3; break;
			case 4:printf("    电池"); b[i] = 4; break;
			case 5:printf("    显示适配器"); b[i] = 5; break;
			case 6:printf("    音频设备"); b[i] = 6; break;
			case 7:printf("    网络适配器"); b[i] = 7; break;
			case 8:printf("    照相机"); b[i] = 8; break;
			case 9:printf("    监视器"); b[i] = 9; break;
			default:printf(" -!-讲道理不该出现这行字的，但是出现了，那就是出现BUG了。/744\n");
				break;
			}
			printf("-%d", i + 1);
		}
		while (qwe)
		{
			dingWei(28, 21);
			printf("  >请输入你选择的配件的序号：");
			scanf_s("%d", &xuanZePeiJian);
			getchar();
			switch (xuanZePeiJian)
			{
			case 1: {
				zengPin = piPeiZengPin(b[0]);
				for (int n = 0; n < 20; n++) {
					p->zengPinXinXi[n] = zengPin[n];
				}
				qwe = false;
				break;
			}
			case 2: {
				zengPin = piPeiZengPin(b[1]);
				for (int n = 0; n < 20; n++) {
					p->zengPinXinXi[n] = zengPin[n];
				}
				qwe = false;
				break;
			}
			case 3: {
				zengPin = piPeiZengPin(b[2]);
				for (int n = 0; n < 20; n++) {
					p->zengPinXinXi[n] = zengPin[n];
				}
				qwe = false;
				break;
			}
			default:printf(" -!-即使你输入这个你也不会拿到其它东西。/758\n"); 
				break;
			}			
		}
	}
	return true;
}


//输入想打印的指针，其余为NULL，打印内容
int daYin(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
	if (a != NULL) {
		daYin_huaKuang();
		dingWei(25, dingWeiDian++);
		printf("进货记录的编号：%d\n", a->bianHao);
		dingWei(25, dingWeiDian++);
		printf("日          期：%d月%d日 %d时%d分\n", a->jinHuoShiJian.yue, a->jinHuoShiJian.ri, a->jinHuoShiJian.shi, a->jinHuoShiJian.fen);
		dingWei(25, dingWeiDian++);
		printf("配  件  名  称：");
		puts(a->peiJianMingCheng);
		dingWei(25, dingWeiDian++);
		printf("型          号：");
		puts(a->xingHao);
		dingWei(25, dingWeiDian++);
		printf("制    造    商：");
		puts(a->zhiZaoShang);
		dingWei(25, dingWeiDian++);
		printf("单          价：%g\n", a->danJia);
		dingWei(25, dingWeiDian++);
		printf("数          量：%d\n", a->shuLiang);
		dingWei(25, dingWeiDian++);
		printf("总          价：%g\n", a->danJia * (float)a->shuLiang);
		dingWei(25, dingWeiDian++);
		printf("供    货    商："); puts(a->gongHuoShang);
		dingWeiDian ++;
		dingWei(25, dingWeiDian++);
		dingWeiDian += 2;
		return 1;
	}
	if (b != NULL) {
		daYin_huaKuang();
		dingWei(25, dingWeiDian++);
		printf("零售记录的编号：%d\n", b->bianHao);
		dingWei(25, dingWeiDian++);
		printf("日          期：%d月%d日 %d时%d分\n", b->lingShouShiJian.yue, b->lingShouShiJian.ri, b->lingShouShiJian.shi, b->lingShouShiJian.fen);
		dingWei(25, dingWeiDian++);
		printf("配  件  名  称：");
		puts(b->peiJianMingCheng);
		dingWei(25, dingWeiDian++);
		printf("型          号："); puts(b->xingHao);
		dingWei(25, dingWeiDian++);
		printf("制    造    商："); puts(b->zhiZaoShang);
		dingWei(25, dingWeiDian++);
		printf("单          价：%g\n", b->danJia);
		dingWei(25, dingWeiDian++);
		printf("数          量：%d\n", b->shuLiang);
		dingWei(25, dingWeiDian++);
		printf("总          价：%g\n", b->danJia * b->shuLiang);
		dingWei(25, dingWeiDian++);
		printf("客  户  信  息："); puts(b->keHuXinXi);
//		dingWei(25, dingWeiDian++); printf("#\n");
		dingWeiDian += 4;
		return 2;
	}
	if (c != NULL) {
		daYin_huaKuang();
		dingWei(25, dingWeiDian++);
		printf("批发记录的编号：%d\n", c->bianHao);
		dingWei(25, dingWeiDian++);
		printf("日          期：%d月%d日 %d时%d分\n", c->piFaShiJian.yue, c->piFaShiJian.ri, c->piFaShiJian.shi, c->piFaShiJian.fen);
		dingWei(25, dingWeiDian++);
		printf("配  件  名  称：");
		puts(c->peiJianMingCheng);
		dingWei(25, dingWeiDian++);
		printf("型          号：");
		puts(c->xingHao);
		dingWei(25, dingWeiDian++);
		printf("制    造    商：");
		puts(c->zhiZaoShang);
		dingWei(25, dingWeiDian++);
		printf("单          价：%g\n", c->danJia);
		dingWei(25, dingWeiDian++);
		printf("数          量：%d\n", c->shuLiang);
		if(strcmp(c->zengPinXinXi,"$empty$")!=0)
		{
			dingWei(25, dingWeiDian++);
			printf("赠          品：");
			puts(c->zengPinXinXi);
		}
		dingWei(25, dingWeiDian++);
		printf("总          价：%g\n", c->danJia * c->shuLiang);
		dingWei(25, dingWeiDian++);
		printf("客  户  信  息：");
		puts(c->keHuXinXi);
//		dingWei(25, dingWeiDian++);
//		printf("#\n");
		dingWeiDian += 4;
		return 3;
	}
	return -1;
}
//打印画框
void daYin_huaKuang() {
	printf("\n                   "); printf("╭ ───────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("│                                                               │"); Sleep(20);
	printf("\n                   "); printf("╰ ───────────────────────────────────────────────────────────── ╯"); Sleep(20);
}
//全部打印
void quanBuDaYin() {
	struct jinHuoJiLu_* a = jinHuoTou;
	struct piFaJiLu_* b = piFaTou;
	struct lingShouJiLu_* c = lingShouTou;
	system("cls");
	dingWeiDian = 4;
	printf("\n             >进货数据：");
	while (a != NULL) {
		daYin(a, NULL, NULL);
		a = a->biaoJi;
	}
	dingWeiDian += 1;
	printf("\n             >批发数据：");
	while (b != NULL) {
		daYin(NULL, NULL, b);
		b = b->biaoJi;
	}
	dingWeiDian += 1;
	printf("\n\n             >零售数据：");
	while (c != NULL) {
		daYin(NULL, c, NULL);
		c = c->biaoJi;
	}
	printf("\n\n\n");
	dingWeiDian = 17;
	system("pause");
}
//根据时间段打印
bool genJuShiJianDuanChaXun()
{
	system("cls");
	int yue_1, ri_1, shi_1, fen_1, yue_2, ri_2, shi_2, fen_2;
	char riQi_1[10], riQi_2[10], shiJian_1[10], shiJian_2[10];
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 4);
	printf(" -?-开始日期为："); gets_s(riQi_1);
	while (!(jieShiRiQi(riQi_1)))
	{
		dingWei(40, 4);
		printf("  >请再试一遍：");
		gets_s(riQi_1);
	}
	yue_1 = yue; ri_1 = ri;
	dingWei(40, 5);
	printf(" -?-开始时间为："); gets_s(shiJian_1);
	while (!(jieShiShiJian(shiJian_1)))
	{
		dingWei(40, 5);
		printf("  >请再试一遍：");
		gets_s(shiJian_1);
	}
	shi_1 = shi; fen_1 = fen;
	dingWei(40, 6);
	printf(" -?-结束日期为："); gets_s(riQi_2);
	while (!(jieShiRiQi(riQi_2)))
	{
		dingWei(40, 6);
		printf("  >请再试一遍：");
		gets_s(riQi_2);
	}
	yue_2 = yue; ri_2 = ri;
	dingWei(40, 7);
	printf(" -?-结束时间为："); gets_s(shiJian_2);
	while (!(jieShiShiJian(shiJian_2)))
	{
		dingWei(40, 7);
		printf("  >请再试一遍：");
		gets_s(shiJian_2);
	}
	shi_2 = shi; fen_2 = fen;
	struct jinHuoJiLu_* p;
	p = jinHuoTou;
	dingWeiDian = 15;
	printf("\n\n\n\n             >进货数据：");
	while (p != NULL) {
		if (p->jinHuoShiJian.yue * 1000000 + p->jinHuoShiJian.ri * 10000 + p->jinHuoShiJian.shi * 100 + p->jinHuoShiJian.fen > yue_1 * 1000000 + ri_1 * 10000 + shi_1 * 100 + fen_1) {
			if (p->jinHuoShiJian.yue * 1000000 + p->jinHuoShiJian.ri * 10000 + p->jinHuoShiJian.shi * 100 + p->jinHuoShiJian.fen < yue_2 * 1000000 + ri_2 * 10000 + shi_2 * 100 + fen_2)
				daYin(p, NULL, NULL);
		}
		p = p->biaoJi;
	}
	struct piFaJiLu_* pp;
	pp = piFaTou;
	printf("\n             >批发数据：");
	while (pp != NULL) {
		if (pp->piFaShiJian.yue * 1000000 + pp->piFaShiJian.ri * 10000 + pp->piFaShiJian.shi * 100 + pp->piFaShiJian.fen > yue_1 * 1000000 + ri_1 * 10000 + shi_1 * 100 + fen_1)
			if (pp->piFaShiJian.yue * 1000000 + pp->piFaShiJian.ri * 10000 + pp->piFaShiJian.shi * 100 + pp->piFaShiJian.fen < yue_2 * 1000000 + ri_2 * 10000 + shi_2 * 100 + fen_2)
				daYin(NULL, NULL, pp);
		pp = pp->biaoJi;
	}
	struct lingShouJiLu_* ppp;
	ppp = lingShouTou;
	printf("\n\n             >零售数据：");
	while (ppp != NULL) {
		if (ppp->lingShouShiJian.yue * 1000000 + ppp->lingShouShiJian.ri * 10000 + ppp->lingShouShiJian.shi * 100 + ppp->lingShouShiJian.fen > yue_1 * 1000000 + ri_1 * 10000 + shi_1 * 100 + fen_1)
			if (ppp->lingShouShiJian.yue * 1000000 + ppp->lingShouShiJian.ri * 10000 + ppp->lingShouShiJian.shi * 100 + ppp->lingShouShiJian.fen < yue_2 * 1000000 + ri_2 * 10000 + shi_2 * 100 + fen_2)
				daYin(NULL, ppp, NULL);
		ppp = ppp->biaoJi;
	}
	dingWeiDian = 17;
	printf("\n                        ");
	system("pause");
	return true;
}


//用于修改单条链表中的数据
int xiuGaiLianBiao(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
		system("cls");
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│          修改下面哪项数据                                           │"); Sleep(20);
		printf("\n                   "); printf("│         >日期-1                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >配件-2                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >单价-3                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >数量-4                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >客户信息-5                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 14);
		int x;
		scanf_s("%d", &x);
		getchar();
		dingWei(0, 18);
		switch (x)
		{
		case 1:if (xiuGaiLianBiao_riQi(a, b, c) == -1) { printf(" -!-输入不符合规范\n"); }break;
		case 2:if (xiuGaiLianBiao_peiJian(a, b, c) == -1) { printf(" -!-输入不符合规范\n"); }break;
		case 3:if (xiuGaiLianBiao_danJia(a, b, c) == -1) { printf(" -!-输入不符合规范\n"); }break;
		case 4:if (xiuGaiLianBiao_shuLiang(a, b, c) == -1) { printf(" -!-输入不符合规范\n"); }break;
		case 5:if (xiuGaiLianBiao_gongHuoShang(a, b, c) == -1) { printf(" -!-输入不符合规范\n"); }break;

		default:
			break;
		}
	return 0;
}
int xiuGaiLianBiao_riQi(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
	if (a != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 23);
		printf("  >目前的时间为：%d月%d日%d时%d分", a->jinHuoShiJian.yue, a->jinHuoShiJian.ri, a->jinHuoShiJian.shi, a->jinHuoShiJian.fen);
		dingWei(30, 24);
		printf("  >请输入修改后的月份和日子：");
		char m[20];
		gets_s(m); while (!(jieShiRiQi(m)))
		{
			dingWei(30, 24);
			printf("                                   ");
			dingWei(30, 24);
			printf("  >请再试一遍：");
			gets_s(m);
		}
		dingWei(30, 25);
		printf("  >请输入修改后的小时和分钟：");
		gets_s(m);
		while (!(jieShiShiJian(m)))
		{
			dingWei(30, 25);
			printf("                                   ");
			dingWei(30, 25);
			printf("  >请再试一遍：");
			gets_s(m);
		}
		dingWei(30, 26);
		printf("  >修改后的时间为：%d月%d日%d时%d分", yue, ri, shi, fen);
		dingWei(30, 27);
		printf("  >确认保存？  y/n  ");
		char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			a->jinHuoShiJian.yue = yue;
			a->jinHuoShiJian.ri = ri;
			a->jinHuoShiJian.shi = shi;
			a->jinHuoShiJian.fen = fen;
			dingWei(30, 28);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 28); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (b != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 23);
		printf("  >目前的时间为：%d月%d日%d时%d分", b->lingShouShiJian.yue, b->lingShouShiJian.ri, b->lingShouShiJian.shi, b->lingShouShiJian.fen);
		dingWei(30, 24);
		printf("  >请输入修改后的月份和日子：");
		char m[20];
		gets_s(m); while (!(jieShiRiQi(m)))
		{
			dingWei(30, 24);
			printf("                                   ");
			dingWei(30, 24);
			printf("  >请再试一遍：");
			gets_s(m);
		}
		dingWei(30, 25);
		printf("  >请输入修改后的小时和分钟：");
		gets_s(m);
		while (!(jieShiShiJian(m)))
		{
			dingWei(30, 25);
			printf("                                   ");
			dingWei(30, 25);
			printf("  >请再试一遍：");
			gets_s(m);
		}
		dingWei(30, 26);
		printf("  >修改后的时间为：%d月%d日%d时%d分", yue, ri, shi, fen);
		dingWei(30, 27);
		printf("  >确认保存？  y/n  ");
		char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			b->lingShouShiJian.yue = yue;
			b->lingShouShiJian.ri = ri;
			b->lingShouShiJian.shi = shi;
			b->lingShouShiJian.fen = fen;
			dingWei(30, 28);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 28); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (c != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 23);
		printf("  >目前的时间为：%d月%d日%d时%d分", c->piFaShiJian.yue, c->piFaShiJian.ri, c->piFaShiJian.shi, c->piFaShiJian.fen);
		dingWei(30, 24);
		printf("  >请输入修改后的月份和日子：");
		char m[20];
		gets_s(m); while (!(jieShiRiQi(m)))
		{
			dingWei(30, 24);
			printf("                                   ");
			dingWei(30, 24);
			printf("  >请再试一遍：");
			gets_s(m);
		}
		dingWei(30, 25);
		printf("  >请输入修改后的小时和分钟：");
		gets_s(m);
		while (!(jieShiShiJian(m)))
		{
			dingWei(30,25);
			printf("                                   ");
			dingWei(30, 25);
			printf("  >请再试一遍：");
			gets_s(m);
		}
		dingWei(30, 26);
		printf("  >修改后的时间为：%d月%d日%d时%d分", yue, ri, shi, fen);
		dingWei(30, 27);
		printf("  >确认保存？  y/n  ");
		char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			c->piFaShiJian.yue = yue;
			c->piFaShiJian.ri = ri;
			c->piFaShiJian.shi = shi;
			c->piFaShiJian.fen = fen;
			dingWei(30, 28);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 28); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
}
int xiuGaiLianBiao_peiJian(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
	if (a != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 23);
		printf("  >目前的配件信息为：");
		dingWei(30, 24);
		printf("   名称：");puts(a->peiJianMingCheng);
		dingWei(30, 25);
		printf("   型号："); puts(a->xingHao);
		dingWei(30, 26);
		printf("   制造商："); puts(a->zhiZaoShang);
		char mingCheng[20], xingHao[20], zhiZaoShang[20];
		dingWei(30, 28);
		printf("  >请输入修改后的配件名称："); gets_s(mingCheng);
		dingWei(30, 29);
		printf("  >请输入修改后的配件型号："); gets_s(xingHao);
		dingWei(30, 30);
		printf("  >请输入修改后的制造商："); gets_s(zhiZaoShang);
		dingWei(30, 31);
		printf("  >确认保存？  y/n");
		char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			strcpy_s(a->peiJianMingCheng, mingCheng);
			strcpy_s(a->xingHao, xingHao);
			strcpy_s(a->zhiZaoShang, zhiZaoShang);
			dingWei(30, 33);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 33); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (b != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 23);
		printf("  >目前的配件信息为：");
		dingWei(30, 24);
		printf("   名称："); puts(b->peiJianMingCheng);
		dingWei(30, 25);
		printf("   型号："); puts(b->xingHao);
		dingWei(30, 26);
		printf("   制造商："); puts(b->zhiZaoShang);
		char mingCheng[20], xingHao[20], zhiZaoShang[20];
		dingWei(30, 28);
		printf("  >请输入修改后的配件名称："); gets_s(mingCheng);
		dingWei(30, 29);
		printf("  >请输入修改后的配件型号："); gets_s(xingHao);
		dingWei(30, 30);
		printf("  >请输入修改后的制造商："); gets_s(zhiZaoShang);
		dingWei(30, 31);
		printf("  >确认保存？  y/n");
		char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			strcpy_s(b->peiJianMingCheng, mingCheng);
			strcpy_s(b->xingHao, xingHao);
			strcpy_s(b->zhiZaoShang, zhiZaoShang);
			dingWei(30, 33);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 33); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (c!= NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 23);
		printf("  >目前的配件信息为：");
		dingWei(30, 24);
		printf("   名称："); puts(c->peiJianMingCheng);
		dingWei(30, 25);
		printf("   型号："); puts(c->xingHao);
		dingWei(30, 26);
		printf("   制造商："); puts(c->zhiZaoShang);
		char mingCheng[20], xingHao[20], zhiZaoShang[20];
		dingWei(30, 28);
		printf("  >请输入修改后的配件名称："); gets_s(mingCheng);
		dingWei(30, 29);
		printf("  >请输入修改后的配件型号："); gets_s(xingHao);
		dingWei(30, 30);
		printf("  >请输入修改后的制造商："); gets_s(zhiZaoShang);
		dingWei(30, 31);
		printf("  >确认保存？  y/n");
		char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			strcpy_s(c->peiJianMingCheng, mingCheng);
			strcpy_s(c->xingHao, xingHao);
			strcpy_s(c->zhiZaoShang, zhiZaoShang);
			dingWei(30, 33);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 33); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	return -1;
}
int xiuGaiLianBiao_danJia(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
	if (a != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的单价为：%g",a->danJia ); 
		float danJia;
		dingWei(30, 26);
		printf("  >请输入修改后的单价："); scanf_s("%f", &danJia); getchar();
		dingWei(30, 27);
		printf("  >确认保存？   y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			a->danJia = danJia;
			a->zongJia = a->danJia * a->shuLiang;
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (b != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的单价为：%g", b->danJia);
		float danJia;
		dingWei(30, 26);
		printf("  >请输入修改后的单价："); scanf_s("%f", &danJia); getchar();
		dingWei(30, 27);
		printf("  >确认保存？   y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			b->danJia = danJia;
			b->zongJia = b->danJia * b->shuLiang;
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (c != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的单价为：%g", c->danJia);
		float danJia;
		dingWei(30, 26);
		printf("  >请输入修改后的单价："); scanf_s("%f", &danJia); getchar();
		dingWei(30, 27);
		printf("  >确认保存？   y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			c->danJia = danJia;
			c->zongJia = c->danJia * c->shuLiang;
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	return -1;
}
int xiuGaiLianBiao_shuLiang(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
	if (a != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的数量为：%d", a->shuLiang );
		int shuLiang;
		dingWei(30, 26);
		printf("  >请输入修改后的数量："); scanf_s("%d", &shuLiang); getchar();
		dingWei(30, 27);
		printf("  >确认保存？  y/n"); char n; scanf_s("%c", &n,1); getchar();
		if (n == 'y' || n == 'Y') {
			a->shuLiang = shuLiang;
			a->zongJia = a->danJia * a->shuLiang;
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (b != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的数量为：%d", b->shuLiang);
		int shuLiang;
		dingWei(30, 26);
		printf("  >请输入修改后的数量："); scanf_s("%d", &shuLiang); getchar();
		dingWei(30, 27);
		printf("  >确认保存？  y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			b->shuLiang = shuLiang;
			b->zongJia = b->danJia * b->shuLiang;
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (c != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的数量为：%d", c->shuLiang);
		int shuLiang;
		dingWei(30, 26);
		printf("  >请输入修改后的数量："); scanf_s("%d", &shuLiang); getchar();
		dingWei(30, 27); printf("  >");
		if (c->shuLiang > zengPinQiBuShu && shuLiang <= zengPinQiBuShu) {
			printf("此操作将会收回送出的赠品，");
		}
		printf("确认保存？  y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			if (c->shuLiang > zengPinQiBuShu && shuLiang <= zengPinQiBuShu) {
				strcpy_s(c->zengPinXinXi, "$empty$");
				tongJiShuJu(false);
			}
			c->shuLiang = shuLiang;
			c->zongJia = c->danJia * c->shuLiang;
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	return -1;
}
int xiuGaiLianBiao_gongHuoShang(struct jinHuoJiLu_* a, struct lingShouJiLu_* b, struct piFaJiLu_* c) {
	if (a != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的供货商为："); puts(a->gongHuoShang);
		char gongHuoShang[20];
		dingWei(30, 26);
		printf("  >请输入修改后的供货商："); gets_s(gongHuoShang);
		dingWei(30, 27);
		printf("  >确认保存？  y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			strcpy_s(a->gongHuoShang, gongHuoShang);
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (b != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的供货商为："); puts(b->keHuXinXi);
		char gongHuoShang[20];
		dingWei(30, 26);
		printf("  >请输入修改后的供货商："); gets_s(gongHuoShang);
		dingWei(30, 27);
		printf("  >确认保存？  y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			strcpy_s(b->keHuXinXi, gongHuoShang);
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
	if (c != NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(30, 24);
		printf("  >目前的供货商为："); puts(c->keHuXinXi);
		char keHuXinXi[20];
		dingWei(30, 26);
		printf("  >请输入修改后的供货商："); gets_s(keHuXinXi);
		dingWei(30, 27);
		printf("  >确认保存？  y/n"); char n; scanf_s("%c", &n, 1); getchar();
		if (n == 'y' || n == 'Y') {
			strcpy_s(c->keHuXinXi, keHuXinXi);
			dingWei(30, 29);
			printf("  >保存成功"); Sleep(800);
			return 1;
		}
		if (n == 'n' || n == 'N') { dingWei(30, 29); printf("  >已放弃修改"); Sleep(800); return 0; }
		return -1;
	}
}
int xiuGaiChaXun(int xiuGaiBiaoJi, int type)
{
	if (type == 1) {
		struct jinHuoJiLu_* p = jinHuoTou;
		while (p != NULL) {
			if (p->bianHao == xiuGaiBiaoJi) {
				xiuGaiLianBiao(p, NULL, NULL);
			}
			p = p->biaoJi;
		}
		return 0;
	}
	if (type == 2) {
		struct lingShouJiLu_* p = lingShouTou;
		while (p != NULL) {
			if (p->bianHao == xiuGaiBiaoJi) {
				xiuGaiLianBiao(NULL, p, NULL);
			}
			p = p->biaoJi;
		}
		return 0;
	}
	if (type == 3) {
		struct piFaJiLu_* p = piFaTou;
		while (p != NULL) {
			if (p->bianHao == xiuGaiBiaoJi) {
				xiuGaiLianBiao(NULL, NULL, p);
			}
			p = p->biaoJi;
		}
		return 0;
	}
	return -1;
}


//从文件导入数条信息，需要分类
//aa=1：进货
//aa=2：批发
//aa=3：零售
int wenJianDaoRu(int aa,char* filename) {
	//文件导入
	int bianHao = 0;//进货记录编号
	char peiJianMingCheng[20];//配件名称
	char xingHao[20];//型号
	char zhiZaoShang[20];//制造商
	char riQi[20], shiJian[20];//日期和时间
	char zengPinXinXi[20] = {'$','e','m','p','t','y','$','\0'};//赠品信息
	float danJia = 0.0;//单价
	int shuLiang = 0;//数量
	float zongJia = 0.0;//总价
	char xinXi[20];//客户信息
	int i = 0, n = 0, x = 0;
	char linShi[25], linShi_1[25];//用于临时储存要判断的中文字符
	fopen_s(&daoRu, filename, "r");
	if (daoRu == NULL) {
		dingWei(0, 24);
		printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                         "); printf("│                \033[40;31m-!-未导入数据，请检查文件\033[0m                │");
		printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		return false;
	}
	char a[20];
	while (!feof(daoRu))
	{
		bianHao = 0;//进货记录编号
		danJia = 0.0;//单价
		shuLiang = 0;//数量
		zongJia = 0.0;//总价
		strcpy_s(zengPinXinXi, "$empty$");
		zengPinXinXi[7] = '\0';
		strcpy_s(a, "tset"); a[4] = '\0';
		while (a[0] != '#')
		{
			strcpy_s(a, "tset"); a[4] = '\0';
			for (i = 0; i < 20; i++)
			{
				fread_s(&a[i], sizeof(char), sizeof(char), 1, daoRu);
				if (a[i] == '\n') {
					a[i] = '\0';
					break;
				}
			}
			strcpy_s(linShi, "编号");
			if ((a[0] == linShi[0]) && (a[1] == linShi[1]))//吸收编号
			{
				for (n = 1; n <= i; n++) {
					if (a[n] == ':') break;
				}
				for (int m = n + 1; m < i; m++) {
					bianHao = bianHao * 10 + (a[m] - '0');
				}
			}
			else
			{
				strcpy_s(linShi, "配件名称"); strcpy_s(linShi_1, "名称");
				if (((a[0] == linShi[0]) && (a[1] == linShi[1]) && (a[2] == linShi[2]) && (a[3] == linShi[3]) && (a[4] == linShi[4]) && (a[5] == linShi[5])) || ((a[0] == linShi_1[0]) && (a[1] == linShi_1[1])))//配件名称
				{
					for (n = 1; n <= i; n++)
					{
						if (a[n] == ':') break;
					}
					x = 0;
					for (int m = n + 1; m < i; m++)
					{
						peiJianMingCheng[x] = a[m];
						x++;
					}
					peiJianMingCheng[x] = '\0';
				}
				else
				{
					strcpy_s(linShi, "配件型号"); strcpy_s(linShi_1, "型号");
					if (((a[0] == linShi[0]) && (a[1] == linShi[1]) && (a[2] == linShi[2]) && (a[3] == linShi[3]) && (a[4] == linShi[4]) && (a[5] == linShi[5])) || ((a[0] == linShi_1[0]) && (a[1] == linShi_1[1])))//配件名称
					{
						for (n = 1; n <= i; n++)
						{
							if (a[n] == ':') break;
						}
						x = 0;
						for (int m = n + 1; m < i; m++)
						{
							xingHao[x] = a[m];
							x++;
						}
						xingHao[x] = '\0';
					}
					else
					{
						strcpy_s(linShi, "制造商");
						if ((a[0] == linShi[0]) && (a[1] == linShi[1]))
						{
							for (n = 1; n <= i; n++)
							{
								if (a[n] == ':') break;
							}
							x = 0;
							for (int m = n + 1; m < i; m++)
							{
								zhiZaoShang[x] = a[m];
								x++;
							}
							zhiZaoShang[x] = '\0';
						}
						else
						{
							strcpy_s(linShi, "单价");
							if ((a[0] == linShi[0]) && (a[1] == linShi[1]))
							{
								for (n = 1; n <= i; n++)
								{
									if (a[n] == ':') break;
								}
								int m = 0;
								for (m = n + 1; m < i; m++)
								{
									if (a[m] != '.')
									{
										danJia = danJia * 10.0 + (float)(a[m] - '0');
									}
									else break;
								}
								if (a[m] == '.')
								{
									m++;
									float danJia_xiaoShu = 0;
									for (m; m < i; m++)
									{
										danJia_xiaoShu = danJia_xiaoShu * 10.0 + (float)(a[m] - '0');
									}
									while (danJia_xiaoShu >= 1) { danJia_xiaoShu = danJia_xiaoShu / 10; }
									danJia += danJia_xiaoShu;
								}
							}
							else
							{
								strcpy_s(linShi, "数量");
								if ((a[0] == linShi[0]) && (a[1] == linShi[1]))
								{
									for (n = 1; n <= i; n++)
									{
										if (a[n] == ':') break;
									}
									for (int m = n + 1; m < i; m++) {
										shuLiang = shuLiang * 10 + (a[m] - '0');
									}
								}
								else
								{
									strcpy_s(linShi, "总价");
									if ((a[0] == linShi[0]) && (a[1] == linShi[1]))
									{
										for (n = 1; n <= i; n++)
										{
											if (a[n] == ':') break;
										}
										int m = 0;
										for (m = n + 1; m < i; m++)
										{
											if (a[m] != '.')
											{
												zongJia = zongJia * 10.0 + (float)(a[m] - '0');
											}
											else break;
										}
										if (a[m] == '.')
										{
											m++;
											float zongJia_xiaoShu = 0;
											for (m; m < i; m++)
											{
												zongJia_xiaoShu = zongJia_xiaoShu * 10.0 + (float)(a[m] - '0');
											}
											while (zongJia_xiaoShu >= 1) { zongJia_xiaoShu = zongJia_xiaoShu / 10; }
											zongJia += zongJia_xiaoShu;
										}
									}
									else
									{
										strcpy_s(linShi, "客户信息"); strcpy_s(linShi_1, "供货商");
										if ((a[0] == linShi[0]) && (a[1] == linShi[1]) || (a[0] == linShi_1[0]) && (a[1] == linShi_1[1]))
										{
											for (n = 1; n <= i; n++)
											{
												if (a[n] == ':') break;
											}
											x = 0;
											for (int m = n + 1; m < i; m++)
											{
												xinXi[x] = a[m];
												x++;
											}
											xinXi[x] = '\0';
										}
										else
										{
											strcpy_s(linShi, "日期");
											if ((a[0] == linShi[0]) && (a[1] == linShi[1]) || (a[0] == linShi_1[0]) && (a[1] == linShi_1[1]))
											{
												for (n = 1; n <= i; n++)
												{
													if (a[n] == ':') break;
												}
												x = 0;
												for (int m = n + 1; m < i; m++)
												{
													riQi[x] = a[m];
													x++;
												}
												riQi[x] = '\0';
											}
											else
											{
												strcpy_s(linShi, "时间");
												if ((a[0] == linShi[0]) && (a[1] == linShi[1]) || (a[0] == linShi_1[0]) && (a[1] == linShi_1[1]))
												{
													for (n = 1; n <= i; n++)
													{
														if (a[n] == ':') break;
													}
													x = 0;
													for (int m = n + 1; m < i; m++)
													{
														shiJian[x] = a[m];
														x++;
													}
													shiJian[x] = '\0';
												}
												else {
													strcpy_s(linShi, "赠品信息");
													if ((a[0] == linShi[0]) && (a[1] == linShi[1]) || (a[0] == linShi_1[0]) && (a[1] == linShi_1[1]))
													{
														for (n = 1; n <= i; n++)
														{
															if (a[n] == ':') break;
														}
														x = 0;
														for (int m = n + 1; m < i; m++)
														{
															zengPinXinXi[x] = a[m];
															x++;
														}
														zengPinXinXi[x] = '\0';
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (aa == 1) {
			lianJie_jinHuo(daoRu_jinHuo(bianHao, peiJianMingCheng, xingHao, zhiZaoShang, riQi, shiJian, danJia, shuLiang, zongJia, xinXi));
		}
		if (aa == 2) {
			lianJie_piFa(daoRu_piFa(bianHao, peiJianMingCheng, xingHao, zhiZaoShang, riQi, shiJian, danJia, shuLiang, zongJia, xinXi,zengPinXinXi));
		}
		if (aa == 3) {
			lianJie_lingShou(daoRu_lingShou(bianHao, peiJianMingCheng, xingHao, zhiZaoShang, riQi, shiJian, danJia, shuLiang, zongJia, xinXi));
		}
	}
	dingWei(0, 24);
	printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
	printf("\n                         "); printf("│                     \033[40;31m-!-数据导入成功\033[0m                     │");
	printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
	Sleep(1000);
	fclose(daoRu);
	return 0;
}
struct jinHuoJiLu_* daoRu_jinHuo(int bianHao, char peiJianMingCheng[20], char xingHao[20], char zhiZaoShang[20], char riQi[20], char shiJian[20], float danJia, int shuLiang, float zongJia, char xinXi[20]) {
	struct jinHuoJiLu_* p;
	p = (struct jinHuoJiLu_*)malloc(sizeof(struct jinHuoJiLu_));
	p->bianHao = bianHao;
	jieShiRiQi(riQi);
	p->jinHuoShiJian.yue = yue; p->jinHuoShiJian.ri = ri;
	jieShiShiJian(shiJian);
	p->jinHuoShiJian.shi = shi; p->jinHuoShiJian.fen = fen;
	strcpy_s(p->peiJianMingCheng,peiJianMingCheng);
	strcpy_s(p->xingHao,xingHao);
	strcpy_s(p->zhiZaoShang,zhiZaoShang);
	p->danJia=danJia;
	p->shuLiang = shuLiang;
	p->zongJia = p->danJia * (float)p->shuLiang;
	strcpy_s(p->gongHuoShang ,xinXi);
	return p;
}
struct piFaJiLu_* daoRu_piFa(int bianHao, char peiJianMingCheng[20], char xingHao[20], char zhiZaoShang[20], char riQi[20], char shiJian[20], float danJia, int shuLiang, float zongJia, char xinXi[20],char zengPinXinXi[20]) {
	struct piFaJiLu_* p;
	p = (struct piFaJiLu_*)malloc(sizeof(struct piFaJiLu_));
	p->bianHao = bianHao;
	jieShiRiQi(riQi);
	p->piFaShiJian.yue = yue; p->piFaShiJian.ri = ri;
	jieShiShiJian(shiJian);
	p->piFaShiJian.shi = shi; p->piFaShiJian.fen = fen;
	strcpy_s(p->peiJianMingCheng, peiJianMingCheng);
	strcpy_s(p->xingHao, xingHao);
	strcpy_s(p->zhiZaoShang, zhiZaoShang);
	p->danJia = danJia;
	p->shuLiang = shuLiang;
	p->zongJia = p->danJia * (float)p->shuLiang;
	strcpy_s(p->keHuXinXi, xinXi);
	strcpy_s(p->zengPinXinXi, zengPinXinXi);
	return p;
}
struct lingShouJiLu_* daoRu_lingShou(int bianHao, char peiJianMingCheng[20], char xingHao[20], char zhiZaoShang[20], char riQi[20], char shiJian[20], float danJia, int shuLiang, float zongJia, char xinXi[20]) {
	struct lingShouJiLu_* p;
	p = (struct lingShouJiLu_*)malloc(sizeof(struct lingShouJiLu_));
	p->bianHao = bianHao;
	jieShiRiQi(riQi);
	p->lingShouShiJian.yue = yue; p->lingShouShiJian.ri = ri;
	jieShiShiJian(shiJian);
	p->lingShouShiJian.shi = shi; p->lingShouShiJian.fen = fen;
	strcpy_s(p->peiJianMingCheng, peiJianMingCheng);
	strcpy_s(p->xingHao, xingHao);
	strcpy_s(p->zhiZaoShang, zhiZaoShang);
	p->danJia = danJia;
	p->shuLiang = shuLiang;
	p->zongJia = p->danJia * (float)p->shuLiang;
	strcpy_s(p->keHuXinXi, xinXi);
	return p;
}


//删除数据
bool shanChuShuJu(int type, int bianHao)
{
	//删除进货记录
	if (type == 1)
	{
		struct jinHuoJiLu_* p, * qian, * hou;
		p = jinHuoTou;
		qian = jinHuoTou;
		hou = jinHuoTou;
		//首项
		if (p->bianHao == bianHao) {
			jinHuoTou = jinHuoTou->biaoJi;
			free(p);
			return true;
		}
		//中间
		while ((p->bianHao != bianHao) && (p->biaoJi != NULL)) {
			qian = p;
			p = p->biaoJi;
			hou = p->biaoJi;
		}
		if (p->bianHao == bianHao) {
			qian->biaoJi = hou;
			free(p);
			return true;
		}
	}
	//删除批发记录
	if (type == 2)
	{
		struct piFaJiLu_* p, * qian, * hou;
		p = piFaTou;
		qian = piFaTou;
		hou = piFaTou;
		//首项
		if (p->bianHao == bianHao) {
			piFaTou = piFaTou->biaoJi;
			free(p);
			return true;
		}
		//中间
		while ((p->bianHao != bianHao) && (p->biaoJi != NULL)) {
			qian = p;
			p = p->biaoJi;
			hou = p->biaoJi;
		}
		if (p->bianHao == bianHao) {
			qian->biaoJi = hou;
			free(p);
			return true;
		}
	}
	//删除零售记录
	if (type == 3)
	{
		struct lingShouJiLu_* p, * qian, * hou;
		p = lingShouTou;
		qian = lingShouTou;
		hou = lingShouTou;
		//首项
		if (p->bianHao == bianHao) {
			lingShouTou = lingShouTou->biaoJi;
			free(p);
			return true;
		}
		//中间
		while ((p->bianHao != bianHao) && (p->biaoJi != NULL)) {
			qian = p;
			p = p->biaoJi;
			hou = p->biaoJi;
		}
		if (p->bianHao == bianHao) {
			qian->biaoJi = hou;
			free(p);
			return true;
		}
	}
	return false;
}


//写入到文件里
bool baoCunJinHuoJiLu_(char* fileName) {
	FILE* fp;
	char linShi[40];
	fopen_s(&fp, fileName, "w");
	if (fp == NULL) {
		printf(" \n-!-进货导出文件创建失败，请检查上一个程序窗口是否完全关闭，清除缓存后再试一遍，注意数据仍未保存！/1513\n");
		return false;
	}
	struct  jinHuoJiLu_* p;
	p = jinHuoTou;
	/*写入过程*/
	while (p != NULL) {
		strcpy_s(linShi, "配件名称");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->peiJianMingCheng, '\n');
		strcpy_s(linShi, "配件型号");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->xingHao, '\n');
		strcpy_s(linShi, "制造商");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->zhiZaoShang, '\n');
		strcpy_s(linShi, "单价");
		fprintf_s(fp, "%s%c%.2f%c", linShi, ':', p->danJia, '\n');
		strcpy_s(linShi, "数量");
		fprintf_s(fp, "%s%c%d%c", linShi, ':', p->shuLiang, '\n');
		strcpy_s(linShi, "总价");
		fprintf_s(fp, "%s%c%.2f%c", linShi, ':', p->zongJia, '\n');
		strcpy_s(linShi, "供货商");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->gongHuoShang, '\n');
		strcpy_s(linShi, "时间");
		fprintf_s(fp, "%s%c%d%s%d%s%d%s%d%s%c%c%c", linShi, ':', p->jinHuoShiJian.yue, "月", p->jinHuoShiJian.ri, "日", p->jinHuoShiJian.shi, "时", p->jinHuoShiJian.fen, "分", '\n', '#', '\n');
		p = p->biaoJi;
	}
	fclose(fp);
	return 0;
}
bool baoCunPiFaJiLu_(char* fileName) {
	FILE* fp;
	char linShi[40];
	fopen_s(&fp, fileName, "w");
	if (fp == NULL) {
		printf(" \n-!-批发导出文件创建失败，请检查上一个程序窗口是否完全关闭，清除缓存后再试一遍，注意数据仍未保存！/1545\n");
		return false;
	}
	struct  piFaJiLu_* p;
	p = piFaTou;
	/*写入过程*/
	while (p != NULL) {
		strcpy_s(linShi, "配件名称");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->peiJianMingCheng, '\n');
		strcpy_s(linShi, "配件型号");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->xingHao, '\n');
		strcpy_s(linShi, "制造商");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->zhiZaoShang, '\n');
		strcpy_s(linShi, "单价");
		fprintf_s(fp, "%s%c%.2f%c", linShi, ':', p->danJia, '\n');
		strcpy_s(linShi, "数量");
		fprintf_s(fp, "%s%c%d%c", linShi, ':', p->shuLiang, '\n');
		strcpy_s(linShi, "总价");
		fprintf_s(fp, "%s%c%.2f%c", linShi, ':', p->zongJia, '\n');
		strcpy_s(linShi, "赠品");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->zengPinXinXi, '\n');
		strcpy_s(linShi, "客户信息");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->keHuXinXi , '\n');
		strcpy_s(linShi, "时间");
		fprintf_s(fp, "%s%c%d%s%d%s%d%s%d%s%c%c%c", linShi, ':', p->piFaShiJian.yue, "月", p->piFaShiJian.ri, "日", p->piFaShiJian.shi, "时", p->piFaShiJian.fen, "分", '\n', '#', '\n');
		p = p->biaoJi;
	}
	fclose(fp);
	return 0;
}
bool baoCunLingShouJiLu_(char* fileName) {
	FILE* fp;
	char linShi[40];
	fopen_s(&fp, fileName, "w");
	if (fp == NULL) {
		printf(" \n-!-批发导出文件创建失败，请检查上一个程序窗口是否完全关闭，清除缓存后再试一遍，注意数据仍未保存！/1545\n");
		return false;
	}
	struct  lingShouJiLu_* p;
	p = lingShouTou;
	while (p != NULL) {
		strcpy_s(linShi, "配件名称");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->peiJianMingCheng, '\n');
		strcpy_s(linShi, "配件型号");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->xingHao, '\n');
		strcpy_s(linShi, "制造商");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->zhiZaoShang, '\n');
		strcpy_s(linShi, "单价");
		fprintf_s(fp, "%s%c%.2f%c", linShi, ':', p->danJia, '\n');
		strcpy_s(linShi, "数量");
		fprintf_s(fp, "%s%c%d%c", linShi, ':', p->shuLiang, '\n');
		strcpy_s(linShi, "总价");
		fprintf_s(fp, "%s%c%.2f%c", linShi, ':', p->zongJia, '\n');
		strcpy_s(linShi, "客户信息");
		fprintf_s(fp, "%s%c%s%c", linShi, ':', p->keHuXinXi, '\n');
		strcpy_s(linShi, "时间");
		fprintf_s(fp, "%s%c%d%s%d%s%d%s%d%s%c%c%c", linShi, ':', p->lingShouShiJian.yue, "月", p->lingShouShiJian.ri, "日", p->lingShouShiJian.shi, "时", p->lingShouShiJian.fen, "分", '\n', '#', '\n');
		p = p->biaoJi;
	}
	fclose(fp);
	return 0;
}


//查询，完全整合
//类型个位数为查询类型，十位数为数据类型，百位数为项目名字
//如果修改为true，则会保存搜索结果
int chaXunXinXi(char* guanJianCi,int leiXin ,bool xiugai) {
	switch (leiXin%10)
	{
		//case 1为暴力查询
	case 1: {
		//这里判断数据类型
		switch ((leiXin%100)/10)
		{
			//case 1为进货数据
		case 1: {
			switch (leiXin/100)
			{
				//case 1为配件名称
			case 1: {
				if (chaXun_baoLi_jinHuo(guanJianCi, 1, xiugai) == NULL)
					return false;
				break;
			}
				//case 2为供货商
			case 2:{
				if (chaXun_baoLi_jinHuo(guanJianCi, 2, xiugai) == NULL)
					return false;
				break;
			}
			default:
				break;
			}
			break;
		}
			//case 2为零售数据
		case 2: {
			switch (leiXin / 100)
			{
				//case 1为配件名称
			case 1: {
				if(chaXun_baoLi_lingShou(guanJianCi, 1, xiugai) == NULL)
					return false;
				break;
			}
				//case 2为客户信息
			case 2: {
				if(chaXun_baoLi_lingShou(guanJianCi, 2, xiugai) == NULL)
					return false;
				break;
			}
			default:
				break;
			}
			break;
		}
			//case 2为零售数据
		case 3: {
			switch (leiXin / 100)
			{
				//case 1为配件名称
			case 1: {
				if(chaXun_baoLi_piFa(guanJianCi, 1, xiugai) == NULL)
					return false;
				break;
			}
				//case 2为客户信息
			case 2: {
				if(chaXun_baoLi_piFa(guanJianCi, 2, xiugai) == NULL)
					return false;
				break;
			}
			default:
				break;
			}
			break;
		}
		default:printf(" -!-内部错误，请联系开发者。");
			break;
		}
		return true;
	}
		//case 2为(伪)模糊查询
	case 2: {
		//这里判断数据类型
		switch ((leiXin%100)/10)
		{
			//case 1为进货记录
		case 1:
		default:
			break;
		}
	}
	default:
		break;
	}
}
//查询进货数据，1-名字 2-供货商
//如果xiugai修改为true则进行修改
int* chaXun_baoLi_jinHuo(char* guanJianCi, int leiXin, bool xiugai)
{
	jinHuoJiLu_* p;
	p = jinHuoTou;
	int i = 0;
	bool aa = false;//用来判断是否查到
	for (i = 0; i < 99; i++) jiLuXiuGaiBianHao[i] = -1;//初始化记录修改项目的标记
	switch (leiXin)
	{
	case 1: {
		while (p != NULL)
		{
			if (strcmp(p->peiJianMingCheng, guanJianCi) == 0) {
				if (xiugai)
				{
					jiLuXiuGaiBianHao[i] = p->bianHao;
					i++;
				}
				daYin(p, NULL, NULL);
				p = p->biaoJi;
				aa = true;
			}
			else p = p->biaoJi;
		}
		dingWeiDian = 17;
		printf("\n\n");
		system("pause");
		break;
	}
	case 2: {
		while (p != NULL) {
			if (strcmp(p->gongHuoShang, guanJianCi) == 0) {
				if (xiugai)
				{
					jiLuXiuGaiBianHao[i] = p->bianHao;
					i++;
				}
				daYin(p, NULL, NULL);
				p = p->biaoJi;
				aa = true;
			}
			else p = p->biaoJi;
		}
		dingWeiDian = 17;
		printf("\n\n");
		system("pause");
		break;
	}
	default:printf(" -!-内部错误，请联系开发者。");
		break;
	}
	if (!aa) {
		printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                         "); printf("│                   \033[40;31m-!-没有查到任何数据\033[0m                   │");
		printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		system("cls");
		return NULL;
	}
	return jiLuXiuGaiBianHao;
}
//查询零售数据，1-名字 2-客户
//如果xiugai修改为true则进行修改
int* chaXun_baoLi_lingShou(char* guanJianCi, int leiXin, bool xiugai) {
	lingShouJiLu_* p;
	p = lingShouTou;
	int i = 0;
	bool aa = false;//用来判断是否查到
	for (i = 0; i < 99; i++) jiLuXiuGaiBianHao[i] = -1;//初始化记录修改项目的标记
	switch (leiXin)
	{
	case 1: {
		while (p != NULL) {
			if (strcmp(p->peiJianMingCheng, guanJianCi) == 0) {
				if (xiugai)
				{
					jiLuXiuGaiBianHao[i] = p->bianHao;
					i++;
				}
				daYin(NULL, p, NULL);
				p = p->biaoJi;
				aa = true;
			}
			else p = p->biaoJi;
		}
		dingWeiDian = 17;
		printf("\n\n");
		system("pause");
		break;
	}
	case 2: {
		while (p != NULL) {
			if (strcmp(p->keHuXinXi, guanJianCi) == 0) {
				if (xiugai)
				{
					jiLuXiuGaiBianHao[i] = p->bianHao;
					i++;
				}
				daYin(NULL, p, NULL);
				p = p->biaoJi;
				aa = true;
			}
			else p = p->biaoJi;
		}
		dingWeiDian = 17;
		printf("\n\n");
		system("pause");
		break;
	}
	default:printf(" -!-内部错误，请联系开发者。");
		break;
	}
	if (!aa) {
		printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                         "); printf("│                   \033[40;31m-!-没有查到任何数据\033[0m                   │");
		printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		system("cls");
		return NULL;
	}
	return jiLuXiuGaiBianHao;
}
//查询批发数据，1-名字 2-客户
//如果xiugai修改为true则进行修改
int* chaXun_baoLi_piFa(char* guanJianCi, int leiXin, bool xiugai) {
	piFaJiLu_* p;
	p = piFaTou;
	int i = 0;
	bool aa = false;//用来判断是否查到
	for (i = 0; i < 99; i++) jiLuXiuGaiBianHao[i] = -1;//初始化记录修改项目的标记
	switch (leiXin)
	{
	case 1: {
		while (p != NULL) {
			if (strcmp(p->peiJianMingCheng, guanJianCi) == 0) {
				if (xiugai)
				{
					jiLuXiuGaiBianHao[i] = p->bianHao;
					i++;
				}
				daYin(NULL, NULL, p);
				p = p->biaoJi;
				aa = true;
			}
			else p = p->biaoJi;
		}
		dingWeiDian = 17;
		printf("\n\n");
		system("pause");
		break;
	}
	case 2: {
		while (p != NULL) {
			if (strcmp(p->keHuXinXi, guanJianCi) == 0) {
				if (xiugai)
				{
					jiLuXiuGaiBianHao[i] = p->bianHao;
					i++;
				}
				daYin(NULL, NULL, p);
				p = p->biaoJi;
				aa = true;
			}
			else p = p->biaoJi;
		}
		dingWeiDian = 17;
		printf("\n\n");
		system("pause");
		break;
	}
	default:printf(" -!-内部错误，请联系开发者。");
		break;
	}
	if (!aa) {
		printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                         "); printf("│                   \033[40;31m-!-没有查到任何数据\033[0m                   │");
		printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		system("cls");
		return NULL;
	}
	return jiLuXiuGaiBianHao;
}


//模糊查询进货数据
int chaXu_moHu_jinHuo(char* guanJianCi) {
	jinHuoJiLu_* p;
	p = jinHuoTou;
	int i = 0;//可以统计目标字长
	int n = 0;//可以统计关键词字长
	float piPeiDu[4] = { 0,0,0,0 };//匹配度
	bool aa = false, bb = false;
	dingWeiDian = 3;
	while (p != NULL)
	{
			//判断名字
		for (i = 0; i < 20; i++)
		{
			if (p->peiJianMingCheng[i] == '\0')
			{
				break;
			}
			else 
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->peiJianMingCheng[i] == guanJianCi[n])
						{
							piPeiDu[0]++;
						}
					}
				}
			}
		}
		piPeiDu[0] = (2 * piPeiDu[0]) / ((float)i + (float)n);
			//判断型号
		for (i = 0; i < 20; i++)
		{
			if (p->xingHao[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->xingHao[i] == guanJianCi[n])
						{
							piPeiDu[1]++;
						}
					}
				}
			}
		}
		piPeiDu[1] = (2 * piPeiDu[1]) / ((float)i + (float)n);
			//判断供货商
		for (i = 0; i < 20; i++)
		{
			if (p->gongHuoShang[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->gongHuoShang[i] == guanJianCi[n])
						{
							piPeiDu[2]++;
						}
					}
				}
			}
		}
		piPeiDu[2] = (2 * piPeiDu[2]) / ((float)i + (float)n);
			//判断制造商
		for (i = 0; i < 20; i++)
		{
			if (p->zhiZaoShang[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->zhiZaoShang[i] == guanJianCi[n])
						{
							piPeiDu[3]++;
						}
					}
				}
			}
		}
		piPeiDu[3] = (2 * piPeiDu[3]) / ((float)i + (float)n);
			//判断匹配度是否达标
		for (int m = 0; m < 4; m++) {
			if (piPeiDu[m] >= pipeidu) {
				aa = true;
			}
		}
			//打印
		if (aa) {
			bb = true;
			daYin(p, NULL, NULL);
			dingWei(0, dingWeiDian-3);
		}
		i = 0; n = 0; aa = false;
			//匹配度置零
		for (int m = 0; m < 4; m++) {
			piPeiDu[m] = 0;
		}
		p = p->biaoJi;
	}
	dingWeiDian = 17;
		//没有搜寻到任何数据
	if (!bb) {
		printf("\n\n\n                               -!-仍未搜寻到相关信息。");
		return false;
	}
	return 0;
}
//模糊查询零售数据
int chaXu_moHu_lingShou(char* guanJianCi) {
	lingShouJiLu_* p;
	p = lingShouTou;
	int i = 0;//可以统计目标字长
	int n = 0;//可以统计关键词字长
	float piPeiDu[4] = { 0,0,0,0 };//匹配度
	bool aa = false, bb = false;
	while (p != NULL)
	{
		//判断名字
		for (i = 0; i < 20; i++)
		{
			if (p->peiJianMingCheng[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->peiJianMingCheng[i] == guanJianCi[n])
						{
							piPeiDu[0]++;
						}
					}
				}
			}
		}
		piPeiDu[0] = (2 * piPeiDu[0]) / ((float)i + (float)n);
		//判断型号
		for (i = 0; i < 20; i++)
		{
			if (p->xingHao[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->xingHao[i] == guanJianCi[n])
						{
							piPeiDu[1]++;
						}
					}
				}
			}
		}
		piPeiDu[1] = (2 * piPeiDu[1]) / ((float)i + (float)n);
		//判断客户信息
		for (i = 0; i < 20; i++)
		{
			if (p->keHuXinXi  [i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->keHuXinXi[i] == guanJianCi[n])
						{
							piPeiDu[2]++;
						}
					}
				}
			}
		}
		piPeiDu[2] = (2 * piPeiDu[2]) / ((float)i + (float)n);
		//判断制造商
		for (i = 0; i < 20; i++)
		{
			if (p->zhiZaoShang[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->zhiZaoShang[i] == guanJianCi[n])
						{
							piPeiDu[3]++;
						}
					}
				}
			}
		}
		piPeiDu[3] = (2 * piPeiDu[3]) / ((float)i + (float)n);
		//判断匹配度是否达标
		for (int m = 0; m < 4; m++) {
			if (piPeiDu[m] >= pipeidu) {
				aa = true;
			}
		}
		//打印
		if (aa) {
			bb = true;
			daYin(NULL, p, NULL);
			dingWei(0, dingWeiDian - 3);
		}
		i = 0; n = 0; aa = false;
		//匹配度置零
		for (int m = 0; m < 4; m++) {
			piPeiDu[m] = 0;
		}
		p = p->biaoJi;
	}
	dingWeiDian = 17;
	//没有搜寻到任何数据
	if (!bb) {
		printf(" -!-仍未搜寻到相关信息。");
		return false;
	}
	
	return 0;
}
//模糊查询批发数据
int chaXu_moHu_piFa(char* guanJianCi) {
	piFaJiLu_* p;
	p = piFaTou;
	int i = 0;//可以统计目标字长
	int n = 0;//可以统计关键词字长
	float piPeiDu[4] = { 0,0,0,0 };//匹配度
	bool aa = false, bb = false;
	while (p != NULL)
	{
		//判断名字
		for (i = 0; i < 20; i++)
		{
			if (p->peiJianMingCheng[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->peiJianMingCheng[i] == guanJianCi[n])
						{
							piPeiDu[0]++;
						}
					}
				}
			}
		}
		piPeiDu[0] = (2 * piPeiDu[0]) / ((float)i + (float)n);
		//判断型号
		for (i = 0; i < 20; i++)
		{
			if (p->xingHao[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->xingHao[i] == guanJianCi[n])
						{
							piPeiDu[1]++;
						}
					}
				}
			}
		}
		piPeiDu[1] = (2 * piPeiDu[1]) / ((float)i + (float)n);
		//判断客户信息
		for (i = 0; i < 20; i++)
		{
			if (p->keHuXinXi[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->keHuXinXi[i] == guanJianCi[n])
						{
							piPeiDu[2]++;
						}
					}
				}
			}
		}
		piPeiDu[2] = (2 * piPeiDu[2]) / ((float)i + (float)n);
		//判断制造商
		for (i = 0; i < 20; i++)
		{
			if (p->zhiZaoShang[i] == '\0')
			{
				break;
			}
			else
			{
				for (n = 0; n < 20; n++)
				{
					if (guanJianCi[n] == '\0')
					{
						break;
					}
					else {
						if (p->zhiZaoShang[i] == guanJianCi[n])
						{
							piPeiDu[3]++;
						}
					}
				}
			}
		}
		piPeiDu[3] = (2 * piPeiDu[3]) / ((float)i + (float)n);
		//判断匹配度是否达标
		for (int m = 0; m < 4; m++) {
			if (piPeiDu[m] >= pipeidu) {
				aa = true;
			}
		}
		//打印
		if (aa) {
			bb = true;
			daYin(NULL, NULL, p);
			dingWei(0, dingWeiDian - 3);
		}
		i = 0; n = 0; aa = false;
		//匹配度置零
		for (int m = 0; m < 4; m++) {
			piPeiDu[m] = 0;
		}
		p = p->biaoJi;
	}
	dingWeiDian = 17;
	//没有搜寻到任何数据
	if (!bb) {
		printf(" -!-仍未搜寻到相关信息。");
		return false;
	}
	return 0;
}
//查询界面
char* chaXun_jieMian() {
	char guanJianCi[20];
	printf("\n\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m查询数据\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >请输入关键词：                                             │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(43, 7);
	gets_s(guanJianCi);
	dingWei(0, 12);
	strcpy_s(guanJianCi_, guanJianCi);
	return guanJianCi;
}
//搜索界面
char* souSuo_jieMian() {
	char guanJianCi[20];
	printf("\n\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m搜索数据\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >请输入关键词：                                             │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         返回上一级菜单-0                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(43, 7);
	gets_s(guanJianCi);
	dingWei(0, 14);
	strcpy_s(guanJianCi_, guanJianCi);
	return guanJianCi;
}


//加密文件
bool jiaMiWenJian(char* muBiaoWenJian, char* filename, char* miMa) {
	FILE* fp, * fpp;
	int i;
	fopen_s(&fp, muBiaoWenJian, "r");
	if (fp == NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                            \033[40;31m文件打开失败\033[0m                             │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		return false;
	}
	fopen_s(&fpp, filename, "w");
	if (fpp == NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                            \033[40;31m文件打开失败\033[0m                             │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		return false;
	}
	char a = '0';
	for (i = 0; i < 17; i++) {
		if (miMa[i] == '\0')
			break;
	}
	if (fp != NULL && fpp != NULL)
	{
		while (!feof(fp))
		{
			for (int n = 0; n < i; n++) {
				fread_s(&a, sizeof(char), sizeof(char), 1, fp);
				a += miMa[n];
				fwrite(&a, sizeof(char), 1, fpp);
			}
		}
	}
	fclose(fp); fclose(fpp);
	return true;
}
//解密文件
bool jieMiWenJian(char* muBiaoWenJian, char* filename, char* miMa) {
	FILE* fp, * fpp;
	int i;
	fopen_s(&fp, muBiaoWenJian, "r");
	if (fp == NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                            \033[40;31m文件打开失败\033[0m                             │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		return false;
	}
	fopen_s(&fpp, filename, "w");
	if (fpp == NULL) {
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                            \033[40;31m文件打开失败\033[0m                             │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		return false;
	}
	char a = '0';
	for (i = 0; i < 17; i++) {
		if (miMa[i] == '\0')
			break;
	}
	if (fp != NULL && fpp != NULL)
	{
		while (!feof(fp))
		{
			for (int n = 0; n < i; n++) {
				fread_s(&a, sizeof(char), sizeof(char), 1, fp);
				a -= miMa[n];
				fwrite(&a, sizeof(char), 1, fpp);
			}
		}
	}
	fclose(fp); fclose(fpp);
	return true;
}


/*菜单*********************************************************************************************/

//主菜单
char* zhuCaiDan() {
	char a[10];
	system("cls");
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                               *\033[40;31m主菜单\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         记录数据-1           修改数据-2           删除数据-3        │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         统计数据-4           查询数据-5           文件操作-6        │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         打印数据-7           附加功能-8           \033[40;33m退出系统-0\033[0m        │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 16);
	gets_s(a);
	dingWei(0, 20);
	return a;
}
//分菜单：添加记录
char* fenCaiDan_jiLuShuJu() {
	system("cls");
	char a[10];
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m记录数据\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >进货记录-1                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >批发记录-2                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >零售记录-3                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 12);
	gets_s(a);
	dingWei(0, 16);
	return a;
}
//分菜单：修改记录
char* fenCaiDan_xiuGaiShuJu() {
	system("cls");
	char a[10];
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m修改数据\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >进货记录-1                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >批发记录-2                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >零售记录-3                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 12);
	gets_s(a);
	dingWei(0, 16);
	return a;
}
//分菜单：查询记录
char* fenCaiDan_chaXunJiLu() {
	system("cls");
	char a[10];
	char* b = NULL;//用于返回
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m搜索数据\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >进货记录-1                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >零售记录-2                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >批发记录-3                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 12);
	gets_s(a);
	dingWei(0, 16);
		//输入不符合规范
	while (a[1] != '\0') {
		printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
		printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		system("cls");
		return fenCaiDan_chaXunJiLu();
	}
	switch (a[0]-'0')
	{
	case 0:
	{
		break;
	}
		//进货数据
	case 1:
	{
		b = fenCaiDan_chaXunJiLu_jinHuo();
		break;
	}
		//零售数据
	case 2:
	{
		b = fenCaiDan_chaXunJiLu_lingShou();
		break;
	}
		//批发数据
	case 3:
	{
		b = fenCaiDan_chaXunJiLu_piFa();
		break;
	}
	default:
	{
		printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
		printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
		printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
		Sleep(1000);
		system("cls");
	}
		break;
	}
	for (int i = 0; i < 10; i++) {
		if (b == NULL) break;
		a[i] = b[i];
	}
	return a;
}
	//分分菜单：查询->进货
	char* fenCaiDan_chaXunJiLu_jinHuo()
	{
		char a[10];
		system("cls");
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m进货记录\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│          根据下面哪项进行搜索                                       │"); Sleep(20);
		printf("\n                   "); printf("│         >配件名字-11                                                │"); Sleep(20);
		printf("\n                   "); printf("│         >供货商-12                                                  │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 11);
		gets_s(a);
		dingWei(0, 15);
		//输入不符合规范
		while (a[2] != '\0') {
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
			system("cls");
			return fenCaiDan_chaXunJiLu_jinHuo();
		}
		return a;
	}
	//分分菜单：查询->零售
	char* fenCaiDan_chaXunJiLu_lingShou()
	{
		char a[10];
		system("cls");
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m零售记录\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│          根据下面哪项进行搜索                                       │"); Sleep(20);
		printf("\n                   "); printf("│         >配件名字-21                                                │"); Sleep(20);
		printf("\n                   "); printf("│         >客户信息-22                                                │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 11);
		gets_s(a);
		dingWei(0, 15);
		//输入不符合规范
		while (a[2] != '\0') {
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
			system("cls");
			return fenCaiDan_chaXunJiLu_lingShou();
		}
		return a;
	}
	//分分菜单：查询->批发
	char* fenCaiDan_chaXunJiLu_piFa()
	{
		char a[10];
		system("cls");
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m批发记录\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│          根据下面哪项进行搜索                                       │"); Sleep(20);
		printf("\n                   "); printf("│         >配件名字-31                                                │"); Sleep(20);
		printf("\n                   "); printf("│         >客户信息-32                                                │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 11);
		gets_s(a);
		dingWei(0, 15);
		//输入不符合规范
		while (a[2] != '\0') {
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
			system("cls");
			return fenCaiDan_chaXunJiLu_piFa();
		}
		return a;
	}
//分菜单：文件操作
char* fenCaiDan_wenJianCaoZuo(){
	system("cls");
	char a[10];
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m文件操作\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >从文件导入进货记录-1                                       │"); Sleep(20);
	printf("\n                   "); printf("│         >从文件导入批发记录-2                                       │"); Sleep(20);
	printf("\n                   "); printf("│         >从文件导入零售记录-3                                       │"); Sleep(20);
	printf("\n                   "); printf("│         >将进货记录写入文件-4                                       │"); Sleep(20);
	printf("\n                   "); printf("│         >将零售记录写入文件-5                                       │"); Sleep(20);
	printf("\n                   "); printf("│         >将批发记录写入文件-6                                       │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 15);
	gets_s(a);
	dingWei(0, 19);
	return a;
}
//分菜单：打印数据
char* fenCaiDan_daYinShuJu() {
	system("cls");
	char a[10];
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m打印数据\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >全部打印-1                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >根据时间打印-2                                             │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 11);
	gets_s(a);
	dingWei(0, 15);
	return a;
}
//分菜单：其它功能
char* fenCaiDan_fuJiaGongNeng() {
	system("cls");
	char a[10];
	printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│                             *\033[40;31m附加功能\033[0m*                              │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         >模糊查询-1                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >文件加密-2                                                 │"); Sleep(20);
	printf("\n                   "); printf("│         >文件解密-3                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
	printf("\n                   "); printf("│                                                                     │"); Sleep(20);
	printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
	dingWei(40, 12);
	gets_s(a);
	dingWei(0, 16);
	return a;
}
	//分分菜单：模糊查询
	char* fenCaiDan_fuJiaGongNeng_moHuChaXun() {
		system("cls");
		char a[10];
		char guanJianCi[20];
		char* b = NULL;
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m搜索数据\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >进货记录-1                                                 │"); Sleep(20);
		printf("\n                   "); printf("│         >零售记录-2                                                 │"); Sleep(20);
		printf("\n                   "); printf("│         >批发记录-3                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         \033[40;33m返回上一级菜单-0\033[0m                                            │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         你的选择是>                                                 │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 12);
		gets_s(a);
		dingWei(0, 16);
		//输入不符合规范
		while (a[1] != '\0') {
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
			system("cls");
			return fenCaiDan_chaXunJiLu();
		}
		system("cls");
		if (a[0] == '0')
			return a;
		chaXun_jieMian();
		switch (a[0] - '0')
		{
		case 0:
		{
			break;
		}
		for (int n = 0; n < 20; n++) {
			guanJianCi[n] = b[n];
		}
		//进货数据
		case 1:
		{
			system("cls");
			chaXu_moHu_jinHuo(guanJianCi_);
			system("pause");
			break;
		}
		//零售数据
		case 2:
		{
			system("cls");
			chaXu_moHu_lingShou(guanJianCi_);
			system("pause");
			break;
		}
		//批发数据
		case 3:
		{
			system("cls");
			chaXu_moHu_piFa(guanJianCi_);
			system("pause");
			break;
		}
		default:
		{
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
			system("cls");
		}
		break;
		}
		for (int i = 0; i < 10; i++)
		{
			if (b == NULL) break;
			a[i] = b[i];
		}
		return a;
	}
	//分分菜单：加密文件
	char* fenCaiDan_fuJiaGongNeng_jiaMiWenJian() {
		system("cls");
		char filename_1[20], filename_2[20];
		char keyword[17], c;
		int i = 0;
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m加密文件\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >输入文件：                                                 │"); Sleep(20);
		printf("\n                   "); printf("│         >输出文件：                                                 │"); Sleep(20);
		printf("\n                   "); printf("│         >密匙：                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 6);
		gets_s(filename_1);
		dingWei(40, 7);
		gets_s(filename_2);
		dingWei(35, 8);
		while ((c = _getch()) != '\r')
		{
			keyword[i] = c;
			printf("*");
			i++;
			if (i >= 16)
			{
				break;
			}
		}
		dingWei(0, 14);
		if (jiaMiWenJian(filename_1, filename_2, keyword)) {
			printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
			printf("\n                   "); printf("│                                                                     │"); Sleep(20);
			printf("\n                   "); printf("│                             加密成功                                │"); Sleep(20);
			printf("\n                   "); printf("│                                                                     │"); Sleep(20);
			printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		}
		Sleep(1000);
		return 0;
	}
	//分分菜单：解密文件
	char* fenCaiDan_fuJiaGongNeng_jieMiWenJian() {
		system("cls");
		char filename_1[20], filename_2[20];
		char keyword[17], c;
		int i = 0;
		printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                             *\033[40;31m解密文件\033[0m*                              │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("│         >输入文件：                                                 │"); Sleep(20);
		printf("\n                   "); printf("│         >输出文件：                                                 │"); Sleep(20);
		printf("\n                   "); printf("│         >密匙：                                                     │"); Sleep(20);
		printf("\n                   "); printf("│                                                                     │"); Sleep(20);
		printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		dingWei(40, 6);
		gets_s(filename_1);
		dingWei(40, 7);
		gets_s(filename_2);
		dingWei(35, 8);
		while ((c = _getch()) != '\r')
		{
			keyword[i] = c;
			printf("*");
			i++;
			if (i >= 16)
			{
				break;
			}
		}
		dingWei(0, 14);
		if (jieMiWenJian(filename_1, filename_2, keyword)) {
			printf("\n\n                   "); printf("╭ ─────────────────────────────────────────────────────────────────── ╮"); Sleep(20);
			printf("\n                   "); printf("│                                                                     │"); Sleep(20);
			printf("\n                   "); printf("│                             解密成功                                │"); Sleep(20);
			printf("\n                   "); printf("│                                                                     │"); Sleep(20);
			printf("\n                   "); printf("╰ ─────────────────────────────────────────────────────────────────── ╯"); Sleep(20);
		}
		Sleep(1000);
		return 0;
	}


/*主函数*************************************************************************/
//主函数
int main() {
	char* xuanZe;//记录用户的当前选择，会被重复使用
	bool xunHuanPanDuan = true;//用来判断是否还要进行循环
	char* guanJianCi = NULL;//查询用的关键词
	int xiuGaiBiaoJi;//用于保存修改
	char filename[20];//保存文件的文件名
	while (xunHuanPanDuan)
	{
		xuanZe = zhuCaiDan();
		//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
		while (xuanZe[1] != '\0') {
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
			system("cls");
			xuanZe = zhuCaiDan();
		}
		//总循环
		switch (xuanZe[0] - '0')
		{
		case 0://退出系统*
		{
			system("cls");
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮"); Sleep(20);
			printf("\n                         "); printf("│                        \033[40;31m感谢使用~\033[0m                        │"); Sleep(20);
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n"); Sleep(20);
			Sleep(1000);
			return 0;
		}
		case 1://记录数据，新增数据*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_jiLuShuJu();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[1] != '\0') {
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_jiLuShuJu();
				}
				//记录数据，分流的实际操作
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级菜单
				{
					xunHuanPanDuan = false;
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-正在返回\033[0m                       │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break; 
				}
				case 1://添加进货数据
				{
					system("cls");
					lianJie_jinHuo(jiLuJinHuoJiLu());
					xunHuanPanDuan = false;
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-数据保存成功\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 2://添加批发数据
				{
					system("cls");
					lianJie_piFa(jiLuPiFaJiLu());
					xunHuanPanDuan = false;
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-数据保存成功\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 3://添加零售数据
				{
					system("cls");
					lianJie_lingShou(jiLuLingShouJiLu());
					xunHuanPanDuan = false;
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-数据保存成功\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				default:
				{
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
				}
				break;
				}
			}
			xunHuanPanDuan = true;
			break;
		}
		case 2://修改数据*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_chaXunJiLu();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[2] != '\0') {
					if (xuanZe[0] == '0') {
						xunHuanPanDuan = false;
						break;
					}
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_chaXunJiLu();
				}
				//记录数据，分流的实际操作
					//数据的查询
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级
				{
					xunHuanPanDuan = false;
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-正在返回\033[0m                       │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 1://进货记录
				{
					//数据类型的选择
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 111, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要修改的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							xiuGaiChaXun(xiuGaiBiaoJi, 1);
						}
						//							getchar();
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 211, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要修改的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							xiuGaiChaXun(xiuGaiBiaoJi, 1);
						}
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				case 2://零售记录
				{
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 121, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要修改的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							xiuGaiChaXun(xiuGaiBiaoJi, 2);
						}
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 221, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要修改的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							xiuGaiChaXun(xiuGaiBiaoJi, 2);
						}
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				case 3://批发记录
				{
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 131, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要修改的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							xiuGaiChaXun(xiuGaiBiaoJi, 3);
						}
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 231, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要修改的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							xiuGaiChaXun(xiuGaiBiaoJi, 3);
						}
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				default:
				{
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
				}
					break;
				}
			}
			xunHuanPanDuan = true;
			break;
		}
		case 3://删除数据*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_chaXunJiLu();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[2] != '\0') {
				if (xuanZe[0] == '0') {
						xunHuanPanDuan = false;
						break;
					}
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_chaXunJiLu();
				}
				//删除数据，分流的实际操作
					//数据的查询
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级
				{
					xunHuanPanDuan = false;
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-正在返回\033[0m                       │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 1://进货记录
				{
					//数据类型的选择
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 111, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要删除的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							if (shanChuShuJu(1, xiuGaiBiaoJi)) {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-删除成功\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
							else {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-出现错误\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
						}
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 211, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要删除的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							if (shanChuShuJu(1, xiuGaiBiaoJi)) {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-删除成功\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
							else {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-出现错误\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
						}
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				case 2://零售记录
				{
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 121, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要删除的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							if (shanChuShuJu(3, xiuGaiBiaoJi)) {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-删除成功\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
							else {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-出现错误\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
						}
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 221, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要删除的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							if (shanChuShuJu(3, xiuGaiBiaoJi)) {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-删除成功\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
							else {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-出现错误\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
						}
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				case 3://批发记录
				{
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 131, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要删除的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							if (shanChuShuJu(2, xiuGaiBiaoJi)) {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-删除成功\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
							else {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-出现错误\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
						}
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						souSuo_jieMian();
						if (chaXunXinXi(guanJianCi_, 231, false)) {
							xunHuanPanDuan = false;
							printf("\n                          请输入要删除的项目编号：");
							scanf_s("%d", &xiuGaiBiaoJi);
							getchar();
							if (shanChuShuJu(2, xiuGaiBiaoJi)) {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-删除成功\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
							else {
								printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
								printf("\n                         "); printf("│                       \033[40;31m-!-出现错误\033[0m                       │");
								printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
								Sleep(1000);
							}
						}
						break;
						}
					default:
				{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				default:
				{
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
				}
					break;
				}
			}
		xunHuanPanDuan = true;
			break;
		}
		case 4://统计数据*
		{
			tongJiShuJu(true);
			system("pause");
			break;
		}
		case 5://查询数据*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_chaXunJiLu();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[2] != '\0') {
					if (xuanZe[0] == '0') {
						xunHuanPanDuan = false;
						break;
					}
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_chaXunJiLu();
				}
				//记录数据，分流的实际操作
					//数据的查询
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级
					break;
				case 1://进货记录
				{
					//数据类型的选择
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						chaXun_jieMian();
						chaXunXinXi(guanJianCi_, 111, true);
						xunHuanPanDuan = false;
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						chaXun_jieMian();
						chaXunXinXi(guanJianCi_, 211, true);
						xunHuanPanDuan = false;
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				case 2://零售记录
				{
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						chaXun_jieMian();
						chaXunXinXi(guanJianCi_, 121, true);
						xunHuanPanDuan = false;
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						chaXun_jieMian();
						chaXunXinXi(guanJianCi_, 221, true);
						xunHuanPanDuan = false;
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
				case 3://零售记录
				{
					switch (xuanZe[1] - '0')
					{
						//case 1为名字
					case 1:
					{
						system("cls");
						chaXun_jieMian();
						chaXunXinXi(guanJianCi_, 131, true);
						xunHuanPanDuan = false;
						break;
					}
					//case 2为供货商
					case 2:
					{
						system("cls");
						chaXun_jieMian();
						chaXunXinXi(guanJianCi_, 231, true);
						xunHuanPanDuan = false;
						break;
					}
					default:
					{
						printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
						printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
						printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
						Sleep(1000);
						system("cls");
					}
					break;
					}
					break;
				}
					default://///////////////////////////////////////////////////////////////////////////////////////////
						break;
				}
			}
			xunHuanPanDuan = true;
			break;
		}
		case 6://文件操作*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_wenJianCaoZuo();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[1] != '\0') {
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_wenJianCaoZuo();
				}
				//记录数据，分流的实际操作
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级菜单
				{
					xunHuanPanDuan = false;
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-正在返回\033[0m                       │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 1://从文件导入进货数据
				{
//					system("cls");
					printf("\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-导入数据\033[0m                       │");
					printf("\n                         "); printf("│                                                         │");
					printf("\n                         "); printf("│        请输入文件名：                                   │");
					printf("\n                         "); printf("│                                                         │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					dingWei(48, 23);
					gets_s(filename);
					wenJianDaoRu(1, filename);
					xunHuanPanDuan = false;
					break;
				}
				case 2://从文件导入进货数据
				{
//					system("cls");
					printf("\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-导入数据\033[0m                       │");
					printf("\n                         "); printf("│                                                         │");
					printf("\n                         "); printf("│        请输入文件名：                                   │");
					printf("\n                         "); printf("│                                                         │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					dingWei(48, 23);
					gets_s(filename);
					wenJianDaoRu(2, filename);
					xunHuanPanDuan = false;
					break;
				}
				case 3://从文件导入进货数据
				{
//					system("cls");
					printf("\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-导入数据\033[0m                       │");
					printf("\n                         "); printf("│                                                         │");
					printf("\n                         "); printf("│        请输入文件名：                                   │");
					printf("\n                         "); printf("│                                                         │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					dingWei(48, 23);
					gets_s(filename);
					wenJianDaoRu(3, filename);
					xunHuanPanDuan = false;
					break;
				}
				case 4://保存进货记录到文件
				{
//					system("cls");
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│       请输入新建文件名：                                │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					dingWei(50, 24); gets_s(filename);
					baoCunJinHuoJiLu_(filename);
					xunHuanPanDuan = false;
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-数据保存成功\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 5://保存零售记录到文件
				{
//					system("cls");
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│       请输入新建文件名：                                │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					dingWei(50, 24); gets_s(filename);
					baoCunLingShouJiLu_(filename);
					xunHuanPanDuan = false;
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-数据保存成功\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 6://保存批发记录到文件
				{
//					system("cls");
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│       请输入新建文件名：                                │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					dingWei(50, 24); gets_s(filename);
					baoCunPiFaJiLu_(filename);
					xunHuanPanDuan = false;
					printf("\n\n\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-数据保存成功\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				default:
				{
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
				}
				break;
				}
			}
			xunHuanPanDuan = true;
			break;
		}
		case 7://打印数据*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_daYinShuJu();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[1] != '\0') {
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_daYinShuJu();
				}
				//记录数据，分流的实际操作
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级菜单
				{
					xunHuanPanDuan = false;
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-正在返回\033[0m                       │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 1://全部打印
				{
					quanBuDaYin();
					break;
				}
				case 2://按照时间打印
				{
					genJuShiJianDuanChaXun();
					break;
				}
				default:
				{
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
				}
				break;
				}
			}
			xunHuanPanDuan = true;
			break;
		}
		case 8://附加功能*
		{
			while (xunHuanPanDuan)
			{
				xuanZe = fenCaiDan_fuJiaGongNeng();
				//如果多输入了字符将会出现这个循环，直到输入一个字符（错误字符暂时没有判断）
				while (xuanZe[1] != '\0') {
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入格式错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
					xuanZe = fenCaiDan_fuJiaGongNeng();
				}
				//记录数据，分流的实际操作
				switch (xuanZe[0] - '0')
				{
				case 0://返回上一级菜单
				{
					xunHuanPanDuan = false;
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                       \033[40;31m-!-正在返回\033[0m                       │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					break;
				}
				case 1://模糊查询
				{
					system("cls");
					fenCaiDan_fuJiaGongNeng_moHuChaXun();
					break;
				}
				case 2://文件加密
				{
					system("cls");
					fenCaiDan_fuJiaGongNeng_jiaMiWenJian();
					break;
				}
				case 3://文件解密
				{
					system("cls");
					fenCaiDan_fuJiaGongNeng_jieMiWenJian();
					break;
				}
				default:
				{
					printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
					printf("\n                         "); printf("│                     \033[40;31m-!-输入字符错误\033[0m                     │");
					printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
					Sleep(1000);
					system("cls");
				}
				break;
				}
			}
			xunHuanPanDuan = true;
			break;
		}
		default: //错误输入*
		{
			system("cls");
			printf("\n\n                         "); printf("╭ ─────────────────────────────────────────────────────── ╮");
			printf("\n                         "); printf("│                       \033[40;31m-!-输入错误\033[0m                       │");
			printf("\n                         "); printf("╰ ─────────────────────────────────────────────────────── ╯\n");
			Sleep(1000);
		}
			break;
		}
		continue;
	}
	return 23333;
}