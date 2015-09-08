#ifndef _UNILIST_H_
#define _UNILIST_H_

#include "comtypedef.h"

typedef void* CListNodeHandle;

///////////////////////////////////////////////////////////////////////
// ���� CList< TValue > ��
///////////////////////////////////////////////////////////////////////
template <class TValue>
class CList   // TValueΪ����Ҫ�洢��ֵ
{
private:
   
   class _Entry   // ���������еĽڵ�����
   {
   public:
      TValue      m_value;      // �˽ڵ������
      _Entry*      m_prev;         // ָ��ǰһ���ڵ�
      _Entry*      m_next;         // ָ����һ���ڵ�
      // �ڵ�Ĺ��캯�������л���ü�ֵ���ú�������������ڵ�ļ�
      _Entry(TValue& value, _Entry* prev, _Entry* next )
      {
         m_value=value;
         m_prev=prev;
         m_next=next;
      }
      
      _Entry(_Entry* prev, _Entry* next )
      {
         m_prev=prev;
         m_next=next;
      }
      // �ڵ���������������е��ü�ֵ�������������˽ڵ�ļ�
      ~_Entry( void )
      {

      }
   };
   
   _Entry* m_pHead;
   _Entry* m_pTail;
   _Entry* m_pCurrent;
   
   _Entry* m_pTag;
   
   INT m_count;
   BOOL _addr(int pos);

public:
   CList();
   ~CList();

   class iterator
   {
   protected:
      _Entry* _Ptr;
   public:
         
      iterator() {}
      iterator(_Entry* _P){ _Ptr=_P;}   //�г�ʼ���Ĺ��죬��ʼ��_Entry

      TValue& operator*() const
      {
         return (_Ptr->m_value); 
      }
      iterator& operator++()
      {
         if(_Ptr!=NULL) _Ptr = _Ptr->m_next;   //ָ����һ���ڵ��ַ
         return (*this);                //���ر�iterator
      }
      
      iterator operator++(int)
      {
         iterator _Tmp = *this;
         ++*this;
         return (_Tmp); 
      }
      
      iterator& operator--()
      {
         if(_Ptr!=NULL) _Ptr = _Ptr->m_prev;   //ָ��ǰһ���ڵ��ַ
         return (*this);                //���ر�iterator
      }

      iterator operator--(int)
      {
         iterator _Tmp = *this;
         --*this;
         return (_Tmp); 
      }
      
      BOOL operator==(const iterator& _X) const
      {
         return (_Ptr == _X._Ptr); 
      }
      
      BOOL operator!=(const iterator& _X) const
      {
         return (!(*this == _X)); 
      }
      int pointTo(_Entry* p)
      {
         return _Ptr == p;
      }
   };
         

   iterator begin()                     //����һ��ָ��ͷ�ڵ��iterator
      {return (iterator(m_pHead)); }
   iterator end()                        //����һ��ָ��β�ڵ��iterator
      {return (iterator(m_pTail->m_next)); }
   iterator pos(int index)
   {
      _Entry* p = m_pHead;
      while(index > 0 && p!= m_pTail->m_next)
      {
         p = p->m_next;
         index --;
      }
      return (iterator(p)); 
   }
   BOOL front(TValue& value);   //���ͷ�ڵ㣬��ú�ǰ�ڵ�Ϊͷ�ڵ� FALSE��û��ͷ�ڵ㣬��
   TValue& front_r();   //���ͷ�ڵ������
   BOOL back(TValue& value);      //���β�ڵ㣬��ú�ǰ�ڵ�Ϊβ�ڵ�FALSE��û��β�ڵ㣬��
   TValue& back_r();      //���β�ڵ������
   BOOL next(TValue& value);   //�����һ���ڵ㣬FALSEû����һ���ڵ���. 
   BOOL prev(TValue& value);   //�����һ���ڵ㣬FALSEû����һ���ڵ���.
   BOOL current(TValue& value);   //��õ�ǰ�ڵ㣬FALSE��ǰ�ڵ�û��ֵ.
   BOOL get(int pos, TValue& value);
   BOOL set(int pos, TValue& value);
    
