#ifndef _VECTOR_H
#define _VECTOR_H

#include "comtypedef.h"
#include "unihashtable.h"
//#include "clist.h"

///////////////////////////////////////////////////////////////////////
// 定义 CVector< TValue > 类
///////////////////////////////////////////////////////////////////////
template <class TValue>
class CVector	// TValue为数组要存储的值
{
private:

	class _Entry	// 定义链表中的节点类型
	{
	public:
		TValue*		m_value;		// 此节点的内容
		_Entry*		m_prev;			// 指向前一个节点
		_Entry*		m_next;			// 指向下一个节点
    
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
	CVector();	//建立空数组，以后可以使用resize改变
	CVector(UINT initsize);		//建立一定长度的数组
	CVector(UINT initsize, TValue initvalue);	//使用初值初始化数组
	~CVector();

	void resize(UINT newSize);		//重置数组长度，需要注意的是，如果 newSize < initSize，则数组长度不会改变
	void resize(UINT newSize, TValue value);	//重置数组长度且赋初值
	
	TValue& operator[](const UINT i);
	
	//整个数组长度
	//	整个数组长度是初始化或resize之后的数值
	UINT size(){return m_size;}	
	
	//数组有效值长度. 
	//	当前数组实际填写的项目数，超过这一项目数的数组内容实际上是无意义的
	UINT count(){return m_count;}	

    //添加一项
	//	在数组最后的有效值后面添加一项
	//	输入：添加的内容
	//	返回：数组有效项目数
	UINT push_back (const TValue& value);	

    //删除一项
	//	删除数组最后一个有效值
	//	返回：数组有效项目数
	UINT pop_back();	

	//删除一项
	//	删除数组最后一个有效值
	//	返回：数组有效项目数，数组最后一个有效值
	UINT pop_back(TValue& value);

	void remove();	//删除所有节点 

	//判断数组有效值是否为空
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
//del by LJL 2005-12-29 将resize修改为内联函数，构造函数的内容和resize是一致的 
/*	if(initsize>0) m_size=initsize;
	m_pValue=new TValue;
	_Entry* pEntry;
	TValue* pValue;
	pEntry=NULL;
	pValue=NULL;
	for(int i=0; i<m_size; i++)
    {
        pValue=new TValue;
        pEntry=new _Entry(pValue,NULL,NULL);
        m_ht.put( i, pEntry );
    }
    */
    resize(initsize); 
}


template <class TValue>
CVector<TValue>::CVector(UINT initsize, TValue initvalue)
{
	m_count=0;
	m_size=0;
	m_pValue=new TValue;
	*m_pValue=initvalue; 
//del by LJL 2005-12-29 将resize修改为内联函数，构造函数的内容和resize是一致的 
/*	if(initsize>0)
    {
       m_count=initsize;
       m_size=initsize;
    }
	m_pValue=new TValue;
	_Entry* pEntry;
	TValue* pValue;
	pEntry=NULL;
	pValue=NULL;
	for(int i=0; i<m_size; i++)
    {
        pValue=new TValue;
        *pValue=initvalue;
        pEntry=new _Entry(pValue,NULL,NULL);
        m_ht.put( i, pEntry );
    }
    */
    resize(initsize, initvalue); 
}


template <class TValue>
CVector<TValue>::~CVector()
{
//del by LJL 2005-12-29 将remove修改为内联函数，析构函数的内容和remove是一致的 
/*	_Entry* pEntry;
	pEntry=NULL;

	if(m_pValue!=NULL) delete m_pValue;
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
*/
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

	if(newSize<m_count)	//如果新的表格大小小于原始有效数量，则有效值减小
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

	if(newSize<m_count)	//如果新的表格大小小于原始有效数量，则有效值减小
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
	
	if(m_count<m_size)	//如果有效数量小于数组大小，则意味着有实际数组空间可用
	{
		//if(!m_ht.get( m_count-1, pEntry )) return 0; Delete By LJL 2006-2-10
		if(!m_ht.get( m_count, pEntry )) return 0;	//应该是写到有效数量之后的那个空间位置中
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
