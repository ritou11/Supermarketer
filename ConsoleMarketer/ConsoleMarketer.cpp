// ConsoleMarketer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "utils.h"
#include "User.h"
#define VERSION "V1.0"
#define USER_FILE "user.json"

bool doLogin(User& us) {
    cout << "-��½"<<endl;
    cout << "--�û�����";
    cin >> us.username;
    string password = getpass("--���룺");
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
    printf("��ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n");
    printf("0: ��¼\n");
    printf("1: ע��\n");
    scanf("%d", &cmd);
    fflush(stdin);
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

STATUS menu2(User& us) {
    int cmd = -1;
    printf("��ӭ����Ա%s����ѡ��˵��еĹ��ܱ�ţ����س���ȷ�ϣ�\n");
    printf("0: ע��\n");
    printf("1: �û�����\n");
    printf("2: ��Ʒ��Ϣ����\n");
    printf("3: ������\n");
    printf("4: �ֵ����\n");
    
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
        printf("ָ����������\n");
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
        printf("            ��ӭ����Ц��������������              \n");
        printf("                ϵͳ�汾��%s                       \n", VERSION);
        printf("****************************************************\n");
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
                STATUS s = menu2(us);
                if (s == EXIT) break;
            }
        }
        else {
            while (1) {
                // �����߶���˵�
                STATUS s = menu3(us);
                if (s == EXIT) break;
            }
        }
        system("pause");
    }
    return 0;
}

