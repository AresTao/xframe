#ifndef _UNIHASHTABLE_H_
#define _UNIHASHTABLE_H_

#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "comtypedef.h"


///////////////////////////////////////////////////////////////////////
// ���峣��ֵ
///////////////////////////////////////////////////////////////////////

#define DEFAULT_HASHTABLE_LENGTH	16		// ������ȱʡ����
#define DEFAULT_HASHTABLE_LOADFACTOR	0.75f	// ȱʡ��װ������

#define HASHTABLE_MIN_LENGTH		8		// ��������С����
#define	HASHTABLE_MAX_LENGTH		131072	// ��������󳤶ȣ�
// 2��17�η����Ѿ�����10��

#define HASHTABLE_MIN_LOADFACTOR	0.4f	// װ�����ӵ���Сֵ
#define HASHTABLE_MAX_LOADFACTOR	1.5f	// װ�����ӵ����ֵ

//  modified by shuangkai 2004-3-4 
// moved to "comtypedef.h"
//typedef UINT HASH_CODE;		// ����hashֵ������
//typedef ULONG HASH_CODE;		// ����hashֵ������
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// ���� CHashTable< TKey, TValue > ��
///////////////////////////////////////////////////////////////////////

template < class TKey, class TValue >
class CHashTable	// ��TKeyΪ�����ͣ�TValueΪֵ���͵Ĺ�����
{
	private:
		class _Entry	// ����������еĽڵ�����
		{
			public:
				HASH_CODE	m_hash;			// �˽ڵ�Ĺ���ֵ
				TKey		m_key;			// �˽ڵ�ļ�ֵ
				TValue		m_value;		// �˽ڵ������
				_Entry*		m_next;			// ָ����һ���ڵ�

				// the following two pointers are for record the put sequence
				_Entry*		m_s_prev;
				_Entry*		m_s_next;

				// �������ýڵ��ֵ�ĺ�������
				typedef void ( *PF_SETKEY )( TKey& dest, TKey src );

				// ��������ڵ��ֵ�ĺ�������
				typedef void ( *PF_REMOVEKEY )( TKey& key );

				// ��̬�ĺ���ָ�룬ָ�����ýڵ��ֵ�ĺ���
				static PF_SETKEY	m_spfSetKey;

				// ��̬�ĺ���ָ�룬ָ������ڵ��ֵ�ĺ���
				static PF_REMOVEKEY	m_spfRemoveKey;

			public:
				// �ڵ�Ĺ��캯�������л���ü�ֵ���ú�������������ڵ�ļ�
				_Entry( HASH_CODE hash, TKey& key, TValue& value, _Entry* next ) :
					m_hash( hash ), m_value( value ), m_next( next )
			{
				( *m_spfSetKey )( m_key, key );
			}

				// �ڵ���������������е��ü�ֵ�������������˽ڵ�ļ�
				~_Entry( void )
				{
					( *m_spfRemoveKey )( m_key );
				}

		};	// �ڵ����Ͷ������
		typedef _Entry* _PEntry;

	private:
		_PEntry*	m_table;			// ���������
		UINT		m_entryCount;		// ��¼Ŀǰ���еĽڵ�����
		UINT		m_tableLength;		// Ŀǰ�Ĺ�������
		UINT		m_modCount;			// ���������£��޸ģ��Ĵ���
		DOUBLE		m_loadFactor;		// ������װ������
		UINT		m_threshold;		// �������ٹ����㷨�Ĵ�������ֵ

		_PEntry		m_pcurEntry;		// ���ڼ�¼����ʱ�ĵ�ǰ�ڵ�
		UINT		m_uicurIndex;		// ����ʱ�����ص�ǰ�ڵ����ڵı������ֵ

		// �������ڱȽ��������ĺ���������
		typedef BOOL ( *PF_CMP )( TKey& keyA, TKey& keyB );

		// �������ڼ������ֵ�ĺ���������
		typedef HASH_CODE ( *PF_HASH )( TKey& key );

