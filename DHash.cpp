//**********************************************************
//* Класс CDHash. Функция хеширования с мнимым увеличением
//*               количества комбинаций
//**********************************************************
//* Дата:    1.10.2005
//* Версия:  4.00
//* Автор:   Дробанов Артём Федорович (DrAF)
//**********************************************************

#pragma once

#include "DHash.h"
#include <math.h>

CDHash::CDHash()
{
        this->UpdateProgress = 0;
	Initialize(1024);
}

CDHash::CDHash(int hashLen)
{
        this->UpdateProgress = 0;
	Initialize(hashLen);
}

CDHash::CDHash(int hashLen, void (*UpdateProgress)(int progress))
{
	// Подписываемся на функтор-обработчик обновления прогресса
	this->UpdateProgress = UpdateProgress;
	Initialize(hashLen);
}

void CDHash::Initialize(int hashLen)
{
	// Фиксируем размер хеша
	hashGammaLen  = hashLen;

	// Выделяем память под свойства класса хеширования
	prgSourceData = new int[hashGammaLen];
	prgHash       = new int[hashGammaLen];

	// Таблица перекодирования блока исходных данных
	int rgCodeTbl[256] =
	{
          34,254, 82,184,160,96,242,222,55,209,212,126,146, 23, 33,43,
          39,134, 59,128,236, 38,155,170, 69,172,252,238, 47,121, 228,		  
	      183,203,135,165,166, 60, 98,7,207,120,189,210,8,226, 41, 72,
	      253,71,24,171,196,101,168,169,186,0,46,68, 95,237,65,53,208,
	      211, 83,114,157,144,32,193,143,36,250,75,234,49,167,125,141,
		  58, 10,103,198,151,109, 37,112, 84,231,224,185,138,152, 94,
	      99,139,22,191,136,111,162,227,118,26,102,12,50,132,21,76,213,
	      73,197,16,119,48,140,110,54,45,4,148,192,205,158,124,214,180,
		  217,230, 86,6, 28,221,107, 2,42,220,201,133, 74, 64, 20,129,
	      159,92,66,246,13,216,40, 62,291,31, 3,85,206,145,79,154,142,
	      204,117,223,195,244, 90,87,116,104,161,56, 179,77,225,150,5,
		  194,174,251,229,115,57,249,215, 19,255,199,147, 70,149, 35,
	      245, 63, 11,97,30,27,240,80,122,106,108, 78,173,182,61,130,
		  88,219, 25,137, 15,175,190,241,181,239,153,232,1,177,233,14,
          51,164,200, 31,156,247,187, 89, 81,176,188,105,243,127, 17,          
	      202, 67, 44,235, 9,18,100,52,113,218,123, 93, 91,163,178,248
	};

    prgCodeTbl = new int[ASC_TABLE_SIZE];

    // Переносим данные из статического массива в динамический
    for (int i = 0; i < ASC_TABLE_SIZE; i++)
    {
		prgCodeTbl[i] = rgCodeTbl[i];
    }
}

CDHash::~CDHash()
{
	delete [] prgSourceData;

	delete [] prgHash;

	delete [] prgCodeTbl;
}

// XOR-подобная операция для целочисленных операндов
inline int CDHash::Encode(int IB, int PB)
{
	int OB;

	OB = IB - (prgCodeTbl[(unsigned char)PB]);

	if (OB < 0)
	{
	   OB += ASC_TABLE_SIZE;
	}

	return OB;
}

// Инициализация структуры подключа
void CDHash::InitSubkey()
{
	for (int i = 0; i < INT_LEN; i++) // Инициализируем структуру
	{
		subkey.Checksums.rgChecksum[i] = 0;
	}
}

// Один шаг вращения блока исходных данных
void CDHash::RotateStep(int sourceDataLen)
{
	for (int i = 0; i < (sourceDataLen - 1); i++)
	{
   		prgSourceData[i] = prgSourceData[i + 1];
	}
}