   void reset();      //��ǰָ��ָ������ĵ�һ���ڵ㣬һ���ڵ���contain֮ǰʹ�á�
   BOOL front();      //��ǰָ��ָ��ͷ�ڵ㣬FALSE��û��ͷ�ڵ㣬��
   BOOL back();      //��ǰָ��ָ��β�ڵ㣬FALSE��û��β�ڵ㣬��
   BOOL next();      //��ǰָ��ָ����һ���ڵ㣬FALSEû����һ���ڵ���. 
   BOOL prev();      //��ǰָ��ָ����һ���ڵ㣬FALSEû����һ���ڵ���. 

   UINT push_front(TValue value);   //���ͷ�ڵ㣬���ر��ȣ�0�����ʧ�ܣ���Ӻ�ǰ�ڵ�Ϊͷ�ڵ� 
   UINT push_front_r(TValue& value);
   UINT push_back(TValue value); //���β�ڵ㣬���ر��ȣ�0�����ʧ�ܣ���Ӻ�ǰ�ڵ�Ϊβ�ڵ� 
   UINT push_back_r(TValue& value);
   UINT insert(TValue value);      //�ڵ�ǰλ��֮�����ڵ㣬���ر��ȣ�0ʧ�ܣ���ǰ�ڵ�Ϊ�����Ľڵ�
   UINT insert_r(TValue& value);

   // insert sortly
   // mode = 0, from min to max
   // mode = 1, from max to min
   UINT insertSort(TValue& value, int mode=0);

   void print();

   BOOL contain(TValue& value);    //�ӵ�ǰ�ڵ㿪ʼ������ͬ�ڵ㣬TRUE���ҵ���ͬ�Ľڵ㣻FALSE��û�ҵ���ͬ�Ľڵ㣬��ǰ�ڵ�Ϊƥ��Ľڵ�
   int index(TValue& value); // find a node, return the index, -1 for not found
   int index(iterator&);

   UINT pop_front();   //ɾ��ͷ�ڵ㣬���ر��ȣ�0��ɾ��ʧ�ܣ�ɾ����ǰ�ڵ�Ϊͷ�ڵ�
   UINT pop_back();   //ɾ��β�ڵ㣬���ر��ȣ�0��ɾ��ʧ�ܣ�ɾ����ǰ�ڵ�Ϊβ�ڵ�
   UINT pop_current();   //ɾ����ǰ�ڵ㣬���ر��ȣ�0��ɾ��ʧ�ܣ�ɾ��֮��ǰ�ڵ�ת�Ƶ���һ����ȱʡ�Ǵ�ͷ��β����һ���ڵ㣩
   UINT pop_front(TValue& value);   //ɾ��ͷ�ڵ㣬����ɾ�������ݼ�����,0 is empty). 
   UINT pop_back(TValue& value);   //ɾ��β�ڵ㣬����ɾ�������ݼ�����,0 is empty).  
   UINT pop_current(TValue& value);   //ɾ����ǰ�ڵ㣬���ر�ɾ���Ľڵ㣬��ǰ�ڵ�ת�Ƶ���һ����ȱʡ�Ǵ�ͷ��β����һ���ڵ㣩 add by ��� 2005-11-18
   UINT remove(TValue& value); //ɾ������ƥ��Ľڵ㣬����ɾ����������0��ɾ��ʧ�ܣ�û��ƥ�����

   CListNodeHandle getCurrentHandle();     
   void removeByHandle(CListNodeHandle handle);

   void remove();   //ɾ�����нڵ� 
   
   BOOL empty();   //�����Ƿ�Ϊ�� 
   BOOL ishead();   //��ǰλ���������ͷ
   BOOL istail();   //��ǰλ���������β
   INT count();   //������. 

   void clear();
   INT  size();
   UINT put(TValue& value);
   void operator=(CList<TValue>& v);
 
};

