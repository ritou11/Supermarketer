// ConsoleMarketer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "utils.h"
#include "User.h"

/*************************************************************
** 程序配置
** 声明宏以便于改变程序中的部分配置。
*/
#define VERSION "V1.0"
#define NEW_USER_COUPONS 10
//************************************************************

/*************************************************************
** 数据文件
** 在实际应用的场合中，可以使用mongodb代替这种数据管理模式
** 此处为了避免mongodb部署问题，使用文件进行数据储存
*/
#define USER_FILE "user.json"
#define GOODS_FILE "goods.json"
#define BRANCH_FILE "branch.json"
//************************************************************

/*************************************************************
** 内存中的数据存储
** 将文件中的数据读取到内存中，避免频繁的硬盘读写操作
** 适用于数据规模较小时
*/
json users;
vector<json> goods;
json branches;
//************************************************************

// 菜单函数返回的可能状态
enum STATUS {
    EXIT,
    LOOP
};

void printBanner();
void listUsers();
void listGoods();
void listBranches();

void loadUsers(json& u);
void loadGoods(vector<json>& g);
void loadBranches(json& b);

bool doLogin(User& us);
int getCmd();

void saveUsers(json& u);
void saveGoods(vector<json>& g);
void saveBranches(json& bs);

STATUS menu1(User& us);

STATUS menuAdmin(User& us);
STATUS menuYHGL(User& us);
STATUS menuSPXXGL(User& us);
STATUS menuKCGL(User& us);
STATUS menuFDGL(User& us);
STATUS(*adminMenus[])(User&) = { menuYHGL, menuSPXXGL, menuKCGL, menuFDGL };

STATUS menuConsumer(User& us);
STATUS menuSC(User& us);
STATUS menuGUC(User& us);
STATUS menuQB(User& us);
STATUS(*consumerMenus[])(User&) = { menuSC, menuGUC, menuQB };

