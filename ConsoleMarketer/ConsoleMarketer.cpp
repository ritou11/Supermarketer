// ConsoleMarketer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "utils.h"
#include "User.h"

/*************************************************************
** ��������
** �������Ա��ڸı�����еĲ������á�
*/
#define VERSION "V1.0"
#define NEW_USER_COUPONS 10
#define ALERT_LINE 100
//************************************************************

/*************************************************************
** �����ļ�
** ��ʵ��Ӧ�õĳ����У�����ʹ��mongodb�����������ݹ���ģʽ
** �˴�Ϊ�˱���mongodb�������⣬ʹ���ļ��������ݴ���
*/
#define USER_FILE "user.json"
#define GOODS_FILE "goods.json"
#define BRANCH_FILE "branch.json"
//************************************************************

/*************************************************************
** �ڴ��е����ݴ洢
** ���ļ��е����ݶ�ȡ���ڴ��У�����Ƶ����Ӳ�̶�д����
** ���������ݹ�ģ��Сʱ
*/
json users;
vector<json> goods;
json branches;
//************************************************************

// �˵��������صĿ���״̬
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

// ʹ�ú���ָ�룬�����ƵĲ˵�����
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

// ��������������ڵ�
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
            // ��¼/ע��ѡ��
            STATUS s = menu1(us);
            if (s == EXIT) break;
        }

        while (1) {
            // ����Ա����˵�
            STATUS s = admin_con_menus[us.type == CONSUMER](us);
            if (s == EXIT) break;
        }
        system("pause");
    }
    return 0;
}

void printBanner() {
    printf("****************************************************\n");
    printf("            ��ӭ����Ц��������������              \n");
    printf("                ϵͳ�汾��%s                       \n", VERSION);
    printf("                ���ߣ�����ӱ                          \n");
    printf("****************************************************\n");
}

bool doLogin(User& us) {
    cout << "-��½-" << endl;
    cout << "--�û�����";
    cin >> us.username;
    us.username = convUTF8(us.username);
    string password = getpass("--���룺");
    //ԭ�����벻�����Ĵ��棬�����Ҫ��SHA512���ܺ��������洢���Ľ��жԱȣ�
    //��Ȼ��������δ���������ļ�д�룬ʹ��ϵͳ������ȫ��
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
    // �����ַ����ٰ�������
}
void doRegister(USER_TYPE ut) {
    cout << "-ע�����û�-" << endl;
    User u;
    cout << "�û�����";
    cin >> u.username;
    u.username = convUTF8(u.username);
    if (findUser(u.username)>0) {
        cout << "�û����ظ���" << endl;
        return;
    }
    u.type = ut;
    string pass;
    while (1) {
        pass = getpass("���룺", true);
        if (!checkPass(pass)) {
            cout << "����̫�򵥣����������룡" << endl;
        }
        else break;
    }
    string con_pass = getpass("�ٴ�ȷ�����룺", true);
    if (con_pass != pass) {
        cout << "������������벻һ�£�ע��ʧ�ܣ�" << endl;
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
    cout << "�û���" << convGBK(u.username) << "��ע��ɹ��������Ʊ����������롣���û�����" << NEW_USER_COUPONS << "��ȯ���Ѿ���������Ǯ�����У�����ա�ף��������죡"
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
    cout << "�����ţ�";
    cin >> cmd;
    // �����������������������������
    cin.clear();
    cin.ignore(10000, '\n');
    return cmd;
}
STATUS menu1(User& us) {
    int cmd = -1;
    printf("��ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n");
    printf("0: ��¼\n");
    printf("1: ע��\n");

    cmd = getCmd();

    if (cmd == 0) {
        if (doLogin(us)) {
            printf("��½�ɹ����������ǡ�%s����\n", us.type == ADMIN ? "����Ա" : "������");
            return EXIT;
        }
        else {
            printf("�û�����������󣬵�¼ʧ�ܣ�\n");
        }
    }
    else if (cmd == 1) {
        doRegister(CONSUMER);
    }
    else {
        printf("ָ����������\n");
    }
    return LOOP;
}

STATUS menuAdmin(User& us) {
    int cmd = -1;
    cout << "��ӭ����Ա" << convGBK(us.username) << "����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n";
    printf("0: ע��\n");
    printf("1: �û�����\n");
    printf("2: ��Ʒ����\n");
    printf("3: ������\n");
    printf("4: �ֵ����\n");
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
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("ָ����������\n");
    }
    return LOOP;
}
STATUS menuConsumer(User& us) {
    int cmd = -1;
    cout << "��ӭ�˿�" << convGBK(us.username) << "����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n";
    printf("0: ע��\n");
    printf("1: �г�\n");
    printf("2: ���ﳵ\n");
    printf("3: Ǯ��\n");
    printf("4: ��¼\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0://Logout
        return EXIT;
        break;
    case 1:case 2:case 3:
        nextMenu = consumerMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    case 4:
        listLogs(us);
        system("pause");
    default:
        printf("ָ����������\n");
    }
    return LOOP;
}

