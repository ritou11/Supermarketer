#pragma once
#include "Utils.h"
namespace Supermarketer {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Security::Cryptography;

    /// <summary>
    /// Login ժҪ
    /// </summary>
    public ref class Login : public System::Windows::Forms::Form
    {
    public:
        Login(void)
        {
            InitializeComponent();
            //
            //TODO:  �ڴ˴���ӹ��캯������
            //
        }

    protected:
        /// <summary>
        /// ������������ʹ�õ���Դ��
        /// </summary>
        ~Login()
        {
            if (components)
            {
                delete components;
            }
        }
    private: System::Windows::Forms::Label^  label1;
    protected:
    private: System::Windows::Forms::Label^  label2;
    private: System::Windows::Forms::Button^  button1;
    private: System::Windows::Forms::Button^  button2;
    private: System::Windows::Forms::TextBox^  txtUsername;
    private: System::Windows::Forms::TextBox^  txtPassword;
    private: System::Windows::Forms::RadioButton^  radConsu;
    private: System::Windows::Forms::RadioButton^  radAdmin;






    private:
        /// <summary>
        /// ����������������
        /// </summary>
        System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
        /// <summary>
        /// �����֧������ķ��� - ��Ҫ�޸�
        /// ʹ�ô���༭���޸Ĵ˷��������ݡ�
        /// </summary>
        void InitializeComponent(void)
        {
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->button2 = (gcnew System::Windows::Forms::Button());
            this->txtUsername = (gcnew System::Windows::Forms::TextBox());
            this->txtPassword = (gcnew System::Windows::Forms::TextBox());
            this->radConsu = (gcnew System::Windows::Forms::RadioButton());
            this->radAdmin = (gcnew System::Windows::Forms::RadioButton());
            this->SuspendLayout();
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Font = (gcnew System::Drawing::Font(L"����", 12));
            this->label1->Location = System::Drawing::Point(22, 47);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(56, 16);
            this->label1->TabIndex = 0;
            this->label1->Text = L"�û���";
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->Font = (gcnew System::Drawing::Font(L"����", 12));
            this->label2->Location = System::Drawing::Point(22, 80);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(40, 16);
            this->label2->TabIndex = 1;
            this->label2->Text = L"����";
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(79, 102);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(80, 27);
            this->button1->TabIndex = 3;
            this->button1->Text = L"��¼";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &Login::button1_Click);
            // 
            // button2
            // 
            this->button2->Location = System::Drawing::Point(165, 102);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(80, 27);
            this->button2->TabIndex = 4;
            this->button2->Text = L"�˳�";
            this->button2->UseVisualStyleBackColor = true;
            this->button2->Click += gcnew System::EventHandler(this, &Login::button2_Click);
            // 
            // txtUsername
            // 
            this->txtUsername->Location = System::Drawing::Point(79, 42);
            this->txtUsername->Name = L"txtUsername";
            this->txtUsername->Size = System::Drawing::Size(166, 21);
            this->txtUsername->TabIndex = 1;
            // 
            // txtPassword
            // 
            this->txtPassword->Location = System::Drawing::Point(79, 75);
            this->txtPassword->Name = L"txtPassword";
            this->txtPassword->PasswordChar = '*';
            this->txtPassword->Size = System::Drawing::Size(166, 21);
            this->txtPassword->TabIndex = 2;
            // 
            // radConsu
            // 
            this->radConsu->AutoSize = true;
            this->radConsu->Checked = true;
            this->radConsu->Location = System::Drawing::Point(79, 20);
            this->radConsu->Name = L"radConsu";
            this->radConsu->Size = System::Drawing::Size(59, 16);
            this->radConsu->TabIndex = 5;
            this->radConsu->TabStop = true;
            this->radConsu->Text = L"������";
            this->radConsu->UseVisualStyleBackColor = true;
            // 
            // radAdmin
            // 
            this->radAdmin->AutoSize = true;
            this->radAdmin->Location = System::Drawing::Point(186, 20);
            this->radAdmin->Name = L"radAdmin";
            this->radAdmin->Size = System::Drawing::Size(59, 16);
            this->radAdmin->TabIndex = 6;
            this->radAdmin->Text = L"����Ա";
            this->radAdmin->UseVisualStyleBackColor = true;
            // 
            // Login
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(283, 152);
            this->Controls->Add(this->radAdmin);
            this->Controls->Add(this->radConsu);
            this->Controls->Add(this->txtPassword);
            this->Controls->Add(this->txtUsername);
            this->Controls->Add(this->button2);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->label2);
            this->Controls->Add(this->label1);
            this->MaximizeBox = false;
            this->MaximumSize = System::Drawing::Size(299, 191);
            this->MinimumSize = System::Drawing::Size(299, 191);
            this->Name = L"Login";
            this->Text = L"��¼";
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
        this->Close();
    }
    private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
        String^ username = txtUsername->Text;
        String^ md5password = Utils::getMD5String(txtPassword->Text);

    }
    };
}
