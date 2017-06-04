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
void listBranches(bool all);

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
STATUS menuSPGL(User& us);
STATUS menuKCGL(User& us);
STATUS menuFDGL(User& us);

STATUS menuConsumer(User& us);
STATUS menuSC(User& us);
STATUS menuGUC(User& us);
STATUS menuQB(User& us);

STATUS doAddUser(User& us);
STATUS doDeleteUser(User& us);
STATUS doEditPwd(User& us);

STATUS menuAddGoods(User& us);
STATUS menuDownGoods(User& us);
STATUS menuEditGoods(User& us);
STATUS menuBranchGoods(User& us);

STATUS menuAddStorage(User& us);
STATUS menuClearStorage(User& us);
STATUS menuCheckStorage(User& us);

STATUS menuAddBranch(User& us);
STATUS menuStopBranch(User& us);

STATUS menuBuy(User& us);
STATUS menuSearch(User& us);

STATUS menuPay(User& us);
STATUS menuDelete(User& us);
STATUS menuAdjust(User& us);

// 使用函数指针，简化相似的菜单操作
STATUS(*adminMenus[])(User&) = { menuYHGL, menuSPGL, menuKCGL, menuFDGL };
STATUS(*consumerMenus[])(User&) = { menuSC, menuGUC, menuQB };
STATUS(*admin_con_menus[])(User&) = {menuAdmin, menuConsumer};
STATUS(*yhglMenus[])(User&) = { doAddUser, doDeleteUser, doEditPwd };
STATUS(*spglMenu[])(User&) = { menuAddGoods ,menuDownGoods ,menuEditGoods ,menuBranchGoods };
STATUS(*kcglMenu[])(User&) = { menuAddStorage, menuClearStorage, menuCheckStorage};
STATUS(*fdglMenu[])(User&) = { menuAddBranch , menuStopBranch };
STATUS(*gucMenu[])(User&) = { menuPay , menuDelete , menuAdjust };
STATUS(*scMenu[])(User&) = { menuBuy , menuSearch };

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

        while (1) {
            // 管理员顶层菜单
            STATUS s = admin_con_menus[us.type==CONSUMER](us);
            if (s == EXIT) break;
        }
        system("pause");
    }
    return 0;
}

