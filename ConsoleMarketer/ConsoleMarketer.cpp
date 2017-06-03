// ConsoleMarketer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "utils.h"
#include "User.h"

/*************************************************************
** ����汾
** ��ǰ汾��Ϣ������BANNER��ʾ
*/
#define VERSION "V1.0"
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
        if (us.type == ADMIN) {
            while (1) {
                // ����Ա����˵�
                STATUS s = menuAdmin(us);
                if (s == EXIT) break;
            }
        }
        else {
            while (1) {
                // �����߶���˵�
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
    printf("            ��ӭ����Ц��������������              \n");
    printf("                ϵͳ�汾��%s                       \n", VERSION);
    printf("****************************************************\n");
}

bool doLogin(User& us) {
    cout << "-��½" << endl;
    cout << "--�û�����";
    cin >> us.username;
    string password = getpass("--���룺");
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
int getCmd() {
    int cmd;
    cout << "���빦�ܱ�ţ�";
    cin >> cmd;
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
        return EXIT;
    }
    else {
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
    return EXIT;
}

STATUS menuSPXXGL(User& us) {
    int cmd = -1;
    printf("--��Ʒ����--\n");
    listGoods();
    printf("0: ����\n");
    printf("1: �����Ʒ\n");
    printf("2: �¼���Ʒ\n");
    printf("3: �޸���Ʒ\n");
    printf("4: �ֵ����\n");
    cmd = getCmd();
    return EXIT;
}

STATUS menuKCGL(User& us) {
    int cmd = -1;
    printf("--������--\n");
    listBranches();
    printf("0: ����\n");
    printf("1: ����\n");
    printf("2: ���\n");
    printf("3: ���\n");
    cmd = getCmd();
    return EXIT;
    return EXIT;
}
STATUS menuFDGL(User& us) {
    int cmd = -1;
    printf("--�ֵ����--\n");
    listBranches();
    printf("0: ����\n");
    printf("1: ��ӷֵ�\n");
    printf("2: �رշֵ�\n");
    cmd = getCmd();
    return EXIT;
}
STATUS menuAdmin(User& us) {
    int cmd = -1;
    cout << "��ӭ����Ա" << us.username << "����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n";
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
    cout << "��ӭ�˿�" << us.username << "����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n";
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
}
void listBranches() {
    cout << "----�ֵ��б�----" << endl;
    for (int i = 0; i < branches.size(); i++) {
        cout << "||";
        cout << "��ţ�" << convGBK(branches[i]["id"].get<string>()) << "||";
        cout << "�ֵ�����" << convGBK(branches[i]["name"].get<string>()) << "||";
        cout << "������" << convGBK(branches[i]["description"].get<string>()) << "||";
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
}
void saveGoods(vector<json>& g) {
}
void saveBranches(json& bs) {
}
