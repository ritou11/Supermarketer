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

// ʹ�ú���ָ�룬�����ƵĲ˵�����
STATUS(*adminMenus[])(User&) = { menuYHGL, menuSPGL, menuKCGL, menuFDGL };
STATUS(*consumerMenus[])(User&) = { menuSC, menuGUC, menuQB };
STATUS(*admin_con_menus[])(User&) = {menuAdmin, menuConsumer};
STATUS(*yhglMenus[])(User&) = { doAddUser, doDeleteUser, doEditPwd };
STATUS(*spglMenu[])(User&) = { menuAddGoods ,menuDownGoods ,menuEditGoods ,menuBranchGoods };
STATUS(*kcglMenu[])(User&) = { menuAddStorage, menuClearStorage, menuCheckStorage};
STATUS(*fdglMenu[])(User&) = { menuAddBranch , menuStopBranch };
STATUS(*gucMenu[])(User&) = { menuPay , menuDelete , menuAdjust };
STATUS(*scMenu[])(User&) = { menuBuy , menuSearch };

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
            STATUS s = admin_con_menus[us.type==CONSUMER](us);
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
    listGoods();
    printf("0: ����\n");
    printf("1: �����Ʒ\n");
    printf("2: �¼���Ʒ\n");
    printf("3: �޸���Ʒ\n");
    printf("4: �ֵ����\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:case 4:

        break;
    default:
        printf("ָ���������\n");
    }
    return LOOP;
}
STATUS menuKCGL(User& us) {
    int cmd = -1;
    printf("--������--\n");
    listBranches(false);
    printf("0: ����\n");
    printf("1: ����\n");
    printf("2: ���\n");
    printf("3: ���\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:

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
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:case 3:

        break;
    default:
        printf("ָ���������\n");
    }
    return LOOP;
}

STATUS menuSC(User& us) {
    int cmd = -1;
    printf("--�г�--\n");
    listGoods();
    printf("0: ����\n");
    printf("1: ����\n");
    printf("2: ����\n");
    cmd = getCmd();
    switch (cmd) {
    case 0:
        return EXIT;
    case 1:case 2:

    default:
        printf("ָ����������\n");
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
    cout << "----����û�----" << endl;
    cout << "��������ӵ�����(0: ����Ա��1��������)";
    int cmd = getCmd();
    if (cmd > 1 || cmd < 0) {
        cout <<"���ͱ���������"<<endl;
        return EXIT;
    }    
    doRegister(cmd ? CONSUMER:ADMIN);
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
void listGoods() {
    // TODO List Goods
}
void listBranches(bool all) {
    cout << "----�ֵ��б�----" << endl;
    for (int i = 0; i < branches.size(); i++) if(all || branches[i]["open"].get<bool>()){
        cout << "||";
        cout << "��ţ�" << convGBK(branches[i]["id"].get<string>()) << "||";
        cout << "�ֵ�����" << convGBK(branches[i]["name"].get<string>()) << "||";
        cout << "������" << convGBK(branches[i]["description"].get<string>()) << "||";
        if (all) {
            cout << "״̬��" << (branches[i]["open"].get<bool>()? "Ӫҵ":"��ͣ") << "||";
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
