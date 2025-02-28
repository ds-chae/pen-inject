#pragma once
#include <windows.h>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

public ref class MyForm : public Form {
public:
    MyForm() {
        this->Text = "C++/CLI WinForms App";
        this->Width = 400;
        this->Height = 300;

        Button^ btn = gcnew Button();
        btn->Text = "Click Me";
        btn->Location = System::Drawing::Point(150, 100);
        btn->Click += gcnew EventHandler(this, &MyForm::OnButtonClick);

        this->Controls->Add(btn);
    }

private:
    void OnButtonClick(Object^ sender, EventArgs^ e) {
        MessageBox::Show("Hello, Windows Forms in C++/CLI!");
    }
};