// Вращение блока исходных данных
void CDHash::RotateSourceData(int cNumRounds, int sourceDataLen)
{
	int temp;

	// Вращаем исходные данные
	for (int i = 0; i < cNumRounds; i++)
	{
		// Сохраняем нулевой элемент пароля,т.к. на его место в функции
		// вращения будут записаны смещаемые влево данные
		temp = prgSourceData[0];

		// Один шаг вращения исходных данных
		RotateStep(sourceDataLen);

		// Восстанавливаем сохраненный элемент Hash-гаммы в его конце,
		// (он был "вытеснен" влево и появился справа)
		prgSourceData[sourceDataLen - 1] = temp;
	}
}

// Нормирование количества раундов вращения
void CDHash::NormcNumRounds(int &cNumRounds, int sourceDataLen)
{
	while (cNumRounds > sourceDataLen)
	{
   		cNumRounds -= sourceDataLen;
	}
}

// Нормирование индекса таблицы подстановок
inline void CDHash::NormISubstTbl(int &iSubstTbl)
{
	if (iSubstTbl >= ASC_TABLE_SIZE)
	{
		iSubstTbl -= ASC_TABLE_SIZE;
	}
}

// Хеш-функция, основанная на делении с остатком
inline int CDHash::HF1(int k, int size)
{
	return (k % size);
}

// Хеш-функция, основанная на делении с остатком
// (дополнение к функции HF1 для двойного хеширования)
inline int CDHash::HF1_2(int k, int size)
{
	return 1 + (k % (size - 2));
}

// Двойное хеширование
inline int CDHash::HFDouble(int k, int size, int numberOfTry)
{
	return (HF1(k, size) + numberOfTry * HF1_2(k, size)) % size;
}

// Нормирование индекса в массиве подключа
void CDHash::NormISubkey(int &iSubkey)
{		
	iSubkey = HFDouble(iSubkey, SUBKEY_LEN, nTry1++);
}

// Нормирование индекса в массиве хеш-гаммы
void CDHash::NormBegCode(int &begCode)
{	
	begCode = HFDouble(begCode, hashGammaLen, nTry2++);
}

// Подготовка блока исходных данных к хешированию
void CDHash::PrepareSourceData(int sourceDataLen)
{
	int i = 0, j = 0, cNumRounds, iCodeTbl;

	// Циклическое копирование блока исходных данных в массив
	// prgHash
	while (i < hashGammaLen)
	{
		// Индекс в таблице подстановок
		iCodeTbl = prgSourceData[j];

		// Табличная подстановка
		prgHash[i] = prgCodeTbl[iCodeTbl];

		// Увеличиваем индекс в массиве исх.данн.
		j++;

		// Проверяем на окончание исх.данн.
		if (j >= sourceDataLen)
		{
			// Вращать исх.данн. из одного элемента бессмысленно
			if (sourceDataLen > 1)
			{
				// Первоначальное количество сдвигов исх.данн.
				cNumRounds = (prgSourceData[0] ^ i);

				// Нормирование количества шагов вращения исх.данн.
				NormcNumRounds(cNumRounds, sourceDataLen);

				// Вращение исх.данн.
				RotateSourceData(cNumRounds, sourceDataLen);
			}
			j = 0; // Обнуляем j (начинаем копировать исх.данн. из
			       // prgSourceData в prgHash) с начала
		}
		i++;
	}
}

// Вычисление подключа (используется для кодирования HASH-гаммы
// prgHash. Подключ вычисляется на основе контрольных сумм гаммы
// prgHash, подлежащей кодированию)
void CDHash::CalculateSubkey()
{
	for (int i = 0; i < hashGammaLen; i++)
	{
		subkey.Checksums.rgChecksum[0] += (prgHash[i] * prgHash[i] ^ prgHash[i] * i * i * i);
		subkey.Checksums.rgChecksum[1] += (prgHash[i] * prgHash[i] * prgHash[i] ^ prgHash[i] + i);
		subkey.Checksums.rgChecksum[2] += (prgHash[i] * prgHash[i] ^ prgHash[i]);
		subkey.Checksums.rgChecksum[3] += (prgHash[i] * prgHash[i] ^ prgHash[i] * i * i);	
	}
}

// Первый раунд кодирования - начиная с BegCode и до конца
void CDHash::FirstCodeRound(int begCode)
{
	int IB, PB;

	for (int i = begCode; i < (hashGammaLen - 1); i++)

	{
		IB = prgHash[i]; 	 // Кодируемый элемент потока
		PB = prgHash[i + 1]; // Кодирующий элемент потока
		prgHash[i] = Encode(IB, PB);
	}
}

