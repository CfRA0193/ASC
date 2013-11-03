//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* password_form.h                                    *
//* Дата:    10.09.2005                                *
//* Версия:  1.2.0.1	                               *
//* Автор:   Дробанов Артём Федорович (DrAF)           *
//******************************************************

//----------------------------------------------------------------------------
#ifndef password_formH
#define password_formH
//----------------------------------------------------------------------------
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
//----------------------------------------------------------------------------
class TPasswordDlg : public TForm
{
__published:
        TLabel *PasswordLabel;
	TEdit *Password;
	TButton *OKBtn;
	TButton *CancelBtn;
private:
public:
	virtual __fastcall TPasswordDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TPasswordDlg *PasswordDlg;
//----------------------------------------------------------------------------
#endif    
