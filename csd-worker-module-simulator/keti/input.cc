#include "input.h"

void Input::InputSnippet(){
        int server_fd;
        int client_fd;
        int opt = 1;
        struct sockaddr_in serv_addr; // 소켓주소
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(struct sockaddr_in);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
              perror("socket failed");
              exit(EXIT_FAILURE);
          }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
              perror("setsockopt");
              exit(EXIT_FAILURE);
          }
        
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(5555); // port

        if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
          perror("bind");
          exit(EXIT_FAILURE);
        } // 소켓을 지정 주소와 포트에 바인딩

        if (listen(server_fd, 8) < 0){
          perror("listen");
          exit(EXIT_FAILURE);
        } // 리스닝

        while(1){
          if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) < 0){
            perror("accept");
                exit(EXIT_FAILURE);
          }
          std::string json;
          char buffer[BUFF_SIZE] = {0};
          
          size_t length;
          int temp = read( client_fd , &length, sizeof(length));
          if(temp == 0){
            cout << "read error" << endl;
          }

          int numread;
          while(1) {
            if ((numread = read( client_fd , buffer, BUFF_SIZE - 1)) == -1) {
              cout << "read error" << endl;
              perror("read");
              exit(1);
            }
            length -= numread;
              buffer[numread] = '\0';
            json += buffer;

              if (length == 0)
              break;
          }

          cout << "*******************Snippet JSON*****************" << endl;
          cout << json.c_str() << endl;
          cout << "************************************************" << endl;
          
          Snippet parsedSnippet(json.c_str());
          calculForReturnData(parsedSnippet);

          //로그
          printf("[CSD Input Interface] Receive Snippet {ID : %d-%d} from Storage Engine Instance\n",parsedSnippet.query_id, parsedSnippet.work_id);

          ScanQueue.push_work(parsedSnippet);
        close(client_fd);
        }

        cout<<"close socket"<<endl;
        close(server_fd);
        }
    // private:
    //     const char* file_path;
    void calculForReturnData(Snippet &snippet){//임시작성
    unordered_map<string, int> col_type, col_offlen;
    for (int i = 0; i < snippet.table_col.size(); i++){
        col_type.insert(make_pair(snippet.table_col[i], snippet.table_datatype[i]));
        col_offlen.insert(make_pair(snippet.table_col[i], snippet.table_offlen[i]));
    }
    
    vector<int> return_datatype, return_offlen;
    for (int i = 0; i < snippet.column_projection.size(); i++){
        //tpc-h 쿼리 동작만 수행하도록 작성 => 수정필요
        if(snippet.column_projection[i].values[0] == "CASE"){
            return_datatype.push_back(3);
            return_offlen.push_back(4);
        }else if(snippet.column_projection[i].values[0] == "EXTRACT"){
            return_datatype.push_back(3);
            return_offlen.push_back(4);
        }else if(snippet.column_projection[i].values[0] == "SUBSTRING"){
            return_datatype.push_back(254);
            return_offlen.push_back(2);
        }else{
            if(snippet.column_projection[i].values.size() == 1){
                return_datatype.push_back(col_type[snippet.column_projection[i].values[0]]);
                return_offlen.push_back(col_offlen[snippet.column_projection[i].values[0]]);
            }else{
                int multiple_count = 0;
                for (int j = 0; j < snippet.column_projection[i].values.size(); j++){
                    if(snippet.column_projection[i].values[j] == "*"){
                      if(snippet.column_projection[i].values[j-1] == "ps_availqty"){//임시로 작성!!!!
                        multiple_count--;
                      }else{
                        multiple_count++;
                    }
                    }
                }
                if(multiple_count == 1){
                    return_datatype.push_back(246);
                    return_offlen.push_back(8);
                }else if(multiple_count == 2){
                    return_datatype.push_back(246);
                    return_offlen.push_back(9);
                }else{
                    return_datatype.push_back(col_type[snippet.column_projection[i].values[0]]);
                    return_offlen.push_back(col_offlen[snippet.column_projection[i].values[0]]);
                }
            }
        }
    }
    
    snippet.projection_datatype = return_datatype;
    snippet.projection_length = return_offlen;
}

