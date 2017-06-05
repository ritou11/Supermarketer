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
#define ALERT_LINE 100
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

struct Point {
    int i, j;
};

void printBanner();
int findBranch(string id);
Point findGoods(string value, string field);
int findUser(string username);
vector<Point> findAllGoods(string value, string field);

void listUsers();
void listGoods(int bi);
void listBranches(bool all);
void listLogs(User& u);
void listCart(User& u);

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

STATUS doAddGoods(User& us);
STATUS doDownGoods(User& us);
STATUS doEditGoods(User& us);
STATUS doBranchGoods(User& us);

STATUS doAddStorage(User& us);
STATUS doClearStorage(User& us);
STATUS doCheckStorage(User& us);

STATUS doAddBranch(User& us);
STATUS doStopBranch(User& us);

STATUS doBuy(User& us);
STATUS doSearch(User& us);

STATUS doPay(User& us);
STATUS doDelete(User& us);
STATUS doAdjust(User& us);

STATUS doTopup(User& us);
STATUS doRank(User& us);
STATUS doSendback(User& us);

// 使用函数指针，简化相似的菜单操作
STATUS(*adminMenus[])(User&) = { menuYHGL, menuSPGL, menuKCGL, menuFDGL };
STATUS(*consumerMenus[])(User&) = { menuSC, menuGUC, menuQB };
STATUS(*admin_con_menus[])(User&) = { menuAdmin, menuConsumer };
STATUS(*yhglMenus[])(User&) = { doAddUser, doDeleteUser, doEditPwd };
STATUS(*spglMenus[])(User&) = { doAddGoods ,doDownGoods ,doEditGoods ,doBranchGoods };
STATUS(*kcglMenus[])(User&) = { doAddStorage, doClearStorage, doCheckStorage };
STATUS(*fdglMenus[])(User&) = { doAddBranch , doStopBranch };
STATUS(*scMenus[])(User&) = { doBuy , doSearch };
STATUS(*gucMenus[])(User&) = { doPay , doDelete , doAdjust };
STATUS(*qbMenus[])(User&) = { doTopup, doRank, doSendback };

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
            STATUS s = admin_con_menus[us.type == CONSUMER](us);
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
    if (findUser(u.username)>0) {
        cout << "用户名重复！" << endl;
        return;
    }
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
int findBranch(string id) {
    for (int i = 0; i < branches.size(); i++) {
        if (branches[i]["id"].get<string>() == id)
            return i;
    }
    return -1;
}
Point findGoods(string value, string field) {
    for (int i = 0; i < goods.size(); i++) {
        for (int j = 0; j < goods[i].size(); j++) {
            if (goods[i][j][field].get<string>() == value) return Point{ i,j };
        }
    }
    return Point{ -1,-1 };
}
vector<Point> findAllGoods(string value, string field) {
    vector<Point> pl;
    for (int i = 0; i < goods.size(); i++) {
        for (int j = 0; j < goods[i].size(); j++) {
            if (goods[i][j][field].get<string>() == value) pl.push_back(Point{ i,j });
        }
    }
    return pl;
}
int findUser(string username) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i]["username"].get<string>() == username)
            return i;
    }
    return -1;
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
    printf("4: 记录\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0://Logout
        return EXIT;
        break;
    case 1:case 2:case 3:
        nextMenu = consumerMenus[cmd - 1];
        while (1) {
            // 下一级菜单
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    case 4:
        listLogs(us);
        system("pause");
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
    listGoods(-1);
    printf("0: 返回\n");
    printf("1: 添加商品\n");
    printf("2: 下架商品\n");
    printf("3: 修改商品\n");
    printf("4: 分店货物\n");
    cmd = getCmd();

    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:case 4:
        nextMenu = spglMenus[cmd - 1];
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
STATUS menuKCGL(User& us) {
    int cmd = -1;
    printf("--库存管理--\n");
    printf("0: 返回\n");
    printf("1: 进货\n");
    printf("2: 清仓\n");
    printf("3: 检查\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:
        nextMenu = kcglMenus[cmd - 1];
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
STATUS menuFDGL(User& us) {
    int cmd = -1;
    printf("--分店管理--\n");
    listBranches(false);
    printf("0: 返回\n");
    printf("1: 添加分店\n");
    printf("2: 关闭分店\n");
    printf("3: 全部分店\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2: 
        nextMenu = fdglMenus[cmd - 1];
        while (1) {
            // 下一级菜单
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    case 3:
        listBranches(true);
        system("pause");
        break;
    default:
        printf("指令输入错误！\n");
    }
    return LOOP;
}

STATUS menuSC(User& us) {
    int cmd = -1;
    printf("--市场--\n");
    listGoods(-1);
    printf("0: 返回\n");
    printf("1: 购买\n");
    printf("2: 搜索\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:
        nextMenu = scMenus[cmd - 1];
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
STATUS menuGUC(User& us) {
    int cmd = -1;
    printf("--购物车--\n");
    listCart(us);
    printf("0: 返回\n");
    printf("1: 付款\n");
    printf("2: 删除\n");
    printf("3: 调整\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:
        nextMenu = gucMenus[cmd - 1];
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
STATUS menuQB(User& us) {
    // TODO wallet
    return EXIT;
}

STATUS doAddUser(User& us) {
    cout << "----添加用户----" << endl;
    cout << "请输入添加的类型(0: 管理员，1：消费者)";
    int cmd = getCmd();
    if (cmd > 1 || cmd < 0) {
        cout << "类型编号输入错误！" << endl;
        return EXIT;
    }
    doRegister(cmd ? CONSUMER : ADMIN);
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

STATUS doAddGoods(User& us) {
    cout << "----添加商品----" << endl;
    string branch, id, name, isbn, source, description;
    float price;
    listBranches(false);
    cout << "请输入添加到的分店编号：";
    cin >> branch;
    int branchIndex = findBranch(branch);
    if (branchIndex < 0) {
        cout << "分店编号输入有误！" << endl;
        return EXIT;
    }
    cout << "请输入商品编号，商品名称，商品代码，产地，以空格或换行分割：\n";
    cin >> id >> name >> isbn >> source;
    id = convUTF8(id);
    name = convUTF8(name);
    isbn = convUTF8(isbn);
    source = convUTF8(source);

    Point p = findGoods(id, "id");
    if (p.i >= 0 || p.j >= 0) {
        cout << "编号重复！" << endl;
        return EXIT;
    }

    cout << "请输入商品价格：";
    cin >> price;
    cout << "请输入商品描述：";
    cin >> description;
    description = convUTF8(description);

    json good = {
        { "id",id },
        { "name",name },
        { "isbn", isbn },
        { "source", source },
        { "price", price},
        {"storage", 0},
        { "last7days_sell",json::array() },
        {"today_sell",0},
        {"description",description},
        {"avaliable",true}
    };

    goods[branchIndex].push_back(good);
    saveGoods(goods);

    return EXIT;
}
STATUS doDownGoods(User& us) {
    cout << "----下架商品----" << endl;
    listGoods(-1);
    string id;
    cout << "输入要下架的商品编号：";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "商品编号有误！" << endl;
        return EXIT;
    }
    goods[p.i][p.j]["avaliable"] = false;
    saveGoods(goods);
    return EXIT;
}
STATUS doEditGoods(User& us) {
    cout << "----修改商品----" << endl;
    listGoods(-1);
    string id;
    cout << "输入要修改的商品编号：";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "商品编号有误！" << endl;
        return EXIT;
    }
    cout << "输入要修改的信息编号（0：名称，1：代码，2：产地:3：价格，4：描述）：";
    int cmd = getCmd();
    string s;
    switch (cmd) {
    case 0:
        cout << "输入名称：";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["name"] = s;
        break;
    case 1:
        cout << "输入代码：";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["isbn"] = s;
        break;
    case 2:
        cout << "输入产地：";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["source"] = s;
        break;
    case 3:
        cout << "输入价格：";
        float price;
        cin >> price;
        goods[p.i][p.j]["price"] = price;
        break;
    case 4:
        cout << "输入描述：";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["description"] = s;
        break;
    default:
        cout << "指令有误！" << endl;
        return EXIT;
    }
    saveGoods(goods);
    return EXIT;
}
STATUS doBranchGoods(User& us) {
    cout << "----下架商品----" << endl;
    listBranches(false);
    string id;
    cout << "输入分店编号：";
    cin >> id;
    id = convUTF8(id);
    int index = findBranch(id);
    if (index < 0) {
        cout << "分店编号有误！" << endl;
        return EXIT;
    }
    listGoods(index);
    return EXIT;
}

STATUS doAddStorage(User& us) {
    cout << "----增加库存----" << endl;
    listGoods(-1);
    string id;
    cout << "输入要进货的商品编号：";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "商品编号有误！" << endl;
        return EXIT;
    }
    cout << "商品" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "现有库存" << goods[p.i][p.j]["storage"].get<int>() << "，输入进货数量：" << endl;
    int add;
    cin >> add;
    if (add < 0) {
        cout << "清仓请切换至清仓菜单！" << endl;
        return EXIT;
    }
    goods[p.i][p.j]["storage"] = goods[p.i][p.j]["storage"].get<int>() + add;
    saveGoods(goods);

    return EXIT;
}
STATUS doClearStorage(User& us) {
    cout << "----减少库存----" << endl;
    listGoods(-1);
    string id;
    cout << "输入要清仓的商品编号：";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "商品编号有误！" << endl;
        return EXIT;
    }
    cout << "商品" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "现有库存" << goods[p.i][p.j]["storage"].get<int>() << "，输入减少数量：" << endl;
    int add;
    cin >> add;
    if (add < 0) {
        cout << "进货请切换至进货菜单！" << endl;
        return EXIT;
    }
    goods[p.i][p.j]["storage"] = goods[p.i][p.j]["storage"].get<int>() - add;
    saveGoods(goods);

    return EXIT;
}
STATUS doCheckStorage(User& us) {
    cout << "----库存检查----" << endl;
    for (int i = 0; i < branches.size(); i++) {
        if (branches[i]["open"].get<bool>() == false) continue;
        cout << "--分店【" << convGBK(branches[i]["name"].get<string>()) << "】库存检查--" << endl;
        int alert_cnt = 0;
        for (int j = 0; j < goods[i].size(); j++) {
            bool alert = goods[i][j]["storage"].get<int>() < ALERT_LINE;
            if (alert) cout << "!!!";
            cout << "||" << convGBK(goods[i][j]["id"].get<string>()) << "||" << convGBK(goods[i][j]["name"].get<string>());
            cout << "||" << goods[i][j]["storage"].get<int>() << "||";
            if (alert) cout << "!!!(警告！库存过少！)" << endl;
            else
                cout << endl;
            alert_cnt += alert;
        }
        if (alert_cnt > 0) cout << "共" << alert_cnt << "件商品存货不足！" << endl;
    }
    return EXIT;
}

STATUS doAddBranch(User& us) {
    cout << "----添加分店----" << endl;
    string id, name, description;
    cout << "请输入要添加的分店编号：";
    cin >> id;
    id = convUTF8(id);
    if (findBranch(id) >= 0) {
        cout << "编号重复！" << endl;
        return EXIT;
    }
    cout << "请输入要添加的分店名称：";
    cin >> name;
    name = convUTF8(name);
    cout << "请输入分店描述：";
    cin >> description; 
    description = convUTF8(description);

    json branch = {
        { "id",id },
        { "name",name },
        { "description", description },
        { "open",true }
    };

    branches.push_back(branch);
    goods.push_back(json::array());

    saveBranches(branches);
    saveGoods(goods);

    return EXIT;
}
STATUS doStopBranch(User& us) {
    cout << "----关停分店----" << endl;
    string id;
    cout << "输入要关停的分店编号(再次关闭可开启分店)：";
    cin >> id;
    id = convUTF8(id);
    int index = findBranch(id);
    if ( index < 0) {
        cout << "编号有误！" << endl;
        return EXIT;
    }
    branches[index]["open"] = !(branches[index]["open"].get<bool>());
    saveBranches(branches);
    return EXIT;
}

STATUS doBuy(User& us) {
    cout << "----添加到购物车----" << endl;
    cout << "请输入您要购买的商品编号：";
    string id;
    Point gp;
    cin >> id;
    id = convUTF8(id);
    gp = findGoods(id,"id");
    if (gp.i < 0 || gp.j < 0) {
        cout << "别淘气！没有这个商品！" << endl;
        return EXIT;
    }
    float price = goods[gp.i][gp.j]["price"].get<float>();
    cout << "亲要买几个呢？：";
    int goodCnt;
    cin >> goodCnt;
    if (goodCnt < 0) {
        cout << "别淘气~我们不收货哦~" << endl;
        return EXIT;
    }
    else if (goodCnt == 0) return EXIT;
    cout << "购买【" << goodCnt << "】个【"<< convGBK(goods[gp.i][gp.j]["name"].get<string>()) <<"】，总计" << goodCnt*price << "元，添加到购物车吗？(0: 取消；1：确认)：";
    int cmd = getCmd();
    if (cmd != 1) {
        cout << "放弃添加！" << endl;
        return EXIT;
    }

    int uid = findUser(us.username);
    if (uid < 0) return EXIT;

    json cc = {
        {"id", id},
        {"num", goodCnt}
    };
    users[uid]["cart"].push_back(cc);

    saveUsers(users);
    return EXIT;
}
STATUS doSearch(User& us) {
    cout << "----搜索商品----" << endl;
    string field, value;
    cout << "输入搜索依据编号（0：编号，1：名称，2：代码）：";
    int cmd = getCmd();
    string s;
    switch (cmd) {
    case 0:
        field = "id";
        break;
    case 1:
        field = "name";
        break;
    case 2:
        field = "isbn";
        break;
    default:
        cout << "指令有误！" << endl;
        return EXIT;
    }
    cout << "目标值：";
    cin >> value;
    value = convUTF8(value);
    vector<Point> res = findAllGoods(value, field);
    for (int i = 0; i < res.size(); i++) {
        Point p = res[i];
        if (p.i < 0 || p.j < 0) {
            cout << "商品编号有误！" << endl;
            return EXIT;
        }
        cout << "商品所在分店：" << convGBK(branches[p.i]["name"].get<string>()) << endl;
        cout << "||";
        cout << "编号：" << convGBK(goods[p.i][p.j]["id"].get<string>()) << "||";
        cout << "名称：" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "||";
        cout << "代码：" << convGBK(goods[p.i][p.j]["isbn"].get<string>()) << "||";
        cout << "产地：" << convGBK(goods[p.i][p.j]["source"].get<string>()) << "||";
        cout << "价格：" << (goods[p.i][p.j]["price"].get<double>()) << "||";
        cout << "库存：" << (goods[p.i][p.j]["storage"].get<int>()) << "||";
        cout << endl;
    }
    if (res.size() <= 0) {
        cout << "没有找到这样的商品！" << endl;
    }
    system("pause");
    return EXIT;
}

STATUS doPay(User& us) {
    int uid = findUser(us.username);
    if (uid < 0) return EXIT;
    float money = users[uid]["wallet"]["money"].get<float>() , coupons = users[uid]["wallet"]["coupons"].get<float>();
    json cart = users[uid]["cart"];

    cout << "----付款----" << endl;
    cout << "请输入您要付款的订单编号：";
    int cid = getCmd();
    if (cid < 0 || cid >= cart.size()) {
        cout << "编号有误！" << endl;
        return EXIT;
    }
    string id = cart[cid]["id"].get<string>();
    Point gp;
    gp = findGoods(id, "id");
    if (gp.i < 0 || gp.j < 0 || goods[gp.i][gp.j]["avaliable"].get<bool>() == false) {
        cout << "商品下架啦~" << endl;
        return EXIT;
    }
    json good = goods[gp.i][gp.j];
    float price = good["price"].get<float>();
    int num = cart[cid]["num"].get<int>();
    if (num*price > coupons + money) {
        cout << "钱包太瘪了哦~请前去充值！" << endl;
        return EXIT;
    }
    cout << "购买【"<< convGBK(good["name"].get<string>()) <<"】，合计【"<< num*price <<"】元，确认付款吗？(0:取消，1:确认)" << endl;
    int cmd = getCmd();
    if (cmd != 1) {
        cout << "放弃付款！" << endl;
        return EXIT;
    }

    if (coupons >= num*price) {
        coupons -= num*price;
    }
    else {
        money -= num*price - coupons;
        coupons = 0;
    }

    users[uid]["wallet"]["money"] = money;
    users[uid]["wallet"]["coupons"] = coupons;

    users[uid]["cart"].erase(cid);
    json log = {
        {"id", id},
        {"num", num},
        {"total", num*price}
    };
    users[uid]["purchase_log"].push_back(log);

    saveUsers(users);
    return EXIT;
}
STATUS doDelete(User& us) {
    int uid = findUser(us.username);
    if (uid < 0) return EXIT;
    json cart = users[uid]["cart"];

    cout << "----删除----" << endl;
    cout << "请输入您要删除的订单编号：";
    int cid = getCmd();
    if (cid < 0 || cid >= cart.size()) {
        cout << "编号有误！" << endl;
        return EXIT;
    }    
    users[uid]["cart"].erase(cid);
    saveUsers(users);
    return EXIT;
}
STATUS doAdjust(User& us) {
    int uid = findUser(us.username);
    if (uid < 0) return EXIT;
    json cart = users[uid]["cart"];

    cout << "----调整数量----" << endl;
    cout << "请输入您要调整的订单编号：";
    int cid = getCmd();
    if (cid < 0 || cid >= cart.size()) {
        cout << "编号有误！" << endl;
        return EXIT;
    }
    cout << "亲要改成几个呢？：";
    int goodCnt;
    cin >> goodCnt;
    if (goodCnt < 0) {
        cout << "别淘气~我们不收货哦~" << endl;
        return EXIT;
    }
    else if (goodCnt == 0) {
        users[uid]["cart"].erase(cid);
    }
    else {
        users[uid]["cart"][cid]["num"] = goodCnt;
    }
    saveUsers(users);
    return EXIT;
}

STATUS doTopup(User& us) {
    return EXIT;
}
STATUS doRank(User& us) {
    return EXIT;
}
STATUS doSendback(User& us) {
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
void listGoods(int bi) {
    if (bi < 0) {
        for (int i = 0; i < goods.size(); i++) {
            listGoods(i);
        }
    }
    else {
        if (bi > goods.size() || bi < 0) return;
        if (branches[bi]["open"].get<bool>() == false) return;
        cout << "----分店【" << convGBK(branches[bi]["name"].get<string>()) << "】商品情况----" << endl;
        for (int i = 0; i < goods[bi].size(); i++) if (goods[bi][i]["avaliable"].get<bool>()) {
            cout << "||";
            cout << "编号：" << convGBK(goods[bi][i]["id"].get<string>()) << "||";
            cout << "名称：" << convGBK(goods[bi][i]["name"].get<string>()) << "||";
            cout << "代码：" << convGBK(goods[bi][i]["isbn"].get<string>()) << "||";
            cout << "产地：" << convGBK(goods[bi][i]["source"].get<string>()) << "||";
            cout << "价格：" << (goods[bi][i]["price"].get<double>()) << "||";
            cout << "库存：" << (goods[bi][i]["storage"].get<int>()) << "||";
            cout << endl;
        }
    }
}
void listBranches(bool all) {
    cout << "----分店列表----" << endl;
    for (int i = 0; i < branches.size(); i++) if (all || branches[i]["open"].get<bool>()) {
        cout << "||";
        cout << "编号：" << convGBK(branches[i]["id"].get<string>()) << "||";
        cout << "分店名：" << convGBK(branches[i]["name"].get<string>()) << "||";
        cout << "描述：" << convGBK(branches[i]["description"].get<string>()) << "||";
        if (all) {
            cout << "状态：" << (branches[i]["open"].get<bool>() ? "营业" : "关停") << "||";
        }
        cout << endl;
    }
}
void listLogs(User& u) {
    int uid = findUser(u.username);
    if (uid < 0) return;
    json logs = users[uid]["purchase_log"];
    for (int i = 0; i < logs.size(); i++) {
        string id = logs[i]["id"].get<string>();
        Point p = findGoods(id, "id");
        if (p.i < 0 || p.j < 0) continue;
        cout << "||";
        cout << "单号：" << i << "||";
        cout << "商品编号：" << convGBK(id) << "||";
        cout << "商品名称：" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "||";
        cout << "购买数量：" << logs[i]["num"].get<int>() << "||";
        cout << "总价：" << logs[i]["total"].get<float>() << "||";
        cout << endl;
    }
}
void listCart(User& u) {
    int uid = findUser(u.username);
    if (uid < 0) return;
    json cart = users[uid]["cart"];
    for (int i = 0; i < cart.size(); i++) {
        string id = cart[i]["id"].get<string>();
        Point p = findGoods(id, "id");
        if (p.i < 0 || p.j < 0) continue;
        cout << "||";
        cout << "单号：" << i << "||";
        cout << "商品编号：" << convGBK(id) << "||";
        cout << "商品名称：" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "||";
        cout << "购买数量：" << cart[i]["num"].get<int>() << "||";
        cout << "总价：" << cart[i]["num"].get<int>() * goods[p.i][p.j]["price"].get<float>() << "||";
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
    if (g.size() != branches.size()) return;
    for (int i = 0; i < g.size(); i++) {
        ofstream fout(branches[i]["id"].get<string>().append(".json"));
        fout << setw(4) << g[i] << endl;
        fout.close();
    }
}
void saveBranches(json& bs) {
    ofstream fout(BRANCH_FILE);
    fout << setw(4) << bs << endl;
    fout.close();
}