		PF_CMP		m_pfCompareKey;		// ����ָ�룬ָ���ֵ�ȽϺ���
		PF_HASH		m_pfGetHash;		// ����ָ�룬ָ�����ֵ���㺯��

		// Ϊ�˱�֤�����Ƽ��ݣ�������uniframe��ʱ���������±����ϵ�.so��
		// ����û�ж���������Ա������ʹ��m_table׷���ں������������λ��
		_PEntry seqHead;
		_PEntry seqTail;
		_PEntry seqCur;

	public:
		/*	�ֱ����ڲ�ͬĿ�ĵ�������캯�������У�

			length			����ָ��������ĳ�ʼ���ȣ��������������
			�Ѷ���Ĺ�������С��󳤶�ֵ��������ʹ���У�
			�����ٹ����㷨��������ĳ��ȿ��ܻ�������

			loadFactor		ָ��װ�����ӣ���������������Ѷ����
			��������С���װ�����ӡ�

			pfCompareKey	ָ����ֵ�ȽϺ�����

			pfGetHash		ָ������ֵ���㺯����

			����û��ָ���Ĳ���������ʹ���Ѷ����ȱʡֵ��*/
		CHashTable();
		CHashTable( UINT length );
		CHashTable( UINT length, DOUBLE loadFactor );
		CHashTable( PF_HASH pfGetHash );
		CHashTable( PF_CMP pfCompareKey, PF_HASH pfGetHash );
		CHashTable( UINT length, DOUBLE loadFactor,
				PF_CMP pfCompareKey, PF_HASH pfGetHash );

		// �������������������Ҫ���ͷ��ڴ�ռ�
		~CHashTable();

	public:
		// ��չ�����
		void	clear( void );

		// �������������Ƿ��Ѱ��������
		BOOL	containsKey( TKey key );

		// ȡ���ӹ������е����ݣ��������������
		BOOL	get( TKey key, TValue& value );
		BOOL	get_r( TKey& key, TValue& value );

		// ����һ�����ݣ�����ֻ�Ǹ���ԭ��������
		TValue	put( TKey key, TValue value );
		TValue	put_r( TKey& key, TValue& value );
		void	put_v( TKey& key, TValue& value );

		// �ӹ����������һ������
		BOOL	remove( TKey key );

		// ��ȡĿǰ�������еı�����Ŀ
		UINT	size( void );

		// �жϹ������Ƿ�Ϊ��
		BOOL	isEmpty( void );

		// ���ñ���ָ�룬һ�α���ǰ������еĶ���
		BOOL	reset( void );
		void	resetSeq( void ); // for sequencial

		// ��ȡ��һ��������������������ʵ�ֱ�����
		BOOL	getNext( TValue & value );
		// ��ȡ��һ�����������ȡֵ��Ҳ����Keyֵ��//Added by LJL 2008.08.5
		BOOL	getNext( TKey &key, TValue & value );
		BOOL	getNextSeq( TKey &key, TValue & value ); // for sequencial

	private:
		// �ɹ��캯�����õĳ�ʼ������������ͳһ������ֹ��캯��
		void	initTable( UINT length, DOUBLE loadFactor,
				PF_CMP pfCompareKey, PF_HASH pfGetHash );

		// �ٹ�������
		void	rehash( void );

		// ͨ������ֵӳ�������������
#define GET_HASH_INDEX(hash) ((hash) % m_tableLength)

		// ������һ�������ڵ�
		BOOL	searchNextEntry( void );

		void addSequence(_Entry*);
		void delSequence(_Entry*);
		void printSequence();
};	// �����������

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// ����ȱʡ�ĺ�������ֵ���ú�������ֵ�����������ֵ�ȽϺ�������������
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Function		: _SetKey< TKey >
//
// Description	: ȱʡ�ļ�ֵ���ú���������������ڵ��еľ�̬����ָ�룩��
//				  ���ڳ��ַ�������������м����͡�
//
// Args			:
//				  1. dest	TKey&	�����õļ�
//				  2. src	TKey	��ֵ��Դ
//
// Return values: None
///////////////////////////////////////////////////////////////////////
	template < class TKey >
