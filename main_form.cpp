//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* main_form.cpp                                      *
//* ����:    4.02.2006                                 *
//* ������:  1.3.0.3	                               *
//* �����:   �������� ���� ��������� (DrAF)           *
//******************************************************

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "main_form.h"
#include "about_form.h"
#include "password_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
        bIsServer = false;
        bProtection = false;
        bUsingKeybdPassword = true;
        lPasswordSize = 0;
        prgbPasswordBuffer = NULL;
        prgbSendBuffer = NULL;
        prgbPasswordBuffer2 = NULL;
        pInKey = NULL;
}
//---------------------------------------------------------------------------
// �������� ��������� Listen...
void __fastcall TMainForm::ChangeListenCondition(TObject *Sender)
{
        // ���� � ������ ������ �������...
        if (FileListenItem->Checked)
        {
               //... ������ ������ �� ���������������
               ListenSpeedButton->Glyph = ListenOffImage->Glyph;
        } else
        {
             ListenSpeedButton->Glyph = ListenOnImage->Glyph;
        }
        FileListenItem->Checked = !FileListenItem->Checked;

        if (FileListenItem->Checked)
        {
                ClientSocket->Close();
                ServerSocket->Open();
                StatusBar->Panels->Items[0]->Text = " Mode: Server";
                StatusBar->Panels->Items[2]->Text = " Connected to:";
        }else
        {
                if (ServerSocket->Active)
                {
                        ServerSocket->Close();
                }
                StatusBar->Panels->Items[0]->Text = " Mode: Idle";
                StatusBar->Panels->Items[2]->Text = " Connected to:";
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FileListenItemClick(TObject *Sender)
{
        // �������� ��������� Listen...
        ChangeListenCondition(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListenSpeedButtonClick(TObject *Sender)
{
        // �������� ��������� Listen...
        ChangeListenCondition(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FileExitItemClick(TObject *Sender)
{
        // ��������� ���������
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::HelpAboutItemClick(TObject *Sender)
{
        // ���������� ������ About
        AboutForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ConnectTo(TObject *Sender)
{
  // ������ ������ IP-������...
  if (InputQuery("Connect to...", "Address Name:", Server))
  {
     // ���� ������ ���� �� ������...
     if (Server.Length() > 0)
     {
       // ���� � ������ ������ ���� �� ��������, ���� ����������...
       if (ClientSocket->Active)
       {
                ClientSocket->Close();
       }
        // �������������� ��������� ������...
        ClientSocket->Host = Server;
        ClientSocket->Open();

        FileListenItem->Checked = false;
        ListenSpeedButton->Glyph=MainForm->ListenOffImage->Glyph;
        StatusBar->Panels->Items[0]->Text = " Mode: Client";
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FileConnectItemClick(TObject *Sender)
{
        ConnectTo(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ConnectSpeedButtonClick(TObject *Sender)
{
        ConnectTo(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
        ServerSocket->Close();
        ClientSocket->Close();

        // ������� ���...
        WipeVCL(Sender);

        // ������������ �������
        FreeObjects();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientSocketConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
      StatusBar->Panels->Items[2]->Text = " Connected to: " + Socket->RemoteHost;       
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Disconnect(TObject *Sender)
{
        // ��������� ��� ����������...
        ClientSocket->Close();
        ServerSocket->Close();

        FileListenItem->Checked = false;
        ListenSpeedButton->Glyph = ListenOffImage->Glyph;

        StatusBar->Panels->Items[0]->Text = " Mode: Idle";
        bProtection = false;
        StatusBar->Panels->Items[1]->Text = " Protection: NO";
        StatusBar->Panels->Items[2]->Text = " Connected to:";

        // ������������ �������
        FreeObjects();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FileDisconnectItemClick(TObject *Sender)
{
        Disconnect(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisconnectSpeedButtonClick(TObject *Sender)
{
        Disconnect(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientSocketRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
        // ���� ���� ���������� �������� � ����, ��������� �� ��, ���
        // ������ ����� ���������...
        if (bInTray)
        {
                IconBlinkTimer->Enabled = true;
        }

        // ���� �������� � ������������ ������
        if (!bProtection)
        {
                ChatMemo->Lines->Add(TimeToStr(Time()) + " (+) " + Socket->ReceiveText());
        } else
        {
                // ��������� ������...
                Socket->ReceiveBuf(prgbSendBuffer,(EncryptionBlockSize + INT_LENGTH));
                //... � ����������
                DecodeSendBuffer(Sender);
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ServerSocketClientRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
        // ���� ���� ���������� �������� � ����, ��������� �� ��, ���
        // ������ ����� ���������...
        if (bInTray)
        {
                IconBlinkTimer->Enabled = true;
        }

        // ���� �������� � ������������ ������
        if (!bProtection)
        {
                ChatMemo->Lines->Add(TimeToStr(Time()) + " (+) " + Socket->ReceiveText());
        } else
        {
                Socket->ReceiveBuf(prgbSendBuffer,(EncryptionBlockSize + INT_LENGTH));
                DecodeSendBuffer(Sender);
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ServerSocketAccept(TObject *Sender,
      TCustomWinSocket *Socket)
{
        bIsServer = true;
        StatusBar->Panels->Items[2]->Text = " Connected to: " + Socket->RemoteAddress;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientSocketError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
        ChatMemo->Lines->Add("Error connecting to: " + Server);
        StatusBar->Panels->Items[0]->Text = " Mode: Idle";
        StatusBar->Panels->Items[2]->Text = " Connected to:";
        ErrorCode = 0;
}
//---------------------------------------------------------------------------
// �������������� ����� ��� ��������
void __fastcall TMainForm::EncodeSendBuffer(TObject *Sender)
{
        // ��� 1 - ������������ ������ �� Edit-� � char* � ���������
        //         ����� ���������� ������ ��������� ������
        ConvertAnsiStringToChar(ChatEdit->Text, prgbSendBuffer);

        long int i;

        for (i = (ChatEdit->Text.Length() + 1); i < EncryptionBlockSize; i++)
        {
                prgbSendBuffer[i] = random(256);
        }

        // ��� 2 - �������� ��������� ������ � �������� ���������� �����
        //         � "����������", ������� � ����, 1024 ��. ��������� ������,
        //         �������� prgbPasswordBuffer2
        long int lRandPos = random(lPasswordSize);

        long int j = lRandPos;

        for (i = 0; i < EncryptionBlockSize; i++)
        {
                prgbPasswordBuffer2[i] = prgbPasswordBuffer[j];
                j++;
                if (j == lPasswordSize)
                {
                        j=0;
                }
        }

        // ��� 3 - ���������� ������ � �������� ������
        pInKey->Encrypt(prgbSendBuffer, EncryptionBlockSize, prgbPasswordBuffer2);

        // ��� 4 - ���������� � ����� ������ ������,
        // � �������� "����������" 1 ��. ��������� ������
        IntToRGB.IntVar = lRandPos;

        j = EncryptionBlockSize;

        for (i = 0; i < INT_LENGTH; i++)
        {
                prgbSendBuffer[j] = IntToRGB.rgbVar[i];
                j++;
        }
}
//---------------------------------------------------------------------------
// ���������� ����� ����� �������
void __fastcall TMainForm::DecodeSendBuffer(TObject *Sender)
{
        long int i, j = EncryptionBlockSize;

        // ��� 1 - ������ ��������� ������� � ��������� �����
        for (i = 0; i < INT_LENGTH; i++)
        {
                IntToRGB.rgbVar[i] = prgbSendBuffer[j];
                j++;
        }

        unsigned long int ulRandPos = IntToRGB.IntVar;

        // ���� ��������� ����� �� ������������� ���� ����� �� ����.
        // �������� ���� ����
        if (ulRandPos >= lPasswordSize)
        {
                ulRandPos = random(lPasswordSize);
        }

        // ��� 2 - "����������" ������ �� ���������� ������

        j = ulRandPos;

        for (i = 0; i < EncryptionBlockSize; i++)
        {
                prgbPasswordBuffer2[i] = prgbPasswordBuffer[j];
                j++;
                if (j == lPasswordSize)
                {
                        j=0;
                }
        }

        // ��� 3 - ���������� ������
        pInKey->Decrypt(prgbSendBuffer, EncryptionBlockSize, prgbPasswordBuffer2);

        // ��� 4 - ����� ������������� ��������� ������ �� ������ � ������
        //         ��� ������ � ChatMemo
        i = 0;

        // ������������� ������ ������ � �������
        asDecodedStr.SetLength(EncryptionBlockSize);

        while (1)
        {
                asDecodedStr[i + 1]=prgbSendBuffer[i];
                if (
                                (prgbSendBuffer[i] == '\0')
                        ||
                                (i == (EncryptionBlockSize - 1))
                    )
                {
                        break;
                }
                i++;
        }

        asDecodedStr.SetLength((i + 1));

        // ��������� ������ � Memo
        ChatMemo->Lines->Add(TimeToStr(Time()) + " (+) " + asDecodedStr);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ChatEditKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        if (Key == VK_RETURN)
        {
                if (bIsServer)
                {
                        try
                        {
                                ServerSocket->Socket->Connections[0]->Connected;
                        }
                        catch (...)
                        {
                               ServerSocket->Close();
                               ChatMemo->Lines->Add("Error: Client is not accessible!");
                               // ������ ����� ���������� ��������� �� ����� Idle
                               ChangeListenCondition(Sender);
                               // ������ ����� ���������� ����� Server
                               ChangeListenCondition(Sender);
                               return;
                        }
                        // ���� �������� � ������������ ������...
                        if (!bProtection)
                        {
                                ServerSocket->Socket->Connections[0]->SendText(
                                ChatEdit->Text);
                        } else
                        {
                                // �������������� ����� ��� ��������
                                EncodeSendBuffer(Sender);
                                // ������ ���������� �������������� prgbSendBuffer
                                ServerSocket->Socket->Connections[0]->SendBuf(
                                prgbSendBuffer,(INT_LENGTH + EncryptionBlockSize));
                        }
                }
                else
                {
                        if (ClientSocket->Socket->Connected)
                        {
                             // ���� �������� � ������������ ������...
                             if (!bProtection)
                             {
                                        ClientSocket->Socket->SendText(ChatEdit->
                                        Text);
                             } else
                             {
                                // �������������� ����� ��� ��������
                                EncodeSendBuffer(Sender);
                                // ������ ���������� �������������� prgbSendBuffer
                                ClientSocket->Socket->SendBuf(prgbSendBuffer,
                                (INT_LENGTH+EncryptionBlockSize));

                             }
                        } else
                        {
                                ClientSocket->Close();
                                ChatMemo->Lines->Add("Error: Server is not accessible!");
                                StatusBar->Panels->Items[0]->Text = " Mode: Idle";
                                StatusBar->Panels->Items[2]->Text = " Connected to:";
                                return;
                        }
                }
                ChatMemo->Lines->Add(TimeToStr(Time()) + " (<) " + ChatEdit->Text);

                int i;

                // ������� ��������� �������� ������ � ChatEdit...
                for (i=1; i <= ChatEdit->Text.Length(); i++)
                {
                        ChatEdit->Text[i] = 0x00;
                }

                // ... � ����� ������� ���� ����� � ���
                ChatEdit->Text = "";
        }
}
//---------------------------------------------------------------------------
// ������ ��������� �� ������ �������� �����
void __fastcall TMainForm::FileErrorMessage(char *szFilename,int ErrKind)
{
        std::stringstream msg;
        msg << "Can't open " << szFilename << (ErrKind == TO_READ ? " to read!" :
         " to write!");
        MessageDlg(msg.str().c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ConvertAnsiStringToChar(AnsiString asStr, char *pCh)
{
        int i = 1;
        while (i <= asStr.Length())
        {
                pCh[i-1] = asStr[i];
                i++;
        }
        pCh[i - 1] = '\0';
}
//---------------------------------------------------------------------------
// ���������� ����� �����
long __fastcall TMainForm::FileLength(char *szFileName)
{
	int fHandle;
	long int lFileSize;

	fHandle = open(szFileName,O_RDONLY);
	lFileSize = filelength(fHandle);
	close(fHandle);

	return lFileSize;
}
//---------------------------------------------------------------------------
// ������������ �������������:
// 1 - ��������� ���������
// 2 - ����������, �������� ����� �����
// � �����: ����� �� ���� ������� ����������� �������� ����� ��� ������
FILE* __fastcall TMainForm::OpenFileToRead(char *szFilename,long &lFileSize)
{
	FILE *fHandle;

	// ��������� ���� �� ����������� ����������� ��������...
	if((fHandle = fopen(szFilename,"rb")) == NULL)
	{
		// ���� ������� ������ - ������� ��������� �� ������...
	  	FileErrorMessage(szFilename,TO_READ);
		// ... � ������� �� �������
		return NULL;
	}

	// ... ���� ���� �������� - �� OK, �� ��� ���������� ����������
	// ����������� ��� �������, ��� ����� ����� ����������� ���
	// � ������ ������, ������� ��� �������� ���������...
	fclose(fHandle);

	// ... � �������� ������ �����.
	lFileSize = FileLength(szFilename);

	// � ��� � �������� ����� ��� ������ � ���...
	fHandle = fopen(szFilename, "rb");

	return fHandle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetPasswordFileSpeedButtonClick(TObject *Sender)
{
        // ���� ���������� �������� ������ � ����������...
        if (bUsingKeybdPassword)
        {
                // 1) ���������� ����� ����� ������...
                if(
                        (PasswordDlg->ShowModal() == mrOk)
                    &&
                        (PasswordDlg->Password->Text.Length() != 0)
                   )
                {
                        // ������������� ������ ���������� ������
                        lPasswordSize = MAX_ENCRYPTION_BLOCK_SIZE;

                        // ������� ����������� �������
                        AllocateObjects();

                        //...������ ��� �����...
                        int PasswordLen = PasswordDlg->Password->Text.Length();

                        //...�������� � ��������� �����...
                        for (int i = 1; i <= PasswordLen; i++)
                        {
                                prgbPasswordBuffer[i - 1] = PasswordDlg->Password->Text[i];
                                PasswordDlg->Password->Text[i] = 0x00;
                        }

                        //...� �������� ������
                        pDHash->Hash(prgbPasswordBuffer, PasswordLen);

                        PasswordDlg->Password->Text = "";

                        // ���������, ��� ������ ������������
                        bProtection = true;
                        StatusBar->Panels->Items[1]->Text = " Protection: YES";
                }

                return;
        }

        if (MainForm->OpenDialog->Execute())
        {
                // ������� ������������ �������, ���������� ��� ������
                // ��������� ����
                FreeObjects();
                bProtection=false;
                StatusBar->Panels->Items[1]->Text = " Protection: NO";

                // ������ ��� ������������ �����
                char *szPasswordFile = new char [MAX_NAME_LENGTH];
                ConvertAnsiStringToChar(OpenDialog->FileName,szPasswordFile);

                // ��������� ��������� ����
                FILE* fPassword = OpenFileToRead(szPasswordFile,lPasswordSize);

                // ���� ��������� ���� �� ��������, ������� �� �������
                if (!fPassword)
                {
                        delete [] szPasswordFile;
                        return;
                }

                // � ����� �������� ������ ��� ������ ���������� �����
                // � ������ ��� � �����
                AllocateObjects();

                fread(prgbPasswordBuffer,lPasswordSize,1,fPassword);
                fclose(fPassword);

                delete [] szPasswordFile;

                // ���������, ��� ������ ������������
                bProtection = true;
                StatusBar->Panels->Items[1]->Text = " Protection: YES";
        }
}
 //---------------------------------------------------------------------------
void __fastcall TMainForm::FileSetPasswordFileItemClick(TObject *Sender)
{
SetPasswordFileSpeedButtonClick(Sender);
}

//---------------------------------------------------------------------------
// ����������� ������ ����������� ������� char �� �����������
// ������������
void __fastcall TMainForm::WipeData (char *prgbData, long lDataSize)
{
	long int i;

	for (i = 0; i < lDataSize; i++)
	{
		prgbData[i] = (char)0x00;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AllocateObjects()
{
        prgbPasswordBuffer = new char[lPasswordSize];
        // ���� �� ������� �������� ������
        if (!prgbPasswordBuffer)
        {
                MessageDlg("Can't allocate memory under a password file!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        prgbSendBuffer = new char[(MAX_ENCRYPTION_BLOCK_SIZE + INT_LENGTH)];
        if (!prgbSendBuffer)
        {
                MessageDlg("Can't allocate memory under a prgbSendBuffer!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        prgbPasswordBuffer2 = new char[MAX_ENCRYPTION_BLOCK_SIZE];
        if (!prgbPasswordBuffer2)
        {
                MessageDlg("Can't allocate memory under a prgbPasswordBuffer2!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        // ������� ��������� ������ �����������
        pDHash = new CDHash(MAX_ENCRYPTION_BLOCK_SIZE);
        if (!pDHash)
        {
                MessageDlg("Can't allocate memory under a pDHash!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        // ������� ��������� ������ ����������
        pInKey = new CInKey;
        if (!pInKey)
        {
                MessageDlg("Can't allocate memory under a pInKey!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        // ���������� ��� ����������
        randomize();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FreeObjects()
{
        // ������������ �������...
        if (prgbPasswordBuffer)
        {
                WipeData(prgbPasswordBuffer,lPasswordSize);
                delete [] prgbPasswordBuffer;
                prgbPasswordBuffer = NULL;
        }

        if (prgbSendBuffer)
        {
                WipeData(prgbSendBuffer,(EncryptionBlockSize+INT_LENGTH));
                delete [] prgbSendBuffer;
                prgbSendBuffer = NULL;
        }

        if (prgbPasswordBuffer2)
        {
                WipeData(prgbPasswordBuffer2,EncryptionBlockSize); // char* data
                delete [] prgbPasswordBuffer2;
                prgbPasswordBuffer2 = NULL;
        }

        if (pDHash)
        {
                delete pDHash;
                pDHash = NULL;
        }

        if (pInKey)
        {
                delete pInKey;
                pInKey = NULL;
        }
 }
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
        // ������������� ��������� ��������� ����� ����������
        EncryptionBlockSize = 1024;
        ChatEdit->MaxLength = EncryptionBlockSize;

        // ��������� ������ � ImageList
        IconsImageList->Add(NormalIconImage->Picture->Bitmap, NULL);
        IconsImageList->Add(BlinkIconImage->Picture->Bitmap, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::IconBlinkTimerTimer(TObject *Sender)
{
        // ������������ ������ ������ � ������� ������
        TrayIcon->IconIndex ^= 0x01;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TrayIconClick(TObject *Sender)
{
        // ���� ���������� � ����������...
        bInTray = false;

        // ...��������� ������...
        IconBlinkTimer->Enabled = false;
        // ... � ���������� ���������� ������
        TrayIcon->IconIndex = 0x00;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TrayIconMinimize(TObject *Sender)
{
        // ���������, ��� ���������� � ����...
        bInTray = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WipeVCL(TObject *Sender)
{
        int i,j;

        // ������� ��������� �������� ������ � ChatEdit...
        for (i=1; i <= ChatEdit->Text.Length(); i++)
        {
                ChatEdit->Text[i] = 0x00;
        }

        // ����� ���������� ChatMemo...
        for (j=0; j < ChatMemo->Lines->Count; j++)
        {
                // �������� ��� ������� � ������ ������...
                for (i=1; i <= (ChatMemo->Lines->operator [](j).Length()); i++)
                {
                        ChatMemo->Lines->operator [](j)[i] = 0x00;
                }
        }

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EditClearClick(TObject *Sender)
{
        // ������� ���...
        WipeVCL(Sender);
        ChatMemo->Clear();
        ChatEdit->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EditTCPPortClick(TObject *Sender)
{
        // �������� ������ ��������� �����...
          if (InputQuery("Set to...", "TCP Port:", Port))
          {
             unsigned int TCPPort = StrToInt(Port);
             if (
                                (Port.Length() > 0)
                        &&
                                ((TCPPort > 0)
                        &&
                                (TCPPort <= 65535))
                )
             {
                        ClientSocket->Port = TCPPort;
                        ServerSocket->Port = TCPPort;
                        StatusBar->Panels->Items[3]->Text = " Port: " + Port;
             }
          }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N8192bitClick(TObject *Sender)
{
        // ���� ����� ���� ��� �� �������, ��������
        if (!((TMenuItem*)Sender)->Checked)
        {
                EncryptionBlockSize = 1024;
                ChatEdit->MaxLength = EncryptionBlockSize;

                ((TMenuItem*)Sender)->Checked = true;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N16384bitClick(TObject *Sender)
{
        // ���� ����� ���� ��� �� �������, ��������
        if (!((TMenuItem*)Sender)->Checked)
        {
                EncryptionBlockSize = 2048;
                ChatEdit->MaxLength = EncryptionBlockSize;

                ((TMenuItem*)Sender)->Checked = true;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N24576bitClick(TObject *Sender)
{
         // ���� ����� ���� ��� �� �������, ��������
        if (!((TMenuItem*)Sender)->Checked)
        {
                EncryptionBlockSize = 3072;
                ChatEdit->MaxLength = EncryptionBlockSize;

                ((TMenuItem*)Sender)->Checked = true;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N32768bitClick(TObject *Sender)
{
        // ���� ����� ���� ��� �� �������, ��������
        if (!((TMenuItem*)Sender)->Checked)
        {
                EncryptionBlockSize = 4096;
                ChatEdit->MaxLength = EncryptionBlockSize;

                ((TMenuItem*)Sender)->Checked = true;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PasswordModeClick(TObject *Sender)
{
         // ���� ����� ���� ��� �� �������, ��������
        if (!((TMenuItem*)Sender)->Checked)
        {
                ((TMenuItem*)Sender)->Checked = true;
                 bUsingKeybdPassword = true;
        } else
        {
                ((TMenuItem*)Sender)->Checked = false;
                 bUsingKeybdPassword = false;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ProtectionResetClick(TObject *Sender)
{
        // ���������, ��� ������ ���������...
        bProtection = false;
        StatusBar->Panels->Items[1]->Text = " Protection: NO";

        // ������������ �������...
        FreeObjects();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearClick(TObject *Sender)
{
        // ������� ���...
        WipeVCL(Sender);
        ChatMemo->Clear();
        ChatEdit->Text = "";

        // ...��������� ������...
        IconBlinkTimer->Enabled = false;
        // ... � ���������� ���������� ������
        TrayIcon->IconIndex = 0x00;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExitClick(TObject *Sender)
{
        Close();
}