template <class TValue>
inline CList<TValue>::CList()
{
   m_count=0;
   m_pTag=new _Entry( NULL, NULL );
   m_pHead=m_pTag;
   m_pTail=m_pTag;
   m_pCurrent=m_pTag;
   m_pTag->m_next=m_pHead;
   m_pTag->m_prev=m_pTail;
}


template <class TValue>
inline CList<TValue>::~CList()
{
   while(m_pHead!=m_pTag)
   {
      m_pCurrent=m_pHead->m_next;
      delete m_pHead;
      m_pHead=m_pCurrent;
   }
   if(m_pTag!=NULL) delete m_pTag;
}


template <class TValue >
inline BOOL CList<TValue>::front()
{
   if ( m_count>0 && m_pHead!=m_pTag )
   {
      m_pCurrent=m_pHead;
      return TRUE;
   }
   return FALSE;
}


template <class TValue >
inline BOOL CList<TValue>::front(TValue& value)
{
   if ( front() )
   {
      value=m_pHead->m_value;
      return TRUE;
   }
   return FALSE;
}

template <class TValue >
inline TValue& CList<TValue>::front_r()
{
   return m_pHead->m_value;
}

template <class TValue >
inline BOOL CList<TValue>::back()
{
   if( m_count>0 && m_pTail!=m_pTag )
   {
      m_pCurrent=m_pTail;
      return TRUE;
   }
   return FALSE;
}

template <class TValue >
inline TValue& CList<TValue>::back_r()
{
   return m_pTail->m_value;
}

template <class TValue >
inline BOOL CList<TValue>::back(TValue& value)
{
   if( back() )
   {
      value=m_pTail->m_value;
      return TRUE;
   }
   return FALSE;
}

template <class TValue >
UINT CList<TValue>::pop_front()
{
   if (m_count>0)
   {
      m_pCurrent=m_pHead->m_next;   
      m_pCurrent->m_prev=m_pTag;
      m_pTag->m_next=m_pCurrent;
      delete m_pHead;
      m_pHead=m_pCurrent;
      m_count-=1;
      
      if(m_count==0) m_pTail=m_pTag;
   }
   return m_count;
}


template <class TValue >
UINT CList<TValue>::pop_back()
{
   if (m_count>0)
   {
      m_pCurrent=m_pTail->m_prev;
      m_pCurrent->m_next=m_pTag;
      m_pTag->m_prev=m_pCurrent;
      delete m_pTail;
      m_pTail=m_pCurrent;
      m_count-=1;
      
      if(m_count==0) m_pHead=m_pTag;
   }
   return m_count;
}


template <class TValue >
UINT CList<TValue>::pop_current()
{
   _Entry* pPrev;
   _Entry* pNext;
   
   if (m_count==0 || m_pCurrent==m_pTag)
   {
      return 0;
   }

   pPrev=m_pCurrent->m_prev;
   pNext=m_pCurrent->m_next;

   if(pNext==m_pTag)   //��ǰ�ڵ���β�ڵ�
   {
      return pop_back();
   }

   if (pPrev==m_pTag)   //��ǰ�ڵ���ͷ�ڵ�
   {
      return pop_front();
   }

   //���м�Ľڵ�
   delete m_pCurrent;
   m_pCurrent=pNext;
   m_pCurrent->m_prev=pPrev;
   pPrev->m_next=m_pCurrent;
   m_count-=1;
   
   return m_count;
}


template <class TValue >
UINT CList<TValue>::pop_front(TValue& value)
{
   if (m_count>0)
   {
      value=m_pHead->m_value;
   }
   return pop_front();
}
   
template <class TValue >
UINT CList<TValue>::pop_back(TValue& value)
{
   if (m_count>0)
   {
      value=m_pTail->m_value;
   }
   return pop_back();
}

template <class TValue >
UINT CList<TValue>::pop_current(TValue& value)
{
   if (m_count>0 && m_pCurrent!=m_pTag)
   {
      value=m_pCurrent->m_value;
   }
   return pop_current();
}