STATUS menuYHGL(User& us) {
    int cmd = -1;
    printf("--�û�����--\n");
    listUsers();
    printf("0: ����\n");
    printf("1: ����û�\n");
    printf("2: ɾ���û�\n");
    printf("3: �޸�����\n");
    cmd = getCmd();

    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:
        nextMenu = yhglMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("ָ���������\n");
    }
    return LOOP;
}
STATUS menuSPGL(User& us) {
    int cmd = -1;
    printf("--��Ʒ����--\n");
    listGoods(-1);
    printf("0: ����\n");
    printf("1: �����Ʒ\n");
    printf("2: �¼���Ʒ\n");
    printf("3: �޸���Ʒ\n");
    printf("4: �ֵ����\n");
    cmd = getCmd();

    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:case 4:
        nextMenu = spglMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("ָ���������\n");
    }
    return LOOP;
}
STATUS menuKCGL(User& us) {
    int cmd = -1;
    printf("--������--\n");
    printf("0: ����\n");
    printf("1: ����\n");
    printf("2: ���\n");
    printf("3: ���\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:
        nextMenu = kcglMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("ָ���������\n");
    }
    return LOOP;
}
STATUS menuFDGL(User& us) {
    int cmd = -1;
    printf("--�ֵ����--\n");
    listBranches(false);
    printf("0: ����\n");
    printf("1: ��ӷֵ�\n");
    printf("2: �رշֵ�\n");
    printf("3: ȫ���ֵ�\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2: 
        nextMenu = fdglMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    case 3:
        listBranches(true);
        system("pause");
        break;
    default:
        printf("ָ���������\n");
    }
    return LOOP;
}

STATUS menuSC(User& us) {
    int cmd = -1;
    printf("--�г�--\n");
    listGoods(-1);
    printf("0: ����\n");
    printf("1: ����\n");
    printf("2: ����\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:
        nextMenu = scMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("ָ����������\n");
    }
    return LOOP;
}
STATUS menuGUC(User& us) {
    int cmd = -1;
    printf("--���ﳵ--\n");
    listCart(us);
    printf("0: ����\n");
    printf("1: ����\n");
    printf("2: ɾ��\n");
    printf("3: ����\n");
    cmd = getCmd();
    STATUS(*nextMenu)(User&);
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:
        nextMenu = gucMenus[cmd - 1];
        while (1) {
            // ��һ���˵�
            STATUS s = nextMenu(us);
            if (s == EXIT) break;
        }
        break;
    default:
        printf("ָ����������\n");
    }
    return LOOP;
}
STATUS menuQB(User& us) {
    // TODO wallet
    return EXIT;
}

