//
//  entry.cpp
//  diameter
//
//  Created by hidayat on 10/14/15.
//  Copyright © 2015 hidayat. All rights reserved.
//

#include <stdio.h>
#include "entry.h"
#include "avputil.h"
#include "logic.h"
#include <iostream>
entry::entry(){
}
void getUnable2Comply(diameter d,avp* &allavp,int &l,int &total){
    avputil util=avputil();
    
    //read avp
    avp ori_host=d.getAVP(264, 0);
    printf("ori len %i \n",ori_host.len);
    if(ori_host.len>0){
        std::cout<<util.decodeAsString(ori_host)<<std::endl;
    }
    
    char f=0x40;
    std::string ori ="vmclient.myrealm.example";
    //printf("size : %i\n",ori.size());
    avp o=util.encodeString(264,0,f,ori);
    //o.dump();
    //printf("\n");
    avp id_t=util.encodeInt32(450, 0, 0x40, 1);
    //id_t.dump();
    //printf("\n");
    avp id_d=util.encodeString(444, 0, 0x40, "628119105569");
    //id_d.dump();
    avp* listavp[2]={&id_t,&id_d};
    avp sid=util.encodeAVP(443, 0, 0x40, listavp, 2);
    
    avp id_t1=util.encodeInt32(450, 0, 0x40, 0);
    avp id_d1=util.encodeString(444, 0, 0x40, "51010628119105569");
    avp* listavp1[2]={&id_t1,&id_d1};
    avp sid1=util.encodeAVP(443, 0, 0x40, listavp1, 2);
    
    //sid.dump();
    //printf("\n");
    total=sid.len+o.len+sid1.len;
    l=3;
    allavp=new avp[l];
    allavp[0]=o;
    allavp[1]=sid;
    allavp[2]=sid1;
}

void getCEA(diameter d,avp* &allavp,int &l,int &total,std::string &host){
    avputil util=avputil();
    
    //read avp
    avp ori_host=d.getAVP(264, 0);
    printf("ori len %i \n",ori_host.len);
    if(ori_host.len>0){
        //std::cout<<util.decodeAsString(ori_host)<<std::endl;
        host=util.decodeAsString(ori_host);
    }
    
    char f=0x40;
    std::string ori ="vmclient.myrealm.example";
    //printf("size : %i\n",ori.size());
    avp o=util.encodeString(264,0,f,ori);
    
    //sid.dump();
    //printf("\n");
    total=o.len;
    l=1;
    allavp=new avp[l];
    allavp[0]=o;
}

diameter entry::process(diameter d){
    d.populateHeader();
    int reqbit=(0x80&d.cflags);
    if(reqbit==0){
        return diameter(0, 0, 0);
    }
    printf("reqbit : %i\n",reqbit);
    int ccode=((*(d.ccode) & 0xff) << 16)| ((*(d.ccode+1) & 0xff) << 8) | ((*(d.ccode+2)& 0xff));
   int i=0;
    logic lojik=logic();
    lojik.db=db;
    avp* allavp=new avp[1];
    int l;
    int total;
    std::string host="";
    //CALL LOGIC HERE
    //lojik.getResult(d, allavp, l,total);
    if (ccode==257) {
        getCEA(d, allavp, l, total,host);
        //if cea success, add sock peer to list
        test(host);
    }else if(ccode==272){//ccr
        lojik.getCCA(d, allavp, l,total);
    }else if(ccode==280){//watchdog
        
    }else{
        getUnable2Comply(d, allavp, l, total);
    }

    char* h=new char[4];
    *h=d.version;
    
    char cflags=d.cflags^0x80;
    //printf(" avp len %i",o.len);
    //int l_resp=o.len+20+sid.len;
    int l_resp=20+total;

    char *ptr1 = (char*)&l_resp;
    char l_byte[3];
    char* lp=l_byte;
    ptr1=ptr1+2;
    i=0;
    while(i<3){
         *lp=*ptr1;
         lp++;
         ptr1--;
         i++;
    }
    //printf(" lbyte %02X %02X %02X ",l_byte[0],l_byte[1],l_byte[2]);
    *(h+1)=l_byte[0];
    *(h+2)=l_byte[1];
    *(h+3)=l_byte[2];
    //char* h=head;
    //printf(" msg len %i ",l_resp);
    char *b=new char[l_resp-4];
    
    *b=cflags;
    //printf(" ccode %02X %02X %02X ",*d.ccode,*(d.ccode+1),*(d.ccode+2));
    *(b+1)=*d.ccode;
    *(b+2)=*(d.ccode+1);
    *(b+3)=*(d.ccode+2);
    //printf(" copy ccode %02X %02X %02X \n",body[1],body[2],body[3]);
    //copy appid hbh e2e to body
    i=0;
    while (i<12) {
        *(b+i+4)=*d.appId;
        d.appId++;
        i++;
    }
    b=b+16;
    for (i=0; i<l; i++) {
        //copy avp
        char *temp=allavp[i].val;
        //allavp[i].dump();
        //printf("\n");
        for (int j=0; j<allavp[i].len; j++) {
            *b=*temp;
            b++;
            temp++;
        }
    }
    b=b-l_resp+4;

    diameter answer=diameter(h, b, l_resp-4);

    
    return answer;
}

void entry::connectCallback(CallbackInterface *cb)
{
    entry::m_cb = cb;
}

// Test the callback to make sure it works.
void entry::test(std::string host)
{
    printf("Caller::test() calling callback...\n");
    entry::m_cb -> cbiCallbackFunction(host);
}
