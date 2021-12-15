// c++ rgen.cpp -std=c++11 -o op
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include <unistd.h>

using namespace std;
struct ip_args{
    int s,n,l,c;
};

struct st_cor{
    float x,y;
};

struct line{
    st_cor pt1,pt2;
};

class strt{
    public:
    string nm;
    vector<st_cor> co_ords;
    int push_name(string n){
        nm = n;
        return 0;
    }
    int push_street(vector<st_cor> st){
        unsigned int i;
        co_ords.reserve(st.size());
        for(i=0;i<st.size();i++){co_ords.push_back(st.at(i));}
        return 0;
    }

};

int rng(int min,int max, int cnt){
    std::uniform_int_distribution<int> d(min,max);
    std::random_device rd1("/dev/urandom");

    if (cnt==1){return d(rd1);}
    else
    {return 0;}
}


float round_val(float val){
    float var = (int)(val*100 + 0.5);
    return (float)var/100;
}

float eu_dist(float pt1_x,float pt1_y,float pt2_x,float pt2_y){
    return sqrt(pow((pt2_x - pt1_x),2)+pow((pt2_y-pt1_y),2));
}

float eu_dist_v2(st_cor pt1, st_cor pt2){
    return sqrt(pow((pt2.x - pt1.x),2)+pow((pt2.y-pt1.y),2));
}

int print_vector(vector<st_cor> obj){
    unsigned int i;
    for(i=0;i<obj.size();i++){
        cout<<"("<<obj[i].x<<","<<obj[i].y<<") ";
    }
    cout<<endl;
    return 0;
}

int print_line(line ln1){
    cout<<"("<<ln1.pt1.x<<","<<ln1.pt1.y<<")"<<"("<<ln1.pt2.x<<","<<ln1.pt2.y<<")"<<endl;
    return 0;
}

int print_line_vec(vector<line> ln){
    unsigned int i=0;
    for(i=0;i<ln.size();i++){
        print_line(ln[i]);
    }
    return 0;
}

int print_point(st_cor pt){
    cout<<"("<<pt.x<<","<<pt.y<<")"<<endl;
    return 0;
}

string random_name(){
    int len,i,idx;
    char alp[]={'a','e','i','o','u','y','b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
    len = rng(5,10,1);
    string name = "";
    for(i=0;i<len;i++){
        idx=rng(0,25,1);
        name=name+alp[idx];
    }
    return name;
}

bool are_lines_same(line ln1, line ln2){
    if((ln1.pt1.x == ln2.pt1.x)&&(ln1.pt1.y == ln2.pt1.y)&&(ln1.pt2.x == ln2.pt2.x)&&(ln1.pt2.y == ln2.pt2.y)){return true;}
    else{return false;}
}

int sub_check_overlap(line ln1, line ln2){
    float d1,d2,d3,d4;
    d1 = eu_dist_v2(ln1.pt1,ln2.pt1);
    d2 = eu_dist_v2(ln2.pt2,ln2.pt2);
    d3 = eu_dist_v2(ln2.pt2,ln1.pt2);
    d4 = eu_dist_v2(ln1.pt1,ln1.pt2);
    if(round_val(d4)==round_val(d1+d2+d3)){return -1;}
    d2 = eu_dist_v2(ln2.pt1,ln1.pt2);
    d3 = eu_dist_v2(ln1.pt2,ln2.pt2);
    d4 = eu_dist_v2(ln1.pt1,ln2.pt2);
    if(round_val(d4)==round_val(d1+d2+d3)){return -1;}
    return 0;
}

int check_for_overlap(line ln1, line ln2){
    if(are_lines_same(ln1,ln2)){return -1;}
    if(sub_check_overlap(ln1,ln2)==-1){return -1;}
    if(sub_check_overlap(ln2,ln1)==-1){return -1;}
    return 0;
}

int print_add_streets(vector<strt> streets){
    unsigned int i=0;
    for(i=0;i<streets.size();i++){
        cout<<"add "<<"\""<<streets.at(i).nm<<"\" ";
        print_vector(streets.at(i).co_ords);
    }
    return 0;
}

int print_rm_streets(vector<strt> streets){
    unsigned int i=0;
    for(i=0;i<streets.size();i++){
        cout<<"rm "<<"\""<<streets.at(i).nm<<"\""<<endl;
    }
    return 0;
}

int overlap_check(vector<st_cor> st1, vector<st_cor> st2){
    int ret;
    unsigned int i,j;
    line ln1,ln2;
    for(i=0;i<st1.size()-1;i++){
        ln1.pt1.x = st1.at(i).x;
        ln1.pt1.y = st1.at(i).y;
        ln1.pt2.x = st1.at(i+1).x;
        ln1.pt2.y = st1.at(i+1).y;
        for(j=0;j<st2.size()-1;j++){
            ln2.pt1.x = st2.at(j).x;
            ln2.pt1.y = st2.at(j).y;
            ln2.pt2.x = st2.at(j+1).x;
            ln2.pt2.y = st2.at(j+1).y;       
            ret = check_for_overlap(ln1,ln2);
            if(ret==-1){return -1;}
        }
    }
    return 0;
}

int find_intr(line ln1, line ln2){
    vector<st_cor> pts;
    st_cor intr;
    float xnum, xden, ynum, yden,d1,d2,d3,d4,d5,d6;
    float x1,x2,x3,x4,y1,y2,y3,y4;
    int i,j, cnt=0, ret;
    pts.push_back(ln1.pt1);
    pts.push_back(ln1.pt2);
    pts.push_back(ln2.pt1);
    pts.push_back(ln2.pt2);
    ret = check_for_overlap(ln1, ln2);
    if (ret==-1){
        return -1;}
    for(i=0;i<3;i++){
        for(j=i+1;j<4;j++){
            if((pts[i].x == pts[j].x)&&(pts[i].y == pts[j].y)){
                cnt++;
            }
        }
    }
    if(cnt==1){return 0;}
    if(cnt>1){
        return -1;}
    x1=ln1.pt1.x;
    y1=ln1.pt1.y;
    x2=ln1.pt2.x;
    y2=ln1.pt2.y;
    x3=ln2.pt1.x; 
    y3=ln2.pt1.y;
    x4=ln2.pt2.x; 
    y4=ln2.pt2.y;
    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4));
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));
    if (xden == 0){return 0;}
    else{intr.x = (xnum/xden);}
    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4);
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    if (yden == 0){return 0;}
    else{intr.y = (ynum/yden);}
    d1 = eu_dist(ln1.pt1.x,ln1.pt1.y,ln1.pt2.x,ln1.pt2.y);
    d2 = eu_dist(ln1.pt1.x,ln1.pt1.y,intr.x,intr.y);
    d3 = eu_dist(ln1.pt2.x,ln1.pt2.y,intr.x,intr.y);
    d4 = eu_dist(ln2.pt1.x,ln2.pt1.y,ln2.pt2.x,ln2.pt2.y);
    d5 = eu_dist(ln2.pt1.x,ln2.pt1.y,intr.x,intr.y);
    d6 = eu_dist(ln2.pt2.x,ln2.pt2.y,intr.x,intr.y);

    if((round_val(d1/(d2+d3))==1.0)&&(round_val(d4/(d5+d6))==1.0)){
        return -1;}
    return 0;
}

