//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* main_form.h                                        *
//* ����:    03.11.2013                                *
//* ������:  1.4.0.0	                               *
//* �����:   �������� ���� ��������� (DrAF)           *
//******************************************************

//---------------------------------------------------------------------------
#ifndef main_formH
#define main_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ScktComp.hpp>
#include "trayicon.h"
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <ImgList.hpp>
#include <sstream>
//---------------------------------------------------------------------------
// ���������� ����� ���������� �� ��������� InKey 5.0
#include "inKey.cpp"
//---------------------------------------------------------------------------
// ���������� ����� ����������� �� ��������� DHash 2.03
#include "DHash.cpp"
//---------------------------------------------------------------------------
#define TO_READ 1                       // ������� ������� FileErrorMessage
#define TO_WRITE 2                      // ������� ������� FileErrorMessage
#define MAX_NAME_LENGTH 215             // ������������ ����� ����� �����
#define INT_LENGTH 4                    // ���-�� ���� � int
#define MAX_ENCRYPTION_BLOCK_SIZE 8192  // ����. ������ ����� ��� ����������
//---------------------------------------------------------------------------
// union ��� �������������� int <=> char[4]
union TIntToRGB
{
        long IntVar;
        char rgbVar[4];
};
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu;
        TClientSocket *ClientSocket;
        TServerSocket *ServerSocket;
        TTrayIcon *TrayIcon;
        TMemo *ChatMemo;
        TEdit *ChatEdit;
        TStatusBar *StatusBar;
        TMenuItem *File;
        TMenuItem *Help;
        TMenuItem *FileConnectItem;
        TMenuItem *FileDisconnectItem;
        TMenuItem *FileSetPasswordFileItem;
        TMenuItem *FileListenItem;
        TMenuItem *N1;
        TMenuItem *FileExitItem;
        TMenuItem *HelpAboutItem;
        TSpeedButton *ConnectSpeedButton;
        TSpeedButton *ListenSpeedButton;
        TSpeedButton *ListenOffImage;
        TSpeedButton *ListenOnImage;
        TSpeedButton *DisconnectSpeedButton;
        TSpeedButton *SetPasswordFileSpeedButton;
        TOpenDialog *OpenDialog;
        TImage *NormalIconImage;
        TImage *BlinkIconImage;
        TImageList *IconsImageList;
        TTimer *IconBlinkTimer;
        TMenuItem *Edit;
        TMenuItem *EditTCPPort;
        TMenuItem *N2;
        TMenuItem *EditClear;
        TMenuItem *Protection;
        TMenuItem *N8192bit;
        TMenuItem *N16384bit;
        TMenuItem *N24576bit;
        TMenuItem *N32768bit;
        TPopupMenu *PopupMenu;
        TMenuItem *Clear;
        TMenuItem *N4;
        TMenuItem *Exit;
        TMenuItem *N5;
        TMenuItem *PasswordMode;
        void __fastcall FileListenItemClick(TObject *Sender);
        void __fastcall ListenSpeedButtonClick(TObject *Sender);
        void __fastcall FileExitItemClick(TObject *Sender);
        void __fastcall HelpAboutItemClick(TObject *Sender);
        void __fastcall ConnectSpeedButtonClick(TObject *Sender);
        void __fastcall FileConnectItemClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ChangeListenCondition(TObject *Sender);
        void __fastcall ConnectTo(TObject *Sender);
        void __fastcall ClientSocketConnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall Disconnect(TObject *Sender);
        void __fastcall FileDisconnectItemClick(TObject *Sender);
        void __fastcall DisconnectSpeedButtonClick(TObject *Sender);
        void __fastcall ClientSocketRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ServerSocketClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ServerSocketAccept(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ClientSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
        void __fastcall EncodeSendBuffer(TObject *Sender);
        void __fastcall DecodeSendBuffer(TObject *Sender);
        void __fastcall ChatEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FileErrorMessage(char *szFilename,int ErrKind);
        void __fastcall ConvertAnsiStringToChar (AnsiString asStr, char *pCh);
        long __fastcall FileLength(char *szFileName);
        FILE* __fastcall OpenFileToRead (char *szFilename,long &lFileSize);
        void __fastcall SetPasswordFileSpeedButtonClick(TObject *Sender);
        void __fastcall WipeData (char *prgbData,long lDataSize);
        void __fastcall AllocateObjects();
        void __fastcall FreeObjects();
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall IconBlinkTimerTimer(TObject *Sender);
        void __fastcall TrayIconClick(TObject *Sender);
        void __fastcall TrayIconMinimize(TObject *Sender);
        void __fastcall EditClearClick(TObject *Sender);
        void __fastcall EditTCPPortClick(TObject *Sender);
        void __fastcall N8192bitClick(TObject *Sender);
        void __fastcall N16384bitClick(TObject *Sender);
        void __fastcall N24576bitClick(TObject *Sender);
        void __fastcall N32768bitClick(TObject *Sender);
        void __fastcall ProtectionResetClick(TObject *Sender);
        void __fastcall ClearClick(TObject *Sender);
        void __fastcall ExitClick(TObject *Sender);
        void __fastcall WipeVCL(TObject *Sender);
        void __fastcall PasswordModeClick(TObject *Sender);
        void __fastcall FileSetPasswordFileItemClick(TObject *Sender);
private:	// User declarations
        bool bIsServer;
        String Server;
        String Port;
        int EncryptionBlockSize; // ������ ����� ��� ����������
        // ������������ ������ ������?
        bool bProtection;
        // ���������� �������� � ����?
        bool bInTray;
        // ���������� ������������ ������?
        bool bUsingKeybdPassword;
        // ��������� �����
        char *prgbPasswordBuffer;
        // ������ ���������� ������
        long int lPasswordSize;
        // ����� ��� ������ �������������� �������
        char *prgbSendBuffer;      // ������ 1028 ��! (char* data + int)
        // ��������� �����2
        char *prgbPasswordBuffer2; // ������ 1024 ��.
        // union ��� �������������� int <=> char[4]
        TIntToRGB IntToRGB;
        // ��������� ������ �����������
        CDHash *pDHash;
        // ��������� ������ ����������
        CInKey *pInKey;
        // �������������� ������ ��� ���������� � Memo
        AnsiString asDecodedStr;

public:		// User declarations

        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
