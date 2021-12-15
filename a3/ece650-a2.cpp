// Compile with c++ ece650-a2.cpp -std=c++11 -o ece650-a2

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string.h>
using namespace std;

struct node{
    int val;
    int parent;
};

struct edge_pair{
    int left;
    int right;
};

int node_check(int start_node, int end_node, vector<edge_pair> eps){
    // Checks if both entered values are present in the given edge input. Throw error if it doesn't exist
    bool stnode_flag = false;
    bool endnode_flag = false;
    unsigned int i;
    for(i=0;i<eps.size();i++){
        if(((start_node == eps[i].left)||(start_node == eps[i].right))&&(!stnode_flag)){
            stnode_flag = true;
        }
        if(((end_node==eps[i].left)||(end_node==eps[i].right))&&(!endnode_flag)){
            endnode_flag = true;
        }
        if (stnode_flag && endnode_flag){
            break;
        }
    }
        // Break if both of them are not present
    if (!(stnode_flag && endnode_flag)){
        std::cerr<<"Error: Invalid command. Vertex out of range"<<endl;
        return -1;
    }
    return 0;
}

int print_vector(vector<node> obj){
    // Prints a node in suitable form. Used for testing
    unsigned int i;
    for(i=0;i<obj.size();i++){
        cout<<endl;
        cout<<"Value --> "<<obj[i].val<<endl;
        cout<<"Parent--> "<<obj[i].parent<<endl;
        cout<<endl;
    }
    return 0;
}

int parse_input_edges(int max_vertex, string edges, vector<edge_pair> *eps){
    // Takes max vertex, edges and edge pair vector as input
    // parses the edges string and creates a node pair vector
    edge_pair one_edge_pair;
    unsigned int i=0,ctr=1;
    // Replacing non numerical characters excluding '-' with spaces
    for(i=0;i<edges.size();i++){
        if ((edges[i]=='0')||(edges[i]=='1')||(edges[i]=='2')||(edges[i]=='3')||(edges[i]=='4')||(edges[i]=='5')||(edges[i]=='6')||(edges[i]=='7')||(edges[i]=='8')||(edges[i]=='9')||(edges[i]=='-')){
            edges[i]=edges[i];
        }
        else{
            edges[i]=' ';
        }
    }

    stringstream ed(edges);
    stringstream ed_check(edges);
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
            std::cerr<<"Error: Invalid edge entry"<<endl;
            return -1;
        }
    }

    // Error checking for the number of edges
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

bool find_in_closed(int val, vector<node> closed){
    unsigned int i;
    for(i=0;i<closed.size();i++){
        if(val == closed[i].val){
            return true;
        }
    }
    return false;
}

bool find_in_open(int val, vector<node> open,int start_index){
    unsigned int i;
    for(i=start_index;i<open.size();i++){
        if(val == open[i].val){
            return true;
        }
    }
    return false;
}

int print_path(vector<int> path){
    // Takes the identified path vector and prints it in a suitable form as per requirement
    cout<<path[path.size()-1];
    path.pop_back();
    while(path.size()>0){
        cout<<"-";
        cout<<path[path.size()-1];
        path.pop_back();
    }
    cout<<endl;
    return 0;
}

int trace_path(vector<node> closed){
    // Uses closed node vector to back track all the way from goal node to start node and find its way back
    vector<int> path;
    node temp;
    int search_node,i;
    temp = closed[closed.size()-1];
    closed.pop_back();
    path.push_back(temp.val);
    search_node = temp.parent;
    i=closed.size()-1;
    while(search_node != -1){
        if(search_node == closed[i].val){
            path.push_back(closed[i].val);
            search_node = closed[i].parent;
        }
        i--;
        if(i<0){
            i=closed.size();
        }
    }
    print_path(path);
    return 0;
}

int shortest_path(int start_node, int goal_node, vector<edge_pair> edge_pairs){

    node open_var; 
    // Intermediate variable to package data before pushing into a vector
    int open_front_index = 0;
    int current_node;
    unsigned int i;
    vector<node> open;
    vector<node> closed;
    open_var.val = start_node;
    open_var.parent = -1; 
    // Starting node has parent -1//
    open.push_back(open_var);
    while((open.size()-open_front_index)>0){
        current_node = open[open_front_index].val;

        if (current_node == goal_node){
            closed.push_back(open[open_front_index]);
            trace_path(closed);   
            return 0;
        }
        else{
            closed.push_back(open[open_front_index]);
        }
        for(i=0;i<edge_pairs.size();i++){

            if((edge_pairs[i].left == current_node)&&(!find_in_open(edge_pairs[i].right,open,open_front_index))&&(!find_in_closed(edge_pairs[i].right,closed))){   
                open_var.val = edge_pairs[i].right;
                open_var.parent = current_node;       
                open.push_back(open_var);
            }
            if((edge_pairs[i].right == current_node)&&(!find_in_open(edge_pairs[i].left,open, open_front_index))&&(!find_in_closed(edge_pairs[i].left,closed))){
                open_var.val = edge_pairs[i].left;
                open_var.parent = current_node;       
                open.push_back(open_var);
            }
        }
        open_front_index++;
    }
    return 1;
}

int main(int argc, char** argv) {
// Variables declarations
        string vertices,edges,cmd,edge_stream,line;
        ifstream mfile(argv[0]);
        string e = "E";
        string v = "V";
        int vertices_max, start_node, end_node;
        int new_line;
        vector<edge_pair> eps;
        vector<node> vec;
        int ret;
        bool vertex_received = false;
        bool edge_received = false;
        bool err_ = false;
    // user input part

        while (getline(cin,line)) {
            fflush(stdout);
            // Getting the input Vertex. Checking for error and storing the maximum value and assigning the same number of memory
            new_line = 0;
            if(line.size()<=1){
                new_line = 1;
            }
            std::istringstream line_ip(line);
            char cmd_action;
            line_ip>>cmd_action;
            
            if(new_line != 1){
                switch(cmd_action){
                    
                    case 'V':
                        line_ip>>vertices_max;
                        cout<<line<<endl;
                        if (vertices_max <= 1){
                            std::cerr<<"Error: Vertex cannot be less than or equal to 1"<<endl;
                            err_ = true;
                            break;
                        }
                        vertex_received = true;
                        edge_received = false;
                        break;
                    
                    case 'E':
                        cout<<line<<endl;
                        if(!vertex_received&&(!err_)){
                            cerr<<"Error: Vertex not received"<<endl;
                            break;
                        }
                        line_ip>>edge_stream;
                        eps.clear();
                        ret = parse_input_edges(vertices_max, edge_stream, &eps);
                        if (ret!=0){
                            break;
                        }
                        vertex_received = false;
                        edge_received = true;
                        break;
                    
                    case 's':
                        if(!edge_received){
                            std::cerr<<"Error: V or E input not found"<<endl;
                            break;
                        }
                        line_ip>>start_node;
                        line_ip>>end_node;
                        ret = node_check(start_node,end_node,eps);
                        if(ret!=0){
                            break;
                        }
                        ret = shortest_path(start_node,end_node,eps);
                        if (ret!=0){
                            std::cerr<<"Error: Path does not exist"<<endl;
                            break;
                        }
                        break;
                }
            }
        } 
        return 0;
}