template <class TValue >
UINT CList<TValue>::push_front_r(TValue& value)
{
   m_pCurrent=new _Entry(value, m_pTag, m_pHead);
   if (0==m_count)
   {
      m_pHead=m_pCurrent;
      m_pTail=m_pCurrent;
      m_pTag->m_prev=m_pTail;
   }
   else
   {
      m_pHead->m_prev=m_pCurrent;
      m_pHead=m_pCurrent;
   }
   m_pTag->m_next=m_pHead;
   m_count+=1;
   return m_count;
}

template <class TValue >
UINT CList<TValue>::push_front(TValue value)
{
   return push_front_r(value);
}

template <class TValue >
UINT CList<TValue>::push_back_r(TValue& value)
{
   m_pCurrent=new _Entry(value, m_pTail, m_pTag);
   if (0==m_count)
   {
      m_pHead=m_pCurrent;
      m_pTail=m_pCurrent;
      m_pTag->m_next=m_pHead;
   }
   else
   {
      m_pTail->m_next=m_pCurrent;
      m_pTail=m_pCurrent;
   }
   m_pTag->m_prev=m_pTail;
   m_count+=1;
   return m_count;
}

template <class TValue >
UINT CList<TValue>::push_back(TValue value)
{
   return push_back_r(value);
}

template <class TValue >
void CList<TValue>::remove()
{
   do 
   {
   } while(pop_front());    
}

template <class TValue >
inline BOOL CList<TValue>::next()
{
   if(m_pCurrent==m_pTag || istail()) return FALSE;
   
   m_pCurrent=m_pCurrent->m_next;
   return TRUE;
}

template <class TValue >
BOOL CList<TValue>::next(TValue& value)
{
   if(next())
   {
      value=m_pCurrent->m_value;
      return TRUE;
   }
   return FALSE;
}

template <class TValue >
inline BOOL CList<TValue>::prev()
{
   if(m_pCurrent==m_pTag || ishead()) return FALSE;
   
   m_pCurrent=m_pCurrent->m_prev;
   return TRUE;
}
   
   
template <class TValue >
BOOL CList<TValue>::prev(TValue& value)
{
   if(prev())
   {
      value=m_pCurrent->m_value;
      return TRUE;
   }
  return FALSE;
}


template <class TValue >
inline BOOL CList<TValue>::current(TValue& value)
{
   if(m_pCurrent==m_pTag || m_pCurrent==NULL) return FALSE;
   value=m_pCurrent->m_value;
   return TRUE;
}

template <class TValue >
inline BOOL CList<TValue>::_addr(int pos)
{
   if(m_count <= 0)
      return FALSE;
   if(pos < 0 || pos >= m_count)
      return FALSE;

   _Entry* p = m_pHead;
   while(pos>0 && p!=m_pTag && p!=NULL)
   {
      p = p->m_next;
      pos--;
   }
   m_pCurrent = p;
   return TRUE;
}
   
template <class TValue >
inline BOOL CList<TValue>::get(int pos, TValue& value)
{
   if(!_addr(pos))
      return FALSE;
   value = m_pCurrent->m_value;
   return TRUE;
}

template <class TValue >
inline BOOL CList<TValue>::set(int pos, TValue& value)
{
   if(!_addr(pos))
      return FALSE;
   m_pCurrent->m_value = value;
   return TRUE;
}

template <class TValue >
BOOL CList<TValue>::contain(TValue& value)
{
   while(m_pCurrent!=m_pTag && m_pCurrent!=NULL)
   {
      if(m_pCurrent->m_value == value) return TRUE;
      m_pCurrent=m_pCurrent->m_next;
   }
   return FALSE;
}

template <class TValue >
int CList<TValue>::index(TValue& value)
{
   int i=0;
   _Entry* p = m_pHead;
   while(p!=m_pTag && p!=NULL)
   {
      if(p->m_value == value)
         return i;
      p=p->m_next;
      i++;
   }
   return -1;
}

