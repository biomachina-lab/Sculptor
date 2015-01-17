/********************************************************************
 *                                                                  *
 *  file: svt_string.cpp                                            *
 *                                                                  *
 *  Description: string utilities                                   *
 *                                                                  *
 *                                                                  *
 *  f.delonge and Sayan Ghosh                                                       *
 *                                                                  *
 ********************************************************************/

// system includes
#include <stdlib.h>
#include <ctype.h>
// svt includes
#include <svt_vector4.h>
#include <svt_string.h>


// duplicate string using new
// -> must be deleted with "delete []" 
// returns NULL if pszString is NULL
char* svt_strdup(const char* pszString)
{
    char* pszDupString = NULL;
    if (pszString != NULL) {
        pszDupString = new char [strlen(pszString)+1];
        strcpy (pszDupString, pszString);
    }
    return pszDupString;
}

// compare strings
// return false if the first unequal character is found
// return false if one string is NULL
// return true if both strings are NULL
bool svt_strequal(const char* pszString1,
                  const char* pszString2)
{
    if ((pszString1 == NULL) && (pszString2 == NULL)) 
        return true;

    if ((pszString1 == NULL) || (pszString2 == NULL)) 
        return false;

    int i = 0;
    while (pszString1[i] == pszString2[i]) 
    {
        if (pszString1[i] == '\0') 
            return true;
        i++;
    }
    return false;
}

// compare strings
// return strcmp(pszString1, pszString2)
// return 1 if first string is NULL <BR>
// return -1 if second string is NULL <BR>
// return 0 if both strings are NULL <BR>
int svt_strcmp(const char* pszString1, const char* pszString2)
{
    if ((pszString1 == NULL) && (pszString2 == NULL)) 
        return 0;

    if (pszString1 == NULL) 
        return 1;

    if (pszString2 == NULL) 
        return -1;

    return strcmp(pszString1, pszString2);
}

// remove leading whitespace characters of a string
// no allocation of new memory, changes are made within pszString itself
// returns pszString, which is compressed afterwards
// (therefore you don't actually need the returned string)
char* svt_strCompressL( char*pszString )
{
    if ( pszString == NULL )
        return( NULL );

    int i=0;
    while ( isspace(pszString[i]) )
        i++;

    memmove( pszString, pszString+i, strlen(pszString+i) );
    pszString[ strlen(pszString+i)] = '\0';
    return pszString;
}

// remove ending whitespace characters of a string
// no allocation of new memory, changes are made within pszString itself
// returns pszString, which is compressed afterwards
// (therefore you don't actually need the returned string)        
char* svt_strCompressR( char*pszString )
{
 
    if ( pszString == NULL )
        return( NULL );
 
    int i=strlen(pszString)-1;
    while ( isspace(pszString[i]) )
        i--;
    pszString[i+1] = '\0';
 
    return pszString;
}

// remove both leading and ending whitespace characters of a string no allocation of new memory,
// changes are made within pszString itself returns pszString, which is compressed afterwards
// (therefore you don't actually need the returned string)
char* svt_strCompress( char*pszString )
{
    return svt_strCompressL( svt_strCompressR( pszString ) );
}

// This function is a non-lexicographical sort which could be used with std::sorted a vector of
// strings containing numbers and chars could be sorted
bool svt_nonLexSort (const string& left, const string& right)
{
    if ( isdigit(left.c_str()[0]) && isdigit(right.c_str()[0]) )
    {
        if (atoi(left.c_str()) < atoi(right.c_str()))
            return true;
        else
            return false;
    }
    else if ( isalpha(left.c_str()[0]) && isalpha(right.c_str()[0]) )
    {
        if (left < right)
            return true;
        else
            return false;
    }
    else if (isdigit(left.c_str()[0]) && isalpha(right.c_str()[0]))
        return true;
    else if (isalpha(left.c_str()[0]) && isdigit(right.c_str()[0]))
        return false;
    else
        return false;
}
 
// Makes the occurences of sep unique, also removes sep from first and last positions of the string
// if present
string svt_removeRedundantSep (string sep, string str)
{
    int pos;
    string newstr;
    string piece;
       
    while ( (unsigned int) (pos = str.find(sep)) != (unsigned int) str.npos )
    {
        piece = str.substr(0,pos);
       
        if (newstr.length() == 0 && !piece.empty())
        {
            newstr = piece;
            newstr += sep;
            str = str.substr(pos+1);
        }
        else if (!piece.empty())
        {
            newstr += piece;
            newstr += sep;
            str = str.substr(pos+1);
        }
        else
        {
            str = str.substr(pos+1);
            continue;
        }
    }
    if (str.length())
    {
        newstr += str;
    }

    return newstr;   

}

// upon passing a string this functions fills in the range and flattens the input for example...if
// the user passes "A-D,1,2 ,3-5", the returned vector (of strings) would contain: 1,2,3,4,5,A,B,C,D
// duplicates are removed and a sorted list of numbers and (uppercase) characters are returned
vector<std::string> svt_expandRange(std::string str)
{
    std::string piece;
    std::stringstream ss;
    vector<std::string> nums;
    vector<std::string> dummy;
                
    unsigned int pos;
    unsigned int inpos = 0; 
    int lo = 0;
    int hi = 0;
    int ischar = 0;
        
    //Remove spaces from the string
    str.erase (std::remove (str.begin(), str.end(), ' '), str.end());
        
    //Remove consecutive commas
    str = svt_removeRedundantSep (",", str);
        
        
    //If only one number or character is input, only that should be returned
    if (str.length() == 1)
    {
        nums.push_back(str);
        return nums;
    }
        
    while ((pos = (unsigned int) str.find(",")) != (unsigned int) str.npos || str.length() > 0) 
    {
        if (pos > 0)
            piece = str.substr(0, pos);
        if (pos == (unsigned int) str.npos)
            piece = str;
        if (piece.empty())
        {
            //svt_exception::ui()->info("Incorrect input...please check again!"); 
            cout << "Incorrect input - empty string encountered" << endl;
            return dummy;
        }
                                        
        if ((inpos = (unsigned int) piece.find("-")) != (unsigned int) str.npos)
        {
            if (isalpha(piece.substr(0,inpos).c_str()[0]) && isalpha(piece.substr(inpos+1).c_str()[0]))
            {
                if (piece.substr(0,inpos).size() == 1 && piece.substr(inpos+1).size() == 1)
                {
                    lo = int(toupper(piece.substr(0,inpos).c_str()[0]));
                    hi = int(toupper(piece.substr(inpos+1).c_str()[0]));
                    ischar = 1;
                }
                else
                {
                    cout << "Incorrect input - range is expected to be like A-D and not A-BDG" << endl;
                    return dummy; 
                }
                                        
            }
                                
            else if (isdigit(piece.substr(0,inpos).c_str()[0]) && isdigit(piece.substr(inpos+1).c_str()[0]))
            {
                lo = atoi(piece.substr(0,inpos).c_str());
                hi = atoi(piece.substr(inpos+1).c_str());
                ischar = 0;
            }
            else if (piece == "-" )
            {
                nums.push_back("-");
            }
            else
            {
                cout << "Incorrect input - to evaluate a range, the start and end should be of the same type; range like 1-A is meaningless" << endl;
                return dummy; 
            }
                                
            while (lo <= hi)
            {
                if (ischar)     
                    //cout << char(lo) << endl;
                    nums.push_back(std::string(1, char(lo)));
                else
                {
                    //cout << lo << endl;
                    ss << lo;
                    nums.push_back(ss.str());
                    ss.str(std::string()); //flush the stream...let the next one in
                }
                                                
                lo++;
            }
        }
//                      else if (!isalnum(piece.c_str()[0])) //for numbers
//                              nums.push_back(piece);
        else if (int(piece.c_str()[0]))
            nums.push_back(piece);
        else if (isalpha(piece.c_str()[0])) //for alphabets //(isalpha(piece.c_str()[0]) && strlen(piece.c_str()) == 1) //for alphabets
            nums.push_back(std::string(1, toupper(piece.c_str()[0])));
        else if (piece.c_str()[0] == '-')
            nums.push_back(piece);
        else 
        {
            cout << "Incorrect input - the input is neither a number nor a character or -" << endl;
            return dummy; 
        }
                        
        //Counters
        if (pos > 0)    
            str = str.substr(pos+1);
        if (pos == (unsigned int) str.npos)
            str = "\0";
    }
                
    //erase duplicates 
    std::sort(nums.begin(), nums.end(), svt_nonLexSort); //call a non lexicographical sorting routine
    nums.erase(std::unique(nums.begin(), nums.end()), nums.end());

    return nums;
  
}

// This function does the opposite of the above, i,e it compacts a vector of number into a range,
// this works for consecutive characters as well as numerals as well
string svt_makeRange(vector<std::string> nums)
{
  
    string piece;
    string range;
    string max;
    string tok;
  
    unsigned int pos = 1;
    unsigned int hyf;
    
    if (!nums.size())
    {
        cout << "Incorrect input - empty vector<string> passed" << endl;
        return "";
    }
    
    //validate input
    for (unsigned int j = 0; j < nums.size(); j++)
    {      
        if (isalpha(nums[j].c_str()[0]) || isdigit(nums[j].c_str()[0]) || nums[j].c_str()[0] == '-')
            continue;
        else
        {
            cout << "Incorrect input - Input vector<string> contain tokens which are neither characters nor numbers or -" << endl;
            return "";
        }
    }    
    
    //capitalize characters in nums 
    for (unsigned int j = 0; j < nums.size(); j++)
    {      
        //capitalize
        if (isalpha(nums[j].c_str()[0]))
            nums[j] = string(1, toupper(nums[j].c_str()[0]));
    }
    
    //sort nums
    sort(nums.begin(),nums.end(), svt_nonLexSort); //call a non lexicographical sorting routine
    //only unique numbers / chars
    nums.erase(std::unique(nums.begin(), nums.end()), nums.end());
    
    piece = nums[0].c_str();
    piece.append(",");

    while (pos < nums.size())
    {
        tok = nums[pos - 1].c_str(); 
      
        //number
        if (isdigit(tok[0]) && atoi(nums[pos].c_str()) - atoi(piece.substr(piece.rfind(",") - tok.length(),piece.rfind(",")).c_str()) == 1) 
        {
            max = nums[pos];
      
            if ((hyf = piece.rfind("-")) != (unsigned int) piece.npos && hyf == (unsigned int) (piece.length() - tok.length() - 2) )
            {
                piece.replace(hyf + 1, (tok.length()), max);
            }
            else if ( piece.length() < 3 || (piece.substr(piece.rfind(",") - max.length() - 1)).rfind("-") == piece.npos )
            {    
            
                range = piece.substr(piece.rfind(",") - tok.length(), tok.length()) + "-" + (nums[pos].c_str());
                piece.replace((piece.rfind(",") - tok.length()), (range.length()), range); 
                piece.append(",");
            }
    
        }
        //character
        else if (isalpha(tok[0]) && int(nums[pos].c_str()[0]) - int(piece.substr(piece.rfind(",") - tok.length(),piece.rfind(",")).c_str()[0]) == 1) 
        {
            max = nums[pos];
      
            if ((hyf = piece.rfind("-")) != (unsigned int) piece.npos && hyf == (unsigned int) (piece.length() - tok.length() - 2) )
            {
                piece.replace(hyf + 1, (tok.length()), max);
            }
            else if ( piece.length() < 3 || (piece.substr(piece.rfind(",") - max.length() - 1)).rfind("-") == piece.npos )
            {    
            
                range = piece.substr(piece.rfind(",") - tok.length(), tok.length()) + "-" + nums[pos];
                piece.replace((piece.rfind(",") - tok.length()), (range.length()), range);   
                piece.append(",");
            }
    
        }        
        else
        {
            piece.append(nums[pos].c_str());
            piece.append(",");
        }
      
        pos++;

    }

    return piece.substr(0,piece.rfind(","));
}