STATUS doAddUser(User& us) {
    cout << "----����û�----" << endl;
    cout << "��������ӵ�����(0: ����Ա��1��������)";
    int cmd = getCmd();
    if (cmd > 1 || cmd < 0) {
        cout << "���ͱ���������" << endl;
        return EXIT;
    }
    doRegister(cmd ? CONSUMER : ADMIN);
    return EXIT;
}
STATUS doDeleteUser(User& us) {
    cout << "----ɾ���û�----" << endl;
    listUsers();
    cout << "����ɾ�����û�����";
    string s;
    cin >> s;
    s = convUTF8(s);
    for (int i = 0; i < users.size(); i++) {
        if (users[i]["username"].get<string>() == s) {
            cout << "ȷ��ɾ��" << convGBK(s) << "��(0: ��1����)��" << endl;
            int cmd = getCmd();
            if (cmd != 1) {
                cout << "������" << endl;
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
    cout << "----�޸�����----" << endl;
    string s;
    cout << "�����޸�������û�����";
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
        cout << "�û�������" << endl;
        return EXIT;
    }
    string pass;
    while (1) {
        pass = getpass("���룺", true);
        if (!checkPass(pass)) {
            cout << "����̫�򵥣����������룡" << endl;
        }
        else break;
    }
    string con_pass = getpass("�ٴ�ȷ�����룺", true);
    if (con_pass != pass) {
        cout << "������������벻һ�£�ע��ʧ�ܣ�" << endl;
        return EXIT;
    }
    pass = sha512(pass);
    users[index]["password"] = pass;
    saveUsers(users);
    return EXIT;
}

STATUS doAddGoods(User& us) {
    cout << "----�����Ʒ----" << endl;
    string branch, id, name, isbn, source, description;
    float price;
    listBranches(false);
    cout << "��������ӵ��ķֵ��ţ�";
    cin >> branch;
    int branchIndex = findBranch(branch);
    if (branchIndex < 0) {
        cout << "�ֵ�����������" << endl;
        return EXIT;
    }
    cout << "��������Ʒ��ţ���Ʒ���ƣ���Ʒ���룬���أ��Կո���зָ\n";
    cin >> id >> name >> isbn >> source;
    id = convUTF8(id);
    name = convUTF8(name);
    isbn = convUTF8(isbn);
    source = convUTF8(source);

    Point p = findGoods(id, "id");
    if (p.i >= 0 || p.j >= 0) {
        cout << "����ظ���" << endl;
        return EXIT;
    }

    cout << "��������Ʒ�۸�";
    cin >> price;
    cout << "��������Ʒ������";
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
    cout << "----�¼���Ʒ----" << endl;
    listGoods(-1);
    string id;
    cout << "����Ҫ�¼ܵ���Ʒ��ţ�";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "��Ʒ�������" << endl;
        return EXIT;
    }
    goods[p.i][p.j]["avaliable"] = false;
    saveGoods(goods);
    return EXIT;
}
STATUS doEditGoods(User& us) {
    cout << "----�޸���Ʒ----" << endl;
    listGoods(-1);
    string id;
    cout << "����Ҫ�޸ĵ���Ʒ��ţ�";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "��Ʒ�������" << endl;
        return EXIT;
    }
    cout << "����Ҫ�޸ĵ���Ϣ��ţ�0�����ƣ�1�����룬2������:3���۸�4����������";
    int cmd = getCmd();
    string s;
    switch (cmd) {
    case 0:
        cout << "�������ƣ�";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["name"] = s;
        break;
    case 1:
        cout << "������룺";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["isbn"] = s;
        break;
    case 2:
        cout << "������أ�";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["source"] = s;
        break;
    case 3:
        cout << "����۸�";
        float price;
        cin >> price;
        goods[p.i][p.j]["price"] = price;
        break;
    case 4:
        cout << "����������";
        cin >> s;
        s = convUTF8(s);
        goods[p.i][p.j]["description"] = s;
        break;
    default:
        cout << "ָ������" << endl;
        return EXIT;
    }
    saveGoods(goods);
    return EXIT;
}
STATUS doBranchGoods(User& us) {
    cout << "----�¼���Ʒ----" << endl;
    listBranches(false);
    string id;
    cout << "����ֵ��ţ�";
    cin >> id;
    id = convUTF8(id);
    int index = findBranch(id);
    if (index < 0) {
        cout << "�ֵ�������" << endl;
        return EXIT;
    }
    listGoods(index);
    return EXIT;
}