// Кодирование первого элемента HASH-гаммы последним - реализуем
// перенос ОС на начало
inline void CDHash::MoveCode()
{
   int IB, PB;

   IB = prgHash[hashGammaLen - 1];
   PB = prgHash[0];
   prgHash[hashGammaLen - 1] = Encode(IB, PB);
}

// Второй раунд кодирования - с начала и до BegCode
void CDHash::SecondCodeRound(int begCode)
{
	int i, IB, PB;

	// Второй раунд кодирования - с начала и до BegCode
	for (i = 0; i < (begCode - 1); i++)
	{
		IB = prgHash[i];      // Кодируемый элемент потока
		PB = prgHash[i + 1];  // Кодирующий элемент потока
		prgHash[i] = Encode(IB, PB);
	}
}

// Кодирование хеш-гаммы
void CDHash::HashCode()
{
	int begCode = 0;
	iSubkey++;

	// Нормирование индекса в массиве подключа
	NormISubkey(iSubkey);

	// Вычисляем значение индекса начала кодирования (BegCode)
	begCode += (unsigned char)(prgHash[0] + subkey.rgubSubkey[iSubkey]);

    // Нормируем параметр автокодирования
    NormBegCode(begCode);

	// Первый раунд связывания - начиная с BegCode и до конца
    FirstCodeRound(begCode);

	// Кодирование первого элемента последним - реализуем
	// перенос "ОС" на начало хеш-гаммы
  	MoveCode();

	// Второй раунд кодирования - с начала и до BegCode
  	SecondCodeRound(begCode);	
}

// Реализация ОС при наложении подключа на HASH-гамму
void CDHash::SubkeyCode()
{	
	for (int i = 0; i < SUBKEY_LEN; i++)
	{
   		subkey.rgubSubkey[i] ^= (unsigned char)rgFeedB[i];
	}
}

// Наложение гаммы подключа на HASH-гамму
void CDHash::ImposeSubkeyGamma()
{
	int IB, PB;

	// Буфер ОС по данным подключа из контрольных сумм

	// Наложение контрольной суммы
	for (int i = 0, j = 0; i < hashGammaLen; i++, j++)
	{
		if (j >= SUBKEY_LEN)
		{
			// Реализация ОС - кодирование подключа unSubkey.Subkey[],
			// состоящего из контрольных сумм prgHash, блоком выходных
			// данных
			SubkeyCode();

			j = 0;
		}

		// Блок кодирования гаммы ключа посредством подключа

		// Кодируемый байт основного ключа
		IB = prgHash[i];
		// Кодирующий байт из подключа
		PB = (int)subkey.rgubSubkey[j];
		
		// Результат кодирования
		prgHash[i] = Encode(IB, PB);
	
		// Формируем буфер ОС
		rgFeedB[j] = prgHash[i];
	}
}

// Функция хеширования данных
void CDHash::Hash(char *prgbSourceData, int sourceLen)
{
    int cRounds;

    iSubkey = 0;
	nTry1 = 0;
	nTry2 = 0;
	
	// Переносим исходные данные в массив int (он и будет хешироваться)
	for (int i = 0; i < sourceLen; i++)
	{
		prgSourceData[i] = (unsigned char)prgbSourceData[i];
	}

	// Инициализируем структуру, содержащую массив контрольных сумм
	// и входящую в объединение Subkey
	InitSubkey();

	// Подготовка исх.данн.: циклическое копирование его в prgHash
  	PrepareSourceData(sourceLen);
          
	// Хеширование блока исходных данных
	for (cRounds = 0; cRounds < hashGammaLen; cRounds++)	
	{     
		// Вычисление подключа
		CalculateSubkey();

		// Кодирование хеш-гаммы XOR-подобной операцией
		HashCode();

		// Наложение гаммы подключа
		ImposeSubkeyGamma();        

		// Обновляем прогресс
		if (UpdateProgress != NULL)
		{
			UpdateProgress(cRounds);
		}
	}

	// Записываем результат в "целевой" массив
	for (int i = 0; i < hashGammaLen; i++)
	{
		prgbSourceData[i] = (char)prgHash[i];
	}
}