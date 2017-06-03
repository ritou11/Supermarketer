// ConsoleMarketer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "utils.h"
#include "User.h"

/*************************************************************
** 程序版本
** 标记版本信息，用于BANNER显示
*/
#define VERSION "V1.0"

/*************************************************************
** 数据文件
** 在实际应用的场合中，可以使用mongodb代替这种数据管理模式
** 此处为了避免mongodb部署问题，使用文件进行数据储存
*/
#define USER_FILE "user.json"
#define GOODS_FILE "goods.json"
#define BRANCH_FILE "branch.json"
#define CART_FILE "cart.json"
//************************************************************

/*************************************************************
** 内存中的数据存储
** 将文件中的数据读取到内存中，避免频繁的硬盘读写操作
** 适用于数据规模较小时
*/
json users;
json goods;
json carts;
json branchs;
//************************************************************

// 菜单函数返回的可能状态
enum STATUS {
    EXIT,
    LOOP
};

void printBanner();

bool doLogin(User& us);

STATUS menu1(User& us);

STATUS menuAdmin(User& us);
STATUS menuYHGL(User& us);
STATUS menuSPXXGL(User& us);
STATUS menuKCGL(User& us);
STATUS menuFDGL(User& us);
STATUS(*adminMenus[])(User&) = { menuYHGL,menuSPXXGL, menuKCGL,menuFDGL };

STATUS menuConsumer(User& us);
STATUS menuSC(User& us);
STATUS menuGUC(User& us);
STATUS menuQB(User& us);
STATUS(*consumerMenus[])(User&) = { menuSC, menuGUC, menuQB };

void listUsers();

void loadUsers(json& u);
void loadGoods(json& g);
void loadCart(json& c);
void loadBranch(json& b);

int main()
{
    int cmd = -1;
    while (1)
    {
        printBanner();

        loadUsers(users);
        loadGoods(goods);

        User us;
        while (1)
        {
            // 登录/注册选择
            STATUS s = menu1(us);
            if (s == EXIT) break;
        }
        if (us.type == ADMIN) {
            while (1) {
                // 管理员顶层菜单
                STATUS s = menuAdmin(us);
                if (s == EXIT) break;
            }
        }
        else {
            while (1) {
                // 消费者顶层菜单
                STATUS s = menuConsumer(us);
                if (s == EXIT) break;
            }
        }
        system("pause");
    }
    return 0;
}

void printBanner() {
    printf("****************************************************\n");
    printf("            欢迎光临笑蛤蛤大型连锁超市              \n");
    printf("                系统版本：%s                       \n", VERSION);
    printf("****************************************************\n");
}

bool doLogin(User& us) {
    cout << "-登陆" << endl;
    cout << "--用户名：";
    cin >> us.username;
    string password = getpass("--密码：");
    string sha512pwd = sha512(password);
    bool flag = false;
    for (int i = 0; i < users.size(); i++) {
        json juser = users[i];
        if (juser["username"].get<string>() == us.username) {
            if (juser["password"].get<string>() == sha512pwd) {
                flag = true;
                int auth = juser["auth"].get<int>();
                if (auth > 1) us.type = ADMIN;
                else us.type = CONSUMER;
                break;
            }
        }
    }
    return flag;
}

STATUS menu1(User& us) {
    int cmd = -1;
    printf("请选择菜单中的功能编号，按回车键确认：\n");
    printf("0: 登录\n");
    printf("1: 注册\n");

    cin >> cmd;

    if (cmd == 0) {
        if (doLogin(us)) {
            printf("登陆成功，你的身份是【%s】。\n", us.type == ADMIN ? "管理员" : "消费者");
            return EXIT;
        }
        else {
            printf("用户名或密码错误，登录失败！\n");
        }
    }
    else if (cmd == 1) {
        return EXIT;
    }
    else {
        printf("指令输入有误！\n");
    }
    return LOOP;
}

STATUS menuYHGL(User& us) {
    int cmd = -1;
    printf("----用户管理----\n");
    listUsers();
    printf("0: 返回\n");
    printf("1: 用户管理\n");
    printf("2: 商品信息管理\n");
    printf("3: 库存管理\n");
    printf("4: 分店管理\n");
}

STATUS menuSPXXGL(User& us) {
    return EXIT;
}

STATUS menuKCGL(User& us) {
    return EXIT;
}

STATUS menuFDGL(User& us) {
    return EXIT;
}

STATUS menuAdmin(User& us) {
    int cmd = -1;
    printf("欢迎管理员%s！请选择菜单中的功能编号，按回车键确认：\n", us.username);
    printf("0: 注销\n");
    printf("1: 用户管理\n");
    printf("2: 商品信息管理\n");
    printf("3: 库存管理\n");
    printf("4: 分店管理\n");

    cin >> cmd;

    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0://Logout
        return EXIT;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        nextMenu = adminMenus[cmd - 1];
        while (1) {
            // 下一级菜单
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("指令输入有误！\n");
    }
    return LOOP;
}

STATUS menuSC(User& us) {
    return EXIT;
}

STATUS menuGUC(User& us) {
    return EXIT;
}

STATUS menuQB(User& us) {
    return EXIT;
}

STATUS menuConsumer(User& us) {
    int cmd = -1;
    printf("欢迎顾客%s！请选择菜单中的功能编号，按回车键确认：\n", us.username);
    printf("0: 注销\n");
    printf("1: 市场\n");
    printf("2: 购物车\n");
    printf("3: 钱包\n");

    cin >> cmd;

    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0://Logout
        return EXIT;
        break;
    case 1:
    case 2:
    case 3:
        nextMenu = adminMenus[cmd - 1];
        while (1) {
            // 下一级菜单
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("指令输入有误！\n");
    }
    return LOOP;
}
void listUsers() {

}

void loadUsers(json& u) {
    std::ifstream fin(USER_FILE);
    fin >> u;
    fin.close();
}

void loadGoods(json& g) {

}