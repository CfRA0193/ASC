//**********************************************************
//* ����� CDHash. ������� ����������� � ������ �����������
//*               ���������� ����������
//**********************************************************
//* ����:    1.10.2005
//* ������:  4.00
//* �����:   �������� ���� ��������� (DrAF)
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
	// ������������� �� �������-���������� ���������� ���������
	this->UpdateProgress = UpdateProgress;
	Initialize(hashLen);
}

void CDHash::Initialize(int hashLen)
{
	// ��������� ������ ����
	hashGammaLen  = hashLen;

	// �������� ������ ��� �������� ������ �����������
	prgSourceData = new int[hashGammaLen];
	prgHash       = new int[hashGammaLen];

	// ������� ��������������� ����� �������� ������
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

    // ��������� ������ �� ������������ ������� � ������������
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

// XOR-�������� �������� ��� ������������� ���������
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

// ������������� ��������� ��������
void CDHash::InitSubkey()
{
	for (int i = 0; i < INT_LEN; i++) // �������������� ���������
	{
		subkey.Checksums.rgChecksum[i] = 0;
	}
}

// ���� ��� �������� ����� �������� ������
void CDHash::RotateStep(int sourceDataLen)
{
	for (int i = 0; i < (sourceDataLen - 1); i++)
	{
   		prgSourceData[i] = prgSourceData[i + 1];
	}
}

// �������� ����� �������� ������
void CDHash::RotateSourceData(int cNumRounds, int sourceDataLen)
{
	int temp;

	// ������� �������� ������
	for (int i = 0; i < cNumRounds; i++)
	{
		// ��������� ������� ������� ������,�.�. �� ��� ����� � �������
		// �������� ����� �������� ��������� ����� ������
		temp = prgSourceData[0];

		// ���� ��� �������� �������� ������
		RotateStep(sourceDataLen);

		// ��������������� ����������� ������� Hash-����� � ��� �����,
		// (�� ��� "��������" ����� � �������� ������)
		prgSourceData[sourceDataLen - 1] = temp;
	}
}

// ������������ ���������� ������� ��������
void CDHash::NormcNumRounds(int &cNumRounds, int sourceDataLen)
{
	while (cNumRounds > sourceDataLen)
	{
   		cNumRounds -= sourceDataLen;
	}
}

// ������������ ������� ������� �����������
inline void CDHash::NormISubstTbl(int &iSubstTbl)
{
	if (iSubstTbl >= ASC_TABLE_SIZE)
	{
		iSubstTbl -= ASC_TABLE_SIZE;
	}
}

// ���-�������, ���������� �� ������� � ��������
inline int CDHash::HF1(int k, int size)
{
	return (k % size);
}

// ���-�������, ���������� �� ������� � ��������
// (���������� � ������� HF1 ��� �������� �����������)
inline int CDHash::HF1_2(int k, int size)
{
	return 1 + (k % (size - 2));
}

// ������� �����������
inline int CDHash::HFDouble(int k, int size, int numberOfTry)
{
	return (HF1(k, size) + numberOfTry * HF1_2(k, size)) % size;
}

// ������������ ������� � ������� ��������
void CDHash::NormISubkey(int &iSubkey)
{		
	iSubkey = HFDouble(iSubkey, SUBKEY_LEN, nTry1++);
}

// ������������ ������� � ������� ���-�����
void CDHash::NormBegCode(int &begCode)
{	
	begCode = HFDouble(begCode, hashGammaLen, nTry2++);
}

// ���������� ����� �������� ������ � �����������
void CDHash::PrepareSourceData(int sourceDataLen)
{
	int i = 0, j = 0, cNumRounds, iCodeTbl;

	// ����������� ����������� ����� �������� ������ � ������
	// prgHash
	while (i < hashGammaLen)
	{
		// ������ � ������� �����������
		iCodeTbl = prgSourceData[j];

		// ��������� �����������
		prgHash[i] = prgCodeTbl[iCodeTbl];

		// ����������� ������ � ������� ���.����.
		j++;

		// ��������� �� ��������� ���.����.
		if (j >= sourceDataLen)
		{
			// ������� ���.����. �� ������ �������� ������������
			if (sourceDataLen > 1)
			{
				// �������������� ���������� ������� ���.����.
				cNumRounds = (prgSourceData[0] ^ i);

				// ������������ ���������� ����� �������� ���.����.
				NormcNumRounds(cNumRounds, sourceDataLen);

				// �������� ���.����.
				RotateSourceData(cNumRounds, sourceDataLen);
			}
			j = 0; // �������� j (�������� ���������� ���.����. ��
			       // prgSourceData � prgHash) � ������
		}
		i++;
	}
}

// ���������� �������� (������������ ��� ����������� HASH-�����
// prgHash. ������� ����������� �� ������ ����������� ���� �����
// prgHash, ���������� �����������)
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

// ������ ����� ����������� - ������� � BegCode � �� �����
void CDHash::FirstCodeRound(int begCode)
{
	int IB, PB;

	for (int i = begCode; i < (hashGammaLen - 1); i++)

	{
		IB = prgHash[i]; 	 // ���������� ������� ������
		PB = prgHash[i + 1]; // ���������� ������� ������
		prgHash[i] = Encode(IB, PB);
	}
}

// ����������� ������� �������� HASH-����� ��������� - ���������
// ������� �� �� ������
inline void CDHash::MoveCode()
{
   int IB, PB;

   IB = prgHash[hashGammaLen - 1];
   PB = prgHash[0];
   prgHash[hashGammaLen - 1] = Encode(IB, PB);
}

// ������ ����� ����������� - � ������ � �� BegCode
void CDHash::SecondCodeRound(int begCode)
{
	int i, IB, PB;

	// ������ ����� ����������� - � ������ � �� BegCode
	for (i = 0; i < (begCode - 1); i++)
	{
		IB = prgHash[i];      // ���������� ������� ������
		PB = prgHash[i + 1];  // ���������� ������� ������
		prgHash[i] = Encode(IB, PB);
	}
}

// ����������� ���-�����
void CDHash::HashCode()
{
	int begCode = 0;
	iSubkey++;

	// ������������ ������� � ������� ��������
	NormISubkey(iSubkey);

	// ��������� �������� ������� ������ ����������� (BegCode)
	begCode += (unsigned char)(prgHash[0] + subkey.rgubSubkey[iSubkey]);

    // ��������� �������� ���������������
    NormBegCode(begCode);

	// ������ ����� ���������� - ������� � BegCode � �� �����
    FirstCodeRound(begCode);

	// ����������� ������� �������� ��������� - ���������
	// ������� "��" �� ������ ���-�����
  	MoveCode();

	// ������ ����� ����������� - � ������ � �� BegCode
  	SecondCodeRound(begCode);	
}

// ���������� �� ��� ��������� �������� �� HASH-�����
void CDHash::SubkeyCode()
{	
	for (int i = 0; i < SUBKEY_LEN; i++)
	{
   		subkey.rgubSubkey[i] ^= (unsigned char)rgFeedB[i];
	}
}

// ��������� ����� �������� �� HASH-�����
void CDHash::ImposeSubkeyGamma()
{
	int IB, PB;

	// ����� �� �� ������ �������� �� ����������� ����

	// ��������� ����������� �����
	for (int i = 0, j = 0; i < hashGammaLen; i++, j++)
	{
		if (j >= SUBKEY_LEN)
		{
			// ���������� �� - ����������� �������� unSubkey.Subkey[],
			// ���������� �� ����������� ���� prgHash, ������ ��������
			// ������
			SubkeyCode();

			j = 0;
		}

		// ���� ����������� ����� ����� ����������� ��������

		// ���������� ���� ��������� �����
		IB = prgHash[i];
		// ���������� ���� �� ��������
		PB = (int)subkey.rgubSubkey[j];
		
		// ��������� �����������
		prgHash[i] = Encode(IB, PB);
	
		// ��������� ����� ��
		rgFeedB[j] = prgHash[i];
	}
}

// ������� ����������� ������
void CDHash::Hash(char *prgbSourceData, int sourceLen)
{
    int cRounds;

    iSubkey = 0;
	nTry1 = 0;
	nTry2 = 0;
	
	// ��������� �������� ������ � ������ int (�� � ����� ������������)
	for (int i = 0; i < sourceLen; i++)
	{
		prgSourceData[i] = (unsigned char)prgbSourceData[i];
	}

	// �������������� ���������, ���������� ������ ����������� ����
	// � �������� � ����������� Subkey
	InitSubkey();

	// ���������� ���.����.: ����������� ����������� ��� � prgHash
  	PrepareSourceData(sourceLen);
          
	// ����������� ����� �������� ������
	for (cRounds = 0; cRounds < hashGammaLen; cRounds++)	
	{     
		// ���������� ��������
		CalculateSubkey();

		// ����������� ���-����� XOR-�������� ���������
		HashCode();

		// ��������� ����� ��������
		ImposeSubkeyGamma();        

		// ��������� ��������
		if (UpdateProgress != NULL)
		{
			UpdateProgress(cRounds);
		}
	}

	// ���������� ��������� � "�������" ������
	for (int i = 0; i < hashGammaLen; i++)
	{
		prgbSourceData[i] = (char)prgHash[i];
	}
}