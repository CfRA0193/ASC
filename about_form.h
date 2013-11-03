//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* about_form.h                                       *
//* ����:    03.11.2013                                *
//* ������:  1.4.0.0	                               *
//* �����:   �������� ���� ��������� (DrAF)           *
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
