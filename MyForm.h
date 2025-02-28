#pragma once
#include <windows.h>
#using <System.Windows.Forms.dll>

#include <vcclr.h>
#using <System.Windows.Forms.dll>
#using <Windows.UI.Input.Preview.Injection.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace Windows::UI::Input::Preview::Injection;

#pragma comment(lib, "User32.lib")  // Link User32.lib to resolve external reference

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
    void InjectPenEvent(int x, int y, float pressure) {
        static PenInputInjector^ penInjector = PenInputInjector::TryCreate();
        if (penInjector != nullptr) {
            PenInjectionPoint^ point = ref new PenInjectionPoint(x, y);
            PenInjectionEvent^ penEvent = ref new PenInjectionEvent(
                PenInjectionEventKind::PenDown,
                point,
                pressure
            );
            penInjector->InjectPenInput(penEvent);
        }
        else {
            MessageBox::Show("Failed to initialize pen injector.");
        }
    }

    void OnButtonClick(Object^ sender, EventArgs^ e) {
        //MessageBox::Show("Hello, Windows Forms in C++/CLI!");
        SetCursorPos(-100, 200);
    }
};