// 主函数，程序入口点
int main()
{
    int cmd = -1;
    while (1)
    {
        printBanner();

        loadUsers(users);
        loadBranches(branches);
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
    cout << "-登陆-" << endl;
    cout << "--用户名：";
    cin >> us.username;
    us.username = convUTF8(us.username);
    string password = getpass("--密码：");
    //原则：密码不能明文储存，因此需要将SHA512加密后的密文与存储密文进行对比；
    //虽然本任务中未限制数据文件写入，使得系统并不安全。
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

bool checkPass(string pass) {
    if (pass.length() <= 6) return false;
    int upper = 0, lower = 0, num = 0, other = 0;
    for (int i = 0; i < pass.length(); i++) {
        if (pass[i] >= 'a'&&pass[i] <= 'z') lower++;
        else if (pass[i] >= 'A' && pass[i] <= 'Z') upper++;
        else if (pass[i] >= '0' && pass[i] <= '9') num++;
        else other++;
    }
    return (upper > 0) + (lower > 0) + (num > 0) + (other > 0) >= 3;
    // 四类字符至少包含三类
}

void doRegister() {
    cout << "-注册新用户-" << endl;
    User u;
    cout << "用户名：";
    cin >> u.username;
    u.username = convUTF8(u.username);
    u.type = CONSUMER;
    string pass;
    while (1) {
        pass = getpass("密码：", true);
        if (!checkPass(pass)) {
            cout << "密码太简单，请重新输入！" << endl;
        }
        else break;
    }
    string con_pass = getpass("再次确认密码：", true);
    if (con_pass != pass) {
        cout << "两次输入的密码不一致，注册失败！" << endl;
        return;
    }
    pass = sha512(pass);
    json user = {
        {"username",u.username},
        {"password",pass},
        {"auth", u.type == ADMIN ? 5 : 0},
        {"cart", json::array()},
        {"wallet",
        {
            {"money", 0},
            {"coupons", NEW_USER_COUPONS}
        }
        },
        {"purchase_log",json::array()}
    };
    users.push_back(user);
    saveUsers(users);
    cout << "用户【" << convGBK(u.username) << "】注册成功，请妥善保管您的密码。新用户赠送" << NEW_USER_COUPONS << "礼券，已经放在您的钱包当中，请查收。祝您购物愉快！"
        << endl;
}
int getCmd() {
    int cmd;
    cout << "输入功能编号：";
    cin >> cmd;
    return cmd;
}
STATUS menu1(User& us) {
    int cmd = -1;
    printf("请选择菜单中的功能编号，按回车键确认：\n");
    printf("0: 登录\n");
    printf("1: 注册\n");

    cmd = getCmd();

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
        doRegister();
    }
    else {
        printf("指令输入有误！\n");
    }
    return LOOP;
}

STATUS menuYHGL(User& us) {
    int cmd = -1;
    printf("--用户管理--\n");
    listUsers();
    printf("0: 返回\n");
    printf("1: 添加用户\n");
    printf("2: 删除用户\n");
    printf("3: 修改密码\n");
    cmd = getCmd();
    return EXIT;
}

STATUS menuSPXXGL(User& us) {
    int cmd = -1;
    printf("--商品管理--\n");
    listGoods();
    printf("0: 返回\n");
    printf("1: 添加商品\n");
    printf("2: 下架商品\n");
    printf("3: 修改商品\n");
    printf("4: 分店货物\n");
    cmd = getCmd();
    return EXIT;
}

STATUS menuKCGL(User& us) {
    int cmd = -1;
    printf("--库存管理--\n");
    listBranches();
    printf("0: 返回\n");
    printf("1: 进货\n");
    printf("2: 清仓\n");
    printf("3: 检查\n");
    cmd = getCmd();
    return EXIT;
    return EXIT;
}
STATUS menuFDGL(User& us) {
    int cmd = -1;
    printf("--分店管理--\n");
    listBranches();
    printf("0: 返回\n");
    printf("1: 添加分店\n");
    printf("2: 关闭分店\n");
    cmd = getCmd();
    return EXIT;
}
STATUS menuAdmin(User& us) {
    int cmd = -1;
    cout << "欢迎管理员" << convGBK(us.username) << "！请选择菜单中的功能编号，按回车键确认：\n";
    printf("0: 注销\n");
    printf("1: 用户管理\n");
    printf("2: 商品管理\n");
    printf("3: 库存管理\n");
    printf("4: 分店管理\n");
    cmd = getCmd();

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
    cout << "欢迎顾客" << convGBK(us.username) << "！请选择菜单中的功能编号，按回车键确认：\n";
    printf("0: 注销\n");
    printf("1: 市场\n");
    printf("2: 购物车\n");
    printf("3: 钱包\n");

    cmd = getCmd();

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
    cout << "----用户列表----" << endl;
    for (int i = 0; i < users.size(); i++) {
        cout << "||";
        cout << "用户名：" << convGBK(users[i]["username"].get<string>()) << "||";
        cout << "类型：" << ((users[i]["auth"].get<int>() > 1) ? "管理员" : "顾客") << "||";
        cout << "钱包余额：" << users[i]["wallet"]["money"].get<float>() << "||";
        cout << "礼券余额：" << users[i]["wallet"]["coupons"].get<float>() << "||";
        cout << "购物次数：" << users[i]["purchase_log"].size() << "||";
        cout << endl;
    }
}
void listGoods() {
}
void listBranches() {
    cout << "----分店列表----" << endl;
    for (int i = 0; i < branches.size(); i++) {
        cout << "||";
        cout << "编号：" << convGBK(branches[i]["id"].get<string>()) << "||";
        cout << "分店名：" << convGBK(branches[i]["name"].get<string>()) << "||";
        cout << "描述：" << convGBK(branches[i]["description"].get<string>()) << "||";
        cout << endl;
    }
}

void loadUsers(json& u) {
    std::ifstream fin(USER_FILE);
    fin >> u;
    fin.close();
}
void loadGoods(vector<json>& g) {
    json j;
    g.clear();
    for (int i = 0; i < branches.size(); i++) {
        std::ifstream fin(branches[i]["id"].get<string>().append(".json"));
        fin >> j;
        g.push_back(j);
        fin.close();
    }
}
void loadBranches(json& bs) {
    std::ifstream fin(BRANCH_FILE);
    fin >> bs;
    fin.close();
}
void saveUsers(json& u) {
    ofstream fout(USER_FILE);
    fout << setw(4) << u << endl;
    fout.close();
}
void saveGoods(vector<json>& g) {
}
void saveBranches(json& bs) {
}
