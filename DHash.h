//**********************************************************
//* ����� CDHash. ������� ����������� � ������ �����������
//*               ���������� ����������
//**********************************************************
//* ����:    1.10.2005
//* ������:  4.00
//* �����:   �������� ���� ��������� (DrAF)
//**********************************************************

#pragma once

#define ASC_TABLE_SIZE 256 // ����� ASCII-�������
#define SUBKEY_LEN 16	   // ����� �������� unSubkey.Subkey[]
#define MODKEY_LEN 3	   // ����� ����� ��� ����������� ������
#define INT_LEN 4	       // ����� � ������ ���������� ���� int
#define NULL 0

// ����������� ��� �������������� ������� ����������� ����� � �������,
// ��������� �� 16 ����
typedef union
{
	struct TChecksums	// ���� ������� �� 4-�� ���������� ���� int
	{
   		int rgChecksum[4];
	} Checksums;

	unsigned char rgubSubkey[SUBKEY_LEN];  // ...������������ ��� ������
   										   // unsigned char �� 16
                                           // ��������� (4*4=16)
} TSubkey;

// ����� ����������� ����� ������
class CDHash  
{
public:	
	CDHash();
	CDHash(int HashLen);
	CDHash(int HashLen, void (*UpdateProgress)(int progress));
	virtual ~CDHash();

	// ������� ����������� ������
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
