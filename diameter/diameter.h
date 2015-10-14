//
//  diameter.h
//  diameter
//
//  Created by hidayat on 10/14/15.
//  Copyright © 2015 hidayat. All rights reserved.
//

#ifndef diameter_h
#define diameter_h

#include "avp.h"

class diameter{
public:
    char* h;
    char* b;
    int len;
    int curr;
    
    char* ccode;
    char* appId;
    char* hbh;
    char* e2e;
    
    diameter(char* h,char* b,int l);
    void compose(char* res);
    void dump();
    void populateHeader();
    avp getAVP(int acode,int vcode);
    avp getAVP(int* code,int l);
};

#endif /* diameter_h */