void printBanner() {
    printf("****************************************************\n");
    printf("            欢迎光临笑蛤蛤大型连锁超市              \n");
    printf("                系统版本：%s                       \n", VERSION);
    printf("                作者：黄珈颖                          \n");
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
void doRegister(USER_TYPE ut) {
    cout << "-注册新用户-" << endl;
    User u;
    cout << "用户名：";
    cin >> u.username;
    u.username = convUTF8(u.username);
    u.type = ut;
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
    cout << "输入编号：";
    cin >> cmd;
    // 清空输入区，避免错误输入持续作用
    cin.clear();
    cin.ignore(10000, '\n');
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
        doRegister(CONSUMER);
    }
    else {
        printf("指令输入有误！\n");
    }
    return LOOP;
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

STATUS menuYHGL(User& us) {
    int cmd = -1;
    printf("--用户管理--\n");
    listUsers();
    printf("0: 返回\n");
    printf("1: 添加用户\n");
    printf("2: 删除用户\n");
    printf("3: 修改密码\n");
    cmd = getCmd();

    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:
        nextMenu = yhglMenus[cmd - 1];
        while (1) {
            // 下一级菜单
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("指令输入错误！\n");
    }
    return LOOP;
}
STATUS menuSPGL(User& us) {
    int cmd = -1;
    printf("--商品管理--\n");
    listGoods();
    printf("0: 返回\n");
    printf("1: 添加商品\n");
    printf("2: 下架商品\n");
    printf("3: 修改商品\n");
    printf("4: 分店货物\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:case 4:

        break;
    default:
        printf("指令输入错误！\n");
    }
    return LOOP;
}
STATUS menuKCGL(User& us) {
    int cmd = -1;
    printf("--库存管理--\n");
    listBranches(false);
    printf("0: 返回\n");
    printf("1: 进货\n");
    printf("2: 清仓\n");
    printf("3: 检查\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:

        break;
    default:
        printf("指令输入错误！\n");
    }
    return LOOP;
}
STATUS menuFDGL(User& us) {
    int cmd = -1;
    printf("--分店管理--\n");
    listBranches(false);
    printf("0: 返回\n");
    printf("1: 添加分店\n");
    printf("2: 关闭分店\n");
    printf("3: 全部分店\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:

        break;
    default:
        printf("指令输入错误！\n");
    }
    return LOOP;
}

STATUS menuSC(User& us) {
    int cmd = -1;
    printf("--市场--\n");
    listGoods();
    printf("0: 返回\n");
    printf("1: 购买\n");
    printf("2: 搜索\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:

    default:
        printf("指令输入有误！\n");
    }
    return LOOP;

}
STATUS menuGUC(User& us) {
    // TODO cart
    return EXIT;
}
STATUS menuQB(User& us) {
    // TODO wallet
    return EXIT;
}

STATUS doAddUser(User& us) {
    cout << "----添加用户----" << endl;
    cout << "请输入添加的类型(0: 管理员，1：消费者)";
    int cmd = getCmd();
    if (cmd > 1 || cmd < 0) {
        cout <<"类型编号输入错误！"<<endl;
        return EXIT;
    }    
    doRegister(cmd ? CONSUMER:ADMIN);
    return EXIT;
}
STATUS doDeleteUser(User& us) {
    cout << "----删除用户----" << endl;
    listUsers();
    cout << "输入删除的用户名：";
    string s;
    cin >> s;
    s = convUTF8(s);
    for (int i = 0; i < users.size(); i++) {
        if (users[i]["username"].get<string>() == s) {
            cout << "确认删除" << convGBK(s) << "吗？(0: 否，1：是)：" << endl;
            int cmd = getCmd();
            if (cmd != 1) {
                cout << "放弃！" << endl;
                break;
            }
            users.erase(i);
            break;
        }
    }
    saveUsers(users);
    return EXIT;
}
STATUS doEditPwd(User& us) {
    cout << "----修改密码----" << endl;
    string s;
    cout << "输入修改密码的用户名：";
    cin >> s;
    s = convUTF8(s);

    int index = -1;
    for (int i = 0; i < users.size(); i++) {
        if (users[i]["username"].get<string>() == s) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        cout << "用户名错误！" << endl;
        return EXIT;
    }
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
        return EXIT;
    }
    pass = sha512(pass);
    users[index]["password"] = pass;
    saveUsers(users);
    return EXIT;
}

STATUS menuAddGoods(User& us) {
    return EXIT;
}
STATUS menuDownGoods(User& us) {
    return EXIT;
}
STATUS menuEditGoods(User& us) {
    return EXIT;
}
STATUS menuBranchGoods(User& us) {
    return EXIT;
}

STATUS menuAddStorage(User& us) {
    return EXIT;
}
STATUS menuClearStorage(User& us) {
    return EXIT;
}
STATUS menuCheckStorage(User& us) {
    return EXIT;
}

STATUS menuAddBranch(User& us) {
    return EXIT;
}
STATUS menuStopBranch(User& us) {
    return EXIT;
}

STATUS menuBuy(User& us) {
    return EXIT;
}
STATUS menuSearch(User& us) {
    return EXIT;
}

STATUS menuPay(User& us) {
    return EXIT;
}
STATUS menuDelete(User& us) {
    return EXIT;
}
STATUS menuAdjust(User& us) {
    return EXIT;
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
    // TODO List Goods
}
void listBranches(bool all) {
    cout << "----分店列表----" << endl;
    for (int i = 0; i < branches.size(); i++) if(all || branches[i]["open"].get<bool>()){
        cout << "||";
        cout << "编号：" << convGBK(branches[i]["id"].get<string>()) << "||";
        cout << "分店名：" << convGBK(branches[i]["name"].get<string>()) << "||";
        cout << "描述：" << convGBK(branches[i]["description"].get<string>()) << "||";
        if (all) {
            cout << "状态：" << (branches[i]["open"].get<bool>()? "营业":"关停") << "||";
        }
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
    // TODO save goods
}
void saveBranches(json& bs) {
    // TODO save branches
}
