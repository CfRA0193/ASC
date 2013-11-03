//******************************************************
//* ASC - Absolutely Safe Chat                         *
//******************************************************
//* main_form.cpp                                      *
//* Дата:    4.02.2006                                 *
//* Версия:  1.3.0.3	                               *
//* Автор:   Дробанов Артём Федорович (DrAF)           *
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
// Изменяет состояние Listen...
void __fastcall TMainForm::ChangeListenCondition(TObject *Sender)
{
        // Если в данный момент слушаем...
        if (FileListenItem->Checked)
        {
               //... меняем иконку на противоположную
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
        // Изменяем состояние Listen...
        ChangeListenCondition(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListenSpeedButtonClick(TObject *Sender)
{
        // Изменяем состояние Listen...
        ChangeListenCondition(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FileExitItemClick(TObject *Sender)
{
        // Закрываем программу
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::HelpAboutItemClick(TObject *Sender)
{
        // Отображаем диалог About
        AboutForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ConnectTo(TObject *Sender)
{
  // Делаем запрос IP-адреса...
  if (InputQuery("Connect to...", "Address Name:", Server))
  {
     // Если строка была не пустая...
     if (Server.Length() > 0)
     {
       // Если в данный момент были на коннекте, рвем соединение...
       if (ClientSocket->Active)
       {
                ClientSocket->Close();
       }
        // Инициализируем параметры сокета...
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

        // Очищаем чат...
        WipeVCL(Sender);

        // Высвобождаем ресурсы
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
        // Закрываем все соединения...
        ClientSocket->Close();
        ServerSocket->Close();

        FileListenItem->Checked = false;
        ListenSpeedButton->Glyph = ListenOffImage->Glyph;

        StatusBar->Panels->Items[0]->Text = " Mode: Idle";
        bProtection = false;
        StatusBar->Panels->Items[1]->Text = " Protection: NO";
        StatusBar->Panels->Items[2]->Text = " Connected to:";

        // Высвобождаем ресурсы
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
        // Если окно приложения свернуто в трей, указываем на то, что
        // пришли новые сообщения...
        if (bInTray)
        {
                IconBlinkTimer->Enabled = true;
        }

        // Если работаем в незащищенном режиме
        if (!bProtection)
        {
                ChatMemo->Lines->Add(TimeToStr(Time()) + " (+) " + Socket->ReceiveText());
        } else
        {
                // Принимаем данные...
                Socket->ReceiveBuf(prgbSendBuffer,(EncryptionBlockSize + INT_LENGTH));
                //... и декодируем
                DecodeSendBuffer(Sender);
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ServerSocketClientRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
        // Если окно приложения свернуто в трей, указываем на то, что
        // пришли новые сообщения...
        if (bInTray)
        {
                IconBlinkTimer->Enabled = true;
        }

        // Если работаем в незащищенном режиме
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
// Подготавливает буфер для отправки
void __fastcall TMainForm::EncodeSendBuffer(TObject *Sender)
{
        // Шаг 1 - Конвертируем данные из Edit-а в char* и добавляем
        //         после записанной строки случайные данные
        ConvertAnsiStringToChar(ChatEdit->Text, prgbSendBuffer);

        long int i;

        for (i = (ChatEdit->Text.Length() + 1); i < EncryptionBlockSize; i++)
        {
                prgbSendBuffer[i] = random(256);
        }

        // Шаг 2 - Выбираем случайный индекс в пределах парольного файла
        //         и "наматываем", начиная с него, 1024 бт. парольных данных,
        //         заполняя prgbPasswordBuffer2
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

        // Шаг 3 - Шифрование данных в исходном буфере
        pInKey->Encrypt(prgbSendBuffer, EncryptionBlockSize, prgbPasswordBuffer2);

        // Шаг 4 - Дописываем в конец буфера индекс,
        // с которого "наматывали" 1 кб. парольных данных
        IntToRGB.IntVar = lRandPos;

        j = EncryptionBlockSize;

        for (i = 0; i < INT_LENGTH; i++)
        {
                prgbSendBuffer[j] = IntToRGB.rgbVar[i];
                j++;
        }
}
//---------------------------------------------------------------------------
// Декодирует буфер после приемки
void __fastcall TMainForm::DecodeSendBuffer(TObject *Sender)
{
        long int i, j = EncryptionBlockSize;

        // Шаг 1 - Узнаем случайную позицию в парольном файле
        for (i = 0; i < INT_LENGTH; i++)
        {
                IntToRGB.rgbVar[i] = prgbSendBuffer[j];
                j++;
        }

        unsigned long int ulRandPos = IntToRGB.IntVar;

        // Если парольные файлы не соответствуют друг другу по разм.
        // скрываем этот факт
        if (ulRandPos >= lPasswordSize)
        {
                ulRandPos = random(lPasswordSize);
        }

        // Шаг 2 - "наматываем" данные из парольного буфера

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

        // Шаг 3 - декодируем данные
        pInKey->Decrypt(prgbSendBuffer, EncryptionBlockSize, prgbPasswordBuffer2);

        // Шаг 4 - После декодирования переносим данные из буфера в строку
        //         для вывода в ChatMemo
        i = 0;

        // Устанавливаем размер строки с запасом
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

        // Добавляем строку в Memo
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
                               // Первый вызов сбрасывает состояние на режим Idle
                               ChangeListenCondition(Sender);
                               // Второй вызов возвращает режим Server
                               ChangeListenCondition(Sender);
                               return;
                        }
                        // Если работаем в незащищенном режиме...
                        if (!bProtection)
                        {
                                ServerSocket->Socket->Connections[0]->SendText(
                                ChatEdit->Text);
                        } else
                        {
                                // Подготавливаем буфер для отправки
                                EncodeSendBuffer(Sender);
                                // Теперь отправляем подготовленный prgbSendBuffer
                                ServerSocket->Socket->Connections[0]->SendBuf(
                                prgbSendBuffer,(INT_LENGTH + EncryptionBlockSize));
                        }
                }
                else
                {
                        if (ClientSocket->Socket->Connected)
                        {
                             // Если работаем в незащищенном режиме...
                             if (!bProtection)
                             {
                                        ClientSocket->Socket->SendText(ChatEdit->
                                        Text);
                             } else
                             {
                                // Подготавливаем буфер для отправки
                                EncodeSendBuffer(Sender);
                                // Теперь отправляем подготовленный prgbSendBuffer
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

                // Сначала полностью затираем строку в ChatEdit...
                for (i=1; i <= ChatEdit->Text.Length(); i++)
                {
                        ChatEdit->Text[i] = 0x00;
                }

                // ... а потом убираем весь текст в нем
                ChatEdit->Text = "";
        }
}
//---------------------------------------------------------------------------
// Выдает сообщение об ошибке открытия файла
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
// Возвращает длину файла
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
// Обеспечивает инициализацию:
// 1 - Файлового указателя
// 2 - Переменной, хранящей длину файла
// В целом: берет на себя функции корректного открытия файла для чтения
FILE* __fastcall TMainForm::OpenFileToRead(char *szFilename,long &lFileSize)
{
	FILE *fHandle;

	// Проверяем файл на возможность корректного открытия...
	if((fHandle = fopen(szFilename,"rb")) == NULL)
	{
		// Если открыть нельзя - выводим сообщение об ошибке...
	  	FileErrorMessage(szFilename,TO_READ);
		// ... и выходим из функции
		return NULL;
	}

	// ... если файл открылся - всё OK, но ещё необходимо произвести
	// определение его размера, для этого нужно переоткрыть его
	// в другом режиме, поэтому его временно закрываем...
	fclose(fHandle);

	// ... и получаем размер файла.
	lFileSize = FileLength(szFilename);

	// А вот и открытие файла для работы с ним...
	fHandle = fopen(szFilename, "rb");

	return fHandle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetPasswordFileSpeedButtonClick(TObject *Sender)
{
        // Если предписано получить пароль с клавиатуры...
        if (bUsingKeybdPassword)
        {
                // 1) Отображаем форму ввода пароля...
                if(
                        (PasswordDlg->ShowModal() == mrOk)
                    &&
                        (PasswordDlg->Password->Text.Length() != 0)
                   )
                {
                        // Устанавливаем размер парольного буфера
                        lPasswordSize = MAX_ENCRYPTION_BLOCK_SIZE;

                        // Создаем необходимые объекты
                        AllocateObjects();

                        //...узнаем его длину...
                        int PasswordLen = PasswordDlg->Password->Text.Length();

                        //...копируем в парольный буфер...
                        for (int i = 1; i <= PasswordLen; i++)
                        {
                                prgbPasswordBuffer[i - 1] = PasswordDlg->Password->Text[i];
                                PasswordDlg->Password->Text[i] = 0x00;
                        }

                        //...и хешируем пароль
                        pDHash->Hash(prgbPasswordBuffer, PasswordLen);

                        PasswordDlg->Password->Text = "";

                        // Указываем, что защита используется
                        bProtection = true;
                        StatusBar->Panels->Items[1]->Text = " Protection: YES";
                }

                return;
        }

        if (MainForm->OpenDialog->Execute())
        {
                // Сначала высвобождаем ресурсы, выделенные под старый
                // парольный файл
                FreeObjects();
                bProtection=false;
                StatusBar->Panels->Items[1]->Text = " Protection: NO";

                // Задаем имя максимальной длины
                char *szPasswordFile = new char [MAX_NAME_LENGTH];
                ConvertAnsiStringToChar(OpenDialog->FileName,szPasswordFile);

                // Открываем парольный файл
                FILE* fPassword = OpenFileToRead(szPasswordFile,lPasswordSize);

                // Если парольный файл не открылся, выходим из функции
                if (!fPassword)
                {
                        delete [] szPasswordFile;
                        return;
                }

                // а иначе выделяем память под данные парольного файла
                // и читаем его в буфер
                AllocateObjects();

                fread(prgbPasswordBuffer,lPasswordSize,1,fPassword);
                fclose(fPassword);

                delete [] szPasswordFile;

                // Указываем, что защита используется
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
// Ликвидирует данные переданного массива char из соображений
// безопасности
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
        // Если не удалось выделить память
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

        // Создаем экземпляр класса хеширования
        pDHash = new CDHash(MAX_ENCRYPTION_BLOCK_SIZE);
        if (!pDHash)
        {
                MessageDlg("Can't allocate memory under a pDHash!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        // Создаем экземпляр класса шифрования
        pInKey = new CInKey;
        if (!pInKey)
        {
                MessageDlg("Can't allocate memory under a pInKey!" ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        // Необходимо для шифрования
        randomize();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FreeObjects()
{
        // Высвобождаем ресурсы...
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
        // Устанавливаем начальное состояние блока шифрования
        EncryptionBlockSize = 1024;
        ChatEdit->MaxLength = EncryptionBlockSize;

        // Загружаем иконки в ImageList
        IconsImageList->Add(NormalIconImage->Picture->Bitmap, NULL);
        IconsImageList->Add(BlinkIconImage->Picture->Bitmap, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::IconBlinkTimerTimer(TObject *Sender)
{
        // Перекидываем индекс иконки в массиве иконок
        TrayIcon->IconIndex ^= 0x01;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TrayIconClick(TObject *Sender)
{
        // Если обратились к приложению...
        bInTray = false;

        // ...отключаем таймер...
        IconBlinkTimer->Enabled = false;
        // ... и возвращаем нормальную иконку
        TrayIcon->IconIndex = 0x00;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TrayIconMinimize(TObject *Sender)
{
        // Указываем, что приложение в трее...
        bInTray = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WipeVCL(TObject *Sender)
{
        int i,j;

        // Сначала полностью затираем строку в ChatEdit...
        for (i=1; i <= ChatEdit->Text.Length(); i++)
        {
                ChatEdit->Text[i] = 0x00;
        }

        // Затем занимаемся ChatMemo...
        for (j=0; j < ChatMemo->Lines->Count; j++)
        {
                // Затираем все символы в каждой строке...
                for (i=1; i <= (ChatMemo->Lines->operator [](j).Length()); i++)
                {
                        ChatMemo->Lines->operator [](j)[i] = 0x00;
                }
        }

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EditClearClick(TObject *Sender)
{
        // Очищаем чат...
        WipeVCL(Sender);
        ChatMemo->Clear();
        ChatEdit->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EditTCPPortClick(TObject *Sender)
{
        // Вызываем диалог установки порта...
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
        // Если пункт меню ещё не выделен, выделяем
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
        // Если пункт меню ещё не выделен, выделяем
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
         // Если пункт меню ещё не выделен, выделяем
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
        // Если пункт меню ещё не выделен, выделяем
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
         // Если пункт меню ещё не выделен, выделяем
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
        // Указываем, что защита отключена...
        bProtection = false;
        StatusBar->Panels->Items[1]->Text = " Protection: NO";

        // Высвобождаем ресурсы...
        FreeObjects();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearClick(TObject *Sender)
{
        // Очищаем чат...
        WipeVCL(Sender);
        ChatMemo->Clear();
        ChatEdit->Text = "";

        // ...отключаем таймер...
        IconBlinkTimer->Enabled = false;
        // ... и возвращаем нормальную иконку
        TrayIcon->IconIndex = 0x00;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExitClick(TObject *Sender)
{
        Close();
}