int parse_cmd_line(int argc, char** argv, ip_args *ip){
    int val;
    int i=1;
    ip->s = 10;
    ip->n = 5;
    ip->l = 5;
    ip->c = 20;
    while(i<argc){
            if(strcmp(argv[i],"-s")==0){
                i++;
                val = atoi(argv[i]); 
                if (val >= 2){
                    ip->s = val;
                }
                else{return -1;}
            }
            else if(strcmp(argv[i],"-n")==0){
                i++;
                val = atoi(argv[i]);
                if (val>=1){
                    ip->n = val;
                    }
                else{return -1;}
            }
            else if(strcmp(argv[i],"-l")==0){
                i++;
                val = atoi(argv[i]);
                if (val>=5){
                    ip->l = val;
                    }
                else{return -1;}
            }
            else if(strcmp(argv[i],"-c")==0){
                i++;
                val = atoi(argv[i]);
                if (val>=1){
                    ip->c = val;
                    }
                else{return -1;}
            }
            else{return -2;}
        i++;
        }
    return 0;
}

int gen_st(vector<line> *st_line, vector<st_cor> *st_pt, int n, int c){
    unsigned int n_gen,i;
    st_cor co_ord;
    line tmp_line;
    n_gen = rng(1,n,1);
    for(i=0;i<n_gen+1;i++){
        co_ord.x = (float)rng(-c,c,1);
        co_ord.y = (float)rng(-c,c,1);
        st_pt->push_back(co_ord);
    }
    for(i=0;i<st_pt->size()-1;i++){
        tmp_line.pt1 = st_pt->at(i);
        tmp_line.pt2 = st_pt->at(i+1);
        st_line->push_back(tmp_line);
    }
    return 0;
}

int is_street_valid(const vector<line> st_lines){
    unsigned int i,j;
    int ret;
    float dist;
    for(i=0;i<st_lines.size()-1;i++){
        for(j=i+1;j<st_lines.size();j++){
            ret = find_intr(st_lines[i],st_lines[j]);
            if(ret==-1){
                
                return -1;
            }
        }
    }
    for(i=0;i<st_lines.size();i++){
        dist=eu_dist_v2(st_lines[i].pt1,st_lines[i].pt2);
        if (dist==0){return -1;}
    }
    return 0;
}

int main(int argc, char** argv){
    ip_args ip;
    int ret, retry_cnt=0, st=0;
    unsigned int i;
    vector<st_cor> street;
    vector<line> st_lines;
    vector<vector<st_cor>> streets;
    vector<strt> streets_master;
    strt stemp;

    ret = parse_cmd_line(argc, argv, &ip);
    if (ret == -1){
        cerr<<"Error: Invalid value"<<endl;
        return -1;
    }
    if (ret == -2){
        cerr<<"Error: Invalid command"<<endl;
        return -1;
    }
    while(true){
        if(streets_master.size()>0){
            print_rm_streets(streets_master);
            streets_master.clear();
        }
        streets.clear();
        st=0;
        retry_cnt=0;
        while(st<ip.s){
            stemp.co_ords.clear();
            st_lines.clear();
            street.clear();
            gen_st(&st_lines,&street,ip.n,ip.c);
            ret = is_street_valid(st_lines);
            if((streets.size()>0)&&(ret!=-1)){
                for(i=0;i<streets.size();i++){
                    ret = overlap_check(street,streets.at(i));
                    if(ret==-1){
                        break;
                    }
                }
            }
            if(ret==-1){
                retry_cnt++;
                if(retry_cnt>25){
                    cerr<<"Error: Failed to generate valid input after 25 simultaneous trials"<<endl;
                    return -1;
                }
                continue;
            }
            if(ret==0){
                streets.push_back(street);
                stemp.push_street(street);
                stemp.push_name(random_name());
                streets_master.push_back(stemp);
                st++;
                retry_cnt = 0;
            }
        }
        print_add_streets(streets_master);
        cout<<"gg"<<endl;
        sleep(rng(5,ip.l,1));
    }
    return 0;
}