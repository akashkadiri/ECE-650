// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/minisat/core/Solver.h"
#include "minisat/minisat/mtl/Vec.h"
// defined std::cout
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string.h>
#include <algorithm>

using namespace std;

struct edge_pair{
    int left;
    int right;
};

int parse_input_edges(int max_vertex, std::string edges, std::vector<edge_pair> *eps){
    /*Takes max vertex, edges and edge pair vector as input and parses the edges string and creates a node pair vector*/
    edge_pair one_edge_pair;
    unsigned int i=0,ctr=1;
    //Replacing non numerical characters excluding '-' with spaces so that the same can be parsed into an intereger variable
    for(i=0;i<edges.size();i++){
        if ((edges[i]=='0')||(edges[i]=='1')||(edges[i]=='2')||(edges[i]=='3')||(edges[i]=='4')||(edges[i]=='5')||(edges[i]=='6')||(edges[i]=='7')||(edges[i]=='8')||(edges[i]=='9')||(edges[i]=='-')){
            edges[i]=edges[i];
        }
        else{
            edges[i]=' ';
        }
    }

    std::stringstream ed(edges);
    std::stringstream ed_check(edges);
    int edge;
    ctr=1;
    while(ed>>edge){
        if ((edge<=max_vertex)&&(edge> 0)){
            if(ctr%2 == 1){
                one_edge_pair.left = edge;
                ctr++;
            }
            else{
                one_edge_pair.right = edge;
                eps->push_back(one_edge_pair);
                ctr++;
            }

        }
        else{
            std::cerr<<"Error: Invalid edge entry"<<std::endl;
            return -1;
        }
    }

    //Error checking for the number of edges
    ctr=0;
    while(ed_check>>edge){
        ctr++;
        if (edge>max_vertex){
            std::cerr<<"Error: Invalid edge entry"<<endl;
            return -1;
        }
    }
    if (ctr%2==1){
        std::cerr<<"Error: Invalid number of edges"<<endl;
        return -1;
    }
    return 0;
}

int cnf_sat(unsigned int k_max, std::vector<edge_pair> *E){
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    unsigned int i, j, i1, j1, left, right;
    unsigned int k=1;
    bool res;   
    Minisat::vec<Minisat::Lit> temp_lit, lits;
    vector<int> cnf;

    while(k<k_max){
        //Assign variables
        unsigned int var_no;
        var_no = k*k_max;
        lits.clear();
        for(i=0;i<var_no;i++){
            lits.push(Minisat::mkLit(solver->newVar()));
        }
        //Create the first set of clauses
        for(i=0; i<k; i++){
            for(j=0; j<k_max; j++){
                temp_lit.push(lits[(i*k_max)+j]);
            }
            solver->addClause(temp_lit);
            temp_lit.clear();
        }
        //Create the second set of clauses
        if(k>1){
            for(j=0;j<k_max;j++){
                for(i=0;i<k-1;i++){
                    for(i1=i+1;i1<k;i1++){
                        solver->addClause(~lits[(i*k_max)+j],~lits[(i1*k_max)+j]);
                    }
                }
            }
        }
        //Create the thrid set of clauses
        for(i=0;i<k;i++){
            for(j=0;j<k_max-1;j++){
                for(j1=j+1;j1<k_max;j1++){
                    solver->addClause(~lits[(i*k_max)+j],~lits[(i*k_max)+j1]);
                }
            }
        }
        //Encoding the edges as clauses
        temp_lit.clear();
        for(j=0;j<E->size();j++){
                left = E->at(j).left;
                right = E->at(j).right;
            for(i=0;i<k;i++){
                temp_lit.push(lits[(i*k_max)+(left-1)]);
                temp_lit.push(lits[(i*k_max)+(right-1)]);
            }
            solver->addClause(temp_lit);
            temp_lit.clear();
        }
        res = solver->solve();
        if (res){
            for(i=0;i<k;i++){
                for(j=0;j<k_max;j++){
                    if(Minisat::toInt(solver->modelValue(lits[(i*k_max)+j]))==0){
                        cnf.push_back(j+1);
                    }
                }
            }
            sort(cnf.begin(), cnf.end());
            for(i=0;i<cnf.size();i++){
                if(i==cnf.size()-1){cout<<cnf.at(i)<<endl;}
                else{cout<<cnf.at(i)<<" ";}
            }
            return 0;  
        }
        solver.reset (new Minisat::Solver());
        k++;  
    }
    return -1;
}

int main(void) {
        string vertices,edges,cmd,edge_stream,line;
        string e = "E";
        string v = "V";
        int vertices_max, new_line;
        std::vector<edge_pair> eps;
        int ret;
        bool vertex_received = false;
        bool err_ = false;
        while (getline(cin,line)) {
            fflush(stdout);
            std::istringstream line_ip(line);
            char cmd_action;
            line_ip>>cmd_action;
            if(new_line != 1){
            switch(cmd_action){
                case 'V':
                    line_ip>>vertices_max;
                    if (vertices_max <= 1){
                        std::cerr<<"Error: Vertex cannot be less than or equal to 1"<<endl;
                        err_ = true;
                        break;
                    }
                    err_ = false;
                    vertex_received = true;
                    break;
                case 'E':
                    if((!vertex_received)&&(!err_)){
                        cerr<<"Error: Vertex not received"<<endl;
                        break;
                    }
                    line_ip>>edge_stream;
                    eps.clear();
                    ret = parse_input_edges((unsigned int) vertices_max, edge_stream, &eps);
                    if (ret!=0){
                        break;
                    }
                    vertex_received = false;
                    ret = cnf_sat(vertices_max,&eps);
                    if(ret!=0){
                        cout<<"Error: CNF SAT Unsuccessful"<<endl;
                    }
                    break;
            }
            }
        } 
        return 0;
    
}