void _SetKey( TKey& dest, TKey src )
{
	dest = src;
}

///////////////////////////////////////////////////////////////////////
// Function		: _SetKey< CHAR* >
//
// Description	: �����ַ����ļ�ֵ���ú��������ڲ������ڴ�ռ䣬
//				  ���������ַ�����
//
// Args			:
//				  1. dest	CHAR*&	�����õļ�
//				  2. src	CHAR*	��ֵ��Դ
//
// Return values: None
///////////////////////////////////////////////////////////////////////

/*template <>
  void _SetKey( CHAR*& dest, CHAR* src )
  {
  int len = strlen( src );
  if ( dest!= NULL ) delete[] dest;
  dest = new CHAR[len + 1];
  strcpy( dest, src );
  }
  */
void _SetKey( CHAR*& dest, CHAR* src );

///////////////////////////////////////////////////////////////////////
// Function		: _RemoveKey< TKey >
//
// Description	: ȱʡ�ļ�ֵ�������������������ڵ��еľ�̬����ָ�룩��
//				  ���ڳ��ַ�������������м����͡�
//
// Args			:
//				  1. key	TKey&	������ļ�
//
// Return values: None
///////////////////////////////////////////////////////////////////////
	template < class TKey >
void _RemoveKey( TKey& key )
{
	return;
}

///////////////////////////////////////////////////////////////////////
// Function		: _RemoveKey< CHAR* >
//
// Description	: �����ַ����ļ�ֵ����������ͷ���ǰ������ڴ�ռ䡣
//
// Args			:
//				  1. key	CHAR*&	������ļ�
//
// Return values: None
///////////////////////////////////////////////////////////////////////
void _RemoveKey( CHAR*& key );

///////////////////////////////////////////////////////////////////////
// Function		: _CompareKey< TKey >
//
// Description	: ȱʡ�ļ�ֵ�ȽϺ����������������еĺ���ָ�룩��
//				  ���ڳ��ַ�������������м�����
//
// Args			:
//				  1. keyA	TKey	���ڱȽϵļ�A
//				  2. keyB	TKey	���ڱȽϵļ�B
//
// Return values:
//				  BOOL	��A���B�Ƿ����
///////////////////////////////////////////////////////////////////////
	template < class TKey >
BOOL _CompareKey( TKey& keyA, TKey& keyB )
{
	return ( keyA == keyB );
}

///////////////////////////////////////////////////////////////////////
// Function		: _CompareKey< CHAR* >
//
// Description	: �����ַ����ļ�ֵ�ȽϺ���
//
// Args			:
//				  1. keyA	CHAR*	���ڱȽϵļ�A
//				  2. keyB	CHAR*	���ڱȽϵļ�B
//
// Return values:
//				  BOOL	��A���B�Ƿ����
///////////////////////////////////////////////////////////////////////
BOOL _CompareKey( CHAR*& keyA, CHAR*& keyB );

///////////////////////////////////////////////////////////////////////
// Function		: _GetHash< TKey >
//
// Description	: ȱʡ�Ĺ��������������������еĺ���ָ�룩��
//				  ���ڳ��ַ�������������м�����
//
// Args			:
//				  1. key	TKey	���ڼ������ֵ�ļ�
//
// Return values:
//				  HASH_CODE		������Ӧ�Ĺ���ֵ
///////////////////////////////////////////////////////////////////////
	template < class TKey >
HASH_CODE _GetHash( TKey& key )
{
	// ֱ��ʹ������ת�����Զ������͵ļ���Ҫ���ش����͵�ת����������
	return ( HASH_CODE )key;
}

