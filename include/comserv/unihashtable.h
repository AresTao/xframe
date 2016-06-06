#ifndef _UNIHASHTABLE_H_
#define _UNIHASHTABLE_H_

#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "comtypedef.h"

#define DEFAULT_HASHTABLE_LENGTH	16		
#define DEFAULT_HASHTABLE_LOADFACTOR	0.75f	

#define HASHTABLE_MIN_LENGTH		8	
#define	HASHTABLE_MAX_LENGTH		131072	

#define HASHTABLE_MIN_LOADFACTOR	0.4f	
#define HASHTABLE_MAX_LOADFACTOR	1.5f	

template < class TKey, class TValue >
class CHashTable	
{
    private:
        class _Entry	
        {
            public:
                HASH_CODE	m_hash;			
                TKey		m_key;			
                TValue		m_value;		
                _Entry*		m_next;			

                // the following two pointers are for record the put sequence
                _Entry*		m_s_prev;
                _Entry*		m_s_next;

                typedef void ( *PF_SETKEY )( TKey& dest, TKey src );

                typedef void ( *PF_REMOVEKEY )( TKey& key );

                static PF_SETKEY	m_spfSetKey;

                static PF_REMOVEKEY	m_spfRemoveKey;

            public:
                _Entry( HASH_CODE hash, TKey& key, TValue& value, _Entry* next ) :
                    m_hash( hash ), m_value( value ), m_next( next )
            {
                ( *m_spfSetKey )( m_key, key );
            }

                ~_Entry( void )
                {
                    ( *m_spfRemoveKey )( m_key );
                }

        };	
        typedef _Entry* _PEntry;

    private:
        _PEntry*	m_table;		
        UINT		m_entryCount;		
        UINT		m_tableLength;		
        UINT		m_modCount;		
        DOUBLE		m_loadFactor;		
        UINT		m_threshold;		

        _PEntry		m_pcurEntry;		
        UINT		m_uicurIndex;		

        typedef BOOL ( *PF_CMP )( TKey& keyA, TKey& keyB );

        typedef HASH_CODE ( *PF_HASH )( TKey& key );

        PF_CMP		m_pfCompareKey;		
        PF_HASH		m_pfGetHash;		

        _PEntry seqHead;
        _PEntry seqTail;
        _PEntry seqCur;

    public:
	CHashTable();
        CHashTable( UINT length );
        CHashTable( UINT length, DOUBLE loadFactor );
        CHashTable( PF_HASH pfGetHash );
        CHashTable( PF_CMP pfCompareKey, PF_HASH pfGetHash );
        CHashTable( UINT length, DOUBLE loadFactor,
                PF_CMP pfCompareKey, PF_HASH pfGetHash );

        ~CHashTable();

    public:
        void	clear( void );

        BOOL	containsKey( TKey key );

        BOOL	get( TKey key, TValue& value );
        BOOL	get_r( TKey& key, TValue& value );

        TValue	put( TKey key, TValue value );
        TValue	put_r( TKey& key, TValue& value );
        void	put_v( TKey& key, TValue& value );

        BOOL	remove( TKey key );

        UINT	size( void );

        BOOL	isEmpty( void );

        BOOL	reset( void );
        void	resetSeq( void ); // for sequencial

        BOOL	getNext( TValue & value );
        BOOL	getNext( TKey &key, TValue & value );
        BOOL	getNextSeq( TKey &key, TValue & value ); // for sequencial

    private:
        void	initTable( UINT length, DOUBLE loadFactor,
                PF_CMP pfCompareKey, PF_HASH pfGetHash );

        void	rehash( void );

#define GET_HASH_INDEX(hash) ((hash) % m_tableLength)

        BOOL	searchNextEntry( void );

        void addSequence(_Entry*);
        void delSequence(_Entry*);
        void printSequence();
};	

    template < class TKey >
void _SetKey( TKey& dest, TKey src )
{
    dest = src;
}

void _SetKey( CHAR*& dest, CHAR* src );
    template < class TKey >
void _RemoveKey( TKey& key )
{
    return;
}
void _RemoveKey( CHAR*& key );
    template < class TKey >