STATUS doAddStorage(User& us) {
    cout << "----���ӿ��----" << endl;
    listGoods(-1);
    string id;
    cout << "����Ҫ��������Ʒ��ţ�";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "��Ʒ�������" << endl;
        return EXIT;
    }
    cout << "��Ʒ" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "���п��" << goods[p.i][p.j]["storage"].get<int>() << "���������������" << endl;
    int add;
    cin >> add;
    if (add < 0) {
        cout << "������л�����ֲ˵���" << endl;
        return EXIT;
    }
    goods[p.i][p.j]["storage"] = goods[p.i][p.j]["storage"].get<int>() + add;
    saveGoods(goods);

    return EXIT;
}
STATUS doClearStorage(User& us) {
    cout << "----���ٿ��----" << endl;
    listGoods(-1);
    string id;
    cout << "����Ҫ��ֵ���Ʒ��ţ�";
    cin >> id;
    Point p = findGoods(id, "id");
    if (p.i < 0 || p.j < 0) {
        cout << "��Ʒ�������" << endl;
        return EXIT;
    }
    cout << "��Ʒ" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "���п��" << goods[p.i][p.j]["storage"].get<int>() << "���������������" << endl;
    int add;
    cin >> add;
    if (add < 0) {
        cout << "�������л��������˵���" << endl;
        return EXIT;
    }
    goods[p.i][p.j]["storage"] = goods[p.i][p.j]["storage"].get<int>() - add;
    saveGoods(goods);

    return EXIT;
}
STATUS doCheckStorage(User& us) {
    cout << "----�����----" << endl;
    for (int i = 0; i < branches.size(); i++) {
        if (branches[i]["open"].get<bool>() == false) continue;
        cout << "--�ֵ꡾" << convGBK(branches[i]["name"].get<string>()) << "�������--" << endl;
        int alert_cnt = 0;
        for (int j = 0; j < goods[i].size(); j++) {
            bool alert = goods[i][j]["storage"].get<int>() < ALERT_LINE;
            if (alert) cout << "!!!";
            cout << "||" << convGBK(goods[i][j]["id"].get<string>()) << "||" << convGBK(goods[i][j]["name"].get<string>());
            cout << "||" << goods[i][j]["storage"].get<int>() << "||";
            if (alert) cout << "!!!(���棡�����٣�)" << endl;
            else
                cout << endl;
            alert_cnt += alert;
        }
        if (alert_cnt > 0) cout << "��" << alert_cnt << "����Ʒ������㣡" << endl;
    }
    return EXIT;
}

STATUS doAddBranch(User& us) {
    cout << "----��ӷֵ�----" << endl;
    string id, name, description;
    cout << "������Ҫ��ӵķֵ��ţ�";
    cin >> id;
    id = convUTF8(id);
    if (findBranch(id) >= 0) {
        cout << "����ظ���" << endl;
        return EXIT;
    }
    cout << "������Ҫ��ӵķֵ����ƣ�";
    cin >> name;
    name = convUTF8(name);
    cout << "������ֵ�������";
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
    cout << "----��ͣ�ֵ�----" << endl;
    string id;
    cout << "����Ҫ��ͣ�ķֵ���(�ٴιرտɿ����ֵ�)��";
    cin >> id;
    id = convUTF8(id);
    int index = findBranch(id);
    if ( index < 0) {
        cout << "�������" << endl;
        return EXIT;
    }
    branches[index]["open"] = !(branches[index]["open"].get<bool>());
    saveBranches(branches);
    return EXIT;
}

