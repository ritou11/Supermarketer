// ConsoleMarketer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "utils.h"
#include "User.h"
#define VERSION "V1.0"
#define USER_FILE "user.json"

bool doLogin(User& us) {
    cout << "-登陆"<<endl;
    cout << "--用户名：";
    cin >> us.username;
    string password = getpass("--密码：");
    string sha512pwd = sha512(password);
    std::ifstream fin(USER_FILE);
    json js_user;
    cout << sha512pwd << endl;
    fin >> js_user;
    bool flag = false;
    for (int i = 0; i < js_user.size(); i++) {
        json juser = js_user[i];
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

enum STATUS {
    EXIT,
    LOOP
};

STATUS menu1(User& us) {
    int cmd = -1;
    printf("请选择菜单中的功能编号，按回车键确认：\n");
    printf("0: 登录\n");
    printf("1: 注册\n");
    scanf("%d", &cmd);
    fflush(stdin);
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

STATUS menu2(User& us) {
    int cmd = -1;
    printf("欢迎管理员%s！请选择菜单中的功能编号，按回车键确认：\n");
    printf("0: 注销\n");
    printf("1: 用户管理\n");
    printf("2: 商品信息管理\n");
    printf("3: 库存管理\n");
    printf("4: 分店管理\n");
    
    scanf("%d",&cmd);
    switch (cmd) {
    case 0:

        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        printf("指令输入有误！\n");
    }
    return LOOP;
}

STATUS menu3(User& us) {

    return LOOP;
}

int main()
{
    int cmd = -1;
    while (1)
    {
        printf("****************************************************\n");
        printf("            欢迎光临笑蛤蛤大型连锁超市              \n");
        printf("                系统版本：%s                       \n", VERSION);
        printf("****************************************************\n");
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
                STATUS s = menu2(us);
                if (s == EXIT) break;
            }
        }
        else {
            while (1) {
                // 消费者顶层菜单
                STATUS s = menu3(us);
                if (s == EXIT) break;
            }
        }
        system("pause");
    }
    return 0;
}