BOOL _CompareKey( TKey& keyA, TKey& keyB )
{
    return ( keyA == keyB );
}
BOOL _CompareKey( CHAR*& keyA, CHAR*& keyB );
    template < class TKey >
HASH_CODE _GetHash( TKey& key )
{
    return ( HASH_CODE )key;
}
HASH_CODE _GetHash( CHAR*& key );
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

    template < class TKey, class TValue >
void CHashTable< TKey, TValue >::initTable( UINT length,
        DOUBLE loadFactor,
        PF_CMP pfCompareKey,
        PF_HASH pfGetHash )
{
    if ( length < HASHTABLE_MIN_LENGTH )
        m_tableLength = HASHTABLE_MIN_LENGTH;
    else if ( length > HASHTABLE_MAX_LENGTH )
        m_tableLength = HASHTABLE_MAX_LENGTH;
    else
        m_tableLength = length;
    m_table = new _PEntry[m_tableLength];
    memset( m_table, 0, sizeof( _PEntry ) * m_tableLength );
    seqHead = NULL;
    seqTail = NULL;
    seqCur = NULL;

    if ( loadFactor < HASHTABLE_MIN_LOADFACTOR )
        m_loadFactor = HASHTABLE_MIN_LOADFACTOR;
    else if ( loadFactor > HASHTABLE_MAX_LOADFACTOR )
        m_loadFactor = HASHTABLE_MAX_LOADFACTOR;
    else
        m_loadFactor = loadFactor;

    m_threshold = ( UINT )( m_tableLength * m_loadFactor );

    if ( !pfCompareKey ) m_pfCompareKey = _CompareKey;
    else m_pfCompareKey = pfCompareKey;

    if ( !pfGetHash )
        m_pfGetHash = _GetHash;
    else
        m_pfGetHash = pfGetHash;

    m_entryCount = 0;
    m_modCount = 0;

    m_pcurEntry = NULL;
    m_uicurIndex = 0;
}

    template < class TKey, class TValue >
CHashTable< TKey, TValue >::~CHashTable()
{
    clear();			
    delete[] m_table;	
    m_table = NULL;
    m_tableLength = 0;
}
    template < class TKey, class TValue >
void CHashTable< TKey, TValue >::clear( void )
{
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
    m_entryCount = 0;	
    m_modCount ++;	

    m_pcurEntry = NULL;
    m_uicurIndex = 0;
}
    template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::containsKey( TKey key )
{
    HASH_CODE hash = ( *m_pfGetHash )( key );	
    UINT index = GET_HASH_INDEX( hash );				

    _PEntry p = m_table[index];
    while ( p )
    {
        if ( ( *m_pfCompareKey )( key, p->m_key ) )
        {
            return TRUE;	
        }
        p = p->m_next;
    }
    return FALSE;	
}
    template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::get_r( TKey& key, TValue& value )
{
    _PEntry p = m_table[GET_HASH_INDEX((*m_pfGetHash)(key))];
    while ( p )
    {
        if ( ( *m_pfCompareKey )( key, p->m_key ) )
        {
            value = p->m_value;
            return TRUE;
        }
        p = p->m_next;
    }
    return FALSE;	
}
    template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::get( TKey key, TValue& value )
{
    return get_r(key, value);
}
    template < class TKey, class TValue >
TValue CHashTable< TKey, TValue >::put_r( TKey& key, TValue& value )
{
    HASH_CODE hash = ( *m_pfGetHash )( key );	
    UINT index = GET_HASH_INDEX( hash );				
    _PEntry p = m_table[index];

    m_modCount ++;		

    while ( p )
    {
        if ( ( *m_pfCompareKey )( key, p->m_key ) )
        {
            TValue oldValue = p->m_value;
            p->m_value = value;				
            return oldValue;				
        }
        p = p->m_next;
    }

    
    m_entryCount ++;	

    
    m_pcurEntry = NULL;
    m_uicurIndex = 0;

    
    if ( m_entryCount >= m_threshold )
    {
        rehash();					
        index = GET_HASH_INDEX( hash );	
    }

    
    _PEntry newEntry = new _Entry( hash, key, value, m_table[index] );
    m_table[index] = newEntry;
    addSequence(newEntry);

    return value;	
}
    template < class TKey, class TValue >