///////////////////////////////////////////////////////////////////////
// Function		: _GetHash< CHAR* >
//
// Description	: �����ַ����Ĺ�������������һ�ּ򵥵�ʵ�֣�
//				  ֻ�Ǹ����ַ����ĵ�һλ���������ֵ�����ڲ����õ������
//				  Ӧ��ָ����ʹ���Զ���Ĺ���������
//
// Args			:
//				  1. key	CHAR*	���ڼ������ֵ�ļ�
//
// Return values:
//				  HASH_CODE		������Ӧ�Ĺ���ֵ
///////////////////////////////////////////////////////////////////////
HASH_CODE _GetHash( CHAR*& key );

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// CHashTable< TKey, TValue > �ķ���ʵ��
///////////////////////////////////////////////////////////////////////

// ������Ĺ��캯�������е�����˽�з���initTable()����ʼ��������
	template < class TKey, class TValue >
inline CHashTable< TKey, TValue >::CHashTable()
{
	initTable( DEFAULT_HASHTABLE_LENGTH, DEFAULT_HASHTABLE_LOADFACTOR,
			_CompareKey, _GetHash );
}

	template < class TKey, class TValue >
inline CHashTable< TKey, TValue >::CHashTable( UINT length )
{
	initTable( length, DEFAULT_HASHTABLE_LOADFACTOR,
			_CompareKey, _GetHash );
}

	template < class TKey, class TValue >
inline CHashTable<TKey, TValue>::CHashTable(PF_HASH pfGetHash )
{
	initTable( DEFAULT_HASHTABLE_LENGTH, DEFAULT_HASHTABLE_LOADFACTOR, _CompareKey, pfGetHash );
}

	template < class TKey, class TValue >
inline CHashTable< TKey, TValue >::CHashTable( UINT length,
		DOUBLE loadFactor )
{
	initTable( length, loadFactor, _CompareKey, _GetHash );
}

	template < class TKey, class TValue >
inline CHashTable< TKey, TValue >::CHashTable( PF_CMP pfCompareKey,
		PF_HASH pfGetHash )
{
	initTable( DEFAULT_HASHTABLE_LENGTH,
			DEFAULT_HASHTABLE_LOADFACTOR,
			pfCompareKey, pfGetHash );
}

	template < class TKey, class TValue >
inline CHashTable< TKey, TValue >::CHashTable( UINT length,
		DOUBLE loadFactor,
		PF_CMP pfCompareKey,
		PF_HASH pfGetHash )
{
	initTable( length, loadFactor, pfCompareKey, pfGetHash );
}

// ���ڳ�ʼ���������˽�з���
	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::initTable( UINT length,
		DOUBLE loadFactor,
		PF_CMP pfCompareKey,
		PF_HASH pfGetHash )
{
	// ����Ԥ����������Сֵ���Ա���������
	if ( length < HASHTABLE_MIN_LENGTH )
		m_tableLength = HASHTABLE_MIN_LENGTH;
	else if ( length > HASHTABLE_MAX_LENGTH )
		m_tableLength = HASHTABLE_MAX_LENGTH;
	else
		m_tableLength = length;
	// �����ռ䣬�����
	m_table = new _PEntry[m_tableLength];
	memset( m_table, 0, sizeof( _PEntry ) * m_tableLength );
	seqHead = NULL;
	seqTail = NULL;
	seqCur = NULL;

	// ����Ԥ����������Сֵ����װ�����ӽ�������
	if ( loadFactor < HASHTABLE_MIN_LOADFACTOR )
		m_loadFactor = HASHTABLE_MIN_LOADFACTOR;
	else if ( loadFactor > HASHTABLE_MAX_LOADFACTOR )
		m_loadFactor = HASHTABLE_MAX_LOADFACTOR;
	else
		m_loadFactor = loadFactor;

	// �ɱ���װ�����ӣ�������������ٹ����㷨�Ĵ�������ֵ
	m_threshold = ( UINT )( m_tableLength * m_loadFactor );

	// ��ʼ����ֵ�ȽϺ����͹��������ĺ���ָ��
	if ( !pfCompareKey ) m_pfCompareKey = _CompareKey;
	else m_pfCompareKey = pfCompareKey;

	if ( !pfGetHash )
		m_pfGetHash = _GetHash;
	else
		m_pfGetHash = pfGetHash;

	// ��ʼ���ڵ�����͸��¼���
	m_entryCount = 0;
	m_modCount = 0;

	// ��ʼ�����ڱ����ĵ�ǰ�ڵ�ָ�룬�Լ�����ֵ
	m_pcurEntry = NULL;
	m_uicurIndex = 0;
}

	template < class TKey, class TValue >