template <class TValue >
int CList<TValue>::index(iterator& j)
{
   int i=0;
   _Entry* p = m_pHead;
   while(p!=m_pTag && p!=NULL)
   {
      if(j.pointTo(p))
         return i;
      p=p->m_next;
      i++;
   }
   return -1;
}


template <class TValue >
inline void CList<TValue>::reset()
{
   m_pCurrent=m_pHead;
}

template <class TValue >
UINT CList<TValue>::remove(TValue& value)
{
   int i=0;
   reset();
   while(contain(value))
   {
      pop_current();
      reset();
      i++;
   }
   return i;
}

template <class TValue >
CListNodeHandle CList<TValue>::getCurrentHandle()     
{
   return m_pCurrent;
}

template <class TValue >
void CList<TValue>::removeByHandle(CListNodeHandle handle)
{
   if(handle == NULL)
      return;
   m_pCurrent = (_Entry*)handle;
   pop_current();
   reset();
}

template <class TValue >
UINT CList<TValue>::insert_r(TValue& value)
{
   _Entry* pPrev;
   _Entry* pNext;
   
   if(0==m_count || ishead())
   {
      return push_front_r(value);
   }
   
   if(m_pCurrent==m_pTag) return 0;

   if(istail())
   {
      return push_back_r(value);
   }
   
   pPrev=m_pCurrent;
   pNext=m_pCurrent->m_next;
   m_pCurrent=new _Entry(value,pPrev,pNext);
   pPrev->m_next=m_pCurrent;
   pNext->m_prev=m_pCurrent;
   m_count+=1;
   return m_count;      
}
template <class TValue >
UINT CList<TValue>::insert(TValue value)
{
   return insert_r(value);
}

template <class TValue >
void CList<TValue>::print()
{
   _Entry* p = m_pHead;
   while(p != m_pTag)
   {
      printf("%p <- %p -> %p\n"
         , p->m_prev
         , p
         , p->m_next
      );
      p = p->m_next;
   }
}

template <class TValue >
UINT CList<TValue>::insertSort(TValue& value, int mode)
{
   reset();
   while(m_pCurrent != m_pTag)
   {
      if(mode == 0 ? (value >= m_pCurrent->m_value) : (value <= m_pCurrent->m_value))
         m_pCurrent = m_pCurrent->m_next;
      else
         break;
   }
   if(m_pCurrent == m_pHead)
      return push_front_r(value);
   if(m_pCurrent == m_pTag)
      return push_back_r(value);

   _Entry* pPrev=m_pCurrent->m_prev;
   _Entry* pNext=m_pCurrent;
   m_pCurrent=new _Entry(value,pPrev,pNext);
   pPrev->m_next=m_pCurrent;
   pNext->m_prev=m_pCurrent;
   m_count+=1;
   return m_count;      
}

template <class TValue >
inline BOOL CList<TValue>::empty()
{
   if (m_count>0) return FALSE;
   return TRUE;
}

template <class TValue >
inline INT CList<TValue>::count()
{
   return m_count;
}


template <class TValue >
inline BOOL CList<TValue>::ishead()
{
   if(m_pCurrent==m_pTag) return FALSE;
   if(m_pCurrent->m_prev == m_pTag) return TRUE;
   return FALSE;
}


template <class TValue >
inline BOOL CList<TValue>::istail()
{
   if(m_pCurrent==m_pTag) return FALSE;
   if(m_pCurrent->m_next == m_pTag) return TRUE;
   return FALSE;
}

template < class TValue >
inline void CList<TValue>::operator=(CList<TValue>& v)
{
        if(&v != this )
        {
                remove();
		_Entry* p = v.m_pHead;
		while(p != v.m_pTag)
		{
			push_back_r(p->m_value);
			p = p->m_next;
		}
	}
}

template < class TValue >
inline void  CList<TValue>::clear() 
{ 
	remove();
}

template < class TValue >
inline INT  CList<TValue>::size()  
{ 
	return count();
};

template < class TValue >
inline UINT CList<TValue>::put(TValue& value) 
{ 
	return push_back_r(value); 
};

#endif