TValue CHashTable< TKey, TValue >::put( TKey key, TValue value )
{
    return put_r(key, value);
}

    template < class TKey, class TValue >
void CHashTable< TKey, TValue >::put_v( TKey& key, TValue& value )
{
    HASH_CODE hash = ( *m_pfGetHash )( key );
    UINT index = GET_HASH_INDEX( hash );
    _PEntry p = m_table[index];

    m_modCount ++;	

    while ( p )
    {
        if ( ( *m_pfCompareKey )( key, p->m_key ) )
        {
            p->m_value = value;	
            return;
        }
        p = p->m_next;
    }

    m_entryCount ++;	

    m_pcurEntry = NULL;
    m_uicurIndex = 0;

    if ( m_entryCount >= m_threshold )
    {
        rehash();					
        index = GET_HASH_INDEX( hash );	
    }

    _PEntry newEntry = new _Entry( hash, key, value, m_table[index] );
    m_table[index] = newEntry;
    addSequence(newEntry);
}
    template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::remove( TKey key )
{
    HASH_CODE hash = ( *m_pfGetHash )( key );	
    UINT index = GET_HASH_INDEX( hash );	
    _PEntry p = m_table[index];

    if ( !p ) return FALSE;	

    if ( ( *m_pfCompareKey )( key, p->m_key ) )
    {
        if ( p == m_pcurEntry )	
            searchNextEntry();

        m_table[index] = p->m_next;
        delSequence(p);
        delete p;
        m_entryCount --;	
        m_modCount ++;		

        return TRUE;
    }

    _PEntry prev = p;
    p = p->m_next;
    while ( p )
    {
        if ( ( *m_pfCompareKey )( key, p->m_key ) )
        {

            if (  m_pcurEntry == p )		
                searchNextEntry();

            prev->m_next = p->m_next;
            delSequence(p);
            delete p;
            m_entryCount --;	
            m_modCount ++;		
            return TRUE;
        }
        prev = p;
        p = p->m_next;
    }

    return FALSE;	
}
    template < class TKey, class TValue >
inline UINT CHashTable< TKey, TValue >::size( void )
{
    return m_entryCount;
}
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
    m_pcurEntry = NULL; 
    m_uicurIndex =0;
    return FALSE;
}
    template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::getNext( TValue & value )
{
    if ( !m_pcurEntry )
        return FALSE;

    value = m_pcurEntry->m_value;

    searchNextEntry();

    return TRUE;
}

    template < class TKey, class TValue >
BOOL CHashTable< TKey, TValue >::getNext( TKey &key, TValue &value )
{
    if ( !m_pcurEntry )
        return FALSE;

    key = m_pcurEntry->m_key;
    value = m_pcurEntry->m_value;

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
    template < class TKey, class TValue >
void CHashTable< TKey, TValue >::rehash( void )
{
    if ( m_tableLength >= HASHTABLE_MAX_LENGTH ) return;

    UINT oldLength = m_tableLength;
    m_tableLength = m_tableLength * 2;	
    if ( m_tableLength >= HASHTABLE_MAX_LENGTH )
        m_tableLength = HASHTABLE_MAX_LENGTH;

    _PEntry* oldTable = m_table;
    m_table = new _PEntry[m_tableLength + 3];
    memset( m_table, 0, sizeof( _PEntry ) * m_tableLength );

    m_threshold = ( UINT )( m_tableLength * m_loadFactor );

    m_modCount ++;	

    _PEntry pentry;
    UINT index;
    for ( UINT i = 0; i < oldLength; i ++ )
    {
        while ( oldTable[i] )
        {

            pentry = oldTable[i];
            oldTable[i] = oldTable[i]->m_next;
            index = GET_HASH_INDEX( pentry->m_hash );
            pentry->m_next = m_table[index];
            m_table[index] = pentry;
        }
    }
    delete[] oldTable;	
}

    template < class TKey, class TValue >
void ( *CHashTable< TKey, TValue >::_Entry::m_spfSetKey )( TKey&, TKey )
    = _SetKey;

    template < class TKey, class TValue >
void ( *CHashTable< TKey, TValue >::_Entry::m_spfRemoveKey )( TKey& )
    = _RemoveKey;


#endif