STATUS doBuy(User& us) {
    cout << "----��ӵ����ﳵ----" << endl;
    cout << "��������Ҫ�������Ʒ��ţ�";
    string id;
    Point gp;
    cin >> id;
    id = convUTF8(id);
    gp = findGoods(id,"id");
    if (gp.i < 0 || gp.j < 0) {
        cout << "��������û�������Ʒ��" << endl;
        return EXIT;
    }
    float price = goods[gp.i][gp.j]["price"].get<float>();
    cout << "��Ҫ�򼸸��أ���";
    int goodCnt;
    cin >> goodCnt;
    if (goodCnt < 0) {
        cout << "������~���ǲ��ջ�Ŷ~" << endl;
        return EXIT;
    }
    else if (goodCnt == 0) return EXIT;
    cout << "����" << goodCnt << "������"<< convGBK(goods[gp.i][gp.j]["name"].get<string>()) <<"�����ܼ�" << goodCnt*price << "Ԫ����ӵ����ﳵ��(0: ȡ����1��ȷ��)��";
    int cmd = getCmd();
    if (cmd != 1) {
        cout << "������ӣ�" << endl;
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
    cout << "----������Ʒ----" << endl;
    string field, value;
    cout << "�����������ݱ�ţ�0����ţ�1�����ƣ�2�����룩��";
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
        cout << "ָ������" << endl;
        return EXIT;
    }
    cout << "Ŀ��ֵ��";
    cin >> value;
    value = convUTF8(value);
    vector<Point> res = findAllGoods(value, field);
    for (int i = 0; i < res.size(); i++) {
        Point p = res[i];
        if (p.i < 0 || p.j < 0) {
            cout << "��Ʒ�������" << endl;
            return EXIT;
        }
        cout << "��Ʒ���ڷֵ꣺" << convGBK(branches[p.i]["name"].get<string>()) << endl;
        cout << "||";
        cout << "��ţ�" << convGBK(goods[p.i][p.j]["id"].get<string>()) << "||";
        cout << "���ƣ�" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "||";
        cout << "���룺" << convGBK(goods[p.i][p.j]["isbn"].get<string>()) << "||";
        cout << "���أ�" << convGBK(goods[p.i][p.j]["source"].get<string>()) << "||";
        cout << "�۸�" << (goods[p.i][p.j]["price"].get<double>()) << "||";
        cout << "��棺" << (goods[p.i][p.j]["storage"].get<int>()) << "||";
        cout << endl;
    }
    if (res.size() <= 0) {
        cout << "û���ҵ���������Ʒ��" << endl;
    }
    system("pause");
    return EXIT;
}

