//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* about_form.h                                       *
//* Дата:    10.09.2005                                *
//* Версия:  1.2.0.1	                               *
//* Автор:   Дробанов Артём Федорович (DrAF)           *
//******************************************************

//---------------------------------------------------------------------------
#ifndef about_formH
#define about_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:	// IDE-managed Components
        TButton *OKButton;
        TImage *AboutImage;
        TBevel *AboutBevel;
        TStaticText *VersionStaticText;
        TStaticText *CopyrightStaticText;
        TStaticText *ProtectionVersionStaticText;
        TStaticText *ProtectionVersionStaticText2;
        void __fastcall OKButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif
