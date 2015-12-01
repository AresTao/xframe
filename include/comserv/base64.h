/*
 *@author     AresTao
 *@function   base64 encoding and decoding
 * */ 

#ifndef ___BASE64_H___  
#define ___BASE64_H___  

#include <string>  

using namespace std;  

class CBase64  
{  
    public:  
        CBase64();  
        ~CBase64();  
        /*
         *function : base64 encode 
         *parameter: [in]pIn     data need to be encoded
         *           [in]uInlen  data length
         *           [out]strOut string after base64 encode
         *result   : true (encode success) false (encode failed)
         * */
        bool static Encode(const unsigned char *pIn, unsigned long uInLen, string& strOut);  
        /*
         *function : base64 encode 
         *parameter: [in]pIn      data need to be encoded
         *           [in]uInlen   data length
         *           [out]pOut    string after base64 encode
         *           [out]uOutLen string length
         *result   : true (encode success) false (encode failed)
         * */

        bool static Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);  

        /*
         *function : base64 decode
         *parameter: [in]strIn    data need to be decoded
         *           [out]pOut    data after decode
         *           [out]uOutLen data length
         *result   : true (decode success) false (decode failed)
         * */
        bool static Decode(const string& strIn, unsigned char *pOut, unsigned long *uOutLen) ;  

        /*
         *function : base64 decode
         *parameter: [in]pIn      data need to be decoded
         *           [in]uInLen   pIn data length
         *           [out]pOut    data after decode
         *           [out]uOutLen data length
         *result   : true (decode success) false (decode failed)
         * */
        bool static Decode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen) ;  
};  

#endif // ___BASE64_H___  