STATUS doPay(User& us) {
    int uid = findUser(us.username);
    if (uid < 0) return EXIT;
    float money = users[uid]["wallet"]["money"].get<float>() , coupons = users[uid]["wallet"]["coupons"].get<float>();
    json cart = users[uid]["cart"];

    cout << "----����----" << endl;
    cout << "��������Ҫ����Ķ�����ţ�";
    int cid = getCmd();
    if (cid < 0 || cid >= cart.size()) {
        cout << "�������" << endl;
        return EXIT;
    }
    string id = cart[cid]["id"].get<string>();
    Point gp;
    gp = findGoods(id, "id");
    if (gp.i < 0 || gp.j < 0 || goods[gp.i][gp.j]["avaliable"].get<bool>() == false) {
        cout << "��Ʒ�¼���~" << endl;
        return EXIT;
    }
    json good = goods[gp.i][gp.j];
    float price = good["price"].get<float>();
    int num = cart[cid]["num"].get<int>();
    if (num*price > coupons + money) {
        cout << "Ǯ��̫����Ŷ~��ǰȥ��ֵ��" << endl;
        return EXIT;
    }
    cout << "����"<< convGBK(good["name"].get<string>()) <<"�����ϼơ�"<< num*price <<"��Ԫ��ȷ�ϸ�����(0:ȡ����1:ȷ��)" << endl;
    int cmd = getCmd();
    if (cmd != 1) {
        cout << "�������" << endl;
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

    cout << "----ɾ��----" << endl;
    cout << "��������Ҫɾ���Ķ�����ţ�";
    int cid = getCmd();
    if (cid < 0 || cid >= cart.size()) {
        cout << "�������" << endl;
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

    cout << "----��������----" << endl;
    cout << "��������Ҫ�����Ķ�����ţ�";
    int cid = getCmd();
    if (cid < 0 || cid >= cart.size()) {
        cout << "�������" << endl;
        return EXIT;
    }
    cout << "��Ҫ�ĳɼ����أ���";
    int goodCnt;
    cin >> goodCnt;
    if (goodCnt < 0) {
        cout << "������~���ǲ��ջ�Ŷ~" << endl;
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
    cout << "----�û��б�----" << endl;
    for (int i = 0; i < users.size(); i++) {
        cout << "||";
        cout << "�û�����" << convGBK(users[i]["username"].get<string>()) << "||";
        cout << "���ͣ�" << ((users[i]["auth"].get<int>() > 1) ? "����Ա" : "�˿�") << "||";
        cout << "Ǯ����" << users[i]["wallet"]["money"].get<float>() << "||";
        cout << "��ȯ��" << users[i]["wallet"]["coupons"].get<float>() << "||";
        cout << "���������" << users[i]["purchase_log"].size() << "||";
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
        cout << "----�ֵ꡾" << convGBK(branches[bi]["name"].get<string>()) << "����Ʒ���----" << endl;
        for (int i = 0; i < goods[bi].size(); i++) if (goods[bi][i]["avaliable"].get<bool>()) {
            cout << "||";
            cout << "��ţ�" << convGBK(goods[bi][i]["id"].get<string>()) << "||";
            cout << "���ƣ�" << convGBK(goods[bi][i]["name"].get<string>()) << "||";
            cout << "���룺" << convGBK(goods[bi][i]["isbn"].get<string>()) << "||";
            cout << "���أ�" << convGBK(goods[bi][i]["source"].get<string>()) << "||";
            cout << "�۸�" << (goods[bi][i]["price"].get<double>()) << "||";
            cout << "��棺" << (goods[bi][i]["storage"].get<int>()) << "||";
            cout << endl;
        }
    }
}
void listBranches(bool all) {
    cout << "----�ֵ��б�----" << endl;
    for (int i = 0; i < branches.size(); i++) if (all || branches[i]["open"].get<bool>()) {
        cout << "||";
        cout << "��ţ�" << convGBK(branches[i]["id"].get<string>()) << "||";
        cout << "�ֵ�����" << convGBK(branches[i]["name"].get<string>()) << "||";
        cout << "������" << convGBK(branches[i]["description"].get<string>()) << "||";
        if (all) {
            cout << "״̬��" << (branches[i]["open"].get<bool>() ? "Ӫҵ" : "��ͣ") << "||";
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
        cout << "���ţ�" << i << "||";
        cout << "��Ʒ��ţ�" << convGBK(id) << "||";
        cout << "��Ʒ���ƣ�" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "||";
        cout << "����������" << logs[i]["num"].get<int>() << "||";
        cout << "�ܼۣ�" << logs[i]["total"].get<float>() << "||";
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
        cout << "���ţ�" << i << "||";
        cout << "��Ʒ��ţ�" << convGBK(id) << "||";
        cout << "��Ʒ���ƣ�" << convGBK(goods[p.i][p.j]["name"].get<string>()) << "||";
        cout << "����������" << cart[i]["num"].get<int>() << "||";
        cout << "�ܼۣ�" << cart[i]["num"].get<int>() * goods[p.i][p.j]["price"].get<float>() << "||";
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