CHashTable< TKey, TValue >::~CHashTable()
{
	clear();			// ��չ�����
	delete[] m_table;	// �ͷű�ռ�
	m_table = NULL;
	m_tableLength = 0;
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::Clear
//
// Description	: ��չ�����
//
// Args			: None
//
// Return values: None
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::clear( void )
{
	// ɨ�������������ͷ�ÿһ���ڵ�
	for ( UINT i = 0; i < m_tableLength; i ++ )
	{
		while ( m_table[i] )
		{
			_PEntry p = m_table[i];
			m_table[i] = p->m_next;
			p->m_next = NULL;
			delSequence(p);
			delete p;
		}
	}
	m_entryCount = 0;	// �ڵ��������Ϊ0
	m_modCount ++;		// ���£��޸ģ�������1

	m_pcurEntry = NULL;
	m_uicurIndex = 0;
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::containsKey
//
// Description	: �������������Ƿ��Ѱ��������
//
// Args			:
//				  1. Key	TKey	���ڼ����ļ�
//
// Return values:
//				  BOOL	�Ƿ���������
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::containsKey( TKey key )
{
	HASH_CODE hash = ( *m_pfGetHash )( key );	// �������ֵ
	UINT index = GET_HASH_INDEX( hash );				// ӳ�������������

	// �������������µ����нڵ�
	_PEntry p = m_table[index];
	while ( p )
	{
		// ���ü�ֵ�ȽϺ������ж��Ƿ����������Ľڵ�
		if ( ( *m_pfCompareKey )( key, p->m_key ) )
		{
			return TRUE;	// �ҵ�������TRUE
		}
		p = p->m_next;
	}
	return FALSE;	// û���ҵ�������FALSE
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::get
//
// Description	: �������������е�һ����
//
// Args			:
//				  1. key	TKey	���ڼ����ļ�
//				  2. value	TValue&	��ż�����������
//
// Return values:
//				  BOOL	�Ƿ�ɹ���������Ӧ����
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::get_r( TKey& key, TValue& value )
{
	_PEntry p = m_table[GET_HASH_INDEX((*m_pfGetHash)(key))];
	while ( p )
	{
		// ���ü�ֵ�ȽϺ������ж��Ƿ����������Ľڵ�
		if ( ( *m_pfCompareKey )( key, p->m_key ) )
		{
			// �ҵ��������ڷ������ݵ�value������ֵ��Ȼ�󷵻�TRUE��
			// ����ʹ��ֱ�ӡ�=����ֵ�����ڸ������ͣ�Ӧ���忽�����캯��
			value = p->m_value;
			return TRUE;
		}
		p = p->m_next;
	}
	return FALSE;	// û�ҵ���Ӧ�������FALSE
}
	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::get( TKey key, TValue& value )
{
	return get_r(key, value);
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::Put
//
// Description	: �ڹ������д��һ�������˼����ж�Ӧ����򸲸�ԭ
//				  �е����ݣ������½�һ����Ŀ
//
// Args			:
//				  1. key	TKey	��������Ӧ�ļ�
//				  2. value	TValue	����ŵ�����
//
// Return values:
//				  TValue	���������ڹ����������ж�Ӧ�����ݣ��򷵻�
//							������ݣ����򷵻��¼��������
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
TValue CHashTable< TKey, TValue >::put_r( TKey& key, TValue& value )
{
	HASH_CODE hash = ( *m_pfGetHash )( key );	// �������ֵ
	UINT index = GET_HASH_INDEX( hash );				// ӳ�������������
	_PEntry p = m_table[index];

	m_modCount ++;		// ���¼�����1

	// �쿴�Ƿ����ж�Ӧ�Ľڵ�
	while ( p )
	{
		if ( ( *m_pfCompareKey )( key, p->m_key ) )
		{
			// �ҵ��ɵĽڵ�
			TValue oldValue = p->m_value;
			p->m_value = value;				// ���½ڵ�����
			return oldValue;				// ���ؾɵ�����
		}
		p = p->m_next;
	}

	// �¼���һ���ڵ�
	m_entryCount ++;	// �ڵ������1

	// �ж����еı���
	m_pcurEntry = NULL;
	m_uicurIndex = 0;

	// ����ڵ����ﵽ�򳬹��ٹ�������ֵ���򴥷��ٹ����㷨
	if ( m_entryCount >= m_threshold )
	{
		rehash();					// �����ٹ����㷨
		index = GET_HASH_INDEX( hash );	// ����ӳ�������������
	}

	// �����µĽڵ㣬�����뵽��������
	_PEntry newEntry = new _Entry( hash, key, value, m_table[index] );
	m_table[index] = newEntry;
	addSequence(newEntry);

	return value;	// �����¼��������
}
	template < class TKey, class TValue >
TValue CHashTable< TKey, TValue >::put( TKey key, TValue value )
{
	return put_r(key, value);
}

	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::put_v( TKey& key, TValue& value )
{
	HASH_CODE hash = ( *m_pfGetHash )( key );// �������ֵ
	UINT index = GET_HASH_INDEX( hash );		// ӳ�������������
	_PEntry p = m_table[index];

	m_modCount ++;	// ���¼�����1

	// �쿴�Ƿ����ж�Ӧ�Ľڵ�
	while ( p )
	{
		if ( ( *m_pfCompareKey )( key, p->m_key ) )
		{
			// �ҵ��ɵĽڵ�
			p->m_value = value;	// ���½ڵ�����
			return;
		}
		p = p->m_next;
	}

	// �¼���һ���ڵ�
	m_entryCount ++;	// �ڵ������1

	// �ж����еı���
	m_pcurEntry = NULL;
	m_uicurIndex = 0;

	// ����ڵ����ﵽ�򳬹��ٹ�������ֵ���򴥷��ٹ����㷨
	if ( m_entryCount >= m_threshold )
	{
		rehash();					// �����ٹ����㷨
		index = GET_HASH_INDEX( hash );	// ����ӳ�������������
	}

	// �����µĽڵ㣬�����뵽��������
	_PEntry newEntry = new _Entry( hash, key, value, m_table[index] );
	m_table[index] = newEntry;
	addSequence(newEntry);
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::Remove
//
// Description	: �ӹ��������Ƴ�һ����
//
// Args			:
//				  1. key	TKey	�����Ӧ�ļ�
//
// Return values:
//				  BOOL	�Ƿ�ɹ��Ƴ�
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::remove( TKey key )
{
	HASH_CODE hash = ( *m_pfGetHash )( key );	// �������ֵ
	UINT index = GET_HASH_INDEX( hash );				// ӳ�������������
	_PEntry p = m_table[index];

	if ( !p ) return FALSE;	// ����������û�нڵ㣬��û�ҵ�ָ������

	// �жϴ��������µ�һ���ڵ��Ƿ�����������
	if ( ( *m_pfCompareKey )( key, p->m_key ) )
	{
		// ��Ҫɾ�������Ǵ��������µĵ�һ���ڵ�
		// �Ƴ�����ڵ�
		if ( p == m_pcurEntry )	// ���Ƴ��Ľڵ������Ǳ����еĵ�ǰ�ڵ�
			searchNextEntry();

		m_table[index] = p->m_next;
		delSequence(p);
		delete p;
		m_entryCount --;	// �ڵ������1
		m_modCount ++;		// ���¼�����1

		return TRUE;
	}

	// �������������������Ľڵ�
	_PEntry prev = p;
	p = p->m_next;
	while ( p )
	{
		if ( ( *m_pfCompareKey )( key, p->m_key ) )
		{
			// �ҵ���Ҫɾ������
			// �Ƴ�����ڵ�

			//Modified by Draon.(Check by LiJinglin), 2002.12.25
			// �����µı�̹淶������Ӧ���ں���
			//if ( p = m_pcurEntry )		// ���Ƴ��Ľڵ������Ǳ����еĵ�ǰ�ڵ�
			if (  m_pcurEntry == p )		// ���Ƴ��Ľڵ������Ǳ����еĵ�ǰ�ڵ�
				searchNextEntry();

			prev->m_next = p->m_next;
			delSequence(p);
			delete p;
			m_entryCount --;	// �ڵ������1
			m_modCount ++;		// ���¼�����1
			return TRUE;
		}
		prev = p;
		p = p->m_next;
	}

	return FALSE;	// û���ҵ�ָ������
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::size
//
// Description	: ��ȡ��ǰ���������������
//
// Args			: None
//
// Return values:
//				  UINT	��ǰ�������
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
inline UINT CHashTable< TKey, TValue >::size( void )
{
	return m_entryCount;
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::isEmpty
//
// Description	: �жϹ������Ƿ�Ϊ��
//
// Args			: None
//
// Return values:
//				  BOOL	�������Ƿ�Ϊ��
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
inline BOOL CHashTable< TKey, TValue >::isEmpty( void )
{
	if ( m_entryCount == 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::reset
//
// Description	: ���ñ���ָ�룬һ�α���ǰ������еĶ�����������ָ��
//					ָ����еĵ�һ���ڵ㡣
//
// Args			: None
//
// Return values:
//				  BOOL	���������Ϊ���򷵻�FALSE�����򷵻�TRUE��
///////////////////////////////////////////////////////////////////////

	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::resetSeq( void )
{
	seqCur = seqHead;
}

	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::reset( void )
{
	for ( UINT i = 0; i < m_tableLength; i ++ )
	{
		if ( m_table[i] )
		{
			m_pcurEntry = m_table[i];
			m_uicurIndex = i;
			return TRUE;
		}
	}
	m_pcurEntry = NULL; //added by lxm. 2008.09.28��if is empty, should set it to NULL.
	m_uicurIndex =0;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::getNext
//
// Description	: ��ȡ��һ��������������������ʵ�ֱ�����
//
// Args			: 1. value	TValue&	���ڴ����ȡ�õ��
//
// Return values:
//				  BOOL	����FALSE��ʾû��ȡ���Ҳ����˵û�к������ˡ�
///////////////////////////////////////////////////////////////////////

	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::getNext( TValue & value )
{
	if ( !m_pcurEntry )
		return FALSE;

	//����������ֵ
	value = m_pcurEntry->m_value;

	// ������һ���ڵ�

	searchNextEntry();

	return TRUE;
}

//Added by LJL 2008.08.5
	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::getNext( TKey &key, TValue &value )
{
	if ( !m_pcurEntry )
		return FALSE;

	//����������ֵ
	key = m_pcurEntry->m_key;
	value = m_pcurEntry->m_value;

	// ������һ���ڵ�
	searchNextEntry();

	return TRUE;
}

	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::getNextSeq( TKey &key, TValue &value )
{
	if(seqCur == NULL)
		return FALSE;
	key = seqCur->m_key;
	value = seqCur->m_value;
	seqCur = seqCur->m_s_next;
	return  TRUE;
}

	template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::searchNextEntry( void )
{
	if ( m_pcurEntry->m_next )
	{
		m_pcurEntry = m_pcurEntry->m_next;
		return TRUE;
	}
	else
	{
		for ( UINT i = m_uicurIndex + 1; i < m_tableLength; i ++ )
		{
			if ( m_table[i] )
			{
				m_pcurEntry = m_table[i];
				m_uicurIndex = i;
				return TRUE;
			}
		}

		// ��Ҳû�к����ڵ���
		m_pcurEntry = NULL;
		m_uicurIndex = 0;
		return FALSE;
	}
}

	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::addSequence(_Entry* entry)
{
	if(seqHead == NULL || seqTail == NULL)
	{
		entry->m_s_next = NULL;
		entry->m_s_prev = NULL;
		seqHead = seqTail = entry;
		return;
	}
	entry->m_s_next = NULL;
	entry->m_s_prev = seqTail;
	seqTail->m_s_next = entry;
	seqTail = entry;
}
	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::delSequence(_Entry* entry)
{
	if(entry->m_s_next == NULL && entry->m_s_prev == NULL) // only one
	{
		seqHead = seqTail = NULL;
		return;
	}
	if(entry->m_s_next == NULL) // is tail
	{
		seqTail = entry->m_s_prev;
		seqTail->m_s_next = NULL;
		return;
	}
	if(entry->m_s_prev == NULL) // is head
	{
		seqHead = entry->m_s_next;
		seqHead->m_s_prev = NULL;
		return;
	}
	entry->m_s_next->m_s_prev = entry->m_s_prev;
	entry->m_s_prev->m_s_next = entry->m_s_next;
}
	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::printSequence()
{
	int i = 0;
	_Entry* p = seqHead;
	while(p!=NULL)
	{
		printf("[%d] %p <- %p -> %p\n", i, p->m_s_prev, p, p->m_s_next);
		p = p->m_s_next;
		i++;
	}
	printf("%d entries\n\n", i);
}

///////////////////////////////////////////////////////////////////////
// Function		: CHashTable< TKey, TValue >::rehash
//
// Description	: �ٹ������������������������Ŀͻ���ٹ�������ֵ����
//				  �������ܳ��Ȳ������������ֵʱ����������ᱻ���á�
//				  ��Ч��������������ȣ����·���������λ�á�
//
// Args			: None
//
// Return values: None
///////////////////////////////////////////////////////////////////////
	template < class TKey, class TValue >
void CHashTable< TKey, TValue >::rehash( void )
{
	// �������������ﵽ���ޣ���������
	if ( m_tableLength >= HASHTABLE_MAX_LENGTH ) return;

	UINT oldLength = m_tableLength;
	m_tableLength = m_tableLength * 2;	// ����������Ϊԭ��������
	// �µı����ܴ��ڱ����ֵ
	if ( m_tableLength >= HASHTABLE_MAX_LENGTH )
		m_tableLength = HASHTABLE_MAX_LENGTH;

	// �����µı�ռ䣬����ʼ��
	_PEntry* oldTable = m_table;
	m_table = new _PEntry[m_tableLength + 3];
	memset( m_table, 0, sizeof( _PEntry ) * m_tableLength );

	// �����µ��ٹ�������ֵ
	m_threshold = ( UINT )( m_tableLength * m_loadFactor );

	m_modCount ++;	// ���¼�����1

	// ת�ƾɱ��еĽڵ����±�
	_PEntry pentry;
	UINT index;
	for ( UINT i = 0; i < oldLength; i ++ )
	{
		while ( oldTable[i] )
		{

			pentry = oldTable[i];
			oldTable[i] = oldTable[i]->m_next;
			// ���ݽڵ��м�¼�Ĺ���ֵ������ӳ�������������
			index = GET_HASH_INDEX( pentry->m_hash );
			pentry->m_next = m_table[index];
			m_table[index] = pentry;
		}
	}
	delete[] oldTable;	// ɾ���ɵĹ������ͷ��ڴ�ռ�
}


///////////////////////////////////////////////////////////////////////
// CHashTable< TKey, TValue >::_Entry ��ľ�̬���ԣ�����ָ�룩��ʼ��
///////////////////////////////////////////////////////////////////////

	template < class TKey, class TValue >
void ( *CHashTable< TKey, TValue >::_Entry::m_spfSetKey )( TKey&, TKey )
	= _SetKey;

	template < class TKey, class TValue >
void ( *CHashTable< TKey, TValue >::_Entry::m_spfRemoveKey )( TKey& )
	= _RemoveKey;

	///////////////////////////////////////////////////////////////////////

#endif	// _CHASHTABLE_H
