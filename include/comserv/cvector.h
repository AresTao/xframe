#ifndef _VECTOR_H
#define _VECTOR_H

#include "comtypedef.h"
#include "unihashtable.h"

template <class TValue>
class CVector	
{
    private:

        class _Entry
        {
            public:
                TValue*		m_value;		
                _Entry*		m_prev;			
                _Entry*		m_next;			

                _Entry( TValue* pvalue, _Entry* prev, _Entry* next )
                {
                    m_value=pvalue;
                    m_prev=prev;
                    m_next=next;
                }
                ~_Entry( void )
                {

                }
        };

        CHashTable<UINT, _Entry*> m_ht;

        _Entry* m_pHead;
        _Entry* m_pTail;
        _Entry* m_pEnd;
        _Entry* m_pCurrent;
        UINT m_count;
        UINT m_size;
        TValue* m_pValue;

    public:
        CVector();	
        CVector(UINT initsize);		
        CVector(UINT initsize, TValue initvalue);	
        ~CVector();

        void resize(UINT newSize);		
        void resize(UINT newSize, TValue value);	

        TValue& operator[](const UINT i);

        UINT size(){return m_size;}	

        UINT count(){return m_count;}	

        UINT push_back (const TValue& value);	

        UINT pop_back();	

        UINT pop_back(TValue& value);

        void remove(); 

        BOOL empty()	
        {
            if(m_count) return FALSE;
            else return TRUE;
        }

};


template <class TValue>
CVector<TValue>::CVector()
{
    m_count=0;
    m_size=0;
    m_pValue=new TValue;
}


template <class TValue>
CVector<TValue>::CVector(UINT initsize)
{
    m_count=0;
    m_size=0;
    m_pValue=new TValue;
    resize(initsize); 
}


template <class TValue>
CVector<TValue>::CVector(UINT initsize, TValue initvalue)
{
    m_count=0;
    m_size=0;
    m_pValue=new TValue;
    *m_pValue=initvalue; 
    resize(initsize, initvalue); 
}


template <class TValue>
CVector<TValue>::~CVector()
{
    if(m_pValue != NULL)
    {
        delete m_pValue;
        m_pValue = NULL;
    }
    remove(); 
}


template <class TValue>
inline void CVector<TValue>::resize(UINT newSize)
{
    _Entry* pEntry;
    TValue* pValue;
    pEntry=NULL;
    pValue=NULL;

    if(newSize>m_size)
    {
        for(int i=m_size; i<newSize; i++)
        {
            pValue=new TValue;
            *pValue=*m_pValue;
            pEntry=new _Entry(pValue,NULL,NULL);
            m_ht.put( i, pEntry );
        }
        m_size=newSize;
    }

    if(newSize<m_count)	
    {
        m_count=newSize;
    }
}


template <class TValue>
inline void CVector<TValue>::resize(UINT newSize, TValue value)
{
    _Entry* pEntry;
    TValue* pValue;
    pEntry=NULL;
    pValue=NULL;
    if(newSize>m_size)
    {
        for(int i=m_size; i<newSize; i++)
        {
            pValue=new TValue;
            *pValue=value;
            pEntry=new _Entry(pValue,NULL,NULL);
            m_ht.put( i, pEntry );
        }
        m_size=newSize;
        m_count=newSize;	
    }

    if(newSize<m_count)	
    {
        m_count=newSize;
    }
}



template <class TValue>
TValue& CVector<TValue>::operator[](const UINT i)
{
    _Entry* pEntry;
    TValue* pValue;
    pEntry=NULL;
    pValue=NULL;

    if(i<m_size)
    {
        if(m_ht.get(i, pEntry))
        {
            if(pEntry!=NULL)
            {
                if(pEntry->m_value!=NULL)
                {
                    pValue=pEntry->m_value;
                    return *pValue;
                }
            }
        }
    }
    return *m_pValue;
}


    template <class TValue>
UINT CVector<TValue>::push_back(const TValue& value)
{
    _Entry* pEntry;
    TValue* pValue;
    pEntry=NULL;
    pValue=NULL;

    if(m_count<m_size)	
    {
        if(!m_ht.get( m_count, pEntry )) return 0;
        if(pEntry==NULL) return 0;
        if(pEntry->m_value==NULL) return 0;

        *(pEntry->m_value)=value;
        m_count++;
    }
    else
    {
        pValue=new TValue;
        if(pValue==NULL) return 0;

        *pValue=value;
        pEntry=new _Entry(pValue,NULL,NULL);
        if(pEntry==NULL)
        {
            delete pValue;
            return 0;
        }

        m_ht.put( m_count, pEntry );
        m_size++;
        m_count++;
    }
    return m_count;	
}


    template <class TValue>
UINT CVector<TValue>::pop_back()
{
    _Entry* pEntry;
    pEntry=NULL;

    if(m_count>0 && m_ht.get( m_count-1, pEntry ))
    {
        m_count-=1;
        return m_count;
    }

    return 0;
}


    template <class TValue>
UINT CVector<TValue>::pop_back(TValue& value)
{
    _Entry* pEntry;
    pEntry=NULL;

    if(m_count>0 && m_ht.get( m_count-1, pEntry ))
    {
        if(pEntry==NULL) return 0;
        if(pEntry->m_value==NULL) return 0;

        value=*(pEntry->m_value);
        m_count-=1;
        return m_count;
    }

    return 0;
}


    template <class TValue>
inline void CVector<TValue>::remove()
{
    _Entry* pEntry;
    pEntry=NULL;

    for(int i=0;i<m_size;i++)
    {
        if(m_ht.get( i , pEntry))
        {
            if(pEntry!=NULL)
            {
                if(pEntry->m_value!=NULL) delete pEntry->m_value;
                delete pEntry;
                pEntry=NULL; 
            }
            m_ht.remove(i);
        }
    }
    m_size=0;
    m_count=0;
}


#endif
