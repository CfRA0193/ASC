//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* about_form.cpp                                     *
//* ����:    03.11.2013                                *
//* ������:  1.4.0.0	                               *
//* �����:   �������� ���� ��������� (DrAF)           *
//******************************************************

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "about_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::OKButtonClick(TObject *Sender)
{
        AboutForm->Close();        
}
//---------------------------------------------------------------------------
