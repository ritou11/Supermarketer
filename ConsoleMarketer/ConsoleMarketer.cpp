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

/*************************************************************
** �����ļ�
** ��ʵ��Ӧ�õĳ����У�����ʹ��mongodb�����������ݹ���ģʽ
** �˴�Ϊ�˱���mongodb�������⣬ʹ���ļ��������ݴ���
*/
#define USER_FILE "user.json"
#define GOODS_FILE "goods.json"
#define BRANCH_FILE "branch.json"
#define CART_FILE "cart.json"
//************************************************************

/*************************************************************
** �ڴ��е����ݴ洢
** ���ļ��е����ݶ�ȡ���ڴ��У�����Ƶ����Ӳ�̶�д����
** ���������ݹ�ģ��Сʱ
*/
json users;
json goods;
json carts;
json branchs;
//************************************************************

// �˵��������صĿ���״̬
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

STATUS menu1(User& us) {
    int cmd = -1;
    printf("��ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n");
    printf("0: ��¼\n");
    printf("1: ע��\n");

    cin >> cmd;

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
    printf("----�û�����----\n");
    listUsers();
    printf("0: ����\n");
    printf("1: �û�����\n");
    printf("2: ��Ʒ��Ϣ����\n");
    printf("3: ������\n");
    printf("4: �ֵ����\n");
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
    printf("��ӭ����Ա%s����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n", us.username);
    printf("0: ע��\n");
    printf("1: �û�����\n");
    printf("2: ��Ʒ��Ϣ����\n");
    printf("3: ������\n");
    printf("4: �ֵ����\n");

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
    printf("��ӭ�˿�%s����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n", us.username);
    printf("0: ע��\n");
    printf("1: �г�\n");
    printf("2: ���ﳵ\n");
    printf("3: Ǯ��\n");

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

}

void loadUsers(json& u) {
    std::ifstream fin(USER_FILE);
    fin >> u;
    fin.close();
}

void loadGoods(json& g) {

}