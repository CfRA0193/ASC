//**********************************************************
//* Класс CDHash. Функция хеширования с мнимым увеличением
//*               количества комбинаций
//**********************************************************
//* Дата:    1.10.2005
//* Версия:  4.00
//* Автор:   Дробанов Артём Федорович (DrAF)
//**********************************************************

#pragma once

#define ASC_TABLE_SIZE 256 // Длина ASCII-таблицы
#define SUBKEY_LEN 16	   // Длина подключа unSubkey.Subkey[]
#define MODKEY_LEN 3	   // Длина ключа для кодирования модуля
#define INT_LEN 4	       // Длина в байтах переменной типа int
#define NULL 0

// Объединение для преобразования четырех контрольных суммм в подключ,
// состоящий из 16 байт
typedef union
{
	struct TChecksums	// Поле массива из 4-ех переменных типа int
	{
   		int rgChecksum[4];
	} Checksums;

	unsigned char rgubSubkey[SUBKEY_LEN];  // ...используется как массив
   										   // unsigned char из 16
                                           // элементов (4*4=16)
} TSubkey;

// Класс хеширования блока данных
class CDHash  
{
public:	
	CDHash();
	CDHash(int HashLen);
	CDHash(int HashLen, void (*UpdateProgress)(int progress));
	virtual ~CDHash();

	// Функция хеширования данных
	void Hash(char *prgbSourceData, int SourceLen);
	
private:
	
	int hashGammaLen;
	TSubkey subkey;

	int rgFeedB[SUBKEY_LEN];

	int *prgSourceData;	
	int *prgHash;
	int iSubkey;
	int *prgCodeTbl;
	int nTry1;
	int nTry2;
	
	void Initialize(int hashLen);
	inline int Encode(int IB, int PB);
	void InitSubkey();
	void RotateStep(int sourceDataLen);
	void RotateSourceData(int cNumRounds, int sourceDataLen);
	void NormcNumRounds(int &cNumRounds, int sourceDataLen);
	inline void NormISubstTbl(int &iSubstTbl);
    inline int HF1(int k, int size);
	inline int HF1_2(int k, int size);
	inline int HFDouble(int k, int size, int numberOfTry);
	void NormISubkey(int &iSubkey);
	void NormBegCode(int &begCode);
	void PrepareSourceData(int sourceDataLen);
	void CalculateSubkey();
	void FirstCodeRound(int begCode);
	void MoveCode();
	void SecondCodeRound(int begCode);
	void HashCode();
	void SubkeyCode();
	void ImposeSubkeyGamma();
	void (*UpdateProgress)(int progress);
};
