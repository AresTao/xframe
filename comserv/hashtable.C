#include "unihashtable.h"
void _SetKey( CHAR*& dest, CHAR* src )
{
	int len = strlen( src );
	dest = new CHAR[len + 1];
	strcpy( dest, src );
}
void _RemoveKey( CHAR*& key )
{
	if ( key ) delete[] key;
	key = 0;
}
BOOL _CompareKey( CHAR*& keyA, CHAR*& keyB )
{
	return ( strcmp(keyA, keyB) == 0 );
}
HASH_CODE _GetHash( CHAR*& key )
{
	HASH_CODE temp_node;
	UINT i = strlen (key);	
	if (i <= 0) 
	{
		temp_node =0;
	}
	else
	{
		// ¼ÆËã¼üÖµ
		temp_node = i *10;
		temp_node += (HASH_CODE)key[0];
		temp_node += (HASH_CODE)key[i-1];
		temp_node += (HASH_CODE)key[i/2];		
	}
	return temp_node;
}
