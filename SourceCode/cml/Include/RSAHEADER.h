
const char gHeaderChecker[36] = "Artic Gamers Encrypted File System";

union RSAHEADER
{
	byte byteHeader[40];

	struct
	{
		char szHeaderChecker[36];
		unsigned long nFileLength;
	};

	RSAHEADER()					{ memset( byteHeader, 0, sizeof( byteHeader));								}
	void SetHeaderChecker()		{ strcpy( szHeaderChecker, gHeaderChecker);									}
	bool IsValidHeader()		{ return (strcmp( szHeaderChecker, gHeaderChecker) == 0) ? true : false;	}
	void SetLength(ULONG nSize)	{ nFileLength = nSize;														}
	ULONG GetLength()			{ return nFileLength;														}
};